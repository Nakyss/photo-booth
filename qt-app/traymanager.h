#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class TrayManager : public QObject
{
    Q_OBJECT
public:
    explicit TrayManager(bool isTTS,QObject *parent = nullptr);
    ~TrayManager();

    void show();    // forcer l'affichage de l'icône
    void hide();    // cacher l'icône

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *autostartAction;
    QAction *clientCountAction;
    QAction *ttsAction;

    QNetworkAccessManager *netManager;
    QString clientCountApiUrl;

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

signals:
    void ttsStatusChanged(bool value);
    void displayWindow();
};

#endif // TRAYMANAGER_H
