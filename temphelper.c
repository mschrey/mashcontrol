#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for strcpy
#include <time.h>
#include "temphelper.h"
#include "onewirediscover.h"


// 'mashcontrol' needs only a single sensor
//if only a single sensor is found, use that one
//if several sensors are found, check label list for label 'watertight'
//
// This function can be run multiple times without problems: On its first run, it 
// determines the corrent temp sensor. On all subsequent runs, it will only return
// the content of its internal temp sensor memory
char * tempsensor_init() 
{
    static char *SENSOR;
    if(SENSOR == NULL) {
        //initialize temp sensors
        struct tempsensorlist *mytempsensorlist = NULL;
        mytempsensorlist = get_temp_sensor(mytempsensorlist, "/sys/bus/w1/devices");   
        if(mytempsensorlist == NULL) {
            printf("Error! No temp sensor found\n");
            exit(-1);
        }    
        SENSOR = calloc(1, 80);
        //check if we discovered only one temp sensor
        if(onewirediscover_count(mytempsensorlist) == 1) {
            strcpy(SENSOR, mytempsensorlist->tempsensor);
        } else {
            //check if we can find a label "watertight" in list
            get_temp_sensor_labels(mytempsensorlist);  
            struct tempsensorlist *current = mytempsensorlist;
            while(current != NULL) {
                if(strcmp(current->label, "watertight") == 0) {
                    strcpy(SENSOR, current->tempsensor);
                }
                current = current->next;
            }
            if(strlen(SENSOR) == 0) {
                printf("no suitable sensors found!\n");
                exit(-1);
            }
        }
    }
    return SENSOR;
}


