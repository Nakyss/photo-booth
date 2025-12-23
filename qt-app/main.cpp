#include "photobooth.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << "No system tray available, exiting.";
    }

    PhotoBooth w;
    //w.show();
    a.setQuitOnLastWindowClosed(false);
    return a.exec();
}
