#include "include/lexer.h"

void lex_identifier(Lexer *lexer, char *contents, size_t len_contents, size_t *cursor, size_t *col, size_t *row) {
    size_t start = *cursor;
    while (isalnum(contents[start]) && start < len_contents)
        start += 1;

    Token token = {
        .lexme = sv_from_parts(contents + *cursor, start - *cursor),
        .type = TT_IDENT,
        .loc = {
            .filename = "",
            .col = *col,
            .row = *row,
        },
    };

    da_append(&lexer->tokens, token);

    *col += start - *cursor;
    *cursor = start;
}

void lex_number(Lexer *lexer, char *contents, size_t len_contents, size_t *cursor, size_t *col, size_t *row) {
    size_t start = *cursor;
    while (isdigit(contents[start]) && start < len_contents)
        start += 1;

    Token token = {
        .lexme = sv_from_parts(contents + *cursor, start - *cursor),
        .type = TT_INT_NUMBER,
        .loc = {
            .filename = "",
            .col = *col,
            .row = *row,
        },
    };

    da_append(&lexer->tokens, token);

    *col += start - *cursor;
    *cursor = start;
}

bool lex_string(Lexer *lexer, char *contents, size_t len_contents, size_t *cursor, size_t *col, size_t *row) {
    size_t start = *cursor;
    start += 1; // advance "

    while (start < len_contents) {
        char at = contents[start];
        if (at == '\\' && contents[start + 1] == '\"') {
            start += 2;
        }
        if (contents[start] == '\"')
            break;
        start += 1;
    }

    if (start >= len_contents) {
        printf("unfinished string. reached end of file.\n");
        printf("string start at: %s:%zu:%zu.\n", "filename.cish", *row, *col);
        return false;
    }

    start += 1; // advance "

    Token token = {
        .lexme = sv_from_parts(contents + *cursor, start - *cursor),
        .type = TT_STRING,
        .loc = {
            .filename = "",
            .row = *row,
            .col = *col,
        },
    };

    da_append(&lexer->tokens, token);

    *col += start - *cursor;
    *cursor = start;

    return true;
}

bool lexer_lex(Lexer *lexer, char *contents, size_t len_contents) {
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
        case ',': {
            Token token = {
                .lexme = sv_from_parts(contents + cursor, 1),
                .type = TT_COMMA,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&lexer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case '(': {
            Token token = {
                .lexme = sv_from_parts(contents + cursor, 1),
                .type = TT_OPEN_PAREN,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&lexer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case ')': {
            Token token = {
                .lexme = sv_from_parts(contents + cursor, 1),
                .type = TT_CLOSE_PAREN,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&lexer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case '*': {
            Token token = {
                .lexme = sv_from_parts(contents + cursor, 1),
                .type = TT_ASTERISK,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&lexer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case '{': {
            Token token = {
                .lexme = sv_from_parts(contents + cursor, 1),
                .type = TT_OPEN_CURLY,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&lexer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case '}': {
            Token token = {
                .lexme = sv_from_parts(contents + cursor, 1),
                .type = TT_CLOSE_CURLY,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&lexer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case ';': {
            Token token = {
                .lexme = sv_from_parts(contents + cursor, 1),
                .type = TT_SEMICOLON,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&lexer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case '[': {
            Token token = {
                .lexme = sv_from_parts(contents + cursor, 1),
                .type = TT_OPEN_SQ_BRACKET,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&lexer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case ']': {
            Token token = {
                .lexme = sv_from_parts(contents + cursor, 1),
                .type = TT_CLOSE_SQ_BRACKET,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&lexer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case '+': {
            Token token = {
                .lexme = sv_from_parts(contents + cursor, 1),
                .type = TT_PLUS,
                .loc = {
                    .filename = "",
                    .col = col,
                    .row = row,
                },
            };
            da_append(&lexer->tokens, token);
            cursor += 1;
            col += 1;
        } break;
        case '"': {
            if (!lex_string(lexer, contents, len_contents, &cursor, &col, &row)) {
                return false;
            }
        } break;
        default:
            if (isalpha(at)) {
                lex_identifier(lexer, contents, len_contents, &cursor, &col, &row);
            } else if (isdigit(at)) {
                lex_number(lexer, contents, len_contents, &cursor, &col, &row);
            } else {
                printf("unimplemented token: %c\n", at);
                return false;
            }
        }
    }

    Token token = {
        .lexme = sv_from_cstr("EOF"),
        .type = TT_EOF,
        .loc = {
            .filename = "",
            .col = col,
            .row = row,
        },
    };
    da_append(&lexer->tokens, token);

    return true;
}
