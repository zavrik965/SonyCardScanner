#include "mainwindow.h"
#include "cliapp.h"

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDebug>

void printHelp() {
    qInfo() << "usage: SonyCardScanner [-h] [-t {mp4,mts,jpg,arw}] [-p PATH] [-d DIR] [-v] [-c | -g]\n";
    qInfo() << "options:";
    qInfo() << "  -h, --help            show this help message and exit";
    qInfo() << "  -t {mp4,mts,jpg,arw}, --type {mp4,mts,jpg,arw}";
    qInfo() << "                        Выбор типа медифайлов";
    qInfo() << "  -p PATH, --path PATH  Выбор пути медиафайлов на фотоаппаратуре";
    qInfo() << "  -d DIR, --dir DIR     Выбор пути сохранения медиафайлов";
    qInfo() << "  -v, --view            Выводит файлы которые были сохранены";
    qInfo() << "  -c, --cli             Режим работы в консоли (по умолчанию)k";
    qInfo() << "  -g, --gui             Режим работы в оконном режиме";
}

int main(int argc, char *argv[])
{
    QStringList rawArgs;
    for(int i=0; i < argc; i++) {
        rawArgs.append(QString(argv[i]));
    }
    QStringList needed_types = {"mp4", "mts", "jpg", "arw"};
    QMap<QString,QStringList> args;
    QStringList cases;

    cases << "-t" << "--type" << "-p" << "--path" << "-d" << "--dir" << "-v" << "--view" << "-c" << "--cli" << "-g" << "--gui" << "-h" << "--help";

    for(int i=0; i < argc; i++) {
        switch(cases.indexOf(rawArgs[i])){
        case 0:
        case 1:
            if(rawArgs[i + 1][0] != "-" && needed_types.contains(rawArgs[i + 1])) {
                if(args.keys().contains("type")) {
                    args["type"].append(rawArgs[i + 1]);
                } else {
                    args["type"] = QStringList({rawArgs[i + 1]});
                }
            }
            break;
        case 2:
        case 3:
            if(rawArgs[i + 1][0] != "-") {
                if(args.keys().contains("path")) {
                    args["path"].append(rawArgs[i + 1]);
                } else {
                    args["path"] = QStringList({rawArgs[i + 1]});
                }
            }
            break;
        case 4:
        case 5:
            if(rawArgs[i + 1][0] != "-") {
                if(args.keys().contains("dir")) {
                    args["dir"].append(rawArgs[i + 1]);
                } else {
                    args["dir"] = QStringList({rawArgs[i + 1]});
                }
            }
            break;
        case 6:
        case 7:
            args["view"].append("True");
            break;
        case 8:
        case 9:
            if(args.keys().contains("gui")) {
                qCritical() << "choose only one from [gui, cli]";
                return 1;
            }
            args["cli"].append("True");
            break;
        case 10:
        case 11:
            if(args.keys().contains("cli")) {
                qCritical() << "SonyCardScanner: error: argument -g/--gui: not allowed with argument -c/--cli";
                return 1;
            }
            args["gui"].append("True");
            break;
        case 12:
        case 13:
            printHelp();
            return 0;
            break;
        default:
            if(rawArgs[i][0] == "-"){
                qInfo() << "usage: SonyCardScanner [-h] [-t {mp4,mts,jpg,arw}] [-p PATH] [-d DIR] [-v] [-c | -g]";
                qInfo() << "SonyCardScanner: error: unrecognized arguments:" << rawArgs[i];
                return 1;
            }
        }
    }
    if(args.keys().contains("cli") && !args.keys().contains("gui")) {
        CliApp app(args);
        app.run();
    } else if(args.keys().contains("gui")) {
        QApplication a(argc, argv);
        MainWindow w(args);
        w.show();
        return a.exec();
    }
    return 0;
}
