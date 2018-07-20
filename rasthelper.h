#ifndef RASTHELPER
#define RASTHELPER

#include "heaterhelper.h"
#include "mashsteplist.h"

extern char * FILEOUT;
extern const int BUZZER;
extern const int HYSTERESIS;
extern const int TIMEOUT_RAST_WAIT;
extern const int TIMEOUT_RAST_HEATUP;
extern const double Kp;
extern const double Kd;
extern const double memFac;
extern double ePrev;

int WAIT_REQUIRED;

long get_time(char *charpointer, time_t starttime);

void print_info(time_t starttime, struct listitem *currentRast, double currentTemp);

double Rast_regulate(double RastTemperature);

void Rast_heatup(struct listitem *currentRast);

void Rast_wait(struct listitem *currentRast);

#endif