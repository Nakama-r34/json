#include <stdio.h>

#include "debug.h"

void print_token(token *token) {
    printf("=== Token %d ===\n", token->type);
    printf("Token contnet %s\n", token->content.str);
    printf("Token length %ld\n", token->content.size);
}

void print_all_tokens(string file) {
    scanner *state = scanner_create(file);
    for(;;) {
        token *tkn = scanner_next_token(state);
        if(tkn->type == TOKEN_EOF) return;
        print_token(tkn);
    }
}

void print_json_obj_item(json_obj_item *item) {
    printf("=> Item <=\n");
    if(item == NULL) {
        printf("-> Is NULL!\n");
    } else {
        printf("-> Key: %s\n", item->key.str);
        printf("-> Ptr Adress: %p\n", item);
        printf("-> Next Adress: %p\n", item->next);
    }
}

void print_json_obj(json_obj *table) {
    printf("=== Table ===\n");
    for(u32 i = 0; i < table->size; i++) {
        printf("At index %d: \n", i);
        if(!table->array[i]) {
            printf("Nothing!\n");
            continue;
        }
        print_json_obj_item(table->array[i]);
        json_obj_item *current = table->array[i];
        while(current->next != NULL) {
            current = current->next;
            print_json_obj_item(current);
        }
    }
}