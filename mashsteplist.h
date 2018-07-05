struct listitem {
    double temperature;
    int duration;
    char * action;
    struct listitem * next;
};

struct listitem * create(double temperature, int duration, char * action);

void push(struct listitem *head, double temperature, int duration, char * action);

void printRast(struct listitem *currentRast);

void printlist(struct listitem * head);
