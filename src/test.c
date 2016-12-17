/*
    RIAPS Timesync Service

    Copyright (C) Vanderbilt University, ISIS 2016

*/
#include <stdio.h>
#include "riaps_ts.h"

int main(int argc, const char* argv[])
{
    struct riaps_ts_timespec now;
    struct riaps_ts_timespec snooze;

    snooze.tv_sec = 1;
    snooze.tv_nsec = 0;

    for(;;) {
        riaps_ts_gettime(&now);
        printf("NOW: %lld.%9ld secs\n", now.tv_sec, now.tv_nsec);
        if (riaps_ts_status(NULL)) {
            printf("YES\n");
        }
        riaps_ts_sleep(RIAPS_TS_RELTIME, &snooze);
    }
    return 0;
}