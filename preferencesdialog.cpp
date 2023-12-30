#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include <QFileDialog>

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::on_sourceDirButton_clicked()
{
    QFileDialog dialog;
    QString source_dir = dialog.getExistingDirectory(this, "Выбор директории устройств", QDir::homePath());
    if(source_dir != ""){
        ui->standardSourceDir->setText(source_dir);
    }
}


void PreferencesDialog::on_outputDirButton_clicked()
{
    QFileDialog dialog;
    QString output_dir = dialog.getExistingDirectory(this, "Выбор директории выгрузки", QDir::homePath());
    if(output_dir != "") {
        ui->standardOutputDir->setText(output_dir);
    }

}

QString PreferencesDialog::getLanguage() {
    return ui->languages->currentText();
}

void PreferencesDialog::setLanguages(QStringList languages) {
    ui->languages->addItems(languages);
}

void PreferencesDialog::setLanguage(QString language) {
    ui->languages->setCurrentText(language);
}

QString PreferencesDialog::getStandardSourceDir() {
    return ui->standardSourceDir->text();
}

void PreferencesDialog::setStandardSourceDir(QString source_dir) {
    ui->standardSourceDir->setText(source_dir);
}

QString PreferencesDialog::getStandardOutputDir() {
    return ui->standardOutputDir->text();
}

void PreferencesDialog::setStandardOutputDir(QString output_dir) {
    ui->standardOutputDir->setText(output_dir);
}
