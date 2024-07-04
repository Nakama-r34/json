#include <stdio.h>

#include "debug.h"

void print_token(Token *token) {
    printf("=== Token %d ===\n", token->type);
    printf("Token contnet %s\n", token->content.content);
    printf("Token length %d\n", token->content.length);
    printf("Token line %d\n", token->line);    
}

void print_all_tokens(string file) {
    init_scanner(file);
    for(;;) {
        Token *token = scanner();
        if(token->type == TOKEN_EOF) return;
        print_token(token);
    }
}

void print_hash_table_item(hash_table_item *item) {
    printf("=> Item <=\n");
    if(item == NULL) {
        printf("-> Is NULL!\n");
    } else {
        printf("-> Key: %s\n", item->key.content);
        printf("-> Ptr Adress: %p\n", item);
        printf("-> Next Adress: %p\n", item->next);
    }
}

void print_hash_table(hash_table *table) {
    printf("=== Table ===\n");
    for(u32 i = 0; i < table->size; i++) {
        printf("At index %d: \n", i);
        if(!table->array[i]) {
            printf("Nothing!\n");
            continue;
        }
        print_hash_table_item(table->array[i]);
        hash_table_item *current = table->array[i];
        while(current->next != NULL) {
            current = current->next;
            print_hash_table_item(current);
        }
    }
}