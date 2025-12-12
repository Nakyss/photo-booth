#ifndef PHOTOBOOTH_H
#define PHOTOBOOTH_H

#include <QMainWindow>
#include "websocketclient.h"
#include "traymanager.h"
#include <QQueue>
#include "session.h"
#include <QLabel>

class TrayManager;

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

    bool getIsTTSEnabled() const;
    void setIsTTSEnabled(bool newIsTTSEnabled);

private:
    Ui::PhotoBooth *ui;
    WebSocketClient *client;
    TrayManager *trayManager;
    QVector<Session*> m_sessions;      // sessions actives
    QQueue<QPair<QUrl, QString>> m_pendingQueue;           // file d'attente
    int m_maxConcurrent = 3;               // configurable
    int m_displayMs = 2500;                // configurable durée d'affichage
    QLabel *infoText;
    bool isTTSEnabled = false;


private slots:
    void handlePhotoBooth(const QString &url, const QString &message);
    void onSessionFinished(Session* session);
    void tryStartNext();
    Session* createSessionForUrl(const QUrl &url, const QString &message);

};
#endif // PHOTOBOOTH_H
