#include "../include/json.h"
#include "../include/debug.h"


scanner *scanner_create(string file) {
    scanner *state = malloc(sizeof(scanner));
    if(state == NULL) {
        fprintf(stderr, "Error creating scanner\n");
        exit(EXIT_FAILURE);
    }
    state->source = file;
    state->line = 1;
    state->length = 0;
    state->count = 0;
    return state;
}

static token *token_create(token_type type, scanner *state) {
    token *tkn = malloc(sizeof(token));
    if(tkn == NULL) {
        fprintf(stderr, "Error creating token\n");
        exit(EXIT_FAILURE);
    }
    tkn->content.str = state->source.str + state->count - state->length;
    tkn->content.size = state->length;
    tkn->type = type;
    state->length = 0;
    return tkn;
}

static u8 next(scanner *state) {
    state->length++;
    u8 c = state->source.str[state->count];
    state->count++;
    return c;
}

static u8 peek(scanner *state) {
    return state->source.str[state->count];
}

static token *token_create_string(scanner *state) {
    next(state);
    for (;;) {
        if('"' == state->source.str[state->count]) break;
        next(state);
    }
    next(state);
    return token_create(TOKEN_STRING, state);
}

static token *token_create_number(scanner *state) {
    for(;;) {
        if(!is_num(state->source.str[state->count])) break;
        next(state);
    }
    return token_create(TOKEN_NUMBER, state);
}

token *scanner_next_token(scanner *state) {
    for(;;) {
        u8 c = '0';
        c = next(state);
        if(is_num(c)) { return token_create_number(state); }
        switch (c) {
            case ' ': state->length--; break;
            case '\t': state->length--; break;
            case '\n': state->length--; state->line++; break;
            case '{': return token_create(TOKEN_LEFT_BRACE, state);
            case '}': return token_create(TOKEN_RIGHT_BRACE, state);
            case '[': return token_create(TOKEN_LEFT_BRACKET, state);
            case ']': return token_create(TOKEN_RIGHT_BRACKET, state);
            case ':': return token_create(TOKEN_COLON, state);
            case ',': return token_create(TOKEN_COMMA, state);
            case '"': return token_create_string(state);
            case '\0': return token_create(TOKEN_EOF, state);
            default: {
                fprintf(stderr, "Error tokenizing %.*s on line %ld.\n", state->length, 
                        state->source.str, state->line);
                exit(1);
            };
        }
    }
}

//////////////////////////////////////////
// NOTE(nakama): creat functions

static json_value *json_value_create() {
    json_value *value = malloc(sizeof(json_value));
    return value;
}

json_obj_item *json_obj_item_create(string key, json_value *value) {
    json_obj_item *item = malloc(sizeof(json_obj_item));
    item->key = key;
    item->value = value;
    item->next = NULL;
    return item;
}

json_obj *json_obj_create(u64 size) {
    json_obj *obj = malloc(sizeof(json_obj));
    obj->array = malloc(size * sizeof(json_obj_item*));
    for(u32 i = 0; i < size; i++) {
        obj->array[i] = NULL;
    } 
    obj->size = size;
    return obj;
}

json_array *json_array_create(u64 size) {
    json_array *array = malloc(sizeof(json_array));
    array->count = 0;
    array->capacity = size;
    array->value = malloc(size * sizeof(json_value*));
    return array;
}

//////////////////////////////////////////////////////////

static u64 hash(string key) {
    if(key.size == 1) {
        return key.str[0];
    }
    else {
        return key.str[0] * key.str[1] * key.size;
    }
}

static u64 indexer(u64 hash, u64 size) {
    return (hash % size);
}

// TODO(nakama): split into linked list part, and rest
b8 json_obj_item_find(json_obj_item **return_item, string key, json_obj *obj) {
    *return_item = NULL;
    u64 hash_value = hash(key);
    u64 index_value = indexer(hash_value, obj->size);
    json_obj_item *current_item = obj->array[index_value];

    if(!current_item) {
        return false;
    }

    if(strcmp(key.str, current_item->key.str) == 0) {
        *return_item = current_item;
        // 2, so json_obj_add can skip adding item
        return 2;
    }

    while(current_item->next != NULL) {
        if(strcmp(current_item->key.str, key.str) == 0) {
            *return_item = current_item;
            return true;
        }
        current_item = current_item->next;
    }
    if(current_item->next == NULL) {
        *return_item = current_item;
        return true;
    }
    fprintf(stderr, "Error searching for key %s.\n", key.str);
    exit(EXIT_FAILURE);
}

// TODO(nakama): try to solve pointer mess
b8 json_obj_item_add(json_obj_item *item, json_obj *table) {
    json_obj_item **previous_item_ptr = malloc(sizeof(json_obj_item*));
    json_obj_item *previous_item = NULL;

    u64 hash_val = hash(item->key);
    u64 i = indexer(hash_val, table->size);
    b8 has_item = json_obj_item_find(previous_item_ptr, item->key, table);

    previous_item = *previous_item_ptr;
    free(previous_item_ptr);
    
    if(has_item == false) {
        table->array[i] = item;
        return 0;
    } else {
        previous_item->next = item;
        return 0;
    }
}

b8 json_obj_add(string key, json_value *value, json_obj *table) {
    json_obj_item *item = json_obj_item_create(key, value);
    json_obj_item_add(item, table);
    return true;
}

void json_array_add(json_array *array, json_value *value) {
    if(array->count+1 < array->capacity) {
        u32 old_capaicty = array->capacity;
        array->capacity = old_capaicty < 8 ? 8 : old_capaicty * 2;
        array->value = realloc(array->value, array->capacity);
    }
    array->value[array->count] = value;
    array->count++;
}

////////////////////////////////////////////////////////////////////////////7

json_obj_item *json_obj_get_item(string key, json_obj *table) {
    json_obj_item **item = malloc(sizeof(json_obj_item*));
    json_obj_item_find(item, key, table);
    return *item;
}

json_value *json_get_value(string key, json_obj *obj) {
    return json_obj_get_item(key, obj)->value;
}
void json_value_free(json_value *value);

void json_obj_item_free(json_obj_item *item) {
    free(item->key.str);
    json_value_free(item->value);
    free(item);
}

// TODO(nakama): split into obj_free, item_free and add array_free
b8 json_obj_free(json_obj *table) {
    for(u32 i = 0; i < table->size; i++) {
        json_obj_item *current = table->array[i];
        if(!table->array[i]) {
            continue;
        }
        while(current->next != NULL) {
            json_obj_item *prev = current;
            current = current->next;
            json_obj_item_free(prev);
        }
        json_obj_item_free(current);
    }

    free(table->array);
    free(table);
    return 0;
}

void json_array_free(json_array *array) {
    for(u64 i = 0; i < array->count; i++) {
        json_value_free(array->value[i]);
    }
    free(array->value);
    free(array);
}

void json_value_free(json_value *value) {
    if(value->type == JSON_STRING) {
        free(value->value.str.str);
    }
    if(value->type == JSON_ARRAY) {
        json_array_free(value->value.array);
    }
    if(value->type == JSON_OBJ) {
        json_obj_free(value->value.obj);
    }
    free(value);
}

/////////////////////////////////////////////////////////////

void next_token(json_state *state, scanner *st) {
    free(state->tkn);
    state->tkn = scanner_next_token(st);
}

json_value *json_value_get(scanner *state, json_state *tkn, u64 size);

json_array *json_array_get(scanner *state, json_state *tkn, u64 size) {
    json_array *array = json_array_create(size);
    while(tkn->tkn->type != TOKEN_RIGHT_BRACKET) {
        json_value *value = json_value_get(state, tkn, size);
        if(tkn->tkn->type == TOKEN_COMMA) {
            next_token(tkn, state);
        }
        json_array_add(array, value);
        if(tkn->tkn->type == TOKEN_RIGHT_BRACKET) break;
    }
    printf("Array end <=\n");
    next_token(tkn, state);
}

json_obj *json_obj_get(scanner *state, json_state *tkn, u64 size) {
    json_obj *table = json_obj_create(size);
    while(tkn->tkn->type != TOKEN_RIGHT_BRACE) {
        string key;
        if(tkn->tkn->type == TOKEN_STRING) {
            char *chr = malloc((tkn->tkn->content.size-1) * sizeof(char));
            strncpy(chr, tkn->tkn->content.str+1, tkn->tkn->content.size-2);
            chr[tkn->tkn->content.size-2] = 0; 
            key = str_lit(chr);
            printf("\nKey: %s\n", key.str);
            next_token(tkn, state);
        } else { printf("No key!\n"); exit(1); }
        if(tkn->tkn->type == TOKEN_COLON) {
            next_token(tkn, state);
        }
        json_value *value = json_value_get(state, tkn, size);
        if(tkn->tkn->type == TOKEN_COMMA) {
            next_token(tkn, state);
        }
        json_obj_add(key, value, table);
        if(tkn->tkn->type == TOKEN_RIGHT_BRACE) break;
    }
    printf("Object end <=\n");
    next_token(tkn, state);
    return table;
}


// TODO: use case instead of if
json_value *json_value_get(scanner *state, json_state *tkn, u64 size) {
    json_value *value = malloc(sizeof(json_value));
    if(tkn->tkn->type == TOKEN_STRING) {
        char *chr = malloc((tkn->tkn->content.size-1) * sizeof(char));
        strncpy(chr, tkn->tkn->content.str+1, tkn->tkn->content.size-2);
        chr[tkn->tkn->content.size-2] = 0; 
        value->value.str = str_lit(chr);
        printf("Value: %s\n", value->value.str.str);
        value->type = JSON_STRING;
        next_token(tkn, state);
    } else if (tkn->tkn->type == TOKEN_NUMBER) {
        value->value.num = strtod(tkn->tkn->content.str, NULL);
        printf("Value: %f\n", value->value.num);
        next_token(tkn, state);
        value->type = JSON_NUM;
    } else if (tkn->tkn->type == TOKEN_LEFT_BRACKET) {
        printf("Value: Array =>\n");
        next_token(tkn, state);
        value->value.array = json_array_get(state, tkn, size);
        value->type = JSON_ARRAY;
    } else if (tkn->tkn->type == TOKEN_LEFT_BRACE) {
        printf("Value: Object =>\n");
        next_token(tkn, state);
        value->value.obj = json_obj_get(state, tkn, size);
        value->type = JSON_OBJ;
    }
    return value;
}

json_state *json_state_create() {
    json_state *state = malloc(sizeof(json_state));
    state->tkn = NULL;
    return state;
}

json_obj *json_convert_file(string file) {
    json_obj *obj = NULL;
    scanner *state = NULL;
    json_state *tkn = json_state_create();
    state = scanner_create(file);
    next_token(tkn, state); 
    if(tkn->tkn->type == TOKEN_LEFT_BRACE) {
        printf("Value: Object =>\n");
        next_token(tkn, state);
        obj = json_obj_get(state, tkn, 16);
    } else { printf("Invalid Json!\n"); }
    free(file.str);
    free(tkn->tkn);
    free(tkn);
    free(state);
    return obj;
}

