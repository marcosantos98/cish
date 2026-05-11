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
    TT_COMMA,
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
        case ',': {
            Token token = {
                .lexme = (char *)(contents + cursor),
                .lexme_len = 1,
                .type = TT_COMMA,
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

typedef enum {
    NT_FN_DECL,
    NT_BLOCK,
} NodeType;

typedef struct Node Node;

typedef struct {
    Node **items;
    int count;
    int capacity;
} Nodes;

typedef struct {
    Node *base;
    // fixme(marco): This should be a type expression
    char *typename;
    int typename_len;

    char *name;
    int name_len;

    // fixme(marco): add parameters;
    Node *block;
} FnDeclStmt;

typedef struct {
    Node *base;
    Nodes nodes;
} BlockStmt;

struct Node {
    NodeType type;
    TokenLoc start;
    TokenLoc end;
    union {
        FnDeclStmt *fn_decl_stmt;
        BlockStmt *block_stmt;
    };
};

typedef struct {
    Tokens tokens;
    int cursor;
    Nodes nodes;
} Parser;

typedef struct {
    bool ok;
    Node *node;
} ParseResult;
#define PARSE_SUCC(node) (ParseResult){true, (node)};
#define INVALID_RES (ParseResult){false, NULL};

Token parser_get_token(Parser *p) {
    return p->tokens.items[p->cursor];
}

// fixme(marco) be sure that the offset is in bounds;
Token parser_get_token_off(Parser *p, int off) {
    return p->tokens.items[p->cursor + off];
}

bool expect(Parser *p, TokenType tt) {
    bool ok = parser_get_token(p).type == tt;
    if (!ok)
        return false;
    p->cursor += 1;
    return true;
}

bool expect_and_get(Parser *p, TokenType tt, Token *token) {
    Token t = parser_get_token(p);
    bool ok = t.type == tt;
    if (!ok)
        return false;
    *token = t;
    p->cursor += 1;
    return true;
}

ParseResult parser_parse_block_stmt(Parser *p) {
    Token start;
    if (!expect_and_get(p, TT_OPEN_CURLY, &start)) {
        printf("Invalid token as start of block stmt\n");
        return INVALID_RES;
    }

    // fixme: actually parse the body;

    Token end;
    if (!expect_and_get(p, TT_CLOSE_CURLY, &end)) {
        printf("Invalid token as start of block stmt\n");
        return INVALID_RES;
    }

    Node *node = temp_alloc(sizeof(Node));
    node->type = NT_BLOCK;
    node->start = start.loc;
    node->end = end.loc;
    BlockStmt *stmt = temp_alloc(sizeof(BlockStmt));
    stmt->base = node;
    stmt->nodes = (Nodes){0};
    node->block_stmt = stmt;

    return PARSE_SUCC(node);
}

ParseResult parser_parse_stmt(Parser *p) {
    Token token = parser_get_token(p);
    switch (token.type) {
    case TT_OPEN_CURLY: {
        return parser_parse_block_stmt(p);
    } break;
    default:
        printf("Invalid token\n");
        return INVALID_RES;
    }

    return INVALID_RES;
}

ParseResult parser_parse_fn_decl_stmt(Parser *p) {
    Token type;
    if (!expect_and_get(p, TT_IDENT, &type)) {
        printf("Failed to parse type of function declaration.\n");
        return INVALID_RES;
    }

    Token name;
    if (!expect_and_get(p, TT_IDENT, &name)) {
        printf("Failed to parse name of function declaration.\n");
        return INVALID_RES;
    }

    if (!expect(p, TT_OPEN_PAREN)) {
        printf("Expected `(` after function name.\n");
        return INVALID_RES;
    }

    // fixme(marco): here we should parse the function parameters

    if (!expect(p, TT_CLOSE_PAREN)) {
        printf("Expected `(` after function name.\n");
        return INVALID_RES;
    }

    ParseResult res = parser_parse_stmt(p);
    if (!res.ok) {
        printf("Failed to parse function body.\n");
        return INVALID_RES;
    }

    Node *node = temp_alloc(sizeof(Node));
    node->type = NT_FN_DECL;
    node->start = type.loc;
    node->end = res.node->end;
    FnDeclStmt *fn = temp_alloc(sizeof(FnDeclStmt));
    fn->base = node;
    fn->typename = type.lexme;
    fn->typename_len = type.lexme_len;
    fn->name = name.lexme;
    fn->name_len = name.lexme_len;
    fn->block = res.node;
    node->fn_decl_stmt = fn;
    return PARSE_SUCC(node);
}

ParseResult parser_parse_top_level_stmt(Parser *p) {
    Token token = parser_get_token(p);
    switch (token.type) {
    case TT_IDENT: {
        Token peek = parser_get_token_off(p, 2);
        if (peek.type == TT_OPEN_PAREN) {
            return parser_parse_fn_decl_stmt(p);
        } else {
            printf("Invalid token: %.*s\n", token.lexme_len, token.lexme);
            return INVALID_RES;
        }
    } break;
    default:
        printf("Invalid top level token\n");
        return INVALID_RES;
    }

    return INVALID_RES;
}

bool parser_parse(Parser *p) {
    while (p->cursor < p->tokens.count) {
        ParseResult res = parser_parse_top_level_stmt(p);
        if (!res.ok)
            return false;
        da_append(&p->nodes, res.node);
    }
    return true;
}

void print_node(Node *node) {
    switch (node->type) {
    case NT_FN_DECL: {
        FnDeclStmt *stmt = node->fn_decl_stmt;
        printf("FnDeclStmt:\n");
        printf("   type: %.*s\n", stmt->typename_len, stmt->typename);
        printf("   name: %.*s\n", stmt->name_len, stmt->name);
        print_node(stmt->block);
    } break;
    case NT_BLOCK: {
        printf("BlockStmt:\n");
    } break;
    }
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

    Parser parser = {0};
    parser.tokens = tokenizer.tokens;
    if (!parser_parse(&parser)) {
        printf("Failed to parse.\n");
        return 1;
    }

    for (int i = 0; i < parser.nodes.count; i += 1) {
        Node *node = parser.nodes.items[i];
        print_node(node);
    }

    return 0;
}
