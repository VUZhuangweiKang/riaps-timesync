/*
  pps-gen - generate a PPS signal on a GPIO

  The PPS signal is aligned halfway between second boundaries (system clock) (see PHASE).
  The halfway point is selected to minimize interference with the GPS PPS timestamping.
  The PPS signal is generated from software for testing:
    - global timesync
    - task release jitter (although, the current implementation
      tries to minimize this effect - see BUSY_WAIT_INTERVAL)

  Copyright 2017, Peter Volgyesi, Vanderbilt University
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>

// P8_19 = GPIO0_22 (EHRPWM2A)
#define PPS_OUTPUT  22
#define BUSY_WAIT_INTERVAL  1000000L // nanoseconds
#define PHASE 500000000L // nanoseconds

#include <libsoc_gpio.h>

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
  struct timespec t, t2;

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
    if (clock_gettime(CLOCK_REALTIME, &t)) {
       perror("clock_gettime failed:");
       exit(-1);
    }

    if (t.tv_nsec > PHASE) {
      t.tv_sec += 1;
    }
    else if (t.tv_nsec > (PHASE - 2 * BUSY_WAIT_INTERVAL)) {
      printf("too close to epoch, skipping one second.\n");
      t.tv_sec += 1;
    }

    t.tv_nsec = PHASE - BUSY_WAIT_INTERVAL;

    // long wait by timer / blocking
    if (clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &t, NULL)) {
      perror("clock_nanosleep failed:");
      exit(-1);
    }

    // busy wait for increased accuracy (approx 2us accuracy on BBB)
    do {
      if (clock_gettime(CLOCK_REALTIME, &t)) {
        perror("clock_gettime failed:");
        exit(-1);
      }
    } while (t.tv_nsec <= PHASE);

    libsoc_gpio_set_level(pps_output, HIGH); // might take 10 us
    if (clock_gettime(CLOCK_REALTIME, &t2)) {
        perror("clock_gettime failed:");
        exit(-1);
    }
    //usleep(10);
    libsoc_gpio_set_level(pps_output, LOW);

    printf("%lld.%.9ld [%ld...%ld ns]\n", (long long)t.tv_sec, t.tv_nsec, (t.tv_nsec - PHASE), (t2.tv_nsec - PHASE)); 
  }

  if (pps_output)
  {
    libsoc_gpio_free(pps_output);
  }
}
