/**
 *  RIAPS Timesync Service
 *
 *  Copyright (C) Vanderbilt University, ISIS 2016
 */

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
    return chrony_tracking();
}
