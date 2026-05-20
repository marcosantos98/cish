#include "include/vm.h"
#include "include/ir.h"
#include <string.h>

#define HT_IMPLEMENTATION
#include "ht.h"

bool vm_run_ir(Ops ops, StringTable strings) {
    int stack[1024] = {0};
    int sp = 1024;
    char mem[640000] = {0};
    int mem_cursor = 0;

    Ht(int, int) mem_locs = {};

    int ip = 0;

    while (ip < ops.count) {
        Op op = ops.items[ip];
        switch (op.type) {
        case OT_JUMP_IF: {
            int top = stack[sp];
            sp += 1;
            if (top != 0) {
                ip += 1;
            } else {
                ip = op.jump.false_index;
            }
        } break;
        case OT_JUMP: {
            ip = op.operand;
        } break;
        case OT_ALLOC: {
            OpAlloc alloc = op.alloc;
            int size = alloc.size;
            stack[--sp] = mem_cursor;
            *ht_put(&mem_locs, alloc.name_link) = mem_cursor;
            mem_cursor += size;
            ip += 1;
        } break;
        case OT_STORE: {
            int val = stack[sp];
            sp += 1;
            int mem_cursor = stack[sp];
            sp += 1;
            // fixme(marco): this is hardcoding the type!
            memcpy(mem + mem_cursor, &val, sizeof(int));
            ip += 1;
        } break;
        case OT_LOAD: {
            int *mem_cursor = ht_find(&mem_locs, op.operand);
            int i = *(int *)(mem + *mem_cursor);
            stack[--sp] = i;
            ip += 1;
        } break;
        case OT_LABEL: {
            ip += 1;
        } break;
        case OT_CALL: {
            String_View name = strings.items[op.operand];
            if (sv_eq(name, sv_from_cstr("puts"))) {
                int argop = stack[sp];
                sp += 1;
                String_View arg = strings.items[argop];
                printf("%.*s\n", SV_Arg(arg));
            } else if (sv_eq(name, sv_from_cstr("putd"))) {
                int res = stack[sp];
                sp += 1;
                printf("%d\n", res);
            } else {
                printf("WARN: Find fncall by name\n");
            }
            ip += 1;
        } break;
        case OT_PUSH_INT:
        case OT_PUSH_STR: {
            stack[--sp] = op.operand;
            ip += 1;
        } break;
        case OT_PLUS: {
            int a = stack[sp];
            sp++;
            int b = stack[sp];
            sp++;
            int res = a + b;
            stack[--sp] = res;
            ip += 1;
        } break;
        case OT_MULT: {
            int a = stack[sp];
            sp++;
            int b = stack[sp];
            sp++;
            int res = a * b;
            stack[--sp] = res;
            ip += 1;
        } break;
        case OT_NONE: {
            return false;
        }
        }
    }

    return true;
}
