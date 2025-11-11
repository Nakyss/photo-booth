#ifndef PHOTOBOOTH_H
#define PHOTOBOOTH_H

#include <QMainWindow>
#include <QPushButton>
#include "overlay.h"
#include "filedownloader.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class PhotoBooth;
}
QT_END_NAMESPACE

class PhotoBooth : public QMainWindow
{
    Q_OBJECT

public:
    PhotoBooth(QWidget *parent = nullptr);
    ~PhotoBooth();

private:
    Ui::PhotoBooth *ui;
    Overlay *overlay;        // Widget pour l'overlay
    QTimer *timer;           // Timer pour cacher l'overlay
    QPushButton *button;
    QPixmap *image;
    FileDownloader *m_pImgCtrl;


private slots:
    void displayOverlay();
    void hideOverlay();      // Méthode pour cacher l'overlay
    void loadOverlay();
};
#endif // PHOTOBOOTH_H
