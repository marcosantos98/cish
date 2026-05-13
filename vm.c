#include "include/vm.h"

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
