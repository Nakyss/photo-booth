#include "photobooth.h"
#include <QTimer>
#include <QUrl>
#include "hosts.h"

PhotoBooth::PhotoBooth() : settingsWindow(nullptr)
{
    webSocket = new WebSocketClient(QUrl(WS_URL), this);

    trayManager = new TrayManager(isTTSEnabled, this);

    connect(webSocket, &WebSocketClient::imageReceived, this, &PhotoBooth::handlePhotoBooth, Qt::QueuedConnection);

    connect(webSocket, &WebSocketClient::connected, this, &PhotoBooth::wsConnected, Qt::QueuedConnection);

    connect(webSocket, &WebSocketClient::disconnected, this, &PhotoBooth::wsDisconnected, Qt::QueuedConnection);

    connect(trayManager, &TrayManager::ttsStatusChanged, this, &PhotoBooth::setIsTTSEnabled);

    connect(trayManager, &TrayManager::displayWindow, this, &PhotoBooth::displaySettings);
}


void PhotoBooth::setIsTTSEnabled(bool newIsTTSEnabled)
{
    isTTSEnabled = newIsTTSEnabled;
}

void PhotoBooth::displaySettings()
{
    if (!settingsWindow) {
        settingsWindow = new Settings();
        // On s'assure qu'il se détruit s'il ferme avec la croix
        settingsWindow->setAttribute(Qt::WA_DeleteOnClose);
        settingsWindow->show();
        qDebug() << "Show settings";
    }
    else {
        settingsWindow->deleteLater();
        qDebug() << "Hide settings";
    }
}

void PhotoBooth::handlePhotoBooth(const QString &urlStr, const QString &message)
{
    QUrl url(urlStr);
    if (!url.isValid()) {
        qWarning() << "PhotoBooth::handlePhotoBooth url invalide:" << urlStr;
        return;
    }

    // si on peut créer une session maintenant, on le fait, sinon on met en queue
    if (sessionsList.size() < maxConcurrent) {
        Session *s = createSessionForUrl(url, message);
        sessionsList.append(s);
        s->start();
    } else {
        pendingQueue.enqueue(QPair<QUrl, QString>(url, message));
    }
}

Session* PhotoBooth::createSessionForUrl(const QUrl &url, const QString &message)
{
    Session *s = new Session(url, message, displayMs, isTTSEnabled, this);
    connect(s, &Session::finished, this, &PhotoBooth::onSessionFinished, Qt::QueuedConnection);
    return s;
}

void PhotoBooth::onSessionFinished(Session* session)
{
    // retire de la liste des sessions actives
    sessionsList.removeAll(session);

    // démarrer la prochaine de la file si présente
    tryStartNext();
}

void PhotoBooth::tryStartNext()
{
    while (!pendingQueue.isEmpty() && sessionsList.size() < maxConcurrent) {
        QPair<QUrl, QString> next = pendingQueue.dequeue();
        Session *s = createSessionForUrl(next.first, next.second);
        sessionsList.append(s);
        s->start();
    }
}

void PhotoBooth::wsConnected()
{
    trayManager->setConnected(true);
}

void PhotoBooth::wsDisconnected()
{
    trayManager->setConnected(false);
}

