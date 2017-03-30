/*
  ppm-gen - generate a PPM signal on a GPIO

  The PPM signal is aligned to the second boundary (system clock).
  The PPM signal is generated from software for testing:
    - global timesync
    - task release jitter

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
#define PPM_OUTPUT  22

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
  gpio *ppm_output;
  struct timespec t1, t2;

  ppm_output = libsoc_gpio_request(PPM_OUTPUT, LS_SHARED);
  if (!ppm_output) {
    perror("unable to request gpio pin:");
    exit(-1);
  }
  libsoc_gpio_set_direction(ppm_output, OUTPUT);

  if (libsoc_gpio_get_direction(ppm_output) != OUTPUT)
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

    // busy wait for increased accuracy
    do {
      if (clock_gettime(CLOCK_REALTIME, &t2)) {
        perror("clock_gettime failed:");
        exit(-1);
      }
    } while (t2.tv_sec <= t1.tv_sec);

    libsoc_gpio_set_level(ppm_output, HIGH);
    //usleep(10);
    libsoc_gpio_set_level(ppm_output, LOW);
    printf("%lld.%.9ld\n", (long long)t2.tv_sec, t2.tv_nsec); // fflush(stdout);
  }

  if (ppm_output)
  {
    libsoc_gpio_free(ppm_output);
  }
}
