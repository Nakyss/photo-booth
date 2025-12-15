#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include "photobooth.h"

class PhotoBooth;

class QNetworkAccessManager;
class QNetworkReply;

class TrayManager : public QObject
{
    Q_OBJECT
public:
    explicit TrayManager(PhotoBooth *mainWindow, QObject *parent = nullptr);
    ~TrayManager();

    void show();    // forcer l'affichage de l'icône
    void hide();    // cacher l'icône

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    PhotoBooth *mainWindow;
    QAction *autostartAction;
    QAction *clientCountAction;
    QAction *ttsAction;

    QNetworkAccessManager *netManager;
    QString clientCountApiUrl;

private:
    bool isAutostartEnabled() const;
    bool enableAutostart();
    bool disableAutostart();

private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowHide();
    void onSendImages();
    void onToggleAutostart(bool checked);
    void onToggleTTS(bool checked);

    void refreshClientCount();
    void onClientCountReply(QNetworkReply* reply);

public slots:
    void setConnected(bool connected);
};

#endif // TRAYMANAGER_H
