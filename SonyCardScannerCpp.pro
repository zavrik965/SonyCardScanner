#   Этот файл — часть SonyCardScanner.
#   SonyCardScanner — свободная программа: вы можете перераспространять ее и/или изменять ее на условиях
#   Стандартной общественной лицензии GNU в том виде, в каком она была опубликована Фондом свободного программного обеспечения;
#   либо версии 3 лицензии, либо (по вашему выбору) любой более поздней версии.
#   SonyCardScanner распространяется в надежде, что она будет полезной, но БЕЗО ВСЯКИХ ГАРАНТИЙ;
#   даже без неявной гарантии ТОВАРНОГО ВИДА или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ.
#   Подробнее см. в Стандартной общественной лицензии GNU.
#   Вы должны были получить копию Стандартной общественной лицензии GNU вместе с этой программой.
#   Если это не так, см. <https://www.gnu.org/licenses/>.

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += opencv

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cliapp.cpp \
    main.cpp \
    mainwindow.cpp \
    photowidget.cpp

HEADERS += \
    cliapp.h \
    mainwindow.h \
    photowidget.h

FORMS += \
    mainwindow.ui

LIBS += \
    -lraw

LIBS += -lopencv_core -lopencv_videoio

INCLUDEPATH += /usr/include/opencv4

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /usr/bin/$${TARGET}
!isEmpty(target.path): INSTALLS += target
