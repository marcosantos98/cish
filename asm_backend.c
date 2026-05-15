#include "include/asm_backend.h"
#include "include/ir.h"
#include "include/parser.h"

#include <assert.h>
#include <stdio.h>

void asm_gen_exit_syscall(FILE *output, int exit_code) {
    fprintf(output, "mov rdi, %d\n", exit_code);
    fprintf(output, "mov rax, 60\n");
    fprintf(output, "syscall\n");
}

void asm_generate_strings(FILE *output, StringTable strings) {
    for (int i = 0; i < strings.count; i += 1) {
        fprintf(output, "str_%d db \"" SV_Fmt "\"\n", i, SV_Arg(strings.items[i]));
        fprintf(output, "str_%d_len equ $ - str_%d\n", i, i);
    }
}

bool asm_nasm_x86_64_gnu_linux(IRGenerator ir) {
    (void)ir;
    FILE *output = fopen("./out.asm", "w");
    if (!output) {
        printf("Failed to open out.asm to write\n");
        return false;
    }

    fprintf(output, "global _start\n");
    fprintf(output, "_start:\n");

    for (int i = 0; i < ir.ops.count; i += 1) {
        Op op = ir.ops.items[i];
        switch (op.type) {
        case OT_PLUS:
            assert(false && "implement PLUS in ASM asm_backend");
        case OT_MULT:
            assert(false && "implement MULT in ASM asm_backend");
        case OT_LABEL:
            break;
        case OT_CALL: {
            String_View name = ir.strings.items[op.operand];

            // note(marco): this is a hack for now
            if (sv_eq(name, sv_from_cstr("puts"))) {
                fprintf(output, "mov rax, 1\n");
                fprintf(output, "syscall\n");
            }
        } break;
        case OT_PUSH_STR: {
            fprintf(output, "mov rsi, str_%d\n", op.operand);
            fprintf(output, "mov rdx, str_%d_len\n", op.operand);
        } break;
        case OT_PUSH_INT: {
            // fixme(marco): dont hardcode the register
            fprintf(output, "mov rax, %d\n", op.operand);
        } break;
        case OT_NONE:
            return false;
        }
    }

    asm_gen_exit_syscall(output, 0);

    fprintf(output, "section .data\n");
    asm_generate_strings(output, ir.strings);

    fclose(output);
    return true;
}
