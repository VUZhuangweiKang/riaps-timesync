/*
*/
#include <stdio.h>
#include "riaps_ts.h"

int main(int argc, const char* argv[])
{
    struct riaps_ts_timespec now;
    riaps_ts_gettime(&now);
    printf("NOW: %ld secs\n", now.tv_sec);
    return 0;
}