/**
 *  RIAPS Timesync Service
 *
 *  Copyright (C) Vanderbilt University, ISIS 2016
 */
#include <ctype.h>

#include "riaps_ts.h"
#include "chrony.h"

int riaps_ts_gettime(struct riaps_ts_timespec *res)
{
    struct timespec tp;
    int ret;
    ret = clock_gettime(CLOCK_REALTIME, &tp);
    if (ret == 0) {
        res->tv_sec = tp.tv_sec;
        res->tv_nsec = tp.tv_nsec;
    }
    return ret;
}


int riaps_ts_sleep(int flags, const struct riaps_ts_timespec *request)
{
    struct timespec cn_req;
    int cn_flags;

    if (flags ==  RIAPS_TS_ABSTIME) {
        cn_flags = TIMER_ABSTIME;
    }
    else if (flags == RIAPS_TS_RELTIME) {
        cn_flags = 0;
    }
    else {
        return -1;
    }

    cn_req.tv_sec = request->tv_sec;
    cn_req.tv_nsec = request->tv_nsec;
    return clock_nanosleep(CLOCK_REALTIME, cn_flags, &cn_req, NULL);
}


int riaps_ts_status(struct riap_ts_status* stat)
{
    chrony_req req;
    chrony_rep rep;

    if (!stat) {
        return -1;
    }

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
