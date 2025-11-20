#include "photobooth.h"
#include <QTimer>
#include <iostream>
#include "QUrl"

PhotoBooth::PhotoBooth(QWidget *parent)
    : QMainWindow(parent)
    , overlay(nullptr)
    , timer(new QTimer(this))
    , image(nullptr)
    , client(nullptr)
    , trayManager(nullptr)
{

    client = new WebSocketClient(QUrl(QStringLiteral("wss://photo.nakyss.fr")), this);

    trayManager = new TrayManager(this, this);

    // connexion pour recevoir l'URL et démarrer le téléchargement
    connect(client, &WebSocketClient::imageReceived,
            this, &PhotoBooth::loadOverlay, Qt::QueuedConnection);

    connect(this->timer, SIGNAL(timeout()), this, SLOT(hideOverlay()));
}

PhotoBooth::~PhotoBooth()
{
    //delete button;
}


void PhotoBooth::loadOverlay(const QString &overlay)
{
    if (overlay.isEmpty()) {
        qWarning() << "loadOverlay: url vide";
        return;
    }

    QUrl imageUrl(overlay);
    std::cout<< "Debut du téléchargement" << std::endl;

    m_pImgCtrl = new FileDownloader(imageUrl, this);

    connect(m_pImgCtrl, SIGNAL(downloaded()), SLOT(displayOverlay()));
}

void PhotoBooth::displayOverlay()
{
    std::cout << "Ouvrir" <<std::endl;

    image = new QPixmap();
    image->loadFromData(m_pImgCtrl->downloadedData());

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


