#ifndef scanner_h
#define scanner_h

#include "utilities.h"

typedef struct {
    string source;
    u32 line;
    u32 count;
    u32 length;
} ScannerState;

typedef enum {
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_QUOTES,
    TOKEN_COLON,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_EOF 
} TokenEnum;

typedef struct {
    string content;
    TokenEnum type;
    u32 line;
    u32 length;
} Token;

Token *scanner();
void init_scanner(string file);

#endif