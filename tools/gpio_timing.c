/*
  gpio_timing - measure the lenght (and jitter) of GPIO operations

  Copyright 2017, Peter Volgyesi, Vanderbilt University
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include <limits.h>
#include <math.h>
#include <libsoc_gpio.h>

// P8_19 = 22 (EHRPWM2A)
#define GPIO_OUTPUT  22

#define N_PULSES     100
#define T_SLEEP      500000000 

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

struct timespec pulse_start[N_PULSES];
struct timespec pulse_end[N_PULSES];

void timespec_diff(struct timespec *start, struct timespec *stop,
                   struct timespec *result)
{
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }

    return;
}


void print_stats()
{
  double mean, rms;
  long min = LONG_MAX, max = LONG_MIN;

  mean = 0.0;
  for (int i = 0; i < N_PULSES; i++) {
    struct timespec diff;
    timespec_diff(&pulse_start[i], &pulse_end[i], &diff);
    if (diff.tv_sec != 0) {
      printf("WARNING: pulse width is too long: %lld.%.9ld\n", (long long)diff.tv_sec, diff.tv_nsec);
    }
    mean += diff.tv_nsec; 
    min = MIN(min, diff.tv_nsec);
    max = MAX(max, diff.tv_nsec);
  }
  mean /= N_PULSES;

  rms = 0.0;
  for (int i = 0; i < N_PULSES; i++) {
    struct timespec diff;
    double dev;
    timespec_diff(&pulse_start[i], &pulse_end[i], &diff);
    if (diff.tv_sec != 0) {
      printf("WARNING: pulse width is too long: %lld.%.9ld\n", (long long)diff.tv_sec, diff.tv_nsec);
    }
    dev = diff.tv_nsec - mean;
    rms += dev * dev; 
  }
  rms = sqrt(rms / N_PULSES);
  
  printf("mean: %.0f ns, rms: %.0f ns, min: %ld ns, max: %ld ns\n", mean, rms, min, max);
}

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
  struct timespec t_sleep = {0, T_SLEEP};

  pps_output = libsoc_gpio_request(GPIO_OUTPUT, LS_SHARED);
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
    for (int i = 0; i < N_PULSES; i++) {
      if (clock_gettime(CLOCK_REALTIME, &pulse_start[i])) {
        perror("clock_gettime failed:");
        exit(-1);
      }
      libsoc_gpio_set_level(pps_output, HIGH);
      libsoc_gpio_set_level(pps_output, LOW);
      if (clock_gettime(CLOCK_REALTIME, &pulse_end[i])) {
        perror("clock_gettime failed:");
        exit(-1);
      }
    }
    print_stats();
    // Becuase of the real time scheduling priority we need to let the system
    // to breath a little.
    if (clock_nanosleep(CLOCK_MONOTONIC, 0, &t_sleep, NULL)) {
        perror("clock_nanosleep failed:");
        exit(-1);
    }
  }

  if (pps_output)
  {
    libsoc_gpio_free(pps_output);
  }
}
