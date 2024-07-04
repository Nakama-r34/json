// { = TOKEN_LEFT_BRACE
// } = TOKEN_RIGHT_BRACE

// , = TOKEN_COMMA

// [ = TOKEN_LEFT_BRACKET
// ] = TOKEN_RIGHT_BRACKET

// " = TOKEN_QUOTES

// : = TOKEN_COLON

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"

ScannerState g_state;

void init_scanner(string file) {
    g_state.source = file;
    g_state.line = 1;
    g_state.length = 0;
    g_state.count = 0;
}

Token *create_token(TokenEnum type) {
    Token *token = malloc(sizeof(Token));
    token->content.content = g_state.source.content + g_state.count -g_state.length;
    token->content.length = g_state.length;
    token->length = g_state.length;
    token->type = type;
    token->line = g_state.line;
    g_state.length = 0;
    return token;
}

u8 isNum(u8 c) {
    if(c >= '0' && c <= '9') return 1;
    else return 0;
}

u8 isStr(u8 c) {
    if(c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z') return 1;
    else return 0;
}

u8 next() {
    g_state.length++;
    u8 c = g_state.source.content[g_state.count];
    g_state.count++;
    return c;
}

u8 peek() {
    return g_state.source.content[g_state.count];
}

Token *create_string() {
    next();
    for (;;) {
        if('"' == g_state.source.content[g_state.count]) break;
        next();
    }
    next();
    return create_token(TOKEN_STRING);
}

Token *create_number() {
    for(;;) {
        if(!isNum(g_state.source.content[g_state.count])) break;
        next();
    }
    return create_token(TOKEN_NUMBER);
}

Token *tokenize() {
    for(;;) {
        u8 c = next();
        if(isNum(c)) {
            return create_number();
        }
        switch (c) {
            case ' ': g_state.length--; break;
            case '\t': g_state.length--; break;
            case '\n': g_state.length--; g_state.line++; break;
            case '{': return create_token(TOKEN_LEFT_BRACE);
            case '}': return create_token(TOKEN_RIGHT_BRACE);
            case '[': return create_token(TOKEN_LEFT_BRACKET);
            case ']': return create_token(TOKEN_RIGHT_BRACKET);
            case ':': return create_token(TOKEN_COLON);
            case ',': return create_token(TOKEN_COMMA);
            case '"': return create_string();
            case '\0': return create_token(TOKEN_EOF);
            default: break;
        }
    }
}

Token *scanner() {
    return tokenize();        
}

