/*
  sync-gen - generate a periodic sync signal on a selected GPIO pin of the
  BeagleBone Black board.

  This file is part of RIAPS project.

  Copyright 2017, Peter Volgyesi, Vanderbilt University
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include <time.h>
#include <argp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


struct arguments
{
  int freq;
  int width;
  char *pin_name;
};


#define NANOS_PER_SEC   1000000000L
#define BUSY_WAIT_INTERVAL  300000L // nanoseconds

static const unsigned int GPIO_BASE_ARRAY[4] = {0x44E07000, 0x4804C000, 0x481AC000, 0x481AE000};
#define GPIO_SIZE        (0x1000)
#define GPIO_OE          (0x134)
#define GPIO_SETDATAOUT  (0x194)
#define GPIO_CLEARDATAOUT (0x190)

struct pin_desc
{
  const char* pin_name;
  int gpio_bank;
  int gpio_pos;
};

struct pin_desc pins[] = {
  {"P8_3",  1,  6}, {"P8_4",  1,  7},      
  {"P8_5",  1,  2}, {"P8_6",  1,  3},      
  {"P8_7",  2,  2}, {"P8_8",  2,  3},      
  {"P8_9",  2,  5}, {"P8_10", 2,  4},     
  {"P8_11", 1, 13}, {"P8_12", 1, 12},     {"P9_11", 0, 30}, {"P9_12", 1, 28},
  {"P8_13", 0, 23}, {"P8_14", 0, 26},     {"P9_13", 0, 31}, {"P9_14", 1, 18},
  {"P8_15", 1, 15}, {"P8_16", 1, 14},     {"P9_15", 1, 16}, {"P9_16", 1, 19},
  {"P8_17", 0, 27}, {"P8_18", 2,  1},     {"P9_17", 0,  5}, {"P9_18", 0,  4},
  {"P8_19", 0, 22}, {"P8_20", 1, 31},     {"P9_19", 0, 13}, {"P9_20", 0, 12},
  {"P8_21", 1, 30}, {"P8_22", 1,  5},     {"P9_21", 0,  3}, {"P9_22", 0,  2},
  {"P8_23", 1,  4}, {"P8_24", 1,  1},     {"P9_23", 1, 17}, {"P9_24", 0, 15},
  {"P8_25", 1,  0}, {"P8_26", 1, 29},     {"P9_25", 3, 21}, {"P9_26", 0, 14},
  {"P8_27", 2, 22}, {"P8_28", 2, 24},     {"P9_27", 3, 19}, {"P9_28", 3, 17},
  {"P8_29", 2, 23}, {"P8_30", 2, 25},     {"P9_29", 3, 15}, {"P9_30", 3, 16},
  {"P8_31", 0, 10}, {"P8_32", 0, 11},     {"P9_31", 3, 14},
  {"P8_33", 0,  9}, {"P8_34", 2, 17},     
  {"P8_35", 0,  8}, {"P8_36", 2, 16},     
  {"P8_37", 2, 14}, {"P8_38", 2, 15},     
  {"P8_39", 2, 12}, {"P8_40", 2, 13},     
  {"P8_41", 2, 10}, {"P8_42", 2, 11},     {"P9_41", 0, 20}, {"P9_42", 0,  7},
  {"P8_43", 2,  8}, {"P8_44", 2,  9},     
  {"P8_45", 2,  6}, {"P8_46", 2,  7},     
  {NULL, 0, 0}
};

volatile unsigned int *gpio_setdataout_addr = NULL;
volatile unsigned int *gpio_cleardataout_addr = NULL;
unsigned int gpio_pin_mask;

// We are using the the dirty MMAP-based method for best performance
void gpio_setup(const char* pin_name)
{
  volatile void *gpio_addr;
  volatile unsigned int *gpio_oe_addr;

  int fd;

  struct pin_desc* pin = pins;
  while (pin->pin_name) {
    if (strcmp(pin->pin_name, pin_name) == 0) {
      break;
    }
    pin++;
  }
  if (!pin->pin_name) {
    printf("unknown pin: %s\n", pin_name);
    exit(-1);
  }

  fd = open("/dev/mem", O_RDWR);
  if (fd < 0) {
    perror("unable to open /dev/mem:");
    exit(-1);
  }

  gpio_addr = mmap(0, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                   fd, GPIO_BASE_ARRAY[pin->gpio_bank]);
  if (gpio_addr == MAP_FAILED) {
    perror("mmap failed:");
    exit(-1);
  }
  gpio_oe_addr = gpio_addr + GPIO_OE;
  gpio_setdataout_addr = gpio_addr + GPIO_SETDATAOUT;
  gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;

  gpio_pin_mask = (1 << pin->gpio_pos);

  *gpio_oe_addr &= (0xFFFFFFFF ^ gpio_pin_mask);
}

static inline void gpio_assert()
{
  *gpio_setdataout_addr = gpio_pin_mask;
}

static inline void gpio_deassert()
{
  *gpio_cleardataout_addr = gpio_pin_mask;
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

long ns_diff(const struct timespec *t1, const struct timespec *t2)
{
  return t1->tv_nsec - t2->tv_nsec + (t1->tv_sec - t2->tv_sec) * 1000000000L;
}

void sync_loop(unsigned int freq, unsigned int width)
{
  unsigned long period = NANOS_PER_SEC / freq;  // rounding errors should be ok

  while (1) {
    for (int period_cnt = 0; period_cnt < freq; period_cnt++) {
      struct timespec t, t_epoch_guard, t_epoch, t_asserted;
      unsigned long phase = period_cnt * period;

      if (clock_gettime(CLOCK_REALTIME, &t)) {
         perror("clock_gettime failed:");
         exit(-1);
      }

      t_epoch.tv_sec = t.tv_sec;
      t_epoch.tv_nsec = phase;
      if (t.tv_nsec > phase) {
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
      usleep(width);
      gpio_deassert();

      printf("%lld.%.9ld [%ld...%ld ns]\n",
               (long long)t.tv_sec, t.tv_nsec,
                ns_diff(&t, &t_epoch), ns_diff(&t_asserted, &t_epoch));
    }
  }
}

static int parse_opt(int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
  switch (key)
  {
    case 'f': 
      arguments->freq = atoi(arg);
      break;
    case 'w':
      arguments->width = atoi(arg);
      break;
    case 'p':
      arguments->pin_name = arg;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
} 

int main(int argc, char* argv[])
{
  struct arguments arguments = {.freq = 1, .width = 10, .pin_name = "P8_19"};

  const char* doc = "Generating a periodic sync signal on a selected GPIO pin";
  struct argp_option options[] = {
    { "freq", 'f', "HZ", 0, "Frequency of the pulse signal (default: 1)"},
    { "width", 'w', "US", 0, "Pulse width in microseconds (default: 10)"},
    { "pin", 'p', "PIN_NAME", 0, "Name of the BBB header pin (default: P8_19)"},
    { 0 }
  };
  struct argp argp = {options, parse_opt, NULL, doc};

  argp_parse (&argp, argc, argv, 0, 0, &arguments);
  
  printf("\nGenerating synch: freq = %d HZ, width = %d us, pin = %s\n\n", 
    arguments.freq, arguments.width, arguments.pin_name);

  gpio_setup(arguments.pin_name);
  scheduler_setup();

  sync_loop(arguments.freq, arguments.width);
}