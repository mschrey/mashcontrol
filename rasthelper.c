#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for strcpy
#include <time.h>
#include <signal.h>    //for SIGTERM and kill
#include <unistd.h>    //for fork and usleep
#include <wiringPi.h>  //for digitalwrite
#include "mashsteplist.h"
#include "rasthelper.h"
#include "temphelper.h"



char * FILEOUT;
const int BUZZER = 1;  //buzzer is connected to GPIO1 (wiringPi pin numbering)
const int HYSTERESIS = 2;
const int TIMEOUT_RAST_WAIT = 10;
const int TIMEOUT_RAST_HEATUP = 10;
const double Kp     = 3;
const double Kd     = 5;
const double memFac = 0.2;
double ePrev = 0;

int WAIT_REQUIRED = 0;


long get_time(char *charpointer, time_t starttime)
{ 
    time_t rawtime;
    struct tm * timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(charpointer, 80, "%F %T", timeinfo);
    
    long seconds_since_start = difftime(rawtime, starttime);
    return seconds_since_start;
}


void print_info(time_t starttime, struct listitem *currentRast, double currentTemp)
{
    char time_string [80];
    get_time(time_string, starttime);
    double RastTemperature = currentRast->temperature;
    char *RastAction = currentRast->action;
    printf("%s: %s, Soll: %5.2f°C, Ist: %5.2f°C, Heizung: %s\n", time_string, RastAction, RastTemperature, currentTemp, heaterStatus);
    FILE * fp = fopen(FILEOUT, "a");
    fprintf(fp, "%s: %s, %5.2f, %5.2f, %s\n", time_string, RastAction, RastTemperature, currentTemp, heaterStatus);
    fclose(fp);
}



/*
//this executes one control step, e.g. turns heater on or off
//input is desired temperature, output is actual temperature
double Rast_regulate(double RastTemperature)
{
    double currentTemp = get_temp(SENSOR4)/1000;
    //double currentTemp = get_temp(SENSOR1)/1000;
    if(currentTemp < (RastTemperature-HYSTERESIS))
        setHeizungStatus("ON");
    else if(currentTemp > RastTemperature)
        setHeizungStatus("OFF");
    return currentTemp;
}
*/

// Bang bang PD controller
// Executes one control step, returns current temperature
// Designed for a sample time of 11 seconds
double Rast_regulate( double RastTemperature )
{
    double y = get_temp(SENSOR4) / 1000;    // Get plant output
    double e = RastTemperature - y;         // Calculate control error
    double eFilt = e * (1 - memFac ) + ePrev * memFac;    // Calculate filtered control error
    double uVirt = e * Kp + Kd * ( e - eFilt );           // Calculate virtual plant input
    double uThreshold = -0.006667 * RastTemperature + 0.83336;   // Calculate adaptive activation threshold
    // Actuate!
    if( uVirt > uThreshold )
        setHeizungStatus("ON");
    else
        setHeizungStatus("OFF");

    ePrev = eFilt;      // Save filtered error
    return y;           // Return current temperature
}


void Rast_heatup(struct listitem *currentRast)
{
    double RastTemperature = currentRast->temperature;
    time_t starttime;
    time(&starttime);
    
    double currentTemp;
    do {
        currentTemp = Rast_regulate(RastTemperature);
        print_info(starttime, currentRast, currentTemp);
        usleep(TIMEOUT_RAST_HEATUP*1000000);    //wait TIMEOUT_RAST_HEATUP second
    } while(currentTemp < RastTemperature);
    setHeizungStatus("OFF");
}


void Rast_wait(struct listitem *currentRast)
{
    double RastTemperature = currentRast->temperature;
    long RastDuration = (long)currentRast->duration;
    time_t starttime;
    time(&starttime);
    
    double currentTemp;
    if (RastDuration == 0) {   //no fixed duration. wait for user interaction
        WAIT_REQUIRED = 1;
        pid_t pid = fork();
        if (pid == 0) {
            //child process
            while(1) {
                currentTemp = Rast_regulate(RastTemperature);
                print_info(starttime, currentRast, currentTemp);
                printf("Temperature reached, %s then press <Enter> to continue\n", currentRast->action);
                digitalWrite(BUZZER, 1);
                usleep(2 * 1000000);
                digitalWrite(BUZZER, 0);
                fflush(NULL);  //flush all open files
                fflush(stdout);
                usleep(TIMEOUT_RAST_WAIT * 1000000);    //wait TIMEOUT_RAST_WAIT seconds
            }
        } else {
            //parent process, pid contains child pid
            printf("waiting for SIGUSR1\n");
            while(WAIT_REQUIRED == 1) {}
            kill(pid, SIGTERM);
            digitalWrite(BUZZER, 0);
        }
    } else {
        char time_string [80];
        time(&starttime);  // get current time; same as: timer = time(NULL)  
        long currentRastDuration;
        do {
            currentTemp = Rast_regulate(RastTemperature);
            currentRastDuration = get_time(time_string, starttime)/60;
            print_info(starttime, currentRast, currentTemp);
            printf("Noch %2ld von %2ldmin\n", RastDuration-currentRastDuration, RastDuration);
            fflush(NULL);  //flush all open files
            fflush(stdout);
            usleep(TIMEOUT_RAST_WAIT*1000000);      //wait TIMEOUT_RAST_WAIT second
        } while(currentRastDuration < RastDuration);
    }
}
