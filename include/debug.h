#ifndef debug_h
#define debug_h

#include "json.h"
#include "base.h"

// #define DEBUG_HASH_TABLE

void print_token(token *token);
void print_all_tokens(string file);
void print_json_obj_item(json_obj_item *item);
void print_json_obj(json_obj *table);

#endif