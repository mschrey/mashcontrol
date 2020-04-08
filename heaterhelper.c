#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "heaterhelper.h"


char *heaterStatus;

const char *COMMAND_ON  = "/home/pi/raspberry-remote/send 11001 1 1 >> /dev/null";  //outlet A
const char *COMMAND_OFF = "/home/pi/raspberry-remote/send 11001 1 0 >> /dev/null";  //outlet A



void setHeizungStatus(const char * status)
{
    heaterStatus = (char*)malloc(20);
    if(strcmp(status, "ON") == 0) {
        system(COMMAND_ON);
        strcpy(heaterStatus, " ON");
    } else {
        system(COMMAND_OFF);
        strcpy(heaterStatus, "OFF");
    }
}

