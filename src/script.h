#ifndef SCRIPT_H
#define SCRIPT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>

#define OPS 22
#define MEM 8
#define ARGS 10
#define FNCS 16;

typedef struct basfunc_t {
    int pos;
    int end;
} basfunc_t;

typedef struct baslike_t {
    char output[1024];
    char stack[512][32];
    int stacksize;
    int opindex;
    bool failed;
    int memory[MEM];
    int snapshot[MEM];
    int labels[512];
    int labelsize;
    basfunc_t functions[512];
    int functionsize;
    int mds;
    int mdx;
    int error;
    int args[ARGS];
    int ret;
    bool infunction;
} baslike_t;

static void scriptoutput(baslike_t* script, char* fmt, ...) {
    char buf[128];
    va_list va;
    va_start(va, fmt);
    vsprintf(buf, fmt, va);
    va_end(va);
    strcat(script->output, buf);
}

static bool startswith(const char* str, const char* text) {
    if (strlen(text) > strlen(str)) return false;
    for (int i = 0; i < strlen(text); i++) if (str[i] != text[i]) return false;
    return true;
}

static void parseargs(baslike_t* script, char* _args) {
    char* args = (char*)malloc(sizeof(char)*strlen(_args)+1);
    strcpy(args, _args);
    
    args++;
    args[strlen(args)-1] = '\0';
    
    int index = 0;
    char *token = strtok(args, ",");
    while(token && index < 10) {
        if (token[0] == '#') {
            token++;
            if (atoi(token) > 9 || atoi(token) < 0) {
                script->failed = true;
                script->error = script->opindex;
                scriptoutput(script, "ERROR: OUT OF RANGE\n");
            }
            script->args[index] = script->memory[atoi(token)];
        } else if (!strcmp(token, "MDS")) {
            script->args[index] = script->memory[script->mds];
        } else if (!strcmp(token, "MDX")) {
            script->args[index] = script->memory[script->mdx];
        } else {
            script->args[index] = atoi(token);
        }
        index++;
        token = strtok(NULL, ",");
    }
    free(token);
}

void execute    (baslike_t*, char*);
void populate   (baslike_t*, char*);
int  isop       (            char*);
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
    "RET",
    "PUSH",
    "POP",
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
    OP_RET,    // function return register
    OP_PUSH,   // push current memory state
    OP_POP,    // pop current memory state
};

#endif
