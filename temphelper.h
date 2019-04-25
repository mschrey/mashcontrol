#ifndef TEMPHELPER
#define TEMPHELPER

#include "heaterhelper.h"

extern const char *SENSOR1;
extern const char *SENSOR2;
extern const char *SENSOR3;
extern const char *SENSOR4;

double get_temp(const char * sensor);

#endif
