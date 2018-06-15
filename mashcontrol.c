// **********************************************
// *                                            *
// *            Maischcontroller                *
// *                                            *
// **********************************************
// 2017-08-17
// Version 0.1: Initial
// 2017-08-27
// Version 0.2: maischcontroller forks while waiting for user input
//              automatic logging
//              reduced duplicate control code
// 2017-11-26
// Version 0.3: included buzzer support to request user interaction
//              cleanup function for graceful exiting on ctrl-C
// 2018-03-30
// Version 0.4: improved control loop algorithm for low air temperatures
//              (maische would not reach 78°C)

// compile with wiringPi (gcc mashcontrol.c -o mashcontrol -lwiringPi)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for strcpy
#include <time.h>
//#include <termios.h> //for stdin_set
#include <sys/types.h>
#include <signal.h>    //for SIGTERM
#include <wiringPi.h>  //for digitalwrite

#define BUZZER 1  //buzzer is connected to GPIO1 (wiringPi pin numbering)

const char SENSOR1[50]     = "/sys/bus/w1/devices/10-000802f7cdae/w1_slave";
const char SENSOR2[50]     = "/sys/bus/w1/devices/10-000802f89e49/w1_slave";
const char SENSOR3[50]     = "/sys/bus/w1/devices/10-0008032e3d80/w1_slave";
const char SENSOR4[50]     = "/sys/bus/w1/devices/28-0316a4a4eaff/w1_slave";  //watertight sensor

const char COMMAND_ON[100]  = "/home/pi/raspberry-remote/send 11001 1 1 >> /home/pi/brewcontrol_raspberry-remote_output.log";  //outlet A
const char COMMAND_OFF[100] = "/home/pi/raspberry-remote/send 11001 1 0 >> /home/pi/brewcontrol_raspberry-remote_output.log";  //outlet A

char * FILEOUT;

char heaterStatus[] = "OFF";
const int HYSTERESIS = 2;

const int TIMEOUT_RAST_WAIT = 10;
const int TIMEOUT_RAST_HEATUP = 10;

// Definitions for controller
const double Kp     = 3;
const double Kd     = 5;
const double memFac = 0.2;

// Filter buffer for D-controller
double ePrev = 0;


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


void print_info(time_t starttime, struct listitem *currentRast, double currentTemp)
{
    char time_string [80];
    get_time(time_string, starttime);
    double RastTemperature = currentRast->temperature;
    long RastDuration = (long)currentRast->duration;
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
    // Get plant output
    double y = get_temp( SENSOR4 ) / 1000;

    // Calculate control error
    double e = RastTemperature - y;

    // Calculate filtered control error
    double eFilt = e * (1 - memFac ) + ePrev * memFac;

    // Calculate virtual plant input
    double uVirt = e * Kp + Kd * ( e - eFilt );

    // Calculate adaptive activation threshold
    double uThreshold = -0.006667 * RastTemperature + 0.83336;

    // Actuate!
    if( uVirt > uThreshold )
        setHeizungStatus("ON");
    else
        setHeizungStatus("OFF");

    // Save filtered error
    ePrev = eFilt;

    // Return current temperature
    return y;
}



void Rast_heatup(struct listitem *currentRast)
{
    double RastTemperature = currentRast->temperature;
    long RastDuration = (long)currentRast->duration;
    time_t starttime;

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

    double currentTemp;
    if (RastDuration == 0) {   //no fixed duration. wait for user interaction
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
                usleep(TIMEOUT_RAST_WAIT * 1000000);    //wait TIMEOUT_RAST_WAIT seconds
            }
        } else {
            //parent process, pid contains child pid
            char str[20];
            fgets(str, 19, stdin);
            kill(pid, SIGTERM);
        }
    } else {
        char time_string [80];
        time(&starttime);  // get current time; same as: timer = time(NULL)  
        long currentRastDuration;
        do {
            currentTemp = Rast_regulate(RastTemperature);
            currentRastDuration = get_time(time_string, starttime)/60;
            print_info(starttime, currentRast, currentTemp);
            printf("Noch %2d von %2dmin\n", RastDuration-currentRastDuration, RastDuration);
            usleep(TIMEOUT_RAST_WAIT*1000000);      //wait TIMEOUT_RAST_WAIT second
        } while(currentRastDuration < RastDuration);
    }
}


char * parse_args(int argc, char *argv[], char *fileout)
{
    if (argc != 2) {
        printf("Error! Too few arguments! Remember to pass desired output logfile name!\n");
        exit(-1);
    } else {
        fileout = (char*)malloc(strlen(argv[1]));
        strcpy(fileout, argv[1]);
    }
    return fileout;
}


void cleanup(int a) {
    printf("CTRL-C caught, exiting...\n");
    fflush(NULL);  //flush all open files
    setHeizungStatus("OFF");  //turn off header
    digitalWrite(BUZZER, 0);  //turn off buzzer
    exit(0);
}


int main(int argc, char *argv[]) 
{
    time_t starttime;
    char time_string [80];
    char str[10] = "";
    
    //Setup interrupt handler to catch strg+C
    struct sigaction act;
    act.sa_handler = cleanup;
    sigaction(SIGINT, &act, NULL);
    
    //init gpio
    if (wiringPiSetup() == -1)
       printf("failed to setup!\n");
    pinMode(BUZZER, OUTPUT);
    
    FILEOUT = parse_args(argc, argv, FILEOUT);
    printf("output file name is %s\n", FILEOUT);
    FILE * fp;
    fp = fopen(FILEOUT, "w");
    fprintf(fp, "Date/Time: Rast, Soll (°C), Ist (°C), Heizung?\n");



    struct listitem * head = NULL; 

    head = create(60, 0, "Einmaischen");
    push(head, 57, 10, "Eiweissrast");
    push(head, 63, 45, "Maltoserast");
    push(head, 73, 20, "Verzuckerungsrast");
    push(head, 78,  0, "Abmaischen");
    printlist(head);

    printf("\n\nstarting Maische Process\n");

    struct listitem *currentRast = head;
    while(currentRast != NULL) {
        printRast(currentRast);

        Rast_heatup(currentRast);

        Rast_wait(currentRast);

        currentRast = currentRast->next;
    }
    
    // Heater off
    setHeizungStatus("OFF");
    
}
