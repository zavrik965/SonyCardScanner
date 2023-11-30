/*
    Этот файл — часть SonyCardScanner.
    SonyCardScanner — свободная программа: вы можете перераспространять ее и/или изменять ее на условиях
    Стандартной общественной лицензии GNU в том виде, в каком она была опубликована Фондом свободного программного обеспечения;
    либо версии 3 лицензии, либо (по вашему выбору) любой более поздней версии.
    SonyCardScanner распространяется в надежде, что она будет полезной, но БЕЗО ВСЯКИХ ГАРАНТИЙ;
    даже без неявной гарантии ТОВАРНОГО ВИДА или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ.
    Подробнее см. в Стандартной общественной лицензии GNU.
    Вы должны были получить копию Стандартной общественной лицензии GNU вместе с этой программой.
    Если это не так, см. <https://www.gnu.org/licenses/>.
*/
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
