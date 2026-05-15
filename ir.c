#include "include/ir.h"
#include "include/lexer.h"
#include "include/parser.h"
#include <assert.h>
#include <stdlib.h>

void ir_generate_for_node(Node *node, Ops *ops, StringTable *strings) {
    switch (node->type) {
    case NT_BIN_EXPR: {
        ir_generate_for_node(node->bin_expr->lhs, ops, strings);
        ir_generate_for_node(node->bin_expr->rhs, ops, strings);
        if (node->bin_expr->operand.type == TT_PLUS) {
            Op op = {
                .type = OT_PLUS,
                .operand = 0,
            };
            da_append(ops, op);
        } else if (node->bin_expr->operand.type == TT_ASTERISK) {
            Op op = {
                .type = OT_MULT,
                .operand = 0,
            };
            da_append(ops, op);
        } else {
            printf(SV_Fmt "\n", SV_Arg(node->bin_expr->operand.lexme));
            assert(false && "unimplement operand");
        }
    } break;
    case NT_LIT_NUMBER: {
        Op op = {
            .type = OT_PUSH_INT,
            // note(marco): if it doesnt break now, it may break later
            .operand = atoi(node->lit_number_expr->lit.data),
        };
        da_append(ops, op);
    } break;
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

bool ir_generate(IRGenerator *ir_gen, Parser p) {

    for (int i = 0; i < p.nodes.count; i += 1) {
        Node *node = p.nodes.items[i];
        ir_generate_for_node(node, &ir_gen->ops, &ir_gen->strings);
    }

#if DEBUG
    for (int i = 0; i < ops.count; i += 1) {
        Op op = ops.items[i];
        printf("[%d] type:(%d) operand:(%d) as_string:(%.*s)\n", i, op.type, op.operand, SV_Arg(strings.items[op.operand]));
    }
#endif

    return true;
}
