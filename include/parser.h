#ifndef __PARSER_H__
#define __PARSER_H__

#include "../nob.h"
#include "lexer.h"

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
    String_View name;
    Node *type;
} FnParam;

typedef struct {
    FnParam *items;
    int count;
    int capacity;
} FnParams;

typedef struct {
    Node *base;
    Node *typename;
    String_View name;
    FnParams parameters;
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

bool parser_parse(Parser *p);

#endif // __PARSER_H__
