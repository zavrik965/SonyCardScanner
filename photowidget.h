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
