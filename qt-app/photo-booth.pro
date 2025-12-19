QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += websockets
QT += network
QT += texttospeech

CONFIG += c++17


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    filedownloader.cpp \
    main.cpp \
    overlay.cpp \
    photobooth.cpp \
    session.cpp \
    traymanager.cpp \
    websocketclient.cpp

HEADERS += \
    filedownloader.h \
    hosts.h \
    photobooth.h \
    overlay.h \
    photobooth.h \
    session.h \
    traymanager.h \
    websocketclient.h

macx {
    HEADERS += overlay_mac.h
}

RC_ICONS = data/icons/logo.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    data.qrc


macx {
    LIBS += -framework Cocoa -framework ApplicationServices -framework CoreGraphics
    OBJECTIVE_SOURCES += overlay_mac.mm
}

