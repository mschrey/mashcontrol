#ifndef HEATERHELPER
#define HEATERHELPER

#include "heaterhelper.h"

extern char *heaterStatus;


struct heaterCtrl {
    double status;
    void (*setHeaterState)(double);
    double (*getHeaterState)(void);
};


void setHeizungStatus(const char * status);

#endif