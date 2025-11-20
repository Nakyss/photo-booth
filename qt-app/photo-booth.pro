QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += websockets
QT += network

CONFIG += c++17


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    filedownloader.cpp \
    main.cpp \
    overlay.cpp \
    photobooth.cpp \
    traymanager.cpp \
    websocketclient.cpp

HEADERS += \
    filedownloader.h \
    photobooth.h \
    overlay.h \
    photobooth.h \
    traymanager.h \
    websocketclient.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    data.qrc
