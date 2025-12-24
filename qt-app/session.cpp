#include "session.h"
#include "overlay_mac.h"
#include <QDebug>
#include <QOperatingSystemVersion>

Session::Session(const QUrl &imageUrl, const QString &message, int displayMs, bool hasTTS, QObject *parent)
    : QObject(parent), m_url(imageUrl), hasTTS(hasTTS), message(message), m_displayMs(displayMs)
{
    if (hasTTS){
        speech = new QTextToSpeech();

        QVoice voiceSelected;

        QList<QVoice> bestVoices = speech->findVoices("French (France)+UniversalRobot");
        QList<QVoice> frenchVoices = speech->findVoices(QLocale("fr-FR"));

        if (bestVoices.size() == 1){
            voiceSelected = bestVoices.constFirst();
        }
        else if (frenchVoices.size() >= 1){
            voiceSelected = frenchVoices.constFirst();
        }
        else{
            voiceSelected = speech->availableVoices().constFirst();
        }


        qDebug() << "Nombre de voix precis :" << bestVoices.size();
        qDebug() << "Nombre de voix FR :" <<frenchVoices.size();
        qDebug() << "Nombre de voix:" << speech->availableVoices().size();
        //speech->setLocale(QLocale("fr-FR"));
        speech->setRate(-0.25);
        speech->setPitch(0.6);
        speech->setVoice(voiceSelected);
        qDebug() << voiceSelected;
    }
}

Session::~Session()
{
    if (hasTTS){
        speech->stop();
        delete speech;
        speech = nullptr;
    }
}

void Session::start()
{
    if (!m_url.isValid()) {
        qWarning() << "ImageSession::start url invalide:" << m_url;
        emit finished(this);
        deleteLater();
        return;
    }

    // Crée le downloader avec 'this' comme parent pour la gestion mémoire
    m_downloader = new FileDownloader(m_url, this);
    // FileDownloader émet downloaded() quand terminé — adapte si le signal est différent
    connect(m_downloader, &FileDownloader::downloaded, this, &Session::onDownloaded, Qt::QueuedConnection);

    // Si FileDownloader supporte une erreur signal, connecte-la et gère le fallback
    // ex: connect(m_downloader, &FileDownloader::errorOccured, this, [...]);
}

void Session::onDownloaded()
{
    // récupère les données
    QByteArray data = m_downloader->downloadedData();
    if (data.isEmpty()) {
        qWarning() << "ImageSession::onDownloaded data vide pour" << m_url;
        emit finished(this);
        deleteLater();
        return;
    }

    m_pixmap = new QPixmap();
    if (!m_pixmap->loadFromData(data)) {
        qWarning() << "ImageSession::onDownloaded impossible de charger pixmap";
        delete m_pixmap;
        m_pixmap = nullptr;
        emit finished(this);
        deleteLater();
        return;
    }

    // crée et affiche l'overlay — parent à nullptr pour qu'il apparaisse comme fenêtre indépendante
    #ifdef Q_OS_MAC
        mac_overlay = new OverlayMac(nullptr, m_pixmap);
    #else
        m_overlay = new Overlay(m_pixmap);
    #endif

    // Timer de durée d'affichage : singleShot pour auto-terminer la session
    QTimer::singleShot(m_displayMs, this, &Session::onTimeout);

    if (hasTTS){
        speech->say(message);
    }

}

void Session::onTimeout()
{
#ifdef Q_OS_MAC
    // --- MAC OS ---
    if (mac_overlay) {
        mac_overlay->close();
        mac_overlay->deleteLater();
        mac_overlay = nullptr;
    }
#else
    // --- WINDOWS / LINUX ---
    if (m_overlay) {
        m_overlay->close();
        m_overlay->deleteLater();
        m_overlay = nullptr;
    }
#endif

    // Libère le QPixmap
    if (m_pixmap) {
        delete m_pixmap;
        m_pixmap = nullptr;
    }

    // Nettoyage du downloader
    if (m_downloader) {
        m_downloader->deleteLater();
        m_downloader = nullptr;
    }

    // Signale la fin de la session
    emit finished(this);

    // Auto-destruction de la session
    deleteLater();
}


