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
} Tokenizer;

void tokenize_identifier(Tokenizer *tokenizer, char *contents, size_t len_contents, size_t *cursor, size_t *col, size_t *row) {
    size_t start = *cursor;
    while (isalnum(contents[start]) && start < len_contents)
        start += 1;

    // fixme(marco): this may fail if the
    // tokenizer reads the indefier until the end of the contents
    Token token = {
        .lexme = sv_from_parts(contents + *cursor, start - *cursor),
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
        .lexme = sv_from_parts(contents + *cursor, start - *cursor),
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
        .lexme = sv_from_parts(contents + *cursor, start - *cursor),
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
                .lexme = sv_from_parts(contents + cursor, 1),
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
                .lexme = sv_from_parts(contents + cursor, 1),
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
                .lexme = sv_from_parts(contents + cursor, 1),
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
                .lexme = sv_from_parts(contents + cursor, 1),
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
                .lexme = sv_from_parts(contents + cursor, 1),
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
                .lexme = sv_from_parts(contents + cursor, 1),
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
                .lexme = sv_from_parts(contents + cursor, 1),
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
                .lexme = sv_from_parts(contents + cursor, 1),
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
                .lexme = sv_from_parts(contents + cursor, 1),
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
                    printf("[%d] %.*s\n", i, SV_Arg(tokenizer->tokens.items[i].lexme));
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
    NT_EXPR_STMT,
    NT_LIT_STRING,
    NT_IDENT_EXPR,
    NT_FN_CALL_EXPR,
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
    String_View typename;

    String_View name;

    // fixme(marco): add parameters;
    Node *block;
} FnDeclStmt;

typedef struct {
    Node *base;
    Nodes nodes;
} BlockStmt;

typedef struct {
    Node *base;
    Node *expr;
} ExprStmt;

typedef struct {
    Node *base;
    String_View lit;
} LitStringExpr;

typedef struct {
    Node *base;
    String_View lit;
} IdentExpr;

typedef struct {
    Node *base;
    String_View name;
    Nodes args;
} FnCallExpr;

struct Node {
    NodeType type;
    TokenLoc start;
    TokenLoc end;
    union {
        FnDeclStmt *fn_decl_stmt;
        BlockStmt *block_stmt;
        ExprStmt *expr_stmt;
        LitStringExpr *lit_string_expr;
        IdentExpr *ident_expr;
        FnCallExpr *fn_call_expr;
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

ParseResult parser_parse_stmt(Parser *);
ParseResult parser_parse_expr(Parser *);

ParseResult parser_parse_block_stmt(Parser *p) {
    Token start;
    if (!expect_and_get(p, TT_OPEN_CURLY, &start)) {
        printf("Invalid token as start of block stmt\n");
        return INVALID_RES;
    }

    Nodes stmts = {0};
    while (parser_get_token(p).type != TT_CLOSE_CURLY) {
        ParseResult res = parser_parse_stmt(p);
        if (!res.ok)
            return INVALID_RES;
        da_append(&stmts, res.node);
    }

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
    stmt->nodes = stmts;
    node->block_stmt = stmt;

    return PARSE_SUCC(node);
}

ParseResult parser_parse_lit_string_expr(Parser *p) {
    Token start;
    if (!expect_and_get(p, TT_STRING, &start)) {
        printf("expect string literal when parse a string literal.\n");
        return INVALID_RES;
    }

    Node *node = temp_alloc(sizeof(Node));
    node->type = NT_LIT_STRING;
    node->start = start.loc;
    node->end = (TokenLoc){node->start.col + start.lexme.count, node->start.row, ""};
    LitStringExpr *lit = temp_alloc(sizeof(LitStringExpr));
    lit->base = node;
    lit->lit = start.lexme;
    node->lit_string_expr = lit;
    return PARSE_SUCC(node);
}

ParseResult parser_parse_ident_expr(Parser *p) {
    Token start;
    if (!expect_and_get(p, TT_IDENT, &start)) {
        printf("expected identifier\n");
        return INVALID_RES;
    }

    Node *node = temp_alloc(sizeof(Node));
    node->type = NT_IDENT_EXPR;
    node->start = start.loc;
    node->end = (TokenLoc){node->start.col + start.lexme.count, node->start.row, ""};
    IdentExpr *expr = temp_alloc(sizeof(IdentExpr));
    expr->base = node;
    expr->lit = start.lexme;
    node->ident_expr = expr;
    return PARSE_SUCC(node);
}

ParseResult parser_parse_primary(Parser *p) {
    Token token = parser_get_token(p);
    switch (token.type) {
    case TT_STRING:
        return parser_parse_lit_string_expr(p);
    case TT_IDENT:
        return parser_parse_ident_expr(p);
    default:
        printf("invalid token as primary: `%.*s`\n", SV_Arg(token.lexme));
        return INVALID_RES;
    }
    return INVALID_RES;
}

ParseResult parser_parse_fn_call_expr(Parser *p, Node *lhs) {
    if (!expect(p, TT_OPEN_PAREN)) {
        Token token = parser_get_token(p);
        printf("expected `(` got `%.*s`\n", SV_Arg(token.lexme));
        return INVALID_RES;
    }

    Nodes args = {0};
    while (parser_get_token(p).type != TT_CLOSE_PAREN) {
        ParseResult res = parser_parse_expr(p);
        if (!res.ok)
            return INVALID_RES;
        da_append(&args, res.node);
        if (parser_get_token(p).type == TT_COMMA)
            expect(p, TT_COMMA);
    }

    Token end;
    if (!expect_and_get(p, TT_CLOSE_PAREN, &end)) {
        printf("expected `(` got `%.*s`\n", SV_Arg(end.lexme));
        return INVALID_RES;
    }

    Node *node = temp_alloc(sizeof(Node));
    node->type = NT_FN_CALL_EXPR;
    node->start = lhs->start;
    node->end = end.loc;
    FnCallExpr *expr = temp_alloc(sizeof(FnCallExpr));
    expr->base = node;
    assert(lhs->type == NT_IDENT_EXPR);
    expr->name = lhs->ident_expr->lit;
    expr->args = args;
    node->fn_call_expr = expr;
    return PARSE_SUCC(node);
}

ParseResult parser_parse_expr(Parser *p) {

    ParseResult res = parser_parse_primary(p);
    if (!res.ok) {
        return INVALID_RES;
    }

    Node *lhs = res.node;

    Token token = parser_get_token(p);
    switch (token.type) {
    case TT_OPEN_PAREN: {
        res = parser_parse_fn_call_expr(p, lhs);
        if (!res.ok)
            return INVALID_RES;
        lhs = res.node;
    } break;
    default:
        break;
    }

    // fixme: this is breakable when something like binops are implemented
    return PARSE_SUCC(lhs);
}

ParseResult parser_parse_expr_stmt(Parser *p) {
    ParseResult res = parser_parse_expr(p);
    if (!res.ok)
        return INVALID_RES;

    Token end;
    if (!expect_and_get(p, TT_SEMICOLON, &end)) {
        printf("expected `;` but got `%.*s`\n", SV_Arg(end.lexme));
        return INVALID_RES;
    }

    Node *node = temp_alloc(sizeof(Node));
    node->type = NT_EXPR_STMT;
    node->start = res.node->start;
    node->end = end.loc;
    ExprStmt *stmt = temp_alloc(sizeof(ExprStmt));
    stmt->base = node;
    stmt->expr = res.node;
    node->expr_stmt = stmt;
    return PARSE_SUCC(node);
}

ParseResult parser_parse_stmt(Parser *p) {
    Token token = parser_get_token(p);
    switch (token.type) {
    case TT_OPEN_CURLY: {
        return parser_parse_block_stmt(p);
    } break;
    default:
        return parser_parse_expr_stmt(p);
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
    fn->name = name.lexme;
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
            printf("Invalid token: %.*s\n", SV_Arg(token.lexme));
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
        printf("   type: %.*s\n", SV_Arg(stmt->typename));
        printf("   name: %.*s\n", SV_Arg(stmt->name));
        print_node(stmt->block);
    } break;
    case NT_BLOCK: {
        BlockStmt *block = node->block_stmt;
        printf("BlockStmt:\n");
        for (int i = 0; i < block->nodes.count; i += 1) {
            print_node(block->nodes.items[i]);
        }
    } break;
    case NT_EXPR_STMT: {
        print_node(node->expr_stmt->expr);
    } break;
    case NT_LIT_STRING: {
        LitStringExpr *expr = node->lit_string_expr;
        printf("LitStringExpr: `%.*s`\n", SV_Arg(expr->lit));
    } break;
    case NT_IDENT_EXPR: {
        IdentExpr *expr = node->ident_expr;
        printf("IdentExpr: `%.*s`\n", SV_Arg(expr->lit));
    } break;
    case NT_FN_CALL_EXPR: {
        FnCallExpr *expr = node->fn_call_expr;
        printf("FnCallExpr: `%.*s`\n", SV_Arg(expr->name));
        for (int i = 0; i < expr->args.count; i += 1) {
            print_node(expr->args.items[i]);
        }
    } break;
    }
}

typedef enum {
    OT_NONE,
    OT_LABEL,
    OT_CALL,
    OT_PUSH_STR,
} OpType;

typedef struct {
    OpType type;
    int operand;
} Op;

typedef struct {
    Op *items;
    int capacity;
    int count;
} Ops;

typedef struct {
    String_View *items;
    int capacity;
    int count;
} StringTable;

void ir_generate_for_node(Node *node, Ops *ops, StringTable *strings) {
    switch (node->type) {
    case NT_FN_DECL: {
        Op op = {
            .type = OT_LABEL,
            .operand = strings->count,
        };
        da_append(strings, node->fn_decl_stmt->name);
        da_append(ops, op);
        ir_generate_for_node(node->fn_decl_stmt->block, ops, strings);
        // fixme(marco): generate end of function
    } break;
    case NT_BLOCK: {
        for (int i = 0; i < node->block_stmt->nodes.count; i += 1) {
            ir_generate_for_node(node->block_stmt->nodes.items[i], ops, strings);
        }
    } break;
    case NT_EXPR_STMT: {
        ir_generate_for_node(node->expr_stmt->expr, ops, strings);
    } break;
    case NT_LIT_STRING: {
        Op op = {
            .type = OT_PUSH_STR,
            .operand = strings->count,
        };
        da_append(strings, node->lit_string_expr->lit);
        da_append(ops, op);
    } break;
    case NT_IDENT_EXPR: {
        printf("NT_IDENT_EXPR\n");
    } break;
    case NT_FN_CALL_EXPR: {
        for (int i = 0; i < node->fn_call_expr->args.count; i += 1) {
            ir_generate_for_node(node->fn_call_expr->args.items[i], ops, strings);
        }
        Op op = {
            .type = OT_CALL,
            .operand = strings->count,
        };
        da_append(strings, node->fn_call_expr->name);
        da_append(ops, op);
    } break;
    }
}

bool ir_generate(Parser p) {
    Ops ops = {0};
    StringTable strings = {0};

    for (int i = 0; i < p.nodes.count; i += 1) {
        Node *node = p.nodes.items[i];
        ir_generate_for_node(node, &ops, &strings);
    }

    for (int i = 0; i < ops.count; i += 1) {
        Op op = ops.items[i];
        printf("[%d] type:(%d) operand:(%d) as_string:(%.*s)\n", i, op.type, op.operand, SV_Arg(strings.items[op.operand]));
    }

    printf("Running IR:\n");

    int stack[1024] = {0};
    int sp = 1024;

    for (int i = 0; i < ops.count; i += 1) {
        Op op = ops.items[i];
        switch (op.type) {
        case OT_LABEL:
            break;
        case OT_CALL: {
            String_View name = strings.items[op.operand];
            if (sv_eq(name, sv_from_cstr("puts"))) {
                int argop = stack[sp];
                sp += 1;
                String_View arg = strings.items[argop];
                printf("%.*s\n", SV_Arg(arg));
            } else {
                printf("Find fncall by name\n");
            }
        } break;
        case OT_PUSH_STR: {
            stack[--sp] = op.operand;
        } break;
        case OT_NONE: {
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
        printf("[%d] %.*s\n", i, SV_Arg(tokenizer.tokens.items[i].lexme));
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

    if (!ir_generate(parser)) {
        printf("Failed to generate ir\n");
        return 1;
    }

    return 0;
}
