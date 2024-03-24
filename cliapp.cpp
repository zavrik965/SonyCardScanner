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
#include "cliapp.h"
#include <QDebug>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

QString osVersion()
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

QString _recurent_search_path(QString path){
    QDir dir(path);
    QFileInfoList files = dir.entryInfoList();
    for (int i = 0; i < files.size(); i++) {
        QFileInfo fileInfo = files.at(i);
        QStringList fileInfoNames;
        if(fileInfo.fileName() != "." && fileInfo.fileName() != ".." && fileInfo.isDir()) {
            QString temp_path = _recurent_search_path(fileInfo.absoluteFilePath());
            if(temp_path != "") {
                return temp_path;
            }
        }
        if(fileInfo.isFile() && fileInfo.fileName() == "autorun.inf") {
            for(QFileInfo info : files){
                fileInfoNames.append(info.fileName());
            }
            if(fileInfoNames.contains("DCIM")) {
                QFileInfoList dcim_files = QDir(files.at(fileInfoNames.indexOf("DCIM")).absoluteFilePath()).entryInfoList();
                for(QFileInfo info : dcim_files){
                    if(info.fileName().endsWith("MSDCF") && QDir(info.absoluteFilePath()).entryInfoList().size() > 2 && QDir(info.absoluteFilePath()).entryInfoList()[2].fileName().startsWith("DSC")) {
                        return path;
                    }
                }
            }
            if(fileInfoNames.contains("MP_ROOT")) {
                QFileInfoList mp_root_files = QDir(files.at(fileInfoNames.indexOf("MP_ROOT")).absoluteFilePath()).entryInfoList();
                for(QFileInfo info : mp_root_files){
                    if(info.fileName().endsWith("ANV01") && QDir(info.absoluteFilePath()).entryInfoList().size() > 2 && QDir(info.absoluteFilePath()).entryInfoList()[2].fileName().endsWith(".MP4")) {
                        return path;
                    }
                }
            }
            if(fileInfoNames.contains("PRIVATE")) {
                QString delimeter;
                QStringList cases;
                cases << "linux" << "windows" << "macos";
                switch(cases.indexOf(osVersion())) {
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
                QFileInfoList mp_root_files = QDir(files.at(fileInfoNames.indexOf("PRIVATE")).absoluteFilePath()).entryInfoList();
                for(QFileInfo info : mp_root_files){
                    if(info.fileName() == ("AVCHD") && QDir(info.absoluteFilePath() + delimeter + "BDMV" + delimeter + "STREAM").exists()) {
                        return path;
                    }
                    if(info.fileName() == ("M4ROOT") && QDir(info.absoluteFilePath() + delimeter + "CLIP").exists()) {
                        return path;
                    }
                }
            }
        }
    }
    return "";
}

CliApp::CliApp(QMap<QString,QStringList> args) {
    QStringList cases;
    cases << "type" << "path" << "dir" << "view" << "cli" << "gui";
    for(QString key : args.keys()) {
        switch(cases.indexOf(key)){
        case 0:
            this->needed_types = args.value(key);
            break;
        case 1:
            this->source_dir = args.value(key)[0];
            break;
        case 2:
            this->output_dir = args.value(key)[0];
            break;
        case 3:
            this->is_viewing = true;
            break;
        case 5:
            this->forGui = true;
            break;
        }
    }
    if(this->output_dir == "") {
        QString delimeter;
        cases = QStringList({});
        cases << "linux" << "windows" << "macos";
        switch(cases.indexOf(osVersion())) {
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
        this->out_directory_root_path = QDir::homePath() + delimeter + "Изображения" + delimeter + "Sony" + delimeter;
        this->output_dir = this->out_directory_root_path + QDateTime::currentDateTime().toString("dd-MM-yyyy-HH-mm-ss");
    }
    this->search_path(false);
}

void CliApp::search_path(bool force) {
    QStringList cases;
    cases << "linux" << "windows" << "macos";
    if(this->source_dir != "" && !force) {
        if(!QDir(this->source_dir).exists() || _recurent_search_path(this->source_dir) != this->source_dir) {
            qCritical() << "Устройства не найдены, попробуйте указать путь вручную!";
            exit(1);
        }
        return;
    }
    switch(cases.indexOf(osVersion())) {
    case 0:
        this->device_root_path = "/run/media/" + QProcessEnvironment::systemEnvironment().value("USER") + "/";
        break;
    case 1:
        this->device_root_path = "D:\\";
        break;
    case 2:
        this->device_root_path = "/Volumes/";
        break;
    default:
        qInfo() << "Автоопределение устройства не работает в Вашей системе. Попробуйте ввести путь вручную";
        exit(0);
    }
    this->source_dir = _recurent_search_path(this->device_root_path);
    if(this->source_dir == "" && !this->forGui) {
        qCritical() << "Устройства не найдены, попробуйте указать путь вручную!";
        exit(1);
    } else if(this->source_dir == "") {
        qCritical() << "Устройства не найдены, попробуйте указать путь вручную!";
    }
}

void CliApp::run() {
    qInfo() << "Вас приветствует утилита сканирования дисков фотоаппаратуры Sony!";
    qInfo() << "Обнаружено устройство по адресу:" << this->source_dir;
    this->founded_files = {};
    for(QString file_type : this->needed_types) {
        founded_files[file_type] = QStringList({});
    }
    this->analyzer();
    if(!this->forGui) {
        this->save(this->founded_files);
    }
}

void CliApp::analyzer() {
    QString delimeter;
    QStringList cases;
    cases << "linux" << "windows" << "macos";
    switch(cases.indexOf(osVersion())) {
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
    QDir dir(this->source_dir);
    QFileInfoList files = dir.entryInfoList();
    for(QFileInfo file : files) {
        if(file.fileName() == "DCIM" && (this->needed_types.contains("jpg") || this->needed_types.contains("arw"))) {
            for(QFileInfo dcim_dir : QDir(file.absoluteFilePath()).entryInfoList()) {
                for(QFileInfo dcim_file : QDir(dcim_dir.absoluteFilePath()).entryInfoList()) {
                    QString file_type = dcim_file.fileName().split(".").back().toLower();
                    if(this->needed_types.contains(file_type)) {
                        this->founded_files[file_type].append(dcim_file.absoluteFilePath());
                    }
                }
            }
        }
        if(file.fileName() == "PRIVATE" && (this->needed_types.contains("mp4") || this->needed_types.contains("mts"))) {
            for(QFileInfo private_dir : QDir(file.absoluteFilePath()).entryInfoList()) {
                if(private_dir.fileName() == "AVCHD" && QDir(private_dir.absoluteFilePath() + delimeter + "BDMV" + delimeter + "STREAM").exists()) {
                    for(QFileInfo avchd_file : QDir(private_dir.absoluteFilePath() + delimeter + "BDMV" + delimeter + "STREAM").entryInfoList()) {
                        QString file_type = avchd_file.fileName().split(".").back().toLower();
                        if(file_type == "mts" && this->needed_types.contains(file_type)) {
                            this->founded_files[file_type].append(avchd_file.absoluteFilePath());
                        }
                    }
                }
                if(private_dir.fileName() == "M4ROOT" && QDir(private_dir.absoluteFilePath() + delimeter + "CLIP").exists()) {
                    for(QFileInfo m4root_file : QDir(private_dir.absoluteFilePath()).entryInfoList()) {
                        QString file_type = m4root_file.fileName().split(".").back().toLower();
                        if(file_type == "mp4" && this->needed_types.contains(file_type)) {
                            this->founded_files[file_type].append(m4root_file.absoluteFilePath());
                        }
                    }
                }
            }
        }
        if(file.fileName() == "MP_ROOT" && this->needed_types.contains("mp4")) {
            for(QFileInfo mp_root_dir : QDir(file.absoluteFilePath()).entryInfoList()) {
                for(QFileInfo mp_root_file : QDir(mp_root_dir.absoluteFilePath()).entryInfoList()) {
                    QString file_type = mp_root_file.fileName().split(".").back().toLower();
                    if(this->needed_types.contains(file_type)) {
                        this->founded_files[file_type].append(mp_root_file.absoluteFilePath());
                    }
                }
            }
        }
    }
}

void CliApp::save(QMap<QString, QStringList> founded_files, QProgressBar* indicator) {
    QString delimeter;
    QStringList cases;
    cases << "linux" << "windows" << "macos";
    switch(cases.indexOf(osVersion())) {
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
    int max_count_files = 0;
    int count=0;
    for(QStringList files : founded_files.values()) {
        max_count_files += files.size();
    }
    for(QString file_type : founded_files.keys()) {
        for(int i=0; i < 52; i++) {
            QTextStream(stdout) << " ";
        }
        QTextStream(stdout) << "\r";
        if(this->is_viewing) {
            QTextStream(stdout) << file_type + ":\n";
        }
        QString dirname = "";
        cases = QStringList({});
        cases << "jpg" << "arw" << "mp4" << "mts";
        switch(cases.indexOf(file_type)) {
        case 0:
            dirname = "PHOTO" + delimeter + "JPG";
            break;
        case 1:
            dirname = "PHOTO" + delimeter + "ARW";
            break;
        case 2:
            dirname = "VIDEO" + delimeter + "MP4";
            break;
        case 3:
            dirname = "VIDEO" + delimeter + "MTS";
            break;
        }
        for(QString file_path : founded_files.value(file_type)) {
            QDir().mkpath(this->output_dir + delimeter + dirname);
            QFile::copy(file_path, this->output_dir + delimeter + dirname + delimeter + QFileInfo(QFile(file_path)).fileName());
            count++;
            if(this->is_viewing) {
                for(int i=0; i < 52; i++) {
                    QTextStream(stdout) << " ";
                }
                QTextStream(stdout) << "\r\t-" + QFileInfo(QFile(file_path)).fileName() << "\n";
                QTextStream(stdout) << "Save [";
                for(int i=0; i < (int)((float)count / max_count_files * 40) - 1; i++) {
                    QTextStream(stdout) << "=";
                }
                QTextStream(stdout) << ">";
                for(int i=0; i < 40 - (int)((float)count / max_count_files * 40); i++) {
                    QTextStream(stdout) << " ";
                }
                QTextStream(stdout) << "] " << (int)((float)count / max_count_files * 100) << "%\r";
            }
            if(indicator != nullptr) {
                indicator->setValue(count * 100 / max_count_files);
            }
        }
    }
    QTextStream(stdout) << "\n";
}

QStringList CliApp::getNeededTypes() {
    return this->needed_types;
}

QMap<QString,QStringList> CliApp::getFoundedFiles() {
    return this->founded_files;
}

QString CliApp::getDirectoryPath() {
    return this->output_dir;
}
void CliApp::setDirectoryPath(QString path) {
    this->output_dir = path;
}

void CliApp::setDevicePath(QString path) {
    this->device_root_path = path;
    this->search_path(true);
}

QString CliApp::getSourceDir() {
    return this->source_dir;
}

QString CliApp::getDeviceRootPath() {
    return this->device_root_path;
}

void CliApp::setDeviceRootPath(QString path) {
    this->device_root_path = path;
}

QString CliApp::getOutDirectoryRootPath() {
    return this->out_directory_root_path;
}

void CliApp::setOutDirectoryRootPath(QString path) {
    this->out_directory_root_path = path;
}

QString CliApp::getLanguage() {
    return this->language;
}

void CliApp::setLanguage(QString language) {
    this->language = language;
}
