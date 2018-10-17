/*
  gpio_timing - measure the latency (and jitter) of generating pulses
  on a GPIO pin using libsoc or mmap

  Copyright 2017, Peter Volgyesi, Vanderbilt University
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#define USE_MMAP

#define N_PULSES     100
#define T_SLEEP      500000000

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

struct timespec pulse_start[N_PULSES];
struct timespec pulse_end[N_PULSES];

// PPS OUTPUT (GPS EXTINT on ChronoCape): P8_19 = GPIO0_22 (EHRPWM2A)

#ifdef USE_MMAP

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define PPS_PIN          (1<<22)
#define GPIO0_START_ADDR (0x44E07000)
#define GPIO0_END_ADDR   (0x44E07FFF)
#define GPIO0_SIZE       (GPIO0_END_ADDR - GPIO0_START_ADDR)
#define GPIO_OE          (0x134)
#define GPIO_SETDATAOUT  (0x194)
#define GPIO_CLEARDATAOUT (0x190)

volatile unsigned int *gpio_setdataout_addr = NULL;
volatile unsigned int *gpio_cleardataout_addr = NULL;

void gpio_setup()
{
  volatile void *gpio_addr;
  volatile unsigned int *gpio_oe_addr;

  int fd;

  fd = open("/dev/mem", O_RDWR);
  if (fd < 0) {
    perror("unable to open /dev/mem:");
    exit(-1);
  }
  gpio_addr = mmap(0, GPIO0_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                   fd, GPIO0_START_ADDR);
  if (gpio_addr == MAP_FAILED) {
    perror("mmap failed:");
    exit(-1);
  }
  gpio_oe_addr = gpio_addr + GPIO_OE;
  gpio_setdataout_addr = gpio_addr + GPIO_SETDATAOUT;
  gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;

  *gpio_oe_addr &= (0xFFFFFFFF - PPS_PIN);
}

static inline void gpio_assert()
{
  *gpio_setdataout_addr= PPS_PIN;
}

static inline void gpio_deassert()
{
  *gpio_cleardataout_addr= PPS_PIN;
}

#else

#include <libsoc_gpio.h>
#define PPS_OUTPUT  22

gpio *pps_output;

void gpio_setup()
{
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
}

static inline void gpio_assert()
{
  libsoc_gpio_set_level(pps_output, HIGH); // might take ~10 us
}

static inline void gpio_deassert()
{
  libsoc_gpio_set_level(pps_output, LOW); // might take ~10 us
}

#endif


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

  printf("pulse width mean: %.0f ns, rms: %.0f ns, min: %ld ns, max: %ld ns\n", mean, rms, min, max);
}

void scheduler_setup()
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
  struct timespec t_sleep = {0, T_SLEEP};

  gpio_setup();
  scheduler_setup();


  while (1) {
    for (int i = 0; i < N_PULSES; i++) {
      if (clock_gettime(CLOCK_REALTIME, &pulse_start[i])) {
        perror("clock_gettime failed:");
        exit(-1);
      }
      gpio_assert();
      gpio_deassert();
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
}
