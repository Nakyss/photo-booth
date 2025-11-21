#include "session.h"
#include <QDebug>

Session::Session(const QUrl &imageUrl, int displayMs, QObject *parent)
    : QObject(parent), m_url(imageUrl), m_displayMs(displayMs)
{
}

Session::~Session()
{
    // les objets parents seront détruits par Qt si parentés ; vérifier si besoin
    if (m_pixmap) {
        delete m_pixmap;
        m_pixmap = nullptr;
    }
    // m_overlay et m_downloader sont deleteLater() dans la logique
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
    QByteArray data = m_downloader->downloadedData(); // adapte si l'API est différente
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
    m_overlay = new Overlay(nullptr, m_pixmap);
    m_overlay->show();

    // Timer de durée d'affichage : singleShot pour auto-terminer la session
    QTimer::singleShot(m_displayMs, this, &Session::onTimeout);
}

void Session::onTimeout()
{
    // Fermer l'overlay proprement
    if (m_overlay) {
        m_overlay->close();
        m_overlay->deleteLater();
        m_overlay = nullptr;
    }

    // libère pixmap
    if (m_pixmap) {
        delete m_pixmap;
        m_pixmap = nullptr;
    }

    // le downloader appartient à 'this' ; on peut le supprimer aussi
    if (m_downloader) {
        m_downloader->deleteLater();
        m_downloader = nullptr;
    }

    // Signal à PhotoBooth pour nettoyage / démarrage file
    emit finished(this);

    // s'auto-détruit
    deleteLater();
}

