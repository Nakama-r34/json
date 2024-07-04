#ifndef json_h
#define json_h

#include "debug.h"
#include "utilities.h"
#include "scanner.h"

typedef hash_table json_obj;
typedef struct json_value json_value;


typedef enum {
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUM,
    JSON_OBJ
} json_value_type;

typedef struct {
    u32 count;
    u32 capacity;
    json_value* value;
} json_array;


struct json_value{
    union {
        string text;
        double num;
        json_obj* obj;
        json_array* array;
    }value;
    json_value_type type;
};

json_obj *json_convert_file(string file);
json_value *json_get_value(char* key, json_obj *obj);
json_value_type json_get_value_type(json_value *value);


#endif