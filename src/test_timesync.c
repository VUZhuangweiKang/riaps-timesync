/*
    RIAPS Timesync Service

    Copyright (C) Vanderbilt University, ISIS 2016

*/
#include <stdio.h>
#include <stdlib.h>
#include "riaps_ts.h"

int main(int argc, const char* argv[])
{
    struct riaps_ts_timespec now;
    struct riaps_ts_timespec snooze;

    if (riaps_ts_gettime(&now)) {
        perror("ERROR: riaps_ts_gettime()");
        exit(-1);
    }

    // Try to wake up on second boundaries (PPS)
    snooze.tv_sec = now.tv_sec + 1;
    snooze.tv_nsec = 0;

    for(;;) {
        riaps_ts_gettime(&now);
        printf("NOW: %lld.%09ld secs\n", (long long)now.tv_sec, (long)now.tv_nsec);
        if (riaps_ts_status(NULL)) {
            printf("YES\n");
        }
        snooze.tv_sec++;
        riaps_ts_sleep(RIAPS_TS_ABSTIME, &snooze);
    }
    return 0;
}
