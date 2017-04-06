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
#include <time.h>

#define USE_MMAP

#define PHASE 999000000L // nanoseconds
#define BUSY_WAIT_INTERVAL  1000000L // nanoseconds

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

long ns_diff(const struct timespec *t1, const struct timespec *t2)
{
  return t1->tv_nsec - t2->tv_nsec + (t1->tv_sec - t2->tv_sec) * 1000000000L;
}

int main(int argc, char* argv[])
{
  struct timespec t, t_epoch_guard, t_epoch, t_asserted;

  gpio_setup();
  scheduler_setup();

  while (1) {
    if (clock_gettime(CLOCK_REALTIME, &t)) {
       perror("clock_gettime failed:");
       exit(-1);
    }

    t_epoch.tv_sec = t.tv_sec;
    t_epoch.tv_nsec = PHASE;
    if (t.tv_nsec > PHASE) {
      t_epoch.tv_sec += 1;
    }

    t_epoch_guard.tv_sec = t_epoch.tv_sec;
    t_epoch_guard.tv_nsec = t_epoch.tv_nsec - BUSY_WAIT_INTERVAL;
    if (t_epoch_guard.tv_nsec < 0) {
      t_epoch_guard.tv_sec -= 1;
      t_epoch_guard.tv_nsec += 1000000000L;
    }
    if (ns_diff(&t_epoch_guard, &t) < 0)
    {
      printf("too close to epoch, skipping one second.\n");
      t_epoch_guard.tv_sec += 1;
      t_epoch.tv_sec += 1;
    }

    // long wait by timer / blocking
    if (clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &t_epoch_guard, NULL)) {
      perror("clock_nanosleep failed:");
      exit(-1);
    }

    // busy wait for increased accuracy (approx 2us accuracy on BBB)
    do {
      if (clock_gettime(CLOCK_REALTIME, &t)) {
        perror("clock_gettime failed:");
        exit(-1);
      }
    } while (ns_diff(&t, &t_epoch) < 0);

    gpio_assert();
    if (clock_gettime(CLOCK_REALTIME, &t_asserted)) {
        perror("clock_gettime failed:");
        exit(-1);
    }
    //usleep(10);
    gpio_deassert();

    printf("%lld.%.9ld [%ld...%ld ns]\n", 
             (long long)t.tv_sec, t.tv_nsec, 
              ns_diff(&t, &t_epoch), ns_diff(&t_asserted, &t_epoch)); 
  }
}
