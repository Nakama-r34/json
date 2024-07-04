#ifndef utilities_h
#define utilities_h

#include "utilities.h"

#define HASH_TABLE_ITEM_GET_VALUE(type, item) ({type *retval; retval = ((type*)item->value) ; retval;})

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;


typedef char i8;
typedef short i16;
typedef int i32;
typedef long i64;

typedef float f16;
typedef double f32;

typedef struct {
    char* content;
    i32 length;
} string;

typedef struct hash_table_item hash_table_item;


struct hash_table_item{
    string key;
    void* value;
    hash_table_item* next;
};

typedef struct {
    hash_table_item** array;
    u32 size;
} hash_table;

// hash_table_item hash_table_create_item(string key, void* value);

hash_table *hash_table_create(u32 size);
// u32 hash_table_add_item(hash_table_item *item, hash_table *table);
// u32 hash_table_find(hash_table_item **return_item_ptr, string *key, hash_table *table);
void *hash_table_get_value(string key, hash_table *table);
u32 hash_table_add(string key, void *value, hash_table *table);
u32 hash_table_free(hash_table *table);

string string_of(char* string);
string read_file_to_string(const char* path);

#endif 