#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "heaterhelper.h"


char *heaterStatus = "OFF";

const char *COMMAND_ON  = "/home/pi/raspberry-remote/send 11001 1 1 >> /home/pi/brewcontrol_raspberry-remote_output.log";  //outlet A
const char *COMMAND_OFF = "/home/pi/raspberry-remote/send 11001 1 0 >> /home/pi/brewcontrol_raspberry-remote_output.log";  //outlet A



void setHeizungStatus(const char * status)
{
    if(strcmp(status, "ON") == 0) {
        system(COMMAND_ON);
        strcpy(heaterStatus, " ON");
    } else {
        system(COMMAND_OFF);
        strcpy(heaterStatus, "OFF");
    }
}

