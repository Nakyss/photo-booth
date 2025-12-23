#include "photobooth.h"
#include <QTimer>
#include <QUrl>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include "hosts.h"
#include "ui_photobooth.h"

PhotoBooth::PhotoBooth(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PhotoBooth)
    , client(nullptr)
    , trayManager(nullptr)
{
    ui->setupUi(this);

    // this->resize(500,500);

    // infoText = new QLabel("Entrée un ID de groupe");


    // Appliquer le layout au widget central (et non à "this")
    // QGridLayout *grid = new QGridLayout(this);

    // QLineEdit *input = new QLineEdit();
    // QVBoxLayout *vlayout = new QVBoxLayout();

    // vlayout->addWidget(infoText);
    // vlayout->addWidget(input);
    // vlayout->addWidget(new QLabel("jfduishfyusgufuyds"));


    // grid->addLayout(vlayout, 0, 0);

    client = new WebSocketClient(QUrl(WS_URL), this);

    trayManager = new TrayManager(this, this);

    // connexion pour recevoir l'URL et démarrer le téléchargement
    connect(client, &WebSocketClient::imageReceived,
            this, &PhotoBooth::handlePhotoBooth, Qt::QueuedConnection);

    // si WebSocketClient expose connected() et disconnected() :
    connect(client, &WebSocketClient::connected,
            trayManager, [this]() { trayManager->setConnected(true); }, Qt::QueuedConnection);

    connect(client, &WebSocketClient::disconnected,
            trayManager, [this]() { trayManager->setConnected(false); }, Qt::QueuedConnection);

}

PhotoBooth::~PhotoBooth()
{
    //delete button;
    delete ui;
}

bool PhotoBooth::getIsTTSEnabled() const
{
    return isTTSEnabled;
}

void PhotoBooth::setIsTTSEnabled(bool newIsTTSEnabled)
{
    isTTSEnabled = newIsTTSEnabled;
}


void PhotoBooth::handlePhotoBooth(const QString &urlStr, const QString &message)
{
    QUrl url(urlStr);
    if (!url.isValid()) {
        qWarning() << "PhotoBooth::handlePhotoBooth url invalide:" << urlStr;
        return;
    }

    // si on peut créer une session maintenant, on le fait, sinon on met en queue
    if (m_sessions.size() < m_maxConcurrent) {
        Session *s = createSessionForUrl(url, message);
        m_sessions.append(s);
        s->start();
    } else {
        m_pendingQueue.enqueue(QPair<QUrl, QString>(url, message));
    }
}

Session* PhotoBooth::createSessionForUrl(const QUrl &url, const QString &message)
{
    Session *s = new Session(url, message, m_displayMs, isTTSEnabled, this);
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
        QPair<QUrl, QString> next = m_pendingQueue.dequeue();
        Session *s = createSessionForUrl(next.first, next.second);
        m_sessions.append(s);
        s->start();
    }
}

