/**
 *  RIAPS Timesync Service
 *
 *  Copyright (C) Vanderbilt University, ISIS 2016
 */

/* Communication with chrony. NOTE: contains code from the crony project. */
#ifndef _CHRONY_H_
#define _CHRONY_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define CHRONY_CMD_ADDR "127.0.0.1"
#define CHRONY_CMD_PORT 323

#define MAX_PADDING_LENGTH 396
#define PROTO_VERSION_NUMBER 6

#define CHRONY_TIMEOUT 1
#define CHRONY_MAX_RETRIES 3

/*** Custom types ***/
#define IPADDR_UNSPEC 0
#define IPADDR_INET4 1
#define IPADDR_INET6 2

typedef struct
{
    int32_t f;
} float_t;

typedef struct
{
    uint32_t tv_sec_high;
    uint32_t tv_sec_low;
    uint32_t tv_nsec;
} timeval_t;

typedef struct
{
    union {
        uint32_t in4;
        uint8_t in6[16];
    } addr;
    uint16_t family;
} ipaddr_t;

#define PKT_TYPE_CMD_REQUEST 1
#define PKT_TYPE_CMD_REPLY 2

/*** Requests ***/
#define REQ_TRACKING 33

typedef struct
{
    int32_t EOR;
} req_null;

typedef struct
{
    uint8_t version;
    uint8_t pkt_type;
    uint8_t res1;
    uint8_t res2;
    uint16_t command;
    uint16_t attempt;
    uint32_t sequence;
    uint32_t pad1;
    uint32_t pad2;
    union {
        req_null null;
        req_null tracking;
    } data;
    uint8_t padding[MAX_PADDING_LENGTH];
} chrony_req;

/*** Replies ***/

#define STT_SUCCESS 0

#define RPY_TRACKING 5

typedef struct
{
    int32_t EOR;
} rep_null;

typedef struct
{
    uint32_t ref_id;
    ipaddr_t ip_addr;
    uint16_t stratum;
    uint16_t leap_status;
    timeval_t ref_time;
    float_t current_correction;
    float_t last_offset;
    float_t rms_offset;
    float_t freq_ppm;
    float_t resid_freq_ppm;
    float_t skew_ppm;
    float_t root_delay;
    float_t root_dispersion;
    float_t last_update_interval;
    int32_t EOR;
} rep_tracking;

typedef struct
{
    uint8_t version;
    uint8_t pkt_type;
    uint8_t res1;
    uint8_t res2;
    uint16_t command;
    uint16_t reply;
    uint16_t status;
    uint16_t pad1;
    uint16_t pad2;
    uint16_t pad3;
    uint32_t sequence;
    uint32_t pad4;
    uint32_t pad5;

    union {
        rep_null null;
        rep_tracking tracking;
    } data;

} chrony_rep;

#define REQ_LENGTH(reply_data_field) \
  offsetof(chrony_req, data.reply_data_field.EOR)

#define REP_LENGTH(reply_data_field) \
  offsetof(chrony_rep, data.reply_data_field.EOR)

int chrony_request(chrony_req* req, int req_len, chrony_rep* rep, int rep_len, int rep_id);

#endif // _CHRONY_H_