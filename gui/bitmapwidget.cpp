#include "bitmapwidget.h"

BitmapWidget::BitmapWidget(QWidget *parent) :
    QWidget(parent)
{
    numberOfPixel = 128;
    sizeOfPixel = 2;
    this->resize(numberOfPixel*sizeOfPixel, numberOfPixel*sizeOfPixel);
 //   matrix = NULL;

    timer = new QTimer(this);
    timer->setInterval(16);
    connect(timer, SIGNAL(timeout()) , this, SLOT(timerHit()));
    timer->start();
}

void BitmapWidget::setVcpu(vcpu_t* vcpu) {
    this->vcpu = vcpu;
}

/*
void BitmapWidget::setMatrix(unsigned char *mat) {
    matrix = mat;
}
*/

void BitmapWidget::timerHit() {
    this->repaint();
}

int BitmapWidget::convert_2(unsigned char c) {
    switch (c) {
    case 3:
        return 255;
    case 2:
        return 192;
    case 1:
        return 88;
    case 0:
        return 0;
    default:
        return 0;
    }
}
int BitmapWidget::convert_3(unsigned char c) {
    switch (c) {
    case 7:
        return 255;
    case 6:
        return 216;
    case 5:
        return 180;
    case 4:
        return 144;
    case 3:
        return 108;
    case 2:
        return 72;
    case 1:
        return 36;
    case 0:
        return 0;
    default:
        return 0;
    }
}

void BitmapWidget::paintEvent(QPaintEvent *event)
{
    union my_short {
        unsigned short int sh;
        unsigned char c[2];
    } a;
    QPainter painter(this);
    QPen myPen(Qt::black, sizeOfPixel, Qt::SolidLine);

    for (int y = 0; y < numberOfPixel; y++)
    for (int x = 0; x < numberOfPixel; x+=2) {

        a.sh = get_vram_val(vcpu, (y*numberOfPixel + x) / 2);

        QColor color = QColor(convert_3((a.c[0]>>5)&7),
                              convert_3((a.c[0]>>2)&7),
                              convert_2((a.c[0])&3));
        myPen.setColor(color);

        painter.setPen(myPen);
        painter.drawPoint(x*sizeOfPixel, y*sizeOfPixel);

        color = QColor(convert_3((a.c[1]>>5)&7),
                       convert_3((a.c[1]>>2)&7),
                       convert_2((a.c[1])&3));
        myPen.setColor(color);

        painter.setPen(myPen);
        painter.drawPoint((x+1)*sizeOfPixel, (y)*sizeOfPixel);
    }
}




