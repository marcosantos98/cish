#include "include/parser.h"

Token parser_get_token(Parser *p) {
    return p->tokens.items[p->cursor];
}

// is end of file or end of tokens
bool is_eof_or_eot(Parser *p) {
    return p->cursor >= p->tokens.count || parser_get_token(p).type == TT_EOF;
}

bool parser_get_token_off(Parser *p, int off, Token *token) {
    if (p->cursor + off >= p->tokens.count)
        return false;
    *token = p->tokens.items[p->cursor + off];
    return true;
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
ParseResult parser_parse_ident_expr(Parser *);

ParseResult parser_parse_type_expr(Parser *p) {
    Token token = parser_get_token(p);
    switch (token.type) {
    case TT_IDENT:
        return parser_parse_ident_expr(p);
    default:
        printf("Invalid token as type expression:" SV_Fmt "\n", SV_Arg(token.lexme));
        return INVALID_RES;
    }

    return INVALID_RES;
}

ParseResult parser_parse_block_stmt(Parser *p) {
    Token start;
    if (!expect_and_get(p, TT_OPEN_CURLY, &start)) {
        printf("Invalid token as start of block stmt\n");
        return INVALID_RES;
    }

    Nodes stmts = {0};
    while (parser_get_token(p).type != TT_CLOSE_CURLY && !is_eof_or_eot(p)) {
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
    while (parser_get_token(p).type != TT_CLOSE_PAREN && !is_eof_or_eot(p)) {
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

    ParseResult type_res = parser_parse_type_expr(p);
    if (!type_res.ok)
        return INVALID_RES;

    Token name;
    if (!expect_and_get(p, TT_IDENT, &name)) {
        printf("Failed to parse name of function declaration.\n");
        return INVALID_RES;
    }

    if (!expect(p, TT_OPEN_PAREN)) {
        printf("Expected `(` after function name.\n");
        return INVALID_RES;
    }

    FnParams params = {0};
    while (!is_eof_or_eot(p) && parser_get_token(p).type != TT_CLOSE_PAREN) {
        ParseResult type_expr = parser_parse_type_expr(p);
        if (!type_expr.ok)
            return INVALID_RES;
        Token name;
        if (!expect_and_get(p, TT_IDENT, &name)) {
            printf("expected name after type parameter.\n");
            return INVALID_RES;
        }
        FnParam param = {
            .name = name.lexme,
            .type = type_expr.node,
        };
        da_append(&params, param);
        if (parser_get_token(p).type == TT_COMMA)
            expect(p, TT_COMMA);
    }

    if (!expect(p, TT_CLOSE_PAREN)) {
        printf("Expected `(` after function name.\n");
        return INVALID_RES;
    }

    ParseResult block_res = parser_parse_stmt(p);
    if (!block_res.ok) {
        printf("Failed to parse function body.\n");
        return INVALID_RES;
    }

    Node *node = temp_alloc(sizeof(Node));
    node->type = NT_FN_DECL;
    node->start = type_res.node->start;
    node->end = block_res.node->end;
    FnDeclStmt *fn = temp_alloc(sizeof(FnDeclStmt));
    fn->base = node;
    fn->typename = type_res.node;
    fn->name = name.lexme;
    fn->block = block_res.node;
    fn->parameters = params;
    node->fn_decl_stmt = fn;
    return PARSE_SUCC(node);
}

ParseResult parser_parse_top_level_stmt(Parser *p) {
    Token token = parser_get_token(p);
    switch (token.type) {
    case TT_IDENT: {
        Token peek;
        if (!parser_get_token_off(p, 2, &peek)) {
            printf("something went wrong: Reached end of file.\n");
            return INVALID_RES;
        }
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
    while (!is_eof_or_eot(p)) {
        ParseResult res = parser_parse_top_level_stmt(p);
        if (!res.ok)
            return false;
        da_append(&p->nodes, res.node);
    }
    return true;
}
