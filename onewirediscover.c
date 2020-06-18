#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "onewirediscover.h"



struct tempsensorlist * onewirediscover_create(char *path, char *tempsensor)
{
    struct tempsensorlist * head = malloc(sizeof(struct tempsensorlist));
    head->tempsensor = malloc(strlen(tempsensor) + strlen(path) + 13);
    strcpy(head->tempsensor, path);
    strcat(head->tempsensor, "/");
    strcat(head->tempsensor, tempsensor);
    strcat(head->tempsensor, "/w1_slave");
    head->next = NULL;
    return head;
}


void onewirediscover_push(struct tempsensorlist *head, char *path, char *tempsensor)
{
    struct tempsensorlist *new = malloc(sizeof(struct tempsensorlist));
    new->tempsensor = malloc(strlen(tempsensor) + strlen(path) + 13);
    strcpy(new->tempsensor, path);
    strcat(new->tempsensor, "/");
    strcat(new->tempsensor, tempsensor);
    strcat(new->tempsensor, "/w1_slave");
    new->next = NULL;
    //iterate to end of list
    struct tempsensorlist *current = head;
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = new;
}

//counts list elements of passed list pointer
int onewirediscover_count(struct tempsensorlist *head)
{
    struct tempsensorlist *current = head;
    int i = 0;
    while(current != NULL) {
        i++;
        current = current->next;
    }
    return i;
}

void print_tempsensorlist(struct tempsensorlist *head)
{
    struct tempsensorlist *current = head;
    printf("printing list of tempsensors and corresponding labels:\n");
    while(current != NULL) {
        printf("%s:\t%s\t temp is %5.2f°C\n", current->tempsensor, current->label, (double)current->temp_mc/1000);
        current = current->next;
    }
}

// this function places the path to the Dallas One Wire Temperatur Sensor in
// 'sensor', if called with a malloc'ed sensor and 'path' containing "/sys/bus/w1/devices"
// This function will return a list of all sensors. 
struct tempsensorlist* get_temp_sensor(struct tempsensorlist *head, char *path)
{
    DIR *d;
    struct dirent *dir;
    char *temp = malloc(100);
    d = opendir(path);
    if(d) {
        while((dir = readdir(d)) != NULL) {         
            if((strcmp(dir->d_name, ".") != 0) && (strcmp(dir->d_name, "..") != 0)) {  //filter out . and .. directory entries
                if(strcmp(dir->d_name, "w1_bus_master1") != 0) {                       //filter out 'bus_master' entry
                    strcpy(temp, dir->d_name);                                         //create copy for strtok to use and modify
                    char *family = strtok(temp, "-");
                    if(strcmp(family, "00") != 0) {                                    //filter out spurious sensors entry (Family Code is 0)                        
                        if(head == NULL) {
                            head = onewirediscover_create(path, dir->d_name);
                        } else {   
                            onewirediscover_push(head, path, dir->d_name);
                        }
                        //printf("%s/%s\n", path, dir->d_name);
                    }
                }
            }
        }
        closedir(d);
        return head;
    }    
    printf("%s could not be found or read!\n", path);
    return NULL;
}



int get_temp(const char * sensor)
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


//queries all tempsensors from passed list pointer for its temperature value
//temperature values are stored in list elements
void get_all_temps(struct tempsensorlist *list)
{
    struct tempsensorlist *current = list;
    while(current != NULL) {
        int temp = get_temp(current->tempsensor);    
        current->temp_mc = temp;
        current = current->next;
    }
}


// this function tries to open file a file called 'tempsensorlabels.txt' and
// assigns appropriate labels to each temp sensor that is found in the passed list pointer
// tempsensorlabels.txt must be in the following format
// /full/path/to/tempsensor/<sensor-id>/w1_slave,labelstring
// -> label string may contain spaces, 
// -> the file may contain more temp sensors than the passed list pointer
// If tempsensorlabels.txt does not contain an entry for a temp sensor, the last 
// four digits of the hex sensor id are used as a label.
void get_temp_sensor_labels(struct tempsensorlist *list)
{
    char line[100];
    char *tempsensor;
    char *label;
    char templabel[10];
    struct tempsensorlist *current = list;
    FILE * fh;
    fh = fopen("tempsensorlabels.txt", "r");
    if (fh != NULL) {
        while(current != NULL) {      //iterate through sensor list
            while(fgets(line, 100, fh) != NULL) {    //iterate through list of labels 
                tempsensor = strtok(line, ",");
                label = strtok(NULL, ",");                
                if(strcmp(current->tempsensor, tempsensor) == 0) {
                    current->label = malloc(strlen(label)+2);
                    strcpy(current->label, label);
                    current->label[strlen(label)-1] = '\0';
                    //printf("  assigning label '%s' to %s\n", current->label, current->tempsensor);
                }
            }
            //assign backup label if no proper label found
            if(current->label == NULL) {                
                strncpy(templabel, (current->tempsensor)+strlen(current->tempsensor)-13, 4);
                //printf("  no label found for %s, assigning '%s' as backup label\n", current->tempsensor, templabel);
                current->label = malloc(strlen(label)+2);
                strcpy(current->label, templabel);
            }
            rewind(fh);
            current = current->next;
        }
    } else {
        printf("label file is missing\n");
        exit(-1);
    }
}


