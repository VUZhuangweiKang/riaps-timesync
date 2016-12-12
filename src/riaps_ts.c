/*
*/

#include "riaps_ts.h"


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
    return -1;
}


int riaps_ts_status(struct riap_ts_status* stat)
{
    return -1;
}
