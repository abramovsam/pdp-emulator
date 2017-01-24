#include "widget.h"
#include "formain.h"
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "cpu_api.h"

void *cpu(void *p){
    Arg *arg = (Arg *)p;

    vcpu_t* vcpu = (vcpu_t*)arg->vcpu;
    cpu_emulation(&vcpu, "/home/parallels/Documents/parallels/computer_architecture/pdp11/fill.txt");

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
    arg.argc = argc;
    arg.argv = argv;
    arg.vcpu = (vcpu_t*)malloc(sizeof(vcpu_t));
    memset(arg.vcpu, 0, sizeof(vcpu_t));

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
    halt_emulator(arg.vcpu);
    errno = 0;
    if (pthread_join(cpu_st, NULL)) {
        printf("error pthread_join cpu\n");
        return errno;
    }

    return 0;
}
