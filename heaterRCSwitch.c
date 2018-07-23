#include <stdio.h>
#include <stdlib.h>
#include "heaterRCSwitch.h"


const char *COMMAND_ON  = "/home/pi/raspberry-remote/send 11001 1 1 >> /home/pi/brewcontrol_raspberry-remote_output.log";  //outlet A
const char *COMMAND_OFF = "/home/pi/raspberry-remote/send 11001 1 0 >> /home/pi/brewcontrol_raspberry-remote_output.log";  //outlet A



void RCSwitchSet(double command)
{
    if (command > 0.5) {
        system(COMMAND_ON);
        RCSwitchHeaterCtrl.status = 1;

    } else {
        system(COMMAND_ON);
        RCSwitchHeaterCtrl.status = 0;
    }
}


double RCSwitchGet()
{
    return (double)RCSwitchHeaterCtrl.status;
}


struct heaterCtrl RCSwitchHeaterCtrl = {
    .status = 0,
    .setHeaterState = RCSwitchSet,
    .getHeaterState = RCSwitchGet,
};