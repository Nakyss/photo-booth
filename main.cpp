#include "photobooth.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PhotoBooth w;
    w.show();
    return a.exec();
}
