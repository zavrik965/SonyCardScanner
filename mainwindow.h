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
