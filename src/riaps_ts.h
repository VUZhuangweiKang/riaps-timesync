/*
    RIAPS Timesync Service

    Copyright (C) Vanderbilt University, ISIS 2016

*/
#ifndef _RIAPS_TS_H_
#define _RIAPS_TS_H_

#include <time.h>

#define RIAPS_TS_RELTIME 0
#define RIAPS_TS_ABSTIME 1
#define RIAPS_TS_MASTER 0
#define RIAPS_TS_SLAVE 1
#define RIAPS_TS_REF_NONE 0
#define RIAPS_TS_REF_GPS 1
#define RIAPS_TS_REF_NTP 2
#define RIAPS_TS_REF_PTP 3

struct riaps_ts_timespec {
    long    tv_sec;        /* seconds, always 32 bit */
    long    tv_nsec;       /* nanoseconds */
};

struct riap_ts_status {
    int role;
    int reference;
    struct riaps_ts_timespec now;
    double last_offset;
    double rms_offset;
    double ppm;                /* clock drift parts per million */
};

int riaps_ts_gettime(struct riaps_ts_timespec *res);
int riaps_ts_sleep(int flags, const struct riaps_ts_timespec *request);
int riaps_ts_status(struct riap_ts_status* stat);

#endif // _RIAPS_TS_H_
