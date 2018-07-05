#include <stdio.h>
#include <stdlib.h>
#include "mashsteplist.h"

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