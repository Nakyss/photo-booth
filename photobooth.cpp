#include "photobooth.h"
#include "ui_photobooth.h"
#include <QTimer>
#include <iostream>
#include "QUrl"

PhotoBooth::PhotoBooth(QWidget *parent)
    : QMainWindow(parent)
    , overlay(nullptr)
    , image(nullptr)
    , timer(new QTimer(this))
{
    //ui->setupUi(this);

    this->resize(250,250);
    button = new QPushButton("Ouvrir overlay",this);

    connect(button, SIGNAL(clicked()), this, SLOT(loadOverlay()));
    connect(this->timer, SIGNAL(timeout()), this, SLOT(hideOverlay()));
}

PhotoBooth::~PhotoBooth()
{
    //delete button;
}


void PhotoBooth::loadOverlay()
{
    QUrl imageUrl("https://thispersondoesnotexist.com/");
    m_pImgCtrl = new FileDownloader(imageUrl, this);

    connect(m_pImgCtrl, SIGNAL(downloaded()), SLOT(displayOverlay()));
}

void PhotoBooth::displayOverlay()
{
    std::cout << "Ouvrir" <<std::endl;

    image = new QPixmap();
    image->loadFromData(m_pImgCtrl->downloadedData());

    //image = new QPixmap("/home/raph/Téléchargements/v-bucks-bfb.png");
    overlay = new Overlay(nullptr,image);
    timer->start(2500);
}

void PhotoBooth::hideOverlay()
{
    std::cout << "Fermer" <<std::endl;
    delete overlay;
    overlay = nullptr;
    if (image != nullptr) {
        delete image;
        image = nullptr;
    }
    delete m_pImgCtrl;
    m_pImgCtrl = nullptr;
    timer->stop();
}


