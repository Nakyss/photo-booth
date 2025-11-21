#include "traymanager.h"
#include <QIcon>
#include <QAction>
#include <QApplication>
#include <QWidget>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

TrayManager::TrayManager(QWidget *mainWindow, QObject *parent)
    : QObject(parent),
    trayIcon(nullptr),
    trayMenu(nullptr),
    mainWindow(mainWindow)
{
    // Vérifie la disponibilité du tray
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << "System tray not available on this platform.";
        return;
    }

    QIcon icon(":/icons/data/icons/icon.png");
    if (icon.isNull()) {
        qWarning() << "Icon not found, tray icon may not appear.";
    }

    trayIcon = new QSystemTrayIcon(icon, this);

    // Menu contextuel
    trayMenu = new QMenu();

    //QAction *showHideAction = new QAction(tr("Afficher / Cacher"), this);
    QAction *sendImagesAction = new QAction(tr("Envoyer des images"), this);
    QAction *quitAction     = new QAction(tr("Quitter"), this);

    //trayMenu->addAction(showHideAction);
    trayMenu->addAction(sendImagesAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayMenu);

    // Connexions
    connect(trayIcon, &QSystemTrayIcon::activated, this, &TrayManager::onActivated);
    //connect(showHideAction, &QAction::triggered, this, &TrayManager::onShowHide);

    // Ouvre la page https://photo.nakyss.fr dans le navigateur par défaut
    connect(sendImagesAction, &QAction::triggered, this, [this]() {
        QUrl url(QStringLiteral("https://photo.nakyss.fr"));
        if (!QDesktopServices::openUrl(url)) {
            qWarning() << "TrayManager: failed to open URL" << url;
        }
    });

    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    trayIcon->show();
}

TrayManager::~TrayManager()
{
    if (trayIcon) {
        trayIcon->hide();
        delete trayIcon;
        trayIcon = nullptr;
    }
    // trayMenu est détruit automatiquement (parent=this)
}

void TrayManager::show()
{
    if (trayIcon) trayIcon->show();
}

void TrayManager::hide()
{
    if (trayIcon) trayIcon->hide();
}

void TrayManager::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    // Clique gauche = toggle show/hide
    if (reason == QSystemTrayIcon::Trigger) {
        onShowHide();
    }
    // Clique droit ouvre le menu par défaut (Qt le gère déjà),
    // double-clic pourrait aussi afficher la fenêtre :
    if (reason == QSystemTrayIcon::DoubleClick) {
        onShowHide();
    }
}

void TrayManager::onShowHide()
{
    if (!mainWindow) return;
    if (mainWindow->isVisible()) {
        mainWindow->hide();
    } else {
        mainWindow->show();
        mainWindow->raise();          // bring to front
        mainWindow->activateWindow(); // focus
    }
}
