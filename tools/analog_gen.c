/*
  analog-gen - generate an analog signal using PWM

  Needs to enable EHRPWM1A: echo BB-PWM1 >  /sys/devices/platform/bone_capemgr/slots
  Need RC filter between P9.14 and GND:

    ______       ___________                     _______
    P9.14 \______| 1k Ohm   |____| 2.2 uF |_____/ GND
    ______/      |__________|    |        |     \_______

  Copyright 2017, Peter Volgyesi, Vanderbilt University
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include <time.h>
#include <math.h>
#include <libsoc_pwm.h>
#include <libsoc_debug.h>

#define SAMPLING_RATE 1000 // Hz
#define SIGNAL_FREQ 60 // Hz
#define PWM_PERIOD 4000 // nanoseconds


#define SAMPLING_INTERVAL (1000000000L / SAMPLING_RATE) // nanoseconds
#define DPHASE (2 * M_PI * SIGNAL_FREQ / SAMPLING_RATE)

// ANALOG (PWM) OUTPUT: P9_14 = (EHRPWM1A)
#define PWM_OUTPUT_CHIP 0
#define PWM_CHIP_OUTPUT 0


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
  double phase = 0.0;
  struct timespec t_wait = {0L, SAMPLING_INTERVAL};
  pwm *pwm_output;

  // libsoc_set_debug(1);

  scheduler_setup();

  pwm_output = libsoc_pwm_request(PWM_OUTPUT_CHIP, PWM_CHIP_OUTPUT, LS_SHARED);
  if (!pwm_output) {
    perror("unable to request PWM pin:");
    fprintf(stderr, "make sure, you enabled the PWM overlay:\n\techo BB-PWM1 >  /sys/devices/platform/bone_capemgr/slots\n");
    exit(-1);
  }

  libsoc_pwm_set_enabled(pwm_output, ENABLED);
  if (!libsoc_pwm_get_enabled(pwm_output))
  {
    perror("unable to enable PWM pin:");
    exit(-1);
  }


  libsoc_pwm_set_polarity(pwm_output, NORMAL);
  if (libsoc_pwm_get_polarity(pwm_output) != NORMAL)
  {
    perror("unable to set PWM polarity:");
    exit(-1);
  }

  libsoc_pwm_set_period(pwm_output, PWM_PERIOD);
  if (libsoc_pwm_get_period(pwm_output) != PWM_PERIOD)
  {
    perror("unable to set PWM period:");
    exit(-1);
  }


  while (1) {
    libsoc_pwm_set_duty_cycle(pwm_output, PWM_PERIOD * (1.0 + sin(phase)) / 2.0 );
    phase += DPHASE;

    if (clock_nanosleep(CLOCK_REALTIME, 0, &t_wait, NULL)) {
      perror("clock_nanosleep failed:");
      exit(-1);
    }

    // if (clock_gettime(CLOCK_REALTIME, &t)) {
    //    perror("clock_gettime failed:");
    //    exit(-1);
    // }

    // printf("%lld.%.9ld [%ld...%ld ns]\n",
    //          (long long)t.tv_sec, t.tv_nsec,
    //           ns_diff(&t, &t_epoch), ns_diff(&t_asserted, &t_epoch));
  }
}
