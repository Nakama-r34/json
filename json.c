#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "json.h"

/*
    Current major issues:
    Globals in scanner and json,
    Tokens not as pointers,
    hash_tables are a mess,
    no real debuging,
    no error handeling what so ever
*/

typedef struct {
    Token *next_token;
    Token *current;
} state;

state g_json_state;

void init_state(Token *t1, Token *t2) {
    g_json_state.next_token = t1;
    g_json_state.current = t2;
}

void next_token() {
    g_json_state.current = g_json_state.next_token;
    g_json_state.next_token = scanner();
}





json_value *get_value();

// TODO: add dynamic arrays in utilities
// TODO: make utilities and data structures seperate
// Renaming
void array_free(json_array *array) {
    free(array->value);
}

void array_init(json_array *array) {
    array->count = 0;
    array->capacity = 0;
    array->value = malloc(0);
}

void array_add(json_array *array, json_value value) {
    if(array->count+1 < array->capacity) {
        u32 old_capaicty = array->capacity;
        array->capacity = old_capaicty < 8 ? 8 : old_capaicty * 2;
        array->value = realloc(array->value, array->capacity);
    }
    array->value[array->count] = value;
    array->count++;
}

json_array *create_array() {
    json_array *array = malloc(sizeof(json_array));
    array_init(array);
    while(g_json_state.current->type != TOKEN_RIGHT_BRACKET) {
        json_value *value;
        value = get_value();
        if(g_json_state.current->type == TOKEN_COMMA) {
            next_token();
        }
        array_add(array, *value);
        if(g_json_state.current->type == TOKEN_RIGHT_BRACKET) break;
    }
    printf("Array end <=\n");
    next_token();
}

json_obj *create_obj() {
    hash_table *table = hash_table_create(32);
    while(g_json_state.current->type != TOKEN_RIGHT_BRACE) {
        string key;
        if(g_json_state.current->type == TOKEN_STRING) {
            u8 *chr = malloc((g_json_state.current->length) * sizeof(u8));
            strncpy(chr, g_json_state.current->content.content+1, g_json_state.current->length-2);
            chr[g_json_state.current->length-1] = 0;
            printf("\nKey: %s\n", chr);
            key = string_of(chr);
            next_token();
        } else { printf("No key!\n"); exit(1); }
        if(g_json_state.current->type == TOKEN_COLON) next_token();
        json_value *value;
        value = get_value();
        if(g_json_state.current->type == TOKEN_COMMA) {
            next_token();
        }
        hash_table_add(key, value, table);
        if(g_json_state.current->type == TOKEN_RIGHT_BRACE) break;
    }
    printf("Object end <=\n");
    next_token();
    return (json_obj*)table;
}


// TODO: use case instead of if
json_value *get_value() {
    json_value *value = malloc(sizeof(json_value));
    if(g_json_state.current->type == TOKEN_STRING) {
        u8 *chr = malloc((g_json_state.current->length) * sizeof(u8));
        strncpy(chr, g_json_state.current->content.content+1, g_json_state.current->length-2);
        chr[g_json_state.current->length-1] = 0;
        printf("Value: %s\n", chr);
        value->value.text = string_of(chr);
        value->type = JSON_STRING;
        next_token();
    } else if (g_json_state.current->type == TOKEN_NUMBER) {
        value->value.num = strtod(g_json_state.current->content.content, NULL);
        printf("Value: %f\n", value->value.num);
        next_token();
        value->type = JSON_NUM;
    } else if (g_json_state.current->type == TOKEN_LEFT_BRACKET) {
        printf("Value: Array =>\n");
        next_token();
        value->value.array = create_array();
        value->type = JSON_ARRAY;
    } else if (g_json_state.current->type == TOKEN_LEFT_BRACE) {
        printf("Value: Object =>\n");
        next_token();
        value->value.obj = create_obj();
        value->type = JSON_OBJ;
    }
    return value;
}

json_obj *json_convert_file(string file) {
    json_obj *obj;
    init_scanner(file);
    Token *prev = scanner();
    Token *current = scanner();
    init_state(current, prev);
    if(g_json_state.current->type == TOKEN_LEFT_BRACE) {
        printf("Value: Object =>\n");
        next_token();
        obj = create_obj();
    } else { printf("Invalid Json!\n"); }
    return obj;
}

json_value *json_get_value(char* in_key, json_obj *obj) {
    string key = string_of(in_key);
    return (json_value*)hash_table_get_value(key, (hash_table*)obj);
}