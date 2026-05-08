#include <stddef.h>
#define NOB_IMPLEMENTATION
#include "nob.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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
} TokenType;

typedef struct {
    // fixme(marco): use a string with size brotha
    char *lexme;
    int lexme_len;
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
} Tokenizer;

void tokenize_identifier(Tokenizer *tokenizer, char *contents, size_t len_contents, size_t *cursor, size_t *col, size_t *row) {
    size_t start = *cursor;
    while (isalnum(contents[start]) && start < len_contents)
        start += 1;

    // fixme(marco): this may fail if the
    // tokenizer reads the indefier until the end of the contents
    Token token = {
        .lexme = (char *)(contents + *cursor),
        .lexme_len = start - *cursor,
        .type = TT_IDENT,
        .loc = {
            .filename = "",
            .col = *col,
            .row = *row,
        },
    };

    da_append(&tokenizer->tokens, token);

    *col += start - *cursor;
    *cursor = start;
}

void tokenize_number(Tokenizer *tokenizer, char *contents, size_t len_contents, size_t *cursor, size_t *col, size_t *row) {
    size_t start = *cursor;
    while (isdigit(contents[start]) && start < len_contents)
        start += 1;

    // fixme(marco): this may fail if the
    // tokenizer reads the indefier until the end of the contents
    Token token = {
        .lexme = (char *)(contents + *cursor),
        .lexme_len = start - *cursor,
        .type = TT_INT_NUMBER,
        .loc = {
            .filename = "",
            .col = *col,
            .row = *row,
        },
    };

    da_append(&tokenizer->tokens, token);

    *col += start - *cursor;
    *cursor = start;
}

void tokenize_string(Tokenizer *tokenizer, char *contents, size_t len_contents, size_t *cursor, size_t *col, size_t *row) {
    size_t start = *cursor;
    start += 1; // advance "

    // fixme: this will break if the string has escaped \"
    while (contents[start] != '"' && start < len_contents)
        start += 1;

    start += 1; // advance "

    Token token = {
        .lexme = (char *)(contents + *cursor),
        .lexme_len = start - *cursor,
        .type = TT_STRING,
        .loc = {
            .filename = "",
            .row = *row,
            .col = *col,
        },
    };

    da_append(&tokenizer->tokens, token);

    *col += start - *cursor;
    *cursor = start;
}

bool tokenizer_lexme(Tokenizer *tokenizer, char *contents, size_t len_contents) {
    (void)tokenizer;

    size_t cursor = 0;
    size_t col = 1;
    size_t row = 1;

    while (cursor < len_contents) {
        char at = contents[cursor];

        switch (at) {
        case ' ': {
            cursor += 1;
            col += 1;
        } break;
        case '\n': {
            cursor += 1;
            col = 1;
            row += 1;
        } break;
        case '\t': {
            cursor += 1;
            col += 1;
        } break;
        case '(': {
            Token token = {
                .lexme = (char *)(contents + cursor),
                .lexme_len = 1,
                .type = TT_OPEN_PAREN,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&tokenizer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case ')': {
            Token token = {
                .lexme = (char *)(contents + cursor),
                .lexme_len = 1,
                .type = TT_CLOSE_PAREN,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&tokenizer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case '*': {
            Token token = {
                .lexme = (char *)(contents + cursor),
                .lexme_len = 1,
                .type = TT_ASTERISK,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&tokenizer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case '{': {
            Token token = {
                .lexme = (char *)(contents + cursor),
                .lexme_len = 1,
                .type = TT_OPEN_CURLY,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&tokenizer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case '}': {
            Token token = {
                .lexme = (char *)(contents + cursor),
                .lexme_len = 1,
                .type = TT_CLOSE_CURLY,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&tokenizer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case ';': {
            Token token = {
                .lexme = (char *)(contents + cursor),
                .lexme_len = 1,
                .type = TT_SEMICOLON,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&tokenizer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case '[': {
            Token token = {
                .lexme = (char *)(contents + cursor),
                .lexme_len = 1,
                .type = TT_OPEN_SQ_BRACKET,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&tokenizer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case ']': {
            Token token = {
                .lexme = (char *)(contents + cursor),
                .lexme_len = 1,
                .type = TT_CLOSE_SQ_BRACKET,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&tokenizer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case '"': {
            tokenize_string(tokenizer, contents, len_contents, &cursor, &col, &row);
        } break;
        default:
            if (isalpha(at)) {
                tokenize_identifier(tokenizer, contents, len_contents, &cursor, &col, &row);
            } else if (isdigit(at)) {
                tokenize_number(tokenizer, contents, len_contents, &cursor, &col, &row);
            } else {
                for (int i = 0; i < tokenizer->tokens.count; i += 1) {
                    printf("[%d] %.*s\n", i, (int)tokenizer->tokens.items[i].lexme_len, tokenizer->tokens.items[i].lexme);
                }
                return false;
            }
        }
    }

    return true;
}

int main(int argc, char **argv) {

    if (argc < 2)
        return 0;

    char *prog = *argv++;
    printf("Program name: %s\n", prog);

    char *filename = *argv++;
    printf("Filename: %s\n", filename);

    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Failed to open file: %s\n", filename);
        return 1;
    }

    int res = fseek(f, 0, SEEK_END);
    if (res != 0) {
        printf("Failed to fseek\n");
        return 1;
    }

    long size = ftell(f);
    res = fseek(f, 0, SEEK_SET);
    if (res != 0) {
        printf("Failed to fseek\n");
        return 1;
    }

    void *contents = temp_alloc(size);
    size_t read = fread(contents, sizeof(uint8_t), size, f);
    assert((long)read == size);

    printf("Contents\n");
    printf("%.*s\n", (int)size, (char *)contents);

    Tokenizer tokenizer = {0};
    if (!tokenizer_lexme(&tokenizer, contents, size)) {
        printf("Failed to tokenize.\n");
        return 1;
    }
    for (int i = 0; i < tokenizer.tokens.count; i += 1) {
        printf("[%d] %.*s\n", i, (int)tokenizer.tokens.items[i].lexme_len, tokenizer.tokens.items[i].lexme);
    }
    fclose(f);

    return 0;
}
