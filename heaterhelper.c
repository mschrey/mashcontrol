#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heaterhelper.h"
#include "heaterAbstraction.h"
#include "heaterPWM.h"
#include "heaterRCSwitch.h"

char *heaterStatus = "OFF";


//struct heaterCtrl myHeaterCtrl = PWMHeaterCtrl;
struct heaterCtrl *myHeaterCtrl = &RCSwitchHeaterCtrl;


void setHeizungStatus(const char * status)
{
    if(strcmp(status, "ON") == 0) {
        //system(COMMAND_ON);
        myHeaterCtrl->setHeaterState(1);
        strcpy(heaterStatus, " ON");
    } else {
        system(COMMAND_OFF);
        strcpy(heaterStatus, "OFF");
    }
}


