#include "photobooth.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << "No system tray available, exiting.";
        // tu peux choisir de continuer quand même, mais l'icône ne sera pas affichée
    }

    PhotoBooth w;
    a.setQuitOnLastWindowClosed(false);
    return a.exec();
}
