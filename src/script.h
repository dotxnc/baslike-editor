#ifndef SCRIPT_H
#define SCRIPT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>

#define OPS 19
#define MEM 8

typedef struct basfunc_t {
    int pos;
    int end;
} basfunc_t;

typedef struct baslike_t {
    char output[1024];
    char stack[512][16];
    int stacksize;
    int opindex;
    bool failed;
    int memory[MEM];
    int labels[512];
    int labelsize;
    basfunc_t functions[512];
    int functionsize;
    int mds;
    int mdx;
    int error;
} baslike_t;

static void scriptoutput(baslike_t* script, char* fmt, ...) {
    char buf[128];
    va_list va;
    va_start(va, fmt);
    vsprintf(buf, fmt, va);
    va_end(va);
    strcat(script->output, buf);
}

void execute    (baslike_t*, char*);
void populate   (baslike_t*, char*);
int  isop       ( char*);
void doop       (baslike_t*,   int);
void reset      (baslike_t*       );
void preprocess (baslike_t*       );
static char* ops[OPS] = {
    "MDS",
    "MDX",
    "IFE",
    "IFL",
    "IFG",
    "ELS",
    "ENF",
    "SET",
    "ADD",
    "MUL",
    "DIV",
    "NEG",
    "PRN",
    "MEM",
    "DEF",
    "JMP",
    "FNC",
    "END",
    "CAL",
};
enum {
    OP_NON=-1, // not an operation
    OP_MDS,    // memory location
    OP_MDX,    // second memory location
    OP_IFE,    // if equal to
    OP_IFL,    // if less than
    OP_IFG,    // if greater than
    OP_ELS,    // else
    OP_ENF,    // end if
    OP_SET,    // set mds
    OP_ADD,    // add
    OP_MUL,    // multiply
    OP_DIV,    // divide
    OP_NEG,    // negate
    OP_PRN,    // print
    OP_MEM,    // print memory (debug)
    OP_DEF,    // define label
    OP_JMP,    // jump to label
    OP_FNC,    // define function
    OP_END,    // end functions
    OP_CAL,    // call function
};

#endif
