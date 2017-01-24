#ifndef FORMAIN_H
#define FORMAIN_H

#include <QApplication>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "cpu_api.h"


typedef struct AsmCommand {
    char *command; // or char[*magic*]
    int address;
    int breakePointIsSet;
} AsmCommand;

/*
typedef struct SharedMem {
    int isFull;
    unsigned char *vidio_memory;
    short int *registers; // 8 registers
    short int *flags;
    AsmCommand *asmCommand;
} SharedMem;

typedef struct CallList {
    int doRun;
    int doStopReset;
    int doStep;
    int setBreakPointForAddress; // contain address, if -1 - not set
} CallList;
*/

typedef struct Arg {
    int argc;
    char **argv;
    vcpu_t* vcpu;
    int working;
} Arg;

#endif // FORMAIN_H
