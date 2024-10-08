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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QCloseEvent>
#include <QDateTime>
#include <QJsonDocument>
#include <QMessageBox>
#include <libraw/libraw.h>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/videoio.hpp>

QString OSVersion()
{
    static QString osVersion;
    if(osVersion.isEmpty())
    {
#if defined(Q_OS_LINUX)
        osVersion = "linux";
#elif defined(Q_OS_MAC)
        osVersion = "macos";
#elif defined(Q_OS_WIN)
        osVersion = "windows";
#else
        osVersion = "unknown";
#endif
    }
    return osVersion;
}

MainWindow::MainWindow(QMap<QString,QStringList> args, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->cliapp = new CliApp(args);
    this->rendered_files = {};
    for(QString file_type : this->cliapp->getNeededTypes()) {
        this->rendered_files[file_type] = QList({QList<photoList *>({})});
    }
    QString delimeter;
    QStringList cases;
    cases << "linux" << "windows" << "macos";
    switch(cases.indexOf(OSVersion())) {
    case 0:
        this->configPath=QDir::homePath() + "/.config/SonyCardScanner/";
        delimeter="/";
        break;
    case 1:
        this->configPath="C:\\Program Files\\SonyCardScanner\\";
        delimeter="\\";
        break;
    case 2:
        this->configPath=QDir::homePath() + "/Library/Preferences/SonyCardScanner/";
        delimeter="/";
        break;
    }
    if(QFileInfo(configPath + "preferences.conf").exists()) {
        QString data;
        QFile json_file;
        json_file.setFileName(configPath + "preferences.conf");
        json_file.open(QIODevice::ReadOnly | QIODevice::Text);
        data = json_file.readAll();
        json_file.close();
        preferences = QJsonDocument::fromJson(data.toUtf8()).object();
        this->cliapp->setDeviceRootPath(preferences["sourceDir"].toString());
        this->cliapp->setLanguage(preferences["language"].toString());
        this->cliapp->setOutDirectoryRootPath(preferences["outputDir"].toString());
        cases = QStringList({});
        cases << "Русский" << "English";
        switch (cases.indexOf(preferences["language"].toString())) {
        case 0:
            this->translator.load(":/translates/SonyCardScanner_ru.qm");
            break;
        case 1:
            this->translator.load(":/translates/SonyCardScanner_en.qm");
            break;
        }
        qApp->installTranslator(&translator);
    } else {
        cases = QStringList({});
        cases << "ru_RU" << "en_US";
        switch (cases.indexOf(QLocale::system().name())) {
        case 0:
            this->cliapp->setLanguage("Русский");
            break;
        case 1:
            this->cliapp->setLanguage("English");
            break;
        default:
            this->cliapp->setLanguage("Русский");
            break;
        }
        QDir().mkpath(configPath);
        preferences["sourceDir"] = this->cliapp->getDeviceRootPath();
        preferences["outputDir"] = this->cliapp->getOutDirectoryRootPath();
        preferences["language"] = this->cliapp->getLanguage();

        QFile json_file;
        json_file.setFileName(configPath + "preferences.conf");
        json_file.open(QIODevice::WriteOnly);
        json_file.write(QJsonDocument(preferences).toJson(QJsonDocument::Indented));
        json_file.close();

    }


    ui->arwTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui->arwTableWidget->horizontalHeader()->setVisible(false);
    ui->arwTableWidget->verticalHeader()->setVisible(false);
    ui->arwTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->jpgTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui->jpgTableWidget->horizontalHeader()->setVisible(false);
    ui->jpgTableWidget->verticalHeader()->setVisible(false);
    ui->jpgTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->mtsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui->mtsTableWidget->horizontalHeader()->setVisible(false);
    ui->mtsTableWidget->verticalHeader()->setVisible(false);
    ui->mtsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->mp4TableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui->mp4TableWidget->horizontalHeader()->setVisible(false);
    ui->mp4TableWidget->verticalHeader()->setVisible(false);
    ui->mp4TableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->progressBar->setMaximum(100);
    this->progressBar->setValue(0);
    ui->statusbar->setSizeGripEnabled(false);
    ui->statusbar->addPermanentWidget(progressBar, 1);

    ui->directoryPath->setText(this->cliapp->getDirectoryPath());
    ui->devicePath->setText(this->cliapp->getSourceDir());
    QIcon icon = QIcon(":/icons/icon.png");
    this->trayIcon = new QSystemTrayIcon(this);
    this->trayIcon->setIcon(icon);
    this->trayIcon->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent * event) {
    this->trayIcon->hide();
    event->accept();
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}

void MainWindow::on_arwTableWidget_cellDoubleClicked(int row, int column)
{
    if(row < this->rendered_files["arw"].size() && column < this->rendered_files["arw"][row].size()) {
        this->rendered_files["arw"][row][column]->choosed = false;
        this->rendered_files["arw"][row][column]->photowidget->setChoosed(false);
    }
}

void MainWindow::on_jpgTableWidget_cellDoubleClicked(int row, int column)
{
    if(row < this->rendered_files["jpg"].size() && column < this->rendered_files["jpg"][row].size()) {
        this->rendered_files["jpg"][row][column]->choosed = false;
        this->rendered_files["jpg"][row][column]->photowidget->setChoosed(false);
    }
}

void MainWindow::on_mp4TableWidget_cellDoubleClicked(int row, int column)
{
    if(row < this->rendered_files["mp4"].size() && column < this->rendered_files["mp4"][row].size()) {
        this->rendered_files["mp4"][row][column]->choosed = false;
        this->rendered_files["mp4"][row][column]->photowidget->setChoosed(false);
    }
}

void MainWindow::on_mtsTableWidget_cellDoubleClicked(int row, int column)
{
    if(row < this->rendered_files["mts"].size() && column < this->rendered_files["mts"][row].size()) {
        this->rendered_files["mts"][row][column]->choosed = false;
        this->rendered_files["mts"][row][column]->photowidget->setChoosed(false);
    }

}

void MainWindow::on_arwTableWidget_itemSelectionChanged()
{
    for(QModelIndex el: ui->arwTableWidget->selectionModel()->selectedIndexes()) {
        if(el.row() < this->rendered_files["arw"].size() && el.column() < this->rendered_files["arw"][el.row()].size()) {
            this->rendered_files["arw"][el.row()][el.column()]->choosed = true;
            this->rendered_files["arw"][el.row()][el.column()]->photowidget->setChoosed(true);
        }
    }
}

void MainWindow::on_jpgTableWidget_itemSelectionChanged()
{
    for(QModelIndex el: ui->jpgTableWidget->selectionModel()->selectedIndexes()) {
        if(el.row() < this->rendered_files["jpg"].size() && el.column() < this->rendered_files["jpg"][el.row()].size()) {
            this->rendered_files["jpg"][el.row()][el.column()]->choosed = true;
            this->rendered_files["jpg"][el.row()][el.column()]->photowidget->setChoosed(true);
        }
    }
}

void MainWindow::on_mp4TableWidget_itemSelectionChanged()
{
    for(QModelIndex el: ui->mp4TableWidget->selectionModel()->selectedIndexes()) {
        if(el.row() < this->rendered_files["mp4"].size() && el.column() < this->rendered_files["mp4"][el.row()].size()) {
            this->rendered_files["mp4"][el.row()][el.column()]->choosed = true;
            this->rendered_files["mp4"][el.row()][el.column()]->photowidget->setChoosed(true);
        }
    }
}

void MainWindow::on_mtsTableWidget_itemSelectionChanged()
{
    for(QModelIndex el: ui->mtsTableWidget->selectionModel()->selectedIndexes()) {
        if(el.row() < this->rendered_files["mts"].size() && el.column() < this->rendered_files["mts"][el.row()].size()) {
            this->rendered_files["mts"][el.row()][el.column()]->choosed = true;
            this->rendered_files["mts"][el.row()][el.column()]->photowidget->setChoosed(true);
        }
    }
}


void MainWindow::on_scanAction_triggered()
{
    this->progressBar->setValue(0);
    this->cliapp->run();
    ui->arwTableWidget->clear();
    ui->jpgTableWidget->clear();
    while(ui->arwTableWidget->rowCount() > 0) {
        ui->arwTableWidget->removeRow(0);
    }
    while(ui->arwTableWidget->columnCount() > 0) {
        ui->arwTableWidget->removeColumn(0);
    }
    while(ui->jpgTableWidget->rowCount() > 0) {
        ui->jpgTableWidget->removeRow(0);
    }
    while(ui->jpgTableWidget->columnCount() > 0) {
        ui->jpgTableWidget->removeColumn(0);
    }
    ui->mp4TableWidget->clear();
    ui->mtsTableWidget->clear();
    while(ui->mp4TableWidget->rowCount() > 0) {
        ui->mp4TableWidget->removeRow(0);
    }
    while(ui->mp4TableWidget->columnCount() > 0) {
        ui->mp4TableWidget->removeColumn(0);
    }
    while(ui->mtsTableWidget->rowCount() > 0) {
        ui->mtsTableWidget->removeRow(0);
    }
    while(ui->mtsTableWidget->columnCount() > 0) {
        ui->mtsTableWidget->removeColumn(0);
    }

    int max_count_files = 0;
    for(QStringList files_of_type: this->cliapp->getFoundedFiles().values()) {
        max_count_files += files_of_type.size();
    }
    int count_files = 0;
    int rowPosition;
    int columnPosition;
    int k = 0, k_1 = 0;
    int count_elements;
    LibRaw rawimg;
    QPixmap qimg;
    QFile img_file;
    if(this->cliapp->getFoundedFiles().keys().contains("arw")) {
        for(QString arw_file: this->cliapp->getFoundedFiles()["arw"]) {
            k_1 = k;
            rowPosition = ui->arwTableWidget->rowCount();
            columnPosition = ui->arwTableWidget->columnCount();
            if(rowPosition == 0) {
                ui->arwTableWidget->insertRow(rowPosition);
                ui->arwTableWidget->setRowHeight(rowPosition, 120);
            }
            if(columnPosition < (int)this->width() / 120) {
                ui->arwTableWidget->insertColumn(columnPosition);
            } else if(k % columnPosition == 0) {
                ui->arwTableWidget->insertRow(rowPosition);
                ui->arwTableWidget->setRowHeight(rowPosition, 120);
                rowPosition++;
                this->rendered_files["arw"].append(QList({QList<photoList *>({})}));
                k_1 = 0;
            }
            count_elements = 0;
            for(QList<photoList *> row : this->rendered_files["arw"]) {
                count_elements += row.size();
            }
            photoList* cell_element = new photoList;
            PhotoWidget* cell;
            if(count_elements <= k_1 || k_1 == 0 || k_1 >= this->rendered_files["arw"].size() || (rowPosition > 0 ? rowPosition - 1 : 0) >= this->rendered_files["arw"][k_1].size()) {
                rawimg.recycle();
                rawimg.open_file(arw_file.toLocal8Bit());
                rawimg.unpack_thumb();
                qimg.loadFromData(reinterpret_cast <unsigned char *> (rawimg.imgdata.thumbnail.thumb), rawimg.imgdata.thumbnail.tlength);
                qimg = qimg.scaled(QSize(120, 120), Qt::AspectRatioMode::KeepAspectRatio);
                cell = new PhotoWidget(this);
                cell->setImage(qimg, QFileInfo(QFile(arw_file)).baseName());
                cell_element->choosed = false;
                cell_element->path = arw_file;
                cell_element->photowidget = cell;
                this->rendered_files["arw"][rowPosition > 0 ? rowPosition - 1 : 0].append(cell_element);
            } else {
                cell = this->rendered_files["arw"][rowPosition > 0 ? rowPosition - 1 : 0][columnPosition < k_1 ? k_1 % columnPosition : k_1]->photowidget;
            }
            ui->arwTableWidget->setCellWidget(rowPosition > 0 ? rowPosition - 1 : 0, columnPosition < k_1 ? k_1 % columnPosition : k_1, cell);
            count_files++;
            this->progressBar->setValue(count_files * 100 / max_count_files);
            k++;
        }
    }
    k = 0;
    if(this->cliapp->getFoundedFiles().keys().contains("jpg")) {
        for(QString jpg_file: this->cliapp->getFoundedFiles()["jpg"]) {
            k_1 = k;
            rowPosition = ui->jpgTableWidget->rowCount();
            columnPosition = ui->jpgTableWidget->columnCount();
            if(rowPosition == 0) {
                ui->jpgTableWidget->insertRow(rowPosition);
                ui->jpgTableWidget->setRowHeight(rowPosition, 120);
            }
            if(columnPosition < (int)this->width() / 120) {
                ui->jpgTableWidget->insertColumn(columnPosition);
            } else if(k % columnPosition == 0) {
                ui->jpgTableWidget->insertRow(rowPosition);
                ui->jpgTableWidget->setRowHeight(rowPosition, 120);
                rowPosition++;
                this->rendered_files["jpg"].append(QList({QList<photoList *>({})}));
                k_1 = 0;
            }
            count_elements = 0;
            for(QList<photoList *> row : this->rendered_files["jpg"]) {
                count_elements += row.size();
            }
            photoList* cell_element = new photoList;
            PhotoWidget* cell;
            if(count_elements <= k_1 || k_1 == 0 || k_1 >= this->rendered_files["jpg"].size() || (rowPosition > 0 ? rowPosition - 1 : 0) >= this->rendered_files["jpg"][k_1].size()) {
                qimg.load(jpg_file);
                qimg = qimg.scaled(QSize(120, 120), Qt::AspectRatioMode::KeepAspectRatio);
                cell = new PhotoWidget(this);
                cell->setImage(qimg, QFileInfo(QFile(jpg_file)).baseName());
                cell_element->choosed = false;
                cell_element->path = jpg_file;
                cell_element->photowidget = cell;
                this->rendered_files["jpg"][rowPosition > 0 ? rowPosition - 1 : 0].append(cell_element);
            } else {
                cell = this->rendered_files["jpg"][rowPosition > 0 ? rowPosition - 1 : 0][columnPosition < k_1 ? k_1 % columnPosition : k_1]->photowidget;
            }
            ui->jpgTableWidget->setCellWidget(rowPosition > 0 ? rowPosition - 1 : 0, columnPosition < k_1 ? k_1 % columnPosition : k_1, cell);
            count_files++;
            this->progressBar->setValue(count_files * 100 / max_count_files);
            k++;
        }
    }
    k = 0;
    if(this->cliapp->getFoundedFiles().keys().contains("mts")) {
        for(QString mts_file: this->cliapp->getFoundedFiles()["mts"]) {
            k_1 = k;
            rowPosition = ui->mtsTableWidget->rowCount();
            columnPosition = ui->mtsTableWidget->columnCount();
            if(rowPosition == 0) {
                ui->mtsTableWidget->insertRow(rowPosition);
                ui->mtsTableWidget->setRowHeight(rowPosition, 120);
            }
            if(columnPosition < (int)this->width() / 120) {
                ui->mtsTableWidget->insertColumn(columnPosition);
            } else if(k % columnPosition == 0) {
                ui->mtsTableWidget->insertRow(rowPosition);
                ui->mtsTableWidget->setRowHeight(rowPosition, 120);
                rowPosition++;
                this->rendered_files["mts"].append(QList({QList<photoList *>({})}));
                k_1 = 0;
            }
            count_elements = 0;
            for(QList<photoList *> row : this->rendered_files["mts"]) {
                count_elements += row.size();
            }
            photoList* cell_element = new photoList;
            PhotoWidget* cell;
            if(count_elements <= k_1 || k_1 == 0 || k_1 >= this->rendered_files["mts"].size() || (rowPosition > 0 ? rowPosition - 1 : 0) >= this->rendered_files["mts"][k_1].size()) {
                cv::VideoCapture cap;
                cap.open(mts_file.toStdString());
                cv::Mat frame;
                cap.read(frame);
                qimg = QPixmap::fromImage(QImage((unsigned char*)frame.data, frame.cols, frame.rows, QImage::Format_BGR888));
                qimg = qimg.scaled(QSize(120, 120), Qt::AspectRatioMode::KeepAspectRatio);
                cell = new PhotoWidget(this);
                cell->setImage(qimg, QFileInfo(QFile(mts_file)).baseName());
                cell_element->choosed = false;
                cell_element->path = mts_file;
                cell_element->photowidget = cell;
                this->rendered_files["mts"][rowPosition > 0 ? rowPosition - 1 : 0].append(cell_element);
            } else {
                cell = this->rendered_files["mts"][rowPosition > 0 ? rowPosition - 1 : 0][columnPosition < k_1 ? k_1 % columnPosition : k_1]->photowidget;
            }
            ui->mtsTableWidget->setCellWidget(rowPosition > 0 ? rowPosition - 1 : 0, columnPosition < k_1 ? k_1 % columnPosition : k_1, cell);
            count_files++;
            this->progressBar->setValue(count_files * 100 / max_count_files);
            k++;
        }
    }
    k = 0;
    if(this->cliapp->getFoundedFiles().keys().contains("mp4")) {
        for(QString mp4_file: this->cliapp->getFoundedFiles()["mp4"]) {
            k_1 = k;
            rowPosition = ui->mp4TableWidget->rowCount();
            columnPosition = ui->mp4TableWidget->columnCount();
            if(rowPosition == 0) {
                ui->mp4TableWidget->insertRow(rowPosition);
                ui->mp4TableWidget->setRowHeight(rowPosition, 120);
            }
            if(columnPosition < (int)this->width() / 120) {
                ui->mp4TableWidget->insertColumn(columnPosition);
            } else if(k % columnPosition == 0) {
                ui->mp4TableWidget->insertRow(rowPosition);
                ui->mp4TableWidget->setRowHeight(rowPosition, 120);
                rowPosition++;
                this->rendered_files["mp4"].append(QList({QList<photoList *>({})}));
                k_1 = 0;
            }
            count_elements = 0;
            for(QList<photoList *> row : this->rendered_files["mp4"]) {
                count_elements += row.size();
            }
            photoList* cell_element = new photoList;
            PhotoWidget* cell;
            if(count_elements <= k_1 || k_1 == 0 || k_1 >= this->rendered_files["mp4"].size() || (rowPosition > 0 ? rowPosition - 1 : 0) >= this->rendered_files["mp4"][k_1].size()) {
                cv::VideoCapture cap;
                cap.open(mp4_file.toStdString());
                cv::Mat frame;
                cap.read(frame);
                qimg = QPixmap::fromImage(QImage((unsigned char*)frame.data, frame.cols, frame.rows, QImage::Format_BGR888));
                qimg = qimg.scaled(QSize(120, 120), Qt::AspectRatioMode::KeepAspectRatio);
                cell = new PhotoWidget(this);
                cell->setImage(qimg, QFileInfo(QFile(mp4_file)).baseName());
                cell_element->choosed = false;
                cell_element->path = mp4_file;
                cell_element->photowidget = cell;
                this->rendered_files["mp4"][rowPosition > 0 ? rowPosition - 1 : 0].append(cell_element);
            } else {
                cell = this->rendered_files["mp4"][rowPosition > 0 ? rowPosition - 1 : 0][columnPosition < k_1 ? k_1 % columnPosition : k_1]->photowidget;
            }
            ui->mp4TableWidget->setCellWidget(rowPosition > 0 ? rowPosition - 1 : 0, columnPosition < k_1 ? k_1 % columnPosition : k_1, cell);
            count_files++;
            this->progressBar->setValue(count_files * 100 / max_count_files);
            k++;
        }
    }
}

void MainWindow::on_selectAction_triggered()
{
    QString type="";
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        type = "arw";
        break;
    case 1:
        type = "jpg";
        break;
    case 2:
        type = "mts";
        break;
    case 3:
        type = "mp4";
        break;
    }
    for(QList<photoList *> row : this->rendered_files[type]) {
        for(photoList* image : row) {
            image->choosed = true;
            image->photowidget->setChoosed(true);
        }
    }
    ui->tabWidget->update();
}

void MainWindow::on_deselectAction_triggered()
{
    QString type="";
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        type = "arw";
        break;
    case 1:
        type = "jpg";
        break;
    case 2:
        type = "mts";
        break;
    case 3:
        type = "mp4";
        break;
    }
    for(QList<photoList *> row : this->rendered_files[type]) {
        for(photoList* image : row) {
            image->choosed = false;
            image->photowidget->setChoosed(false);
        }
    }
    ui->tabWidget->update();
}

void MainWindow::on_chooseDirectoryButton_clicked()
{
    QFileDialog dialog;
    QString output_dir = dialog.getExistingDirectory(this, tr("Выбор директории"), QDir::homePath());
    if(output_dir != "") {
        this->cliapp->setDirectoryPath(output_dir);
    }
    ui->directoryPath->setText(this->cliapp->getDirectoryPath());
}


void MainWindow::on_saveButton_clicked()
{
    QMap<QString,QStringList> pack;
    for(QString file_type : this->cliapp->getNeededTypes()) {
        pack[file_type] = QStringList({});
    }
    for(QString type : this->rendered_files.keys()){
        for(QList<photoList *> row : this->rendered_files[type]) {
            for(photoList* image : row) {
                if(image->choosed){
                    pack[type].append(image->path);
                }
            }
        }
    }
    this->cliapp->save(pack, this->progressBar);
}


void MainWindow::on_selectDevice_triggered()
{
    QIcon icon = QIcon(":/icons/icon.png");
    QFileDialog dialog;
    QString device_dir = dialog.getExistingDirectory(this, tr("Выбор директории устройства"), QDir::homePath());
    if(device_dir != "") {
        this->cliapp->setDevicePath(device_dir);
    }
    if(this->cliapp->getSourceDir() != "") {
        this->trayIcon->showMessage(tr("Выбор устройства"), tr("Устройство успешно обнаружено"), icon, 3000);
    } else {
        this->trayIcon->showMessage(tr("Выбор устройства"), tr("Устройство не обнаружено. Проверьте пожалуйста путь"), icon, 3000);
    }
    ui->devicePath->setText(this->cliapp->getSourceDir());
}


void MainWindow::on_settings_triggered()
{
    PreferencesDialog * preferences_dialog = new PreferencesDialog();
    preferences_dialog->setLanguages(QStringList({"English", "Русский"}));
    preferences_dialog->setLanguage(this->preferences["language"].toString());
    QString standardSource;
    QString delimeter;
    QStringList cases;
    cases << "linux" << "windows" << "macos";
    switch(cases.indexOf(OSVersion())) {
    case 0:
        delimeter="/";
        break;
    case 1:
        delimeter="\\";
        break;
    case 2:

        delimeter="/";
        break;
    }
    preferences_dialog->setStandardSourceDir(preferences["sourceDir"].toString());
    preferences_dialog->setStandardOutputDir(preferences["outputDir"].toString());

    int answer = preferences_dialog->exec();
    if(answer == QDialog::Accepted) {
        preferences["language"] = preferences_dialog->getLanguage();
        preferences["sourceDir"] = preferences_dialog->getStandardSourceDir();
        preferences["outputDir"] = preferences_dialog->getStandardOutputDir();
        QFile file;
        file.setFileName(configPath + "preferences.conf");
        file.open(QIODevice::WriteOnly);
        file.write(QJsonDocument(preferences).toJson(QJsonDocument::Indented));
        file.close();
        cases = QStringList({});
        cases << "Русский" << "English";
        switch (cases.indexOf(preferences["language"].toString())) {
        case 0:
            this->translator.load(":/translates/SonyCardScanner_ru.qm");
            break;
        case 1:
            this->translator.load(":/translates/SonyCardScanner_en.qm");
            break;
        }
    }
    qApp->installTranslator(&translator);
    delete preferences_dialog;
}


void MainWindow::on_aboutAction_triggered()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("О приложении"));
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setIconPixmap(QPixmap(":/icons/icon.svg"));
    msgBox.setText(tr("SonyCardScanner - утилита для сканирования папок на карте памяти фотокамер Sony.<br>SonyCardScanner — свободная программа, распространяемая под лицензией GPL3.<br>SonyCardScanner распространяется в надежде, что она будет полезной, но БЕЗО ВСЯКИХ ГАРАНТИЙ даже без неявной гарантии ТОВАРНОГО ВИДА или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ.<br>Автор:<br><a href='mailto:zaz965@stm32f0.ru'>ZAvrikDinozavrik</a><br>Исходный код:<br><a href='https://git.alexavr.ru/ZAvrikDinozavrik/SonyCardScanner'>https://git.alexavr.ru/ZAvrikDinozavrik/SonyCardScanner</a>"));
    msgBox.exec();
}

