#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog();

    QString getLanguage();
    void setLanguage(QString language);
    void setLanguages(QStringList languages);

    QString getStandardSourceDir();
    void setStandardSourceDir(QString source_dir);

    QString getStandardOutputDir();
    void setStandardOutputDir(QString output_dir);

private slots:
    void on_sourceDirButton_clicked();
    void on_outputDirButton_clicked();

private:
    Ui::PreferencesDialog *ui;
};

#endif // PREFERENCESDIALOG_H
