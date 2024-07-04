#ifndef debug_h
#define debug_h

#include "utilities.h"
#include "scanner.h"

// #define DEBUG_HASH_TABLE

void print_token(Token *token);
void print_all_tokens(string file);
void print_hash_table_item(hash_table_item* item);
void print_hash_table(hash_table *table);

#endif