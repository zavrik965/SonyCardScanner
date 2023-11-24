#include "photowidget.h"
#include <QPainter>

PhotoWidget::PhotoWidget(QWidget *parent) : QWidget{parent}
{

}

void PhotoWidget::setChoosed(bool isChoosed) {
    this->choosed = isChoosed;
}

void PhotoWidget::setImage(QPixmap pic, QString text) {
    this->pic = pic;
    this->text = text;
}

void PhotoWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawPixmap((int)(this->width() - 120) / 2, 5, this->pic);
    painter.drawText((int)(this->width() - 120) / 2, 110, this->text);
    if(this->choosed) {
        painter.fillRect(0, 0, this->width(),this->height(), QColor(0, 200, 200,120));
    }
}
