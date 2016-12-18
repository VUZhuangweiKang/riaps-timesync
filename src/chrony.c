/**
 *  RIAPS Timesync Service
 *
 *  Copyright (C) Vanderbilt University, ISIS 2016
 */

/* Communication with chrony. NOTE: contains code from the crony project. */
#include "chrony.h"

static int chrony_socket = -1;
static int chrony_seq = 0;

static int close_chrony_socket()
{
    close(chrony_socket);
    chrony_socket = -1;
}


static int get_chrony_socket()
{
    if (chrony_socket < 0) {
        struct hostent *chrony_host;
        struct sockaddr_in chrony_addr;
        struct timeval timeout = {CHRONY_TIMEOUT, 0};

        chrony_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (chrony_socket < 0) {
            chrony_socket = 0;
            goto ondemand_end;
        }

        if (setsockopt(chrony_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,
                        sizeof(struct timeval))) {
            close_chrony_socket();
            goto ondemand_end;
        }

        chrony_host = gethostbyname(CHRONY_CMD_ADDR);
        if (chrony_host == NULL) {
            close_chrony_socket();
            goto ondemand_end;
        }

        bzero((char *) &chrony_addr, sizeof(chrony_addr));
        chrony_addr.sin_family = AF_INET;
        bcopy((char *)chrony_host->h_addr,
            (char *)&chrony_addr.sin_addr.s_addr, chrony_host->h_length);
        chrony_addr.sin_port = htons(CHRONY_CMD_PORT);
        if (connect(chrony_socket, (struct sockaddr *) &chrony_addr,
                        sizeof(chrony_addr)) < 0) {
            close_chrony_socket();
            goto ondemand_end;
        }
    }

ondemand_end:
    return chrony_socket;
}

static int chrony_request(chrony_req* req, int req_len, chrony_rep* rep, int rep_len, int rep_id)
{
    req->version = PROTO_VERSION_NUMBER;
    req->pkt_type = PKT_TYPE_CMD_REQUEST;
    req->res1 = 0;
    req->res2 = 0;
    req->sequence = htonl(chrony_seq++);
    req->attempt = -1;
    req->pad1 = 0;
    req->pad2 = 0;

    req_len = req_len > rep_len ? req_len : rep_len;

    while (req->attempt++ > CHRONY_MAX_RETRIES) {
        int sock_fd;
        int recvlen;

        sock_fd = get_chrony_socket();
        if (sock_fd < 0) {
            return -1;
        }
        if (send(sock_fd, (void *)req, req_len, 0) < 0) {
            close_chrony_socket();
        }

        recvlen = recv(sock_fd, (void *)rep, sizeof(chrony_rep), 0);
        if (recvlen < rep_len) {
            continue;
        }
        if (rep->sequence != req->sequence) {
            continue;
        }
        if (rep->version != PROTO_VERSION_NUMBER) {
            continue;
        }
        if (rep->pkt_type != PKT_TYPE_CMD_REPLY) {
            continue;
        }
        if (rep->command != req->command) {
            continue;
        }
        if (ntohs(rep->status) != STT_SUCCESS) {
            continue;
        }
        if (ntohs(rep->reply) != rep_id) {
            continue;
        }

        // everything seems ok
        return 0;

    }

    return -1;
}

int chrony_tracking()
{
    chrony_req req;
    chrony_rep rep;

    req.command = htons(REQ_TRACKING);
    if (chrony_request(&req, REQ_LENGTH(tracking), &rep, REP_LENGTH(tracking), RPY_TRACKING)) {
        return -1;
    }
    if (ntohs(rep.data.tracking.ip_addr.family == IPADDR_UNSPEC)) {
        int i;
        uint32_t ref_id = ntohl(rep.data.tracking.ref_id);
        char buf[sizeof(ref_id) + 1];

	memset(buf, '\0', sizeof(buf));
        for (i = 0; i < sizeof(ref_id); i++) {
            char c = (ref_id >> (24 - i * 8)) & 0xff;
            if (isprint(c)) {
                buf[i] = c;
            }
        }
        printf("%s\n", buf);
    }
    else {
        printf("NTP\n");
    }
    return 0;
}
