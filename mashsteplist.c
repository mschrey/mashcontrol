#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mashsteplist.h"




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


void printRastList(struct listitem * head)
{
    struct listitem *current = head;
    while(current != NULL) {
        printRast(current);
        current = current->next;
    }

}


struct listitem * parse_msf_recipe(char * filename, struct listitem * head)
{
    printf("reading from %s\n", filename);
    FILE * filep = fopen(filename, "r");
    char rastline[100];
    char rast_name[30];
    char * pch;
    int rast_temp = 0;
    int rast_duration = 0;
    while(fgets(rastline, 100, filep) != NULL) {
        if(rastline[0] != '#') {
            pch = strtok(rastline, ", ");
            strcpy(rast_name, pch);
            pch = strtok(NULL, ", ");
            rast_temp = atoi(pch);
            pch = strtok(NULL, ", ");
            rast_duration = atoi(pch);
            printf("rast found -> name:%s, temperature:%d°C, duration:%dmin\n", rast_name, rast_temp, rast_duration);
            if(head == NULL)
                head = create(rast_temp, rast_duration, rast_name);
            else
                push(head, rast_temp, rast_duration, rast_name);
        } else {
            printf("comment found\n");
        }
    }
    return head;
}