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
#include <unistd.h>
#include <string.h>  //for strcpy, strlen
#include <time.h>
#include <wiringPi.h>  //for digitalwrite
#include <signal.h>    //for SIGTERM and kill
#include <sys/types.h>
#include "parse_json.h"
#include "mashsteplist.h"
#include "parse_json.h"
#include "rasthelper.h"
#include "temphelper.h"
#include "heaterhelper.h"
#include "onewirediscover.h"




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



int exists(const char *fname)
{
    if (access(fname, F_OK) != -1) {
        return 1;  // file exists
    } else {
        return 0;  // file doesn't exist
    }
}



//checks and possibly modifies given filenames, if they exist already.
char* check_filename(const char *fname)
{
    char* filename = malloc(strlen(fname)+5);
    char buffer[10];
    int counter = 0;
    if(exists(fname)) {
        strcpy(filename, fname);
        strcat(filename, "_");
        sprintf(buffer, "%03d", counter);
        strcat(filename, buffer);
        while(exists(filename)) {
            counter++;
            strcpy(filename, fname);
            strcat(filename, "_");
            sprintf(buffer, "%03d", counter);
            strcat(filename, buffer);
        }
    } else {
        strcpy(filename, fname);
    }
    printf("new filename is '%s'\n", filename);
    return filename;
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
    FILEOUT = check_filename(FILEOUT);
    printf("output file name is %s\n", FILEOUT);

    //defining mash step file
    char * mash_step_file = (char*)malloc(100);
    strcpy(mash_step_file, filebase);
    strcat(mash_step_file, ".msf");
    printf("mash step file is %s\n", mash_step_file);




    while(0) {
        //Variante 1: pass json file name
        //            below filebase.json is parsed for recipe data using 'parse_json'
        //            recipe data is contained in 'new_json_recipe'
        //            parsed recipe data can be 
        //             - written to msf file
        //             - passed back to mashcontrol as struct listitem list
        //             - or both (e.g. for documentation purposes)
        char * json_file = (char*)malloc(100);
        strcpy(json_file, filebase);
        strcat(json_file, ".json");
        printf("json recipe file  is %s\n", json_file);

        struct listitem *new_json_recipe = NULL;
        new_json_recipe = parse_json_recipe(json_file, 2);
        printf("prining json-generated mash step list from mashcontrol:\n");
        printRastList(new_json_recipe);
        exit(0);
    }



    //Variante 2: pass msf file name
    //  msf file is parsed for recipe data using 'read_msf_file'
    struct listitem * head = NULL;
    head = parse_msf_recipe(mash_step_file, head); //read_msf_file(mash_step_file, head);



    FILE * fp;
    fp = fopen(FILEOUT, "w");
    fprintf(fp, "Date/Time: Rast, Soll (°C), Ist (°C), Heizung?\n");


//    head = create(60, 0, "Einmaischen");
//    push(head, 57, 10, "Eiweissrast");
//    push(head, 63, 45, "Maltoserast");
//    push(head, 73, 20, "Verzuckerungsrast");
//    push(head, 78,  0, "Abmaischen");

    printRastList(head);

    printf("\n\nstarting mash Process\n");

    struct listitem *currentRast = head;
    setHeizungStatus("OFF");
    while(currentRast != NULL) {
        printRast(currentRast);

        Rast_heatup(currentRast);

        Rast_wait(currentRast);

        currentRast = currentRast->next;
    }
    
    
    cleanup(0);
}
