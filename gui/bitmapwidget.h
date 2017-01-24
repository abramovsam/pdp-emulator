#ifndef BITMAPWIDGET_H
#define BITMAPWIDGET_H

#include <QWidget>
#include <QtGui>
#include "cpu_api.h"

class BitmapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BitmapWidget(QWidget *parent = 0);
    int numberOfPixel; // read only

//    void setMatrix(unsigned char *matrix);
    void setVcpu(vcpu_t* vcpu);
signals:

protected:
    void paintEvent(QPaintEvent *event);

private:
    int sizeOfPixel;
    QTimer *timer;
//    unsigned char *matrix;
    int convert_2(unsigned char c);
    int convert_3(unsigned char c);
    vcpu_t *vcpu;

private slots:
    void timerHit();
};

#endif // BITMAPWIDGET_H
