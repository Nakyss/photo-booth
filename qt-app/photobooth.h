#ifndef PHOTOBOOTH_H
#define PHOTOBOOTH_H

#include <QMainWindow>
#include "websocketclient.h"
#include "traymanager.h"
#include <QQueue>
#include "session.h"
#include <QLabel>

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
    WebSocketClient *client;
    TrayManager *trayManager;
    QVector<Session*> m_sessions;      // sessions actives
    QQueue<QUrl> m_pendingQueue;           // file d'attente
    int m_maxConcurrent = 3;               // configurable
    int m_displayMs = 2500;                // configurable durée d'affichage
    QLabel *infoText;


private slots:
    void handleImageUrl(const QString &url);
    void onSessionFinished(Session* session);
    void tryStartNext();
    Session* createSessionForUrl(const QUrl &url);

};
#endif // PHOTOBOOTH_H
