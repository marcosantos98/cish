#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char **argv) {

    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd cmd = {0};
    cmd_append(&cmd, "cc");
    cmd_append(&cmd, "-o", "main", "main.c", "lexer.c", "parser.c", "ir.c");
    cmd_append(&cmd, "-Wextra");
    cmd_append(&cmd, "-Wall");
    cmd_append(&cmd, "-Werror");
    cmd_append(&cmd, "-ggdb");

    if (!cmd_run(&cmd))
        return 1;

    return 0;
}
