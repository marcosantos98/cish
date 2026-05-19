#ifndef __IR_H__
#define __IR_H__

#include "parser.h"

typedef enum {
    OT_NONE,
    OT_LABEL,
    OT_CALL,
    OT_PUSH_STR,
    OT_PUSH_INT,
    OT_PLUS,
    OT_MULT,
    OT_ALLOC,
    OT_STORE,
    OT_LOAD,
} OpType;

typedef struct Op Op;

typedef struct {
    int size;
    int name_link;
} OpAlloc;

struct Op {
    OpType type;
    union {
        int operand;
        OpAlloc alloc;
    };
};

typedef struct {
    Op *items;
    int capacity;
    int count;
} Ops;

typedef struct {
    String_View *items;
    int capacity;
    int count;
} StringTable;

typedef struct {
    Ops ops;
    StringTable strings;
} IRGenerator;

bool ir_generate(IRGenerator *irgen, Parser p);

#endif // __IR_H__
