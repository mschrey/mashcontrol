#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for strcpy
#include <time.h>
#include "temphelper.h"


const char *SENSOR1 = "/sys/bus/w1/devices/10-000802f7cdae/w1_slave";
const char *SENSOR2 = "/sys/bus/w1/devices/10-000802f89e49/w1_slave";
const char *SENSOR3 = "/sys/bus/w1/devices/10-0008032e3d80/w1_slave";
const char *SENSOR4 = "/sys/bus/w1/devices/28-0316a4a4eaff/w1_slave";  //watertight sensor


double get_temp(const char * sensor)
{
    char temp_str1[20], temp_str2[10];
    char str_long[100];
    static int temp_millicelsius = 0;
    FILE * fh;
    fh = fopen(sensor, "r");
    if (fh != NULL) {
        fgets(str_long, 100, fh); //read first line
        fscanf(fh, "%*2x %*2x %*2x %*2x %*2x %*2x %*2x %*2x %*2x %s\n", temp_str1);
        fclose(fh);
        sprintf(temp_str2, temp_str1+2, 5);
        
        //temp_millicelsius = atoi(temp_str2);
    } else {
        printf("sensor %s not found\n", sensor);
        temp_millicelsius = -1000;
    }
    
    //simulate heating for testing purposes
    if(strcmp(heaterStatus, " ON") == 0)
        temp_millicelsius += 6000;
    else
        temp_millicelsius -= 3000;
    return temp_millicelsius;
}
