#ifndef __LEXER_H__
#define __LEXER_H__

#include <stddef.h>

#include "../nob.h"

typedef struct {
    int col;
    int row;
    char *filename;
} TokenLoc;

typedef enum {
    TT_IDENT,
    TT_STRING,
    TT_INT_NUMBER,
    TT_OPEN_PAREN,
    TT_CLOSE_PAREN,
    TT_OPEN_SQ_BRACKET,
    TT_CLOSE_SQ_BRACKET,
    TT_OPEN_CURLY,
    TT_CLOSE_CURLY,
    TT_ASTERISK,
    TT_SEMICOLON,
    TT_COMMA,
    TT_EOF,
} TokenType;

typedef struct {
    String_View lexme;
    TokenType type;
    TokenLoc loc;
} Token;

typedef struct {
    Token *items;
    int capacity;
    int count;
} Tokens;

typedef struct {
    Tokens tokens;
} Lexer;

bool lexer_lex(Lexer *lexer, char *contents, size_t len_contents);

#endif // __LEXER_H__
