#ifndef MASHSTEPLIST
#define MASHSTEPLIST

struct listitem {
    double temperature;
    int duration;
    char * action;
    struct listitem * next;
};

struct listitem * mashsteplist_create(double temperature, int duration, char * action);

void mashsteplist_push(struct listitem *head, double temperature, int duration, char * action);

void printRast(struct listitem *currentRast);

void printRastList(struct listitem * head);

struct listitem * parse_msf_recipe(char * filename, struct listitem * head);

#endif
