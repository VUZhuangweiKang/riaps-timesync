/*
  pps-gen - generate a PPS signal on a GPIO

  The PPS signal is aligned to the second boundary (system clock).
  The PPS signal is generated from software for testing:
    - global timesync
    - task release jitter (although, the current implementation
      tries to minimize this effect

  Copyright 2017, Peter Volgyesi, Vanderbilt University
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include <libsoc_gpio.h>

// P8_19 = 22 (EHRPWM2A)
#define PPS_OUTPUT  22

#define BUSY_WAIT_INTERVAL  1000000L // nanoseconds

void setup_scheduler()
{
  struct sched_param schedp;

  memset(&schedp, 0, sizeof(schedp));
  schedp.sched_priority = 90;
  if (sched_setscheduler(0, SCHED_FIFO, &schedp)) {
    perror("unable to set real-time scheduler:");
    exit(-1);
  }
}

int main(int argc, char* argv[])
{
  gpio *pps_output;
  struct timespec t1, t2;
  struct timespec dbg[5];
  int idbg;

  pps_output = libsoc_gpio_request(PPS_OUTPUT, LS_SHARED);
  if (!pps_output) {
    perror("unable to request gpio pin:");
    exit(-1);
  }
  libsoc_gpio_set_direction(pps_output, OUTPUT);

  if (libsoc_gpio_get_direction(pps_output) != OUTPUT)
  {
    perror("unable to set output direction:");
    exit(-1);
  }

  setup_scheduler();

  while (1) {
    if (clock_gettime(CLOCK_REALTIME, &t1)) {
       perror("clock_gettime failed:");
       exit(-1);
    }

    if (t1.tv_nsec > 800000000L) {
      printf("too close to second boundary, skipping.\n");
      t1.tv_sec += 1;
    }

    t1.tv_nsec = 1000000000L - BUSY_WAIT_INTERVAL;

    // long wait by timer / blocking
    if (clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &t1, NULL)) {
      perror("clock_nanosleep failed:");
      exit(-1);
    }

    // busy wait for increased accuracy (approx 2us accuracy on BBB)
    idbg = 0;
    do {
      if (clock_gettime(CLOCK_REALTIME, &t2)) {
        perror("clock_gettime failed:");
        exit(-1);
      }
      dbg[idbg] = t2; idbg = (idbg + 1) % 5;
    } while (t2.tv_sec <= t1.tv_sec);

    libsoc_gpio_set_level(pps_output, HIGH);
    //usleep(10);
    libsoc_gpio_set_level(pps_output, LOW);

    //printf("%lld.%.9ld\n", (long long)t2.tv_sec, t2.tv_nsec); // fflush(stdout);
    printf("%ld ns\n", t2.tv_nsec); // fflush(stdout);
    for (int i = 0; i < 5; i++) {
      long nsec = dbg[(idbg + i) % 5].tv_nsec;
      if (nsec > 500000000) {
        nsec = 1000000000 - nsec;
      }
      printf("\t%ld ns\n", nsec); 
    }
  }

  if (pps_output)
  {
    libsoc_gpio_free(pps_output);
  }
}
