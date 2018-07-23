#include <stdio.h>
#include <stdlib.h>
#include "heaterAbstraction.h"

extern const char *COMMAND_ON;
extern const char *COMMAND_OFF;



extern struct heaterCtrl RCSwitchHeaterCtrl;

void RCSwitchSet(double command);

double RCSwitchGet();

