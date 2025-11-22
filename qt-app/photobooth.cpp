#include "photobooth.h"
#include <QTimer>
#include <QUrl>

PhotoBooth::PhotoBooth(QWidget *parent)
    : QMainWindow(parent)
    , client(nullptr)
    , trayManager(nullptr)
{
    this->resize(500,500);

    infoText = new QLabel("Il n'y a rien a voir ici, tu peut fermer la fenetre ça reste en arrière plan", this);
    infoText->adjustSize();

    client = new WebSocketClient(QUrl(QStringLiteral("ws://localhost:59875/users")), this);

    trayManager = new TrayManager(this, this);

    // connexion pour recevoir l'URL et démarrer le téléchargement
    connect(client, &WebSocketClient::imageReceived,
            this, &PhotoBooth::handleImageUrl, Qt::QueuedConnection);
}

PhotoBooth::~PhotoBooth()
{
    //delete button;
}


void PhotoBooth::handleImageUrl(const QString &urlStr)
{
    QUrl url(urlStr);
    if (!url.isValid()) {
        qWarning() << "PhotoBooth::handleImageUrl url invalide:" << urlStr;
        return;
    }

    // si on peut créer une session maintenant, on le fait, sinon on met en queue
    if (m_sessions.size() < m_maxConcurrent) {
        Session *s = createSessionForUrl(url);
        m_sessions.append(s);
        s->start();
    } else {
        m_pendingQueue.enqueue(url);
    }
}

Session* PhotoBooth::createSessionForUrl(const QUrl &url)
{
    Session *s = new Session(url, m_displayMs, this);
    connect(s, &Session::finished, this, &PhotoBooth::onSessionFinished, Qt::QueuedConnection);
    return s;
}

void PhotoBooth::onSessionFinished(Session* session)
{
    // retire de la liste des sessions actives
    m_sessions.removeAll(session);

    // démarrer la prochaine de la file si présente
    tryStartNext();
}

void PhotoBooth::tryStartNext()
{
    while (!m_pendingQueue.isEmpty() && m_sessions.size() < m_maxConcurrent) {
        QUrl next = m_pendingQueue.dequeue();
        Session *s = createSessionForUrl(next);
        m_sessions.append(s);
        s->start();
    }
}

