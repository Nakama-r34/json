#ifndef json_h
#define json_h

#include "base.h"
#include "debug.h"

////////////////////////////////////////////
// NOTE(nakama): scanner strcuts / enums
typedef struct {
    string source;
    u64 line;
    u64 count;
    u64 length;
} scanner;

typedef enum {
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,        // { }
    TOKEN_COMMA, TOKEN_COLON,                   // , :
    TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,    // [ ]
    TOKEN_STRING, TOKEN_NUMBER,                 // "text" 56
    TOKEN_EOF                                   // '\0' aka end of file
} token_type;

typedef struct {
    string content;
    token_type type;
} token;

scanner *scanner_create(string file);
token *scanner_next_token(scanner *state);

///////////////////////////////////////////////////////
// NOTE(nakama): json structs / enums
typedef struct json_value json_value;
typedef struct json_obj_item json_obj_item;

typedef enum {
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUM,
    JSON_OBJ
} json_value_type;

typedef struct {
    token *tkn;
} json_state;

struct json_obj_item {
    string key;
    json_value* value;
    json_obj_item* next;
};

typedef struct {
    json_obj_item** array;
    u64 size;
} json_obj;

typedef struct {
    u32 count;
    u32 capacity;
    json_value** value;
} json_array;

struct json_value{
    union {
        string str;
        double num;
        json_obj* obj;
        json_array* array;
    } value;
    json_value_type type;
};

// NOTE(nakama): json read functions
json_value_type json_get_value_type(json_value *value);
json_obj *json_convert_file(string file);
json_value *json_get_value(string key, json_obj *obj);

// NOTE(nakama): json write functions
json_obj *json_obj_create(u64 size);
b8 json_obj_add(string key, json_value *value, json_obj *obj);

json_array *json_array_create(u64 size);
void json_array_append(json_value *value);

// NOTE(nakama): json free
void json_free(json_obj *obj);

#endif