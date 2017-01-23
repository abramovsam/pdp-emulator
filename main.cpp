#include "widget.h"
#include "formain.h"
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "cpu_api.h"

void *cpu(void *p){
    Arg *arg = (Arg *)p;
    Process process(arg->sharedMem, arg->callList);
    useconds_t usec = 300;

    vcpu_t* vcpu = (vcpu_t*)arg->vcpu;
   	cpu_emulation(&vcpu, "/home/sabramov/test_pdp/hello.txt"); 
 //   cpu_emulation(&vcpu, "/home/sabramov/test_pdp/array.txt");

    while (arg->working){
        process.checkCallList();
        usleep(usec);
    }
    return NULL;
}

void *gui(void *p) {
    Arg *arg = (Arg *)p;
    QApplication a(arg->argc, arg->argv);
    Widget *w = new Widget(NULL, arg);
    w->show();

    a.exec();
    return NULL;
}

int main(int argc, char *argv[])
{

    Arg arg;
    CallList callList;
    callList.doRun = 0;
    callList.doStep = 0;
    callList.doStopReset = 0;
    callList.setBreakPointForAddress = -1;

    printf("PDP 11 Emulator\n");

    SharedMem sharedMem;
    sharedMem.isFull = 0;

    arg.argc = argc;
    arg.argv = argv;
    arg.sharedMem = &sharedMem;
    arg.callList = &callList;
    arg.vcpu = (vcpu_t*)malloc(sizeof(vcpu_t));
    memset(arg.vcpu, 0, sizeof(vcpu_t));
    arg.working = 1;

    pthread_t gui_st, cpu_st;
    errno = 0;
    if (pthread_create(&gui_st, NULL, gui, &arg) > 0) {
        printf("error pthread_create gui\n");
        return errno;
    }
    errno = 0;
    if (pthread_create(&cpu_st, NULL, cpu, &arg) > 0) {
        printf("error pthread_create cpu\n");
        return errno;
    }
    errno = 0;
    if (pthread_join(gui_st, NULL)) {
        printf("error pthread_join gui\n");
        return errno;
    }
    arg.working = 0;
    errno = 0;
    if (pthread_join(cpu_st, NULL)) {
        printf("error pthread_join cpu\n");
        return errno;
    }

    return 0;
}
