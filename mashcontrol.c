// **********************************************
// *                                            *
// *            Maischcontroller                *
// *                                            *
// **********************************************
// 2017-08-17
// Version 0.1: Initial

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for strcpy
#include <time.h>
#include <termios.h> //for stdin_set

const char SENSOR1[50]     = "/sys/bus/w1/devices/10-000802f7cdae/w1_slave";
const char SENSOR2[50]     = "/sys/bus/w1/devices/10-000802f89e49/w1_slave";
const char SENSOR3[50]     = "/sys/bus/w1/devices/10-0008032e3d80/w1_slave";
const char SENSOR4[50]     = "/sys/bus/w1/devices/28-0316a4a4eaff/w1_slave";  //watertight sensor

const char COMMAND_ON[100]  = "/home/pi/raspberry-remote/send 11001 1 1 >> /home/pi/brewcontrol_raspberry-remote_output.log";  //outlet A
const char COMMAND_OFF[100] = "/home/pi/raspberry-remote/send 11001 1 0 >> /home/pi/brewcontrol_raspberry-remote_output.log";  //outlet A



char heaterStatus[] = "OFF";
const int HYSTERESIS = 2;

const int TIMEOUT_RAST_WAIT = 10;
const int TIMEOUT_RAST_HEATUP = 10;

struct listitem {
    double temperature;
    int duration;
    char * action;
    struct listitem * next;
};


struct listitem * create(double temperature, int duration, char * action)
{
    struct listitem * head = malloc(sizeof(struct listitem));
    head->temperature = temperature;
    head->duration = duration;
    head->action = malloc(strlen(action)+1);
    strcpy(head->action, action);
    head->next = NULL;
    return head;
}


void push(struct listitem *head, double temperature, int duration, char * action)
{
    struct listitem *new = malloc(sizeof(struct listitem));
    new->temperature = temperature;
    new->duration = duration;
    new->action = malloc(strlen(action)+1);
    strcpy(new->action, action);
    new->next = NULL;
    //iterate to end of list
    struct listitem *current = head;
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = new;
}


void printRast(struct listitem *currentRast)
{
    printf("========new Rast==============\n");
    printf("Temperature: %2.0f°C\n", currentRast->temperature);
    printf("Duration: %dmin\n", currentRast->duration);
    printf("Action: %s\n", currentRast->action);
    printf("==============================\n");
}


void printlist(struct listitem * head)
{
    struct listitem *current = head;
    while(current != NULL) {
        printRast(current);
        current = current->next;
    }

}


double get_temp(const char * sensor)
{
    char temp_str1[20], temp_str2[10];
    char str_long[100];
    int temp_millicelsius;
    FILE * fh;
    fh = fopen(sensor, "r");
    if (fh != NULL) {
        fgets(str_long, 100, fh); //read first line
        fscanf(fh, "%*2x %*2x %*2x %*2x %*2x %*2x %*2x %*2x %*2x %s\n", temp_str1);
        fclose(fh);
        sprintf(temp_str2, temp_str1+2, 5);
        
        temp_millicelsius = atoi(temp_str2);
        //printf("temp    : %5.2f°C\n", (double)temp_millicelsius/1000);
        return temp_millicelsius;
    } else {
        printf("sensor %s not found\n", sensor);
        return -1000;
    }

}


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


//from https://stackoverflow.com/questions/448944/c-non-blocking-keyboard-input
void stdin_set(int cmd)
{
    struct termios t;
    tcgetattr(1,&t);
    switch (cmd) {
    case 1:
            t.c_lflag &= ~ICANON;
            break;
    default:
            t.c_lflag |= ICANON;
            break;
    }
    tcsetattr(1,0,&t);
}


void Rast_heatup(struct listitem *currentRast)
{
    double RastTemperature = currentRast->temperature;
    long RastDuration = (long)currentRast->duration;
    char time_string [80];
    time_t starttime;

    double currentTemp = get_temp(SENSOR4)/1000;
    while(currentTemp < RastTemperature) {
        setHeizungStatus("ON");
        get_time(time_string, starttime);
        printf("%s: Hochheizen, Soll: %5.2f°C, Ist: %5.2f°C\n", time_string, RastTemperature, currentTemp);
        usleep(TIMEOUT_RAST_HEATUP*1000000);    //wait TIMEOUT_RAST_HEATUP second
        currentTemp = get_temp(SENSOR4)/1000;
    }
    setHeizungStatus("OFF");
}


void Rast_wait(struct listitem *currentRast)
{
    double RastTemperature = currentRast->temperature;
    long RastDuration = (long)currentRast->duration;
    char time_string [80];
    time_t starttime;

    double currentTemp = get_temp(SENSOR4)/1000;
    if (RastDuration == 0) {   //no fixed duration. wait for user interaction
        printf("Temperature reached. %s, then press <Enter> to continue\n", currentRast->action);

        fd_set readfds;
        struct timeval tv;
        FD_ZERO(&readfds);
        FD_SET(0, &readfds); /* set the stdin in the set of file descriptors to be selected */
        tv.tv_sec = 0;
        tv.tv_usec = 500000;
        stdin_set(1);

        while(1)
        {
            if(currentTemp < (RastTemperature-HYSTERESIS))
                setHeizungStatus("ON");
            else if(currentTemp > RastTemperature)
                setHeizungStatus("OFF");
            currentTemp = get_temp(SENSOR4)/1000;
            printf("Soll: %5.2f, Ist: %5.2f, Heizung=%s\n", heaterStatus, RastTemperature, currentTemp);
            printf("Temperature reached. %s, then press <Enter> to continue\n", currentRast->action);

            FD_ZERO(&readfds);
            FD_SET(0, &readfds);
            tv.tv_sec = 0;
            tv.tv_usec = 1000000;            
            int retval = select(1, &readfds, NULL, NULL, &tv);
            if (retval) 
                break;
        }
        stdin_set(0);
    } else {
        time(&starttime);  // get current time; same as: timer = time(NULL)  
        long currentRastDuration = get_time(time_string, starttime)/60;
        //printf("currentRastDuration: %ld\n", currentRastDuration);
        //printf("get_time(): %ld\n", get_time(time_string, starttime));
        //printf("get_time()/60: %ld\n", get_time(time_string, starttime)/60);
        currentTemp = get_temp(SENSOR4)/1000;
        while(currentRastDuration < RastDuration) {
            if(currentTemp < (RastTemperature-HYSTERESIS))
                setHeizungStatus("ON");
            else if(currentTemp > RastTemperature)
                setHeizungStatus("OFF");
            printf("%s: Rast, Noch %2d von %2dmin, Heizung=%s, Soll: %5.2f°C, Ist: %5.2f°C\n", time_string, RastDuration-currentRastDuration, RastDuration, heaterStatus, RastTemperature, currentTemp);
            usleep(TIMEOUT_RAST_WAIT*1000000);      //wait TIMEOUT_RAST_WAIT second
            currentRastDuration = get_time(time_string, starttime)/60;
            currentTemp = get_temp(SENSOR4)/1000;
        }
    }
}


int main(void) 
{
    time_t starttime;
    char time_string [80];
    char str[10] = "";

    struct listitem * head = NULL; 

    //globaltemp = 20;
    
    head = create(55, 0, "Einmaischen");
    push(head, 52, 10, "Eiweissrast");
    push(head, 63, 40, "Maltoserast");
    push(head, 72, 10, "Verzuckerungsrast");
    push(head, 74,  0, "Abmaischen");
    printlist(head);

    printf("\n\nstarting Maische Process\n");

    struct listitem *currentRast = head;
    while(currentRast != NULL) {
        printRast(currentRast);

        Rast_heatup(currentRast);

        Rast_wait(currentRast);

        currentRast = currentRast->next;
    }
}
