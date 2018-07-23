#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>

#include "heaterAbstraction.h"


struct heaterCtrl PWMHeaterCtrl;

void PWMSet(double command);

double PWMGet();