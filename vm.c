#include "include/vm.h"
#include "include/ir.h"

bool vm_run_ir(Ops ops, StringTable strings) {
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
            } else if (sv_eq(name, sv_from_cstr("putd"))) {
                int res = stack[sp];
                sp += 1;
                printf("%d\n", res);
            } else {
                printf("WARN: Find fncall by name\n");
            }
        } break;
        case OT_PUSH_INT:
        case OT_PUSH_STR: {
            stack[--sp] = op.operand;
        } break;
        case OT_PLUS: {
            int a = stack[sp];
            sp++;
            int b = stack[sp];
            sp++;
            int res = a + b;
            stack[--sp] = res;
        } break;
        case OT_MULT: {
            int a = stack[sp];
            sp++;
            int b = stack[sp];
            sp++;
            int res = a * b;
            stack[--sp] = res;
        } break;
        case OT_NONE: {
            return false;
        }
        }
    }

    return true;
}
