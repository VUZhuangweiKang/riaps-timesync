/*
  posix_clocks - experiment with and compare POSIX clock sources

  Copyright 2017, Peter Volgyesi, Vanderbilt University
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    clockid_t id;
    const char* name;
    struct timespec t_last;
} clocksource_t;

clocksource_t clocks[] = {
    {CLOCK_REALTIME, "REALTIME"},
    {CLOCK_MONOTONIC, "MONOTONIC"},
    {CLOCK_MONOTONIC_RAW, "MONOTONIC_RAW"},
    {CLOCK_PROCESS_CPUTIME_ID, "PROCESS_CPUTIME"},
    {CLOCK_THREAD_CPUTIME_ID, "THREAD_CPUTIME"},
    {(clockid_t) -1, NULL}
};

long ns_diff(const struct timespec *t1, const struct timespec *t2)
{
  return t1->tv_nsec - t2->tv_nsec + (t1->tv_sec - t2->tv_sec) * 1000000000L;
}

int main(int argc, char* argv[])
{
    int i;
    int cnt = 0;
    struct timespec sleep_interval = {1, 0};

    // Header
    for (i = 0; clocks[i].id != (clockid_t) -1; i++) {
        printf ("%20s [%d] ", clocks[i].name, clocks[i].id);
    }
    printf ("\n");

    // Resolution
    for (i = 0; clocks[i].id != (clockid_t) -1; i++) {
        struct timespec res;
        if (clock_getres (clocks[i].id, &res)) {
            perror ("clock_getres");
            exit(-1);
        }

        printf ("     res = %10ld ns ", res.tv_nsec);
    }
    printf ("\n");

    while (1) {
        // Times
        for (i = 0; clocks[i].id != (clockid_t) -1; i++) {
            struct timespec res;
            if (clock_gettime (clocks[i].id, &res)) {
                perror ("clock_gettime");
                exit(-1);
            }
            if (cnt > 0) {
                printf ("             +%-10ld ", ns_diff(&res, &clocks[i].t_last));
            }
            else {
                printf (" %13ld.%-9ld ", res.tv_sec, res.tv_nsec);
            }
            clocks[i].t_last = res;
        }
        printf ("\n");

        if (clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep_interval, NULL)) {
            perror("clock_nanosleep failed:");
            exit(-1);
        }
        cnt++;
    }

    return 0;
}