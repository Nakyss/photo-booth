#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>

class QWidget;

class TrayManager : public QObject
{
    Q_OBJECT
public:
    explicit TrayManager(QWidget *mainWindow, QObject *parent = nullptr);
    ~TrayManager();

    void show();    // forcer l'affichage de l'icône
    void hide();    // cacher l'icône

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QWidget *mainWindow;

private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowHide();
};

#endif // TRAYMANAGER_H
