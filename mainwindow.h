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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cliapp.h"
#include "photowidget.h"
#include <QProgressBar>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QMap<QString,QStringList> args, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_arwTableWidget_cellDoubleClicked(int row, int column);
    void on_jpgTableWidget_cellDoubleClicked(int row, int column);
    void on_mtsTableWidget_cellDoubleClicked(int row, int column);
    void on_mp4TableWidget_cellDoubleClicked(int row, int column);

    void on_arwTableWidget_itemSelectionChanged();
    void on_jpgTableWidget_itemSelectionChanged();
    void on_mtsTableWidget_itemSelectionChanged();
    void on_mp4TableWidget_itemSelectionChanged();

    void on_scanAction_triggered();

    void on_selectAction_triggered();
    void on_deselectAction_triggered();

    void on_chooseDirectoryButton_clicked();

    void on_saveButton_clicked();

private:
    Ui::MainWindow *ui;
    struct photoList {
        bool choosed;
        PhotoWidget* photowidget;
        QString path;
    };
    QMap<QString,QList<QList<photoList *>>> rendered_files;
    QProgressBar* progressBar = new QProgressBar();
    CliApp* cliapp;
};
#endif // MAINWINDOW_H
