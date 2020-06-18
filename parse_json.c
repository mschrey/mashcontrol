#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "jsmn.h"
#include "parse_json.h"
#include "mashsteplist.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */


int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

int json_search(const char *json, jsmntok_t *tok, const char *s) {
    int i;
    int searchlen = tok->end - tok->start;
    //printf("checking %.*s for %s\n", searchlen, json + tok->start, s);
    if(tok->type == JSMN_STRING && searchlen >= strlen(s)) {
        for(i=0;i<(searchlen-strlen(s)+1);i++) {
            //printf("i=%d, s1=%.*s, s2=%s\n", i, searchlen, json + tok->start, s);
            if (strncmp(json + tok->start + i, s, strlen(s)) == 0) {
                return 0;
            }
        }
        return -1;
    } else {
        //printf("not a string type\n");
        return -1;
    }

}


void temperature2mashname(char *mashname, double temperature)
{
    //mash names and functions taken from https://hobbybrauer.de/forum/wiki/doku.php/rast
    double EPSILON = 0.01;
    if (temperature >= 75)
        strcpy(mashname, "Abmaischen");
    else if (temperature > 70 && temperature < 75)
        strcpy(mashname, "Verzuckerung (Dextrine)");
    else if (temperature > 60 && temperature < 65)
        strcpy(mashname, "Maltose");
    else if (temperature > 55 && temperature < 60)
        strcpy(mashname, "Dextrine");
    else if (temperature - 50 < EPSILON)
        strcpy(mashname, "Glucose/Fructose");
    else if (temperature > 45 && temperature < 50)
        strcpy(mashname, "Kurze Peptide/FAN");
    else if (temperature > 70 && temperature < 75)
        strcpy(mashname, "Verzuckerung");
    else
        strcpy(mashname, "unknown mash step temperature");
}



// parse_json_recipe will open a json file and search for certain strings to find
// mash-in temperature, mash-out temperature, number of mash steps as well as
// temperature and duration of these mash steps
// At first, the complete json file will be parsed; afterwards, number of mash 
// steps is determined; then corresponding search keys are generated. Finally, 
// these search keys are searched for to extract mash step duration and mash step 
// temperature. 
//
// return value: parse_json_recipe will always return a pointer to a list of 
//   mash steps  (struct * listitem)
//
// parameters: 
// filein is json file to be opened and parsed
// if outmode == 1, additionally an msf file is generated with the same file name
// as filein
struct listitem * parse_json_recipe(char *filein, int outmode) {
    int i;
    int r;
    jsmn_parser p;
    jsmntok_t t[128]; /* We expect no more than 128 tokens */

    jsmn_init(&p);

    int ret = 1;
    int file_len = 0;
    const int buffer_size = 100;
    FILE *fp;
    char *buf = malloc(sizeof(char)*buffer_size);
    char *jsonfile = NULL;
    fp = fopen(filein, "r");
    if (fp == NULL) {
        printf("error opening file!\n");
        exit(-1);
    }
    /* read in json file in chunks. After loop completion, contents are in jsonfile */
    while(ret > 0) {
        ret = fread(buf, 1, buffer_size, fp);
        jsonfile = realloc(jsonfile, file_len + ret);
        strncpy(jsonfile+file_len, buf, ret);        
        file_len += ret;        
    }
    
    r = jsmn_parse(&p, jsonfile, strlen(jsonfile), t, sizeof(t)/sizeof(t[0]));
    if (r < 0) {
        printf("Failed to parse JSON: %d\n", r);
        exit(-1);
    }

    /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT) {
        printf("Object expected\n");
        exit(-1);
    }

    /* Make sure we are about to do infusion instead of decoction */
    for (i = 1; i < r; i++) {
        if (jsoneq(jsonfile, &t[i], "Maischform") == 0) {
            if (json_search(jsonfile, &t[i+1], "dekoktion") == 0) {
                printf("Decoction is not supported currently! Sorry!\n");
                exit(-1);
            }
        }
    }

    /* Loop over all keys of the root object to determine number of mash steps*/
    int NumberOfMashSteps = 0;
    for (i = 1; i < r; i++) {
        if (json_search(jsonfile, &t[i], "Rasttemperatur") == 0) {
            NumberOfMashSteps += 1;
        }
    }
    //printf("Number of Mash Steps: %d\n", NumberOfMashSteps);
    
    //Now we know the number of mash steps, we can generate the search keys
    char *Mashtemperature_searchkey[NumberOfMashSteps];
    char *Mashduration_searchkey[NumberOfMashSteps];
    //reuse buf
    for(i=0; i<NumberOfMashSteps;i++) {
        Mashtemperature_searchkey[i] = (char*)malloc(sizeof(char)*40);
        Mashduration_searchkey[i] = (char*)malloc(sizeof(char)*40);
        strcpy(Mashtemperature_searchkey[i], "Infusion_Rasttemperatur");
        strcpy(Mashduration_searchkey[i], "Infusion_Rastzeit");
        sprintf(buf, "%d", i+1);
        strcat(Mashtemperature_searchkey[i], buf);
        strcat(Mashduration_searchkey[i], buf);
    }

    ////output search keys to check them
    //for(i=0; i<NumberOfMashSteps; i++) {
    //    printf("%s    \t%s\n", Mashtemperature_searchkey[i], Mashduration_searchkey[i]);
    //}


    /* Loop over all keys of the root object, search for previously generated search keys */
    int j = 0;
    int temperature = 0;
    int duration = 0;
    
    struct listitem * head = NULL; 

    //reuse buf once again
    for (i = 1; i < r; i++) {
        if (jsoneq(jsonfile, &t[i], "Infusion_Einmaischtemperatur") == 0) {
            temperature = atoi(strndup(jsonfile + t[i+1].start, t[i+1].end - t[i+1].start));
            head = mashsteplist_create(temperature, 0, "Einmaischen");
            i++;
        } else if(jsoneq(jsonfile, &t[i], "Abmaischtemperatur") == 0) {
            temperature = atoi(strndup(jsonfile + t[i+1].start, t[i+1].end - t[i+1].start));
            mashsteplist_push(head, temperature, 0, "Abmaischen");
            i++;
        } else {
            for(j=0;j<NumberOfMashSteps;j++) {
                if (jsoneq(jsonfile, &t[i], Mashtemperature_searchkey[j]) == 0) {    //search for "Infusion_RasttemperaturX"
                    temperature = atoi(strndup(jsonfile + t[i+1].start, t[i+1].end - t[i+1].start));
                    i++;
                }
                if (jsoneq(jsonfile, &t[i], Mashduration_searchkey[j]) == 0) {      //search for "Infusion_RastzeitX"    
                    duration = atoi(strndup(jsonfile + t[i+1].start, t[i+1].end - t[i+1].start));
                    i++;
                }
                if (duration != 0 && temperature != 0) {
                    temperature2mashname(buf, temperature);
                    mashsteplist_push(head, temperature, duration, buf);
                    duration = 0;
                    temperature = 0;
                }
            }
        }
    }

    printRastList(head);

    /*                 */
    /* generate output */
    /*                 */

    //write recipe to msf file
    if(outmode == 1) {
        //strip ".json" from input filename, replace with ".msf" to generate output file name
        char * fileout = malloc(strlen(filein)*sizeof(char));
        strncpy(fileout, filein, strlen(filein) - 5);
        strcat(fileout, ".msf");
        //printf("output file name is %s\n", fileout);

        FILE * fh_out = fopen(fileout, "w");
        if (fh_out == NULL) {
            printf("error opening out file %s\n", fileout);
            exit(-1);
        }

        //write header to msf file 
        fprintf(fh_out, "# Maischprogram fuer %.*s\n", strlen(fileout)-4, fileout);
        char time_string [80];
        time_t rawtime;
        time(&rawtime);
        strftime(time_string, 80, "%F", localtime(&rawtime));
        fprintf(fh_out, "# created by parse_json module on %s\n", time_string);

        //write mash steps to msf file
        struct listitem *currentRast = head;
        while(currentRast != NULL) {
            fprintf(fh_out, "%s, %d, %d\n", currentRast->action, (int)currentRast->temperature, currentRast->duration);
            currentRast = currentRast->next;
        }
        fclose(fh_out);
    }

    return head;    
}
