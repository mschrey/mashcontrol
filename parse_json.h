#ifndef PARSE_JSON
#define PARSE_JSON

#include "jsmn.h"

int jsoneq(const char *json, jsmntok_t *tok, const char *s);

int json_search(const char *json, jsmntok_t *tok, const char *s);

struct listitem * parse_json_recipe(char *filein, int outmode);

#endif