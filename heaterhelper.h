#ifndef HEATERHELPER
#define HEATERHELPER

#include "heaterhelper.h"

extern char *heaterStatus;

extern const char *COMMAND_ON ;  //outlet A
extern const char *COMMAND_OFF;  //outlet A



void setHeizungStatus(const char * status);

#endif