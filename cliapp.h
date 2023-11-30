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
#ifndef CLIAPP_H
#define CLIAPP_H
#include <QString>
#include <QStringList>
#include <stdbool.h>
#include <QMap>
#include <QProgressBar>

class CliApp
{
public:
    CliApp(QMap<QString,QStringList> args);
    void run();
    //void save(QMap<QString,QStringList> founded_files);
    void save(QMap<QString,QStringList> founded_files, QProgressBar* indicator=nullptr);
    QStringList getNeededTypes();
    QMap<QString,QStringList> getFoundedFiles();
    QString getDirectoryPath();
    void setDirectoryPath(QString path);
private:
    QString output_dir="";
    QString source_dir="";
    QStringList needed_types = {"mp4", "mts", "jpg", "arw"};
    bool is_viewing = false;
    bool forGui = false;
    QMap<QString,QStringList> founded_files;
    void search_path();
    void analyzer();
};

#endif // CLIAPP_H
