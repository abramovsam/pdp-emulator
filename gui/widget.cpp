#include "widget.h"
#include "ui_widget.h"
#include "formain.h"
#include <QTableWidget>

#include "cpu_api.h"
#include "cpu.h"

extern int emulator_initialized;

Widget::Widget(QWidget *parent, Arg *arg) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    while (!emulator_initialized);

    vcpu = arg->vcpu;   // Added

    tableModel = new TableModel();
 //   tableModel->setSharedMem(sharedMem);
    ui->tableCommand->setModel(tableModel);
    ui->tableCommand->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableCommand->resizeColumnsToContents();
 //   ui->tableCommand->setItemDelegate(new StyledDelegate(this, sharedMem->asmCommand));

    ui->bitmap->setVcpu(vcpu);

    connect(ui->buttonRun,      SIGNAL(clicked()), this, SLOT(slotButtonRun()));
    connect(ui->buttonStop,     SIGNAL(clicked()), this, SLOT(slotButtonStop()));
    connect(ui->buttonStep,     SIGNAL(clicked()), this, SLOT(slotButtonStep()));
    connect(ui->buttonContinue, SIGNAL(clicked()), this, SLOT(slotButtonContinue()));
    connect(ui->buttonReset,    SIGNAL(clicked()), this, SLOT(slotButtonReset()));
    connect(ui->tableCommand,   SIGNAL(doubleClicked(QModelIndex)), this,
            SLOT(userClicked(QModelIndex)));

    ui->buttonContinue->setEnabled(false);
    ui->buttonStep->setEnabled(false);
    ui->buttonStop->setEnabled(true);
    ui->buttonReset->setEnabled(false);

    timer = new QTimer(this);
    timer->setInterval(16);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotUpdateRegister()));
    timer->start();

}

void Widget::userClicked(QModelIndex index) {
    tableModel->headerDoubleClick(index);
    ui->tableCommand->reset();
 //   int address = sharedMem->asmCommand[index.row()].address;
 //   callList->setBreakPointForAddress = address;

    tableModel->setData(index, QBrush(Qt::red), Qt::BackgroundRole);
}

void Widget::slotUpdateRegister() {
    char strtmp[100];
    sprintf(strtmp, "0x%x", get_register(vcpu, 0));
    ui->val_R_0->setText(strtmp);
    sprintf(strtmp, "0x%x", get_register(vcpu, 1));
    ui->val_R_1->setText(strtmp);
    sprintf(strtmp, "0x%x", get_register(vcpu, 2));
    ui->val_R_2->setText(strtmp);
    sprintf(strtmp, "0x%x", get_register(vcpu, 3));
    ui->val_R_3->setText(strtmp);
    sprintf(strtmp, "0x%x", get_register(vcpu, 4));
    ui->val_R_4->setText(strtmp);
    sprintf(strtmp, "0x%x", get_register(vcpu, 5));
    ui->val_R_5->setText(strtmp);
    sprintf(strtmp, "0x%x", get_register(vcpu, 6));
    ui->val_R_6->setText(strtmp);
    sprintf(strtmp, "0x%x", get_register(vcpu, 7));
    ui->val_R_7->setText(strtmp);

    // print flags c n v z
    sprintf(strtmp, "%x", get_cflag(vcpu));
    ui->flag_c->setText(strtmp);
    sprintf(strtmp, "%x", get_nflag(vcpu));
    ui->flag_n->setText(strtmp);
    sprintf(strtmp, "%x", get_vflag(vcpu));
    ui->flag_v->setText(strtmp);
    sprintf(strtmp, "%x", get_zflag(vcpu));
    ui->flag_z->setText(strtmp);
}
void Widget::keyPressEvent(QKeyEvent *event) {
    int key = event->key();     // целочисленный код клавиши
    printf("key = %d\t c = %c\n", key, (char)key);
    throw_kb_interrupt(vcpu, key);
}

void Widget::setEnableButton(State state) {
    switch (state) {
    case StateInit:
        ui->buttonRun->setEnabled(true);
        ui->buttonContinue->setEnabled(false);
        ui->buttonStep->setEnabled(false);
        ui->buttonStop->setEnabled(true);
        ui->buttonReset->setEnabled(false);
        break;
    case StateRun:
        ui->buttonRun->setEnabled(false);
        ui->buttonContinue->setEnabled(false);
        ui->buttonStep->setEnabled(false);
        ui->buttonStop->setEnabled(true);
        ui->buttonReset->setEnabled(true);
        break;
    case StateStop:
        ui->buttonRun->setEnabled(false);
        ui->buttonContinue->setEnabled(true);
        ui->buttonStep->setEnabled(true);
        ui->buttonStop->setEnabled(false);
        ui->buttonReset->setEnabled(true);
        break;
    }
}

void Widget::slotButtonRun() {
    run_emulator(vcpu);
    setEnableButton(StateRun);
}

void Widget::slotButtonStop() {
    stop_emulator(vcpu);
    setEnableButton(StateStop);
}

void Widget::slotButtonReset() {
    reset_emulator(vcpu);
    setEnableButton(StateInit);
}

void Widget::slotButtonContinue() {
    run_emulator(vcpu);
    setEnableButton(StateRun);
}

void Widget::slotButtonStep() {
    step_emulator(vcpu);
    setEnableButton(StateStop);
}

Widget::~Widget()
{
    delete ui;
}
