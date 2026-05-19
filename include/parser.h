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
    NT_LIT_NUMBER,
    NT_BIN_EXPR,
    NT_VAR_DECL_STMT,
} NodeType;

typedef struct Node Node;

typedef struct {
    Node **items;
    int count;
    int capacity;
} Nodes;

typedef struct {
    Node *base;
    Node *type;
    String_View name;
    Node *decl;
} VarDeclStmt;

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
} LitNumberExpr;

typedef struct {
    Node *base;
    String_View lit;
} IdentExpr;

typedef struct {
    Node *base;
    String_View name;
    Nodes args;
} FnCallExpr;

typedef struct {
    Node *base;
    Node *lhs;
    Token operand;
    Node *rhs;
} BinExpr;

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
        LitNumberExpr *lit_number_expr;
        BinExpr *bin_expr;
        VarDeclStmt *var_decl_stmt;
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
void parser_print_node(Node *);

#endif // __PARSER_H__
