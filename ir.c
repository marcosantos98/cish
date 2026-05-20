#include "include/ir.h"
#include "include/lexer.h"
#include "include/parser.h"
#include <assert.h>
#include <stdlib.h>

void ir_generate_for_node(Node *node, Ops *ops, StringTable *strings) {
    switch (node->type) {
    case NT_IF_STMT: {
        Op if_label = {
            .type = OT_LABEL,
            .operand = 0,
        };
        da_append(ops, if_label);
        ir_generate_for_node(node->if_stmt->condition, ops, strings);
        Op jump = {
            .type = OT_JUMP_IF,
        };
        int jmpidx = ops->count;
        da_append(ops, jump);
        ir_generate_for_node(node->if_stmt->block, ops, strings);

        Op if_block_jmp = {
            .type = OT_JUMP,
        };
        int if_block_jmp_idx = ops->count;
        da_append(ops, if_block_jmp);

        if (node->if_stmt->_else != NULL) {
            Op else_label = {
                .type = OT_LABEL,
                .operand = 0,
            };
            ops->items[jmpidx].jump.false_index = ops->count;
            da_append(ops, else_label);
            ir_generate_for_node(node->if_stmt->_else, ops, strings);
            ops->items[if_block_jmp_idx].operand = ops->count;
        } else {
            Op end_label = {
                .type = OT_LABEL,
                .operand = 0,
            };
            ops->items[jmpidx].jump.false_index = ops->count;
            ops->items[if_block_jmp_idx].operand = ops->count;
            da_append(ops, end_label);
        }
    } break;
    case NT_VAR_DECL_STMT: {
        VarDeclStmt *stmt = node->var_decl_stmt;
        int name_idx = strings->count;
        // fixme(marco): don't hardcode the size;
        Op op = {
            .type = OT_ALLOC,
            .alloc = {
                .size = 4,
                .name_link = name_idx,
            },
        };
        da_append(strings, stmt->name);
        da_append(ops, op);
        ir_generate_for_node(stmt->decl, ops, strings);
        op = (Op){
            .type = OT_STORE,
            .operand = name_idx,
        };
        da_append(ops, op);
    } break;
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

        String_View lit = node->ident_expr->lit;
        if (sv_eq(lit, sv_from_cstr("true"))) {
            Op op = {
                .type = OT_PUSH_INT,
                .operand = 1,
            };
            da_append(ops, op);
            break;
        } else if (sv_eq(lit, sv_from_cstr("false"))) {
            Op op = {
                .type = OT_PUSH_INT,
                .operand = 0,
            };
            da_append(ops, op);
            break;
        }

        IdentExpr *expr = node->ident_expr;
        int idx = -1;
        for (int i = 0; i < strings->count; i += 1) {
            if (sv_eq(expr->lit, strings->items[i])) {
                idx = i;
                break;
            }
        }

        assert(idx >= 0 && "couldnt find name");

        Op op = {
            .type = OT_LOAD,
            .operand = idx,
        };
        da_append(ops, op);
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

char *ir_op_type_to_cstr(OpType type) {
    switch (type) {
    case OT_NONE:
        return "OT_NONE";
    case OT_LABEL:
        return "OT_LABEL";
    case OT_CALL:
        return "OT_CALL";
    case OT_PUSH_STR:
        return "OT_PUSH_STR";
    case OT_PUSH_INT:
        return "OT_PUSH_INT";
    case OT_PLUS:
        return "OT_PLUS";
    case OT_MULT:
        return "OT_MULT";
    case OT_ALLOC:
        return "OT_ALLOC";
    case OT_STORE:
        return "OT_STORE";
    case OT_LOAD:
        return "OT_LOAD";
    case OT_JUMP:
        return "OT_JUMP";
    case OT_JUMP_IF:
        return "OT_JUMP_IF";
    }
    return "Unknown";
}

bool ir_generate(IRGenerator *ir_gen, Parser p) {

    for (int i = 0; i < p.nodes.count; i += 1) {
        Node *node = p.nodes.items[i];
        ir_generate_for_node(node, &ir_gen->ops, &ir_gen->strings);
    }

#if 0
    printf("Strings:\n");
    for (int i = 0; i < ir_gen->strings.count; i += 1) {
        printf("[%d] %.*s\n", i, SV_Arg(ir_gen->strings.items[i]));
    }
    for (int i = 0; i < ir_gen->ops.count; i += 1) {
        Op op = ir_gen->ops.items[i];
        if (op.type == OT_JUMP_IF) {
            printf("[%d] type:(%s) true:(%d) false:(%d) as_string:(%.*s)\n", i, ir_op_type_to_cstr(op.type), op.jump.true_index, op.jump.false_index, SV_Arg(ir_gen->strings.items[op.operand]));
        } else {
            printf("[%d] type:(%s) operand:(%d) as_string:(%.*s)\n", i, ir_op_type_to_cstr(op.type), op.operand, SV_Arg(ir_gen->strings.items[op.operand]));
        }
    }
#endif

    return true;
}
