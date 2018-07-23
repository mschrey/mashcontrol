#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   //for getuid
#include <wiringPi.h>
#include <softPwm.h>
#include "pwm.h"

//compile linking against wiringPi and pthread (gcc pwm.c -o pwm -lwiringPi -lpthread)


int setPWM(int softPWMvalue)
{
    int pinSoftPWM = 0;   //remember that these are wiringPi pin numbers, see http://wiringpi.com/pins/ for details
    int retval;
    
    retval = wiringPiSetup();
    if (retval)
        printf("error setting up wiringpi!\n");

// ================================
// ====  notes using softpwm   ====
// ================================
//
// * intermittent missing pulses, no periodicy in pulses noticeable
// * pwmrange =   10 -> 840Hz, 6% cpu usage, high jitter
// * pwmrange =  100 -> 100Hz, 1% cpu usage
// * pwmrange = 1000 -> 10Hz, no noticeable cpu usage
// softPWMvalue = 0 and = 100 result in true 0% and 100% duty cycle
// PWM signal is only present on specified pin during program run time. As soon as
// the executable is exited, the signal disappears. 

    const int softPWMrange = 10000;

    if(softPWMvalue > softPWMrange) {
        printf("Error! PWM value is greater than PWM range! Exiting!\n");
        exit(-1);
    }
    
    printf("starting softPWM with range %d and value %d\n", softPWMrange, softPWMvalue);
    retval = softPwmCreate(pinSoftPWM, 0, softPWMrange);
    if (retval)
        printf("error creating soft PWM!\n");

    softPwmWrite(pinSoftPWM, softPWMvalue);    
    return 0;
}






