#ifndef __VM_H__
#define __VM_H__

#include "ir.h"

typedef struct {
    int stack[1024];
    int sp;
} VM;

bool vm_run_ir(Ops ops, StringTable strings);

#endif // __VM_H__
