#include "script.h"

void execute(baslike_t* script, char* text)
{
    reset(script);
    populate(script, text);
    preprocess(script);
    for (script->opindex = 0; script->opindex < script->stacksize; script->opindex++)
    {
        int op = isop(script->stack[script->opindex]);
        doop(script, op);
        if (script->failed) break;
    }
}

void linkfunction(baslike_t* script, baslinkedfunc_t function, const char* name)
{
    baslink_t link;
    link.function = function;
    strcpy(link.name, name);
    script->linkedfunctions[script->numlinks++] = link;
}

void reset(baslike_t* script) {
    for (int i = 0; i < script->stacksize; i++)
        memset(script->stack[i], '\0', 32);
    for (int i = 0; i < MEM; i++) {
        script->memory[i] = 0;
        script->snapshot[i] = 0;
    }
    for (int i = 0; i < script->labelsize; i++)
        script->labels[i] = -1;
    for (int i = 0; i < script->functionsize; i++)
        script->functions[i] = (basfunc_t){};
    // for (int i = 0; i < script->numlinks; i++)
    //     script->linkedfunctions[i] = (baslink_t){};
    script->stacksize = 0;
    script->labelsize = 0;
    script->functionsize = 0;
    script->opindex = 0;
    script->mds = 0;
    script->mdx = 0;
    script->failed = false;
    script->error = -1;
    script->infunction = false;
    script->ret = 0;
    memset(script->output, '\0', 1024);
}

void preprocess(baslike_t* script)
{
    // pre process labels/definitions
    for (int i = 0; i < script->stacksize; i++) {
        if (isop(script->stack[i]) == OP_DEF) {
            script->labels[script->labelsize] = i+1;
            script->labelsize++;
        }
    }
    
    // pre process functions
    for (int i = 0; i < script->stacksize; i++) {
        if (isop(script->stack[i]) == OP_FNC) {
            basfunc_t function;
            function.pos = i+1;
            function.end = 0;
            for (int j = i+1; j < script->stacksize; j++) {
                if (isop(script->stack[j]) == OP_END) {
                    function.end = j;
                    break;
                }
                if (isop(script->stack[j]) == OP_FNC) {
                    break;
                }
            }
            if (function.end == 0) {
                scriptoutput(script, "ERROR: NO FNC END\n");
                script->error = i;
                script->failed = true;
                break;
            }
            script->functions[script->functionsize] = function;
            script->functionsize++;
        }
    }
}

void populate(baslike_t* script, char* text)
{
    // pre pre process to get rid of comments
    for (int i = 0; i < strlen(text); i++) {
        if (text[i] == '(') {
            int end = -1;
            for (int j = i; j < strlen(text); j++) {
                if (text[j] == ')') {
                    end = j;
                    break;
                }
            }
            if (end != -1)
                for (int j = i; j < end+1; j++)
                    text[j] = ' ';
        }
    }
    
    // populate stack full of opcodes
    int i;
    for (i=0;i<512;i++)memset(script->stack[i], '\0', 32);
    for (i=0;i<strlen(text);i++)if(text[i]=='\n')text[i]=' ';
    int index = 0;
    char *token = strtok(text, " \n");
    while(token) {
        strcpy(script->stack[index], token);
        index++;
        token = strtok(NULL, " ");
    }
    free(token);
    script->stacksize = index;
}

int isop(char* op)
{
    for (int i = 0; i < OPS; i++)
        if (!strcmp(op, ops[i])) return i;
    return OP_NON;
}

void doop(baslike_t* script, int op)
{
    switch (op)
    {
        case OP_NON: {
            scriptoutput(script->output, "ERROR: NO OP (%s:%d)\n", script->stack[script->opindex], script->opindex);
            script->error = script->opindex;
            script->failed=true;
        } break;
        case OP_MDS: {
            if (isop(script->stack[script->opindex+1]) == OP_MDX) script->mds = script->memory[script->mdx];
            else script->mds = atoi(script->stack[script->opindex+1]);
            script->opindex++;
        } break;
        case OP_IFE: {
            int els = -1;
            int enf = -1;
            for (int i = script->opindex; i < script->stacksize; i++) {
                int eop = isop(script->stack[i]);
                if (eop == OP_ENF) {
                    enf = i;
                    break;
                }
                if (eop == OP_ELS) {
                    els = i;
                }
            }
            if (enf == -1) {
                scriptoutput(script->output, "ERROR: NO ENF\n");
                script->error = script->opindex;
                script->failed=true;
                break;
            }
            int m;
            bool jumped=false;
            if (isop(script->stack[script->opindex+1]) == OP_MDX) m = script->memory[script->mdx];
            else if (isop(script->stack[script->opindex+1]) == OP_RET) m = script->ret;
            else if (startswith(script->stack[script->opindex+1], "ARG") && script->infunction) {
                char* s = script->stack[script->opindex+1];
                s+=3;
                m = script->args[atoi(s)];
            }
            else m = atoi(script->stack[script->opindex+1]);
            if (script->memory[script->mds] == m) {
                script->opindex+=2;
                int to = els > -1 ? els : enf;
                for (; script->opindex < to; script->opindex++) {
                    if (isop(script->stack[script->opindex])==OP_JMP) {
                        doop(script, isop(script->stack[script->opindex]));
                        jumped=true;
                        break;
                    }
                    doop(script, isop(script->stack[script->opindex]));
                    if (script->failed) break;
                }
            } else {
                if (els > -1) {
                    script->opindex = els;
                    for (; script->opindex < enf; script->opindex++) {
                        if (isop(script->stack[script->opindex])==OP_JMP) {
                            doop(script, isop(script->stack[script->opindex]));
                            jumped=true;
                            break;
                        }
                        doop(script, isop(script->stack[script->opindex]));
                        if (script->failed) break;
                    }
                }
            }
            if (!jumped)
                script->opindex = enf;
        } break;
        case OP_SET: {
            int setop = isop(script->stack[script->opindex+1]);
            if (setop == OP_MDX)
                script->memory[script->mds] = script->memory[script->mdx];
            else if (setop == OP_RET)
                script->memory[script->mds] = script->ret;
            else if (startswith(script->stack[script->opindex+1], "ARG") && script->infunction) {
                char* s = script->stack[script->opindex+1];
                s+=3;
                script->memory[script->mds] = script->args[atoi(s)];
            }
            else
                script->memory[script->mds] = atoi(script->stack[script->opindex+1]);
            script->opindex++;
        } break;
        case OP_ADD: {
            int addop = isop(script->stack[script->opindex+1]);
            if (addop == OP_MDX)
                script->memory[script->mds] += script->memory[script->mdx];
            else if (addop == OP_RET)
                script->memory[script->mds] += script->ret;
            else if (startswith(script->stack[script->opindex+1], "ARG") && script->infunction) {
                char* s = script->stack[script->opindex+1];
                s+=3;
                script->memory[script->mds] += script->args[atoi(s)];
            }
            else
                script->memory[script->mds] += atoi(script->stack[script->opindex+1]);
            script->opindex++;
        } break;
        case OP_ENF: {
            
        } break;
        case OP_PRN: {
            if (isop(script->stack[script->opindex+1]) == OP_MDX)
                scriptoutput(script->output, "MDX: %d\n", script->memory[script->mdx]);
            else if (isop(script->stack[script->opindex+1]) == OP_MDS)
                scriptoutput(script->output, "MDS: %d\n", script->memory[script->mds]);
            else
                scriptoutput(script->output, "OUT: %s\n", script->stack[script->opindex+1]);
            script->opindex++;
        } break;
        case OP_ELS: {
        } break;
        case OP_MEM: {
            scriptoutput(script->output, "MEM: ");
            for (int i = 0; i < MEM; i++) {
                scriptoutput(script->output, "%d ", script->memory[i]);
            }
            scriptoutput(script->output, "\n");
        } break;
        case OP_DEF: {
            script->opindex++;
        } break;
        case OP_JMP: {
            bool found=false;
            for (int i = 0; i < script->labelsize; i++) {
                if (!strcmp(script->stack[script->labels[i]], script->stack[script->opindex+1])) {
                    script->opindex = script->labels[i];
                    found=true;
                    break;
                }
            }
            if (!found) {
                scriptoutput(script->output, "ERROR: NO LABEL (%s)\n", script->stack[script->opindex+1]);
                script->error = script->opindex;
                script->failed=true;
                break;
            }
        } break;
        case OP_IFL: {
            int els = -1;
            int enf = -1;
            for (int i = script->opindex; i < script->stacksize; i++) {
                int eop = isop(script->stack[i]);
                if (eop == OP_ENF) {
                    enf = i;
                    break;
                }
                if (eop == OP_ELS) {
                    els = i;
                }
            }
            if (enf == -1) {
                scriptoutput(script->output, "ERROR: NO ENF\n");
                script->error = script->opindex;
                script->failed=true;
                break;
            }
            int m;
            bool jumped=false;
            if (isop(script->stack[script->opindex+1]) == OP_MDX) m = script->memory[script->mdx];
            else if (isop(script->stack[script->opindex+1]) == OP_RET) m = script->ret;
            else if (startswith(script->stack[script->opindex+1], "ARG") && script->infunction) {
                char* s = script->stack[script->opindex+1];
                s+=3;
                m = script->args[atoi(s)];
            }
            else m = atoi(script->stack[script->opindex+1]);
            if (script->memory[script->mds] < m) {
                script->opindex+=2;
                int to = els > -1 ? els : enf;
                for (; script->opindex < to; script->opindex++) {
                    if (isop(script->stack[script->opindex])==OP_JMP) {
                        doop(script, isop(script->stack[script->opindex]));
                        jumped=true;
                        break;
                    }
                    doop(script, isop(script->stack[script->opindex]));
                    if (script->failed) break;
                }
            } else {
                if (els > -1) {
                    script->opindex = els;
                    for (; script->opindex < enf; script->opindex++) {
                        if (isop(script->stack[script->opindex])==OP_JMP) {
                            doop(script, isop(script->stack[script->opindex]));
                            jumped=true;
                            break;
                        }
                        doop(script, isop(script->stack[script->opindex]));
                        if (script->failed) break;
                    }
                }
            }
            if (!jumped)
                script->opindex = enf;
        } break;
        case OP_IFG: {
            int els = -1;
            int enf = -1;
            for (int i = script->opindex; i < script->stacksize; i++) {
                int eop = isop(script->stack[i]);
                if (eop == OP_ENF) {
                    enf = i;
                    break;
                }
                if (eop == OP_ELS) {
                    els = i;
                }
            }
            if (enf == -1) {
                scriptoutput(script->output, "ERROR: NO ENF\n");
                script->error = script->opindex;
                script->failed=true;
                break;
            }
            int m;
            bool jumped=false;
            if (isop(script->stack[script->opindex+1]) == OP_MDX) m = script->memory[script->mdx];
            else if (isop(script->stack[script->opindex+1]) == OP_RET) m = script->ret;
            else if (startswith(script->stack[script->opindex+1], "ARG") && script->infunction) {
                char* s = script->stack[script->opindex+1];
                s+=3;
                m = script->args[atoi(s)];
            }
            else m = atoi(script->stack[script->opindex+1]);
            if (script->memory[script->mds] > m) {
                script->opindex+=2;
                int to = els > -1 ? els : enf;
                for (; script->opindex < to; script->opindex++) {
                    if (isop(script->stack[script->opindex])==OP_JMP) {
                        doop(script, isop(script->stack[script->opindex]));
                        jumped=true;
                        break;
                    }
                    doop(script, isop(script->stack[script->opindex]));
                    if (script->failed) break;
                }
            } else {
                if (els > -1) {
                    script->opindex = els;
                    for (; script->opindex < enf; script->opindex++) {
                        if (isop(script->stack[script->opindex])==OP_JMP) {
                            doop(script, isop(script->stack[script->opindex]));
                            jumped=true;
                            break;
                        }
                        doop(script, isop(script->stack[script->opindex]));
                        if (script->failed) break;
                    }
                }
            }
            if (!jumped)
                script->opindex = enf;
        } break;
        case OP_MDX: {
            script->mdx = atoi(script->stack[script->opindex+1]);
            script->opindex++;
        } break;
        case OP_NEG: {
            script->memory[script->mds] = -script->memory[script->mds];
        } break;
        case OP_MUL: {
            int addop = isop(script->stack[script->opindex+1]);
            if (addop == OP_MDX)
                script->memory[script->mds] *= script->memory[script->mdx];
            else if (addop == OP_RET)
                script->memory[script->mds] *= script->ret;
            else if (startswith(script->stack[script->opindex+1], "ARG") && script->infunction) {
                char* s = script->stack[script->opindex+1];
                s+=3;
                script->memory[script->mds] *= script->args[atoi(s)];
            }
            else
                script->memory[script->mds] *= atoi(script->stack[script->opindex+1]);
            script->opindex++;
        } break;
        case OP_DIV: {
            int addop = isop(script->stack[script->opindex+1]);
            if (addop == OP_MDX)
                script->memory[script->mds] /= script->memory[script->mdx];
            else if (addop == OP_RET)
                script->memory[script->mds] /= script->ret;
            else if (startswith(script->stack[script->opindex+1], "ARG") && script->infunction) {
                char* s = script->stack[script->opindex+1];
                s+=3;
                script->memory[script->mds] /= script->args[atoi(s)];
            }
            else
                script->memory[script->mds] /= atoi(script->stack[script->opindex+1]);
            script->opindex++;
        } break;
        case OP_FNC: {
            for (int i = script->opindex; i < script->stacksize; i++) {
                if (isop(script->stack[i]) == OP_END) {
                    script->opindex = i;
                    break;
                }
            }
        } break;
        case OP_CAL: {
            int oppos = script->opindex+1;
            int fnc = -1;
            int opmds = script->mds;
            int opmdx = script->mdx;
            for (int i = 0; i < script->functionsize; i++) {
                if (!strcmp(script->stack[script->functions[i].pos], script->stack[oppos])) {
                    fnc = i;
                    break;
                }
            }
            if (fnc < 0) {
                bool islinked = false;
                for (int i = 0; i < script->numlinks; i++) {
                    if (!strcmp(script->linkedfunctions[i].name, script->stack[oppos])) {
                        if (isop(script->stack[oppos+1]) == OP_NON) {
                            if (script->stack[oppos+1][0] == '<' && script->stack[oppos+1][strlen(script->stack[oppos+1])-1] == '>') {
                                parseargs(script, script->stack[oppos+1]);
                                oppos++;
                            }
                        }
                        script->linkedfunctions[i].function(script);
                        islinked = true;
                        break;
                    }
                }
                if (!islinked) {
                    scriptoutput(script, "ERROR: NO FUNCTION %s\n", script->stack[oppos]);
                    script->error = script->opindex;
                    script->failed = true;
                }
            } else {
                script->opindex = script->functions[fnc].pos+1;
                if (isop(script->stack[oppos+1]) == OP_NON) {
                    if (script->stack[oppos+1][0] == '<' && script->stack[oppos+1][strlen(script->stack[oppos+1])-1] == '>') {
                        parseargs(script, script->stack[oppos+1]);
                        oppos++;
                    }
                }
                script->infunction = true;
                for (; script->opindex < script->functions[fnc].end; script->opindex++) {
                    if (isop(script->stack[script->opindex]) == OP_RET) {
                        doop(script, isop(script->stack[script->opindex]));
                        break;
                    }
                    doop(script, isop(script->stack[script->opindex]));
                    if (script->failed) break;
                }
                script->infunction = false;
            }
            script->mds = opmds;
            script->mdx = opmdx;
            script->opindex = oppos;
        } break;
        case OP_END: {
            script->opindex++;
            scriptoutput(script, "OP_END:%d\n", script->opindex-1);
        } break;
        case OP_RET: {
            int m = 0;
            if (isop(script->stack[script->opindex+1]) == OP_MDX) m = script->memory[script->mdx];
            else if (startswith(script->stack[script->opindex+1], "ARG") && script->infunction) {
                char* s = script->stack[script->opindex+1];
                s+=3;
                m = script->args[atoi(s)];
            }
            else m = atoi(script->stack[script->opindex+1]);
            
            if (script->infunction) {
                script->ret = m;
            }
            script->opindex++;
        } break;
        case OP_PUSH: {
            for (int i = 0; i < MEM; i++) {
                script->snapshot[i] = script->memory[i];
            }
        } break;
        case OP_POP: {
            for (int i = 0; i < MEM; i++) {
                script->memory[i] = script->snapshot[i];
            }
        } break;
        default: {
            scriptoutput(script->output, "UNHANDLED OPERATION (%s)\n", script->stack[script->opindex]);
        } break;
    }
}
