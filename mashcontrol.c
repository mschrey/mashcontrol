// **********************************************
// *                                            *
// *               Mashcontrol                  *
// *                                            *
// **********************************************
// 2017-08-17
// Version 0.1: Initial
// 2017-08-27
// Version 0.2: mashcontrol forks while waiting for user input
//              automatic logging
//              reduced duplicate control code
// 2017-11-26
// Version 0.3: included buzzer support to request user interaction
//              cleanup function for graceful exiting on ctrl-C
// 2018-03-30
// Version 0.4: improved control loop algorithm for low air temperatures
//              (mash would not reach 78°C)
// 2018-04-14
// Version 0.5: introduced mash step file (.msf)
//              file name requested as argument (without extension) is input file name
//              for mash step definition (*.msf) and output log file (*.log)

// compile with wiringPi (gcc mashcontrol.c -o mashcontrol -lwiringPi)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for strcpy
#include <time.h>
//#include <termios.h> //for stdin_set
#include <sys/types.h>
#include <signal.h>    //for SIGTERM
#include <unistd.h>    //for fork and usleep
#include <wiringPi.h>  //for digitalwrite
#include "mashsteplist.h"
#include "rasthelper.h"
#include "temphelper.h"
#include "heaterhelper.h"




char * parse_args(int argc, char *argv[], char *fileout)
{
    if (argc != 2) {
        printf("Error! Too few arguments! Remember to pass desired output logfile name!\n");
        exit(-1);
    } else {
        strcpy(fileout, argv[1]);
    }
    return fileout;
}


void cleanup(int a) {
    printf("CTRL-C caught, exiting...\n");
    fflush(NULL);  //flush all open files
    setHeizungStatus("OFF");  //turn off heater
    digitalWrite(BUZZER, 0);  //turn off buzzer
    exit(0);
}



void cont_mash_step(int b)
{
    printf("caught SIGUSR1, continuing mash steps...\n");
    WAIT_REQUIRED = 0;
}


int main(int argc, char *argv[]) 
{
    
    //Setup interrupt handler to catch strg+C
    struct sigaction act;
    act.sa_handler = cleanup;
    sigaction(SIGINT, &act, NULL);
    
    //setup interrupt handler to continue after user interaction
    struct sigaction act2;
    act2.sa_handler = cont_mash_step;
    sigaction(SIGUSR1, &act2, NULL);
    
    //init gpio
    if (wiringPiSetup() == -1)
       printf("failed to setup!\n");
    pinMode(BUZZER, OUTPUT);
    
    //defining filebase
    char * filebase = (char*)malloc(100);
    filebase = parse_args(argc, argv, filebase);

    //defining FILEOUT
    FILEOUT = (char*)malloc(100);
    strcpy(FILEOUT, filebase);
    strcat(FILEOUT, ".log");
    printf("output file name is %s\n", FILEOUT);

    //defining mash step file
    char * mash_step_file = (char*)malloc(100);
    strcpy(mash_step_file, filebase);
    strcat(mash_step_file, ".msf");
    printf("mash step file is %s\n", mash_step_file);

    FILE * fp;
    fp = fopen(FILEOUT, "w");
    fprintf(fp, "Date/Time: Rast, Soll (°C), Ist (°C), Heizung?\n");


    struct listitem * head = NULL; 

//    head = create(60, 0, "Einmaischen");
//    push(head, 57, 10, "Eiweissrast");
//    push(head, 63, 45, "Maltoserast");
//    push(head, 73, 20, "Verzuckerungsrast");
//    push(head, 78,  0, "Abmaischen");



    head = parse_msf_recipe(mash_step_file, head);
    printRastList(head);

    printf("\n\nstarting mash Process\n");

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
