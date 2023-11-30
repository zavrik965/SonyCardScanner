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
#ifndef PHOTOWIDGET_H
#define PHOTOWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QString>

class PhotoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PhotoWidget(QWidget *parent = nullptr);
    void setChoosed(bool isChoosed);
    void setImage(QPixmap pic, QString text);

private:
    bool choosed = false;
    QPixmap pic;
    QString text;

protected:
    void paintEvent(QPaintEvent *event);

signals:

};

#endif // PHOTOWIDGET_H
