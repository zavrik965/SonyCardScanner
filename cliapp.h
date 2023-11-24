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
