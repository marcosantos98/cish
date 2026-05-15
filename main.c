#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "include/asm_backend.h"
#include "include/ir.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/vm.h"

#define NOB_IMPLEMENTATION
#include "nob.h"

#define DEBUG 0

int main(int argc, char **argv) {

    if (argc < 2)
        return 1;

    char *prog = *argv++;
    (void)prog;

    char *filename = *argv++;

    argc -= 2;

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
        parser_print_node(node);
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
        return 0;
    }

    if (!asm_nasm_x86_64_gnu_linux(ir_gen)) {
        return 1;
    }

    Nob_Cmd cmd = {0};
    cmd_append(&cmd, "nasm", "-felf64", "out.asm");
    if (!cmd_run(&cmd))
        return 1;
    cmd_append(&cmd, "ld", "out.o", "-o", "out");
    if (!cmd_run(&cmd))
        return 1;

    return 0;
}
