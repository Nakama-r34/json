// open_file_to_string
// string (char* + len)
// redefines of ints

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utilities.h"
#include "debug.h"


string string_of(char* c_string) {
    string output = {
        .content = c_string,
        .length = strlen(c_string)
    };
    return output;
}

string read_file_to_string(const char* path) {
    FILE *fd = fopen(path, "r");
    u8 *buffer = NULL;
    u32 size = 0;
    if(fd) {
        fseek(fd, 0, SEEK_END);
        size = ftell(fd) + 1;
        fseek(fd, 0, SEEK_SET);
        buffer = malloc(size * sizeof(u8));
        fread(buffer, size, 1, fd);
    } else {
        buffer = (u8*)"Error reading file";
    }

    if(buffer == NULL) {
        buffer = (u8*)"Error creating buffer";
    }

    string output = {
        .content = (char*)buffer,
        .length = size
    };

    return output;
}

hash_table_item hash_table_create_item(string key, void *value) {
    hash_table_item item;
    item.key = key;
    item.value = value;
    item.next = NULL;
    return item;
}

hash_table_item *hash_table_item_create(string key, void *value) {
    hash_table_item *item = malloc(sizeof(hash_table_item));
    item->key = key;
    item->value = value;
    item->next = NULL;
    return item;
}

hash_table *hash_table_create(u32 size) {
    hash_table *table = malloc(sizeof(hash_table));
    table->array = malloc(size * sizeof(hash_table_item));
    for(u32 i = 0; i < size; i++) {
        table->array[i] = NULL;
    } 
    table->size = size;
    return table;
}

static u32 hash_table_hash(string key) {
    if(key.length == 1) {
        return key.content[0];
    }
    else {
        return key.content[0] * key.content[1] * key.length;
    }
}

static u32 hash_table_index(u32 hash, u32 size) {
    return (hash % size);
}

// return is ptr_ptr
u32 hash_table_find(hash_table_item **return_item, string key, hash_table *table) {
    // ptr = NULL; ptr -> ptr -> NULL
    *return_item = NULL;
    u32 hash = hash_table_hash(key);
    u32 index = hash_table_index(hash, table->size);
    hash_table_item *current_item = table->array[index];

#ifdef DEBUG_HASH_TABLEi
    printf("=== Debug: %s ===\n", key.content);
    printf("Hash = %d\n", hash);
    printf("Index = %d\n", index);
#endif

    if(!current_item) {

#ifdef DEBUG_HASH_TABLE
        printf("-> Index %d not found\n", index);
#endif

        return 0;
    }

    if(strcmp(key.content, current_item->key.content) == 0) {

#ifdef DEBUG_HASH_TABLE
        printf("-> Return (Key: %s == Item->Key: %s)\n", key.content, current_item->key.content);
#endif

        // ptr = ptr; ptr -> current_item -> ...
        *return_item = current_item;
        return 1;
    }

#ifdef DEBUG_HASH_TABLE 
    printf("-> Search next viable spot with %p\n", current_item->next);
#endif

    while(current_item->next != NULL) {
        if(current_item->key.content == key.content) {
            *return_item = current_item;
            return 1;
        }
        current_item = current_item->next;
    }
    if(current_item->next == NULL) {
        *return_item = current_item;
        return 1;
    }
    printf("Error: No %s key found\n", key.content);
    return 0; 
}

u32 hash_table_add_item(hash_table_item *item, hash_table *table) {
#ifdef DEBUG_HASH_TABLE
    printf("==== ADD ====\n");
#endif
    hash_table_item **previous_item_ptr = malloc(sizeof(hash_table_item**));
    hash_table_item *previous_item = NULL;

    u32 hash = hash_table_hash(item->key);
    u32 index = hash_table_index(hash, table->size);
    u32 has_item = hash_table_find(previous_item_ptr, item->key, table);

    previous_item = *previous_item_ptr;
    free(previous_item_ptr);
    
    if(!has_item) {
#ifdef DEBUG_HASH_TABLE
        printf("-> Index %d assigned with %s\n", index, item->key.content);
#endif
        table->array[index] = item;
        return 0;
    } else {
#ifdef DEBUG_HASH_TABLE
        printf("-> Extend Linked list at Index %d with %s\n", index, item->key.content);
#endif
        previous_item->next = item;
        return 0;
    }
}

u32 hash_table_free(hash_table *table) {
    for(u32 i = 0; i < table->size; i++) {
        hash_table_item *current = table->array[i];
        if(!table->array[i]) {
            continue;
        }
        while(current->next != NULL) {
            hash_table_item *prev = current;
            current = current->next;
            free(prev);
        }
        free(current);
    }

    free(table->array);
    return 0;
}

u32 hash_table_add(string key, void *value, hash_table *table) {
    hash_table_item *item = hash_table_item_create(key, value);
#ifdef DEBUG_HASH_TABLE    
    printf("Created Item: \n");
    print_hash_table_item(item);
#endif
    hash_table_add_item(item, table);
    return 0;
}

void *hash_table_get_value(string key, hash_table *table) {
    hash_table_item **item = malloc(sizeof(hash_table_item**));
    hash_table_find(item, key, table);
    void * return_val = (*item)->value;
    free(item);
    return return_val;
}
