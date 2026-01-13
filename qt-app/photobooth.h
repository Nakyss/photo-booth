#ifndef PHOTOBOOTH_H
#define PHOTOBOOTH_H

#include <QMainWindow>
#include "websocketclient.h"
#include "traymanager.h"
#include <QQueue>
#include "session.h"
#include <QLabel>
#include "settings.h"
#include <QPointer>


class PhotoBooth : public QObject
{
    Q_OBJECT

public:
    PhotoBooth();

private:
    WebSocketClient *webSocket;
    TrayManager *trayManager;
    QVector<Session*> sessionsList;
    QQueue<QPair<QUrl, QString>> pendingQueue;
    QPointer<Settings> settingsWindow;
    int maxConcurrent = 3;               // configurable
    int displayMs = 2500;                // configurable durée d'affichage
    QLabel *infoText;
    bool isTTSEnabled = false;

    void wsConnected();
    void wsDisconnected();

private slots:
    void handlePhotoBooth(const QString &url, const QString &message);
    void onSessionFinished(Session* session);
    void tryStartNext();
    Session* createSessionForUrl(const QUrl &url, const QString &message);
    void setIsTTSEnabled(bool newIsTTSEnabled);
    void displaySettings();


};
#endif // PHOTOBOOTH_H
