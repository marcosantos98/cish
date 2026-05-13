#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "include/ir.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/vm.h"

#define NOB_IMPLEMENTATION
#include "nob.h"

void print_node(Node *node) {
    switch (node->type) {
    case NT_FN_DECL: {
        FnDeclStmt *stmt = node->fn_decl_stmt;
        printf("FnDeclStmt: name:(" SV_Fmt ")\n", SV_Arg(stmt->name));
        print_node(stmt->typename);
        for (int i = 0; i < stmt->parameters.count; i += 1) {
            FnParam param = stmt->parameters.items[i];
            print_node(param.type);
            printf("name:(" SV_Fmt ")\n", SV_Arg(param.name));
        }
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

int main(int argc, char **argv) {

    if (argc < 2)
        return 1;

    char *prog = *argv++;
    (void)prog;

    char *filename = *argv++;

    argc -= 2;
    printf("%d\n", argc);

    bool run_IR = false;
    for (int i = 0; i < argc; i += 1) {
        if (strncmp(argv[i], "-run-ir", 7) == 0) {
            run_IR = true;
        }
    }

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

    Lexer lexer = {0};
    if (!lexer_lex(&lexer, contents, size)) {
        printf("Failed to tokenize.\n");
        return 1;
    }

#if DEBUG
    for (int i = 0; i < lexer.tokens.count; i += 1) {
        printf("[%d] %.*s\n", i, SV_Arg(lexer.tokens.items[i].lexme));
    }
#endif

    fclose(f);

    Parser parser = {0};
    parser.tokens = lexer.tokens;
    if (!parser_parse(&parser)) {
        printf("Failed to parse.\n");
        return 1;
    }

#if DEBUG
    for (int i = 0; i < parser.nodes.count; i += 1) {
        Node *node = parser.nodes.items[i];
        print_node(node);
    }
#endif

    IRGenerator ir_gen = {0};
    if (!ir_generate(&ir_gen, parser)) {
        printf("Failed to generate ir\n");
        return 1;
    }

    if (run_IR) {
        if (!vm_run_ir(ir_gen.ops, ir_gen.strings)) {
            printf("Failed to run IR.\n");
            return 1;
        }
    }

    return 0;
}
