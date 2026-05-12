#ifndef __IR_H__
#define __IR_H__

#include "parser.h"

typedef enum {
    OT_NONE,
    OT_LABEL,
    OT_CALL,
    OT_PUSH_STR,
} OpType;

typedef struct {
    OpType type;
    int operand;
} Op;

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

bool ir_generate(Parser p);

#endif // __IR_H__
