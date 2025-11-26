#include "traymanager.h"
#include <QIcon>
#include <QAction>
#include <QApplication>
#include <QWidget>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QCoreApplication>

#ifdef Q_OS_WIN
#include <QSettings>
#include <windows.h>
#endif

TrayManager::TrayManager(QWidget *mainWindow, QObject *parent)
    : QObject(parent),
    trayIcon(nullptr),
    trayMenu(nullptr),
    mainWindow(mainWindow),
    autostartAction(nullptr)
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
    autostartAction = new QAction(tr("Lancer au démarrage"), this);
    autostartAction->setCheckable(true);
    QAction *quitAction     = new QAction(tr("Quitter"), this);

    //trayMenu->addAction(showHideAction);
    trayMenu->addAction(sendImagesAction);
    trayMenu->addAction(autostartAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayMenu);

    // Connexions
    connect(trayIcon, &QSystemTrayIcon::activated, this, &TrayManager::onActivated);
    //connect(showHideAction, &QAction::triggered, this, &TrayManager::onShowHide);
    connect(autostartAction, &QAction::toggled, this, &TrayManager::onToggleAutostart);

    // Ouvre la page https://photo.nakyss.fr dans le navigateur par défaut
    connect(sendImagesAction, &QAction::triggered, this, &TrayManager::onSendImages);

    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    bool enabled = isAutostartEnabled();
    autostartAction->setChecked(enabled);

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

void TrayManager::onSendImages()
{
    QUrl url(QStringLiteral("https://photo.nakyss.fr"));
    if (!QDesktopServices::openUrl(url)) {
        qWarning() << "TrayManager: failed to open URL" << url;
    }
}

void TrayManager::onToggleAutostart(bool checked)
{
    bool ok = false;
    if (checked) {
        ok = enableAutostart();
        if (!ok) {
            qWarning() << "Failed to enable autostart";
            autostartAction->setChecked(false);
        }
    } else {
        ok = disableAutostart();
        if (!ok) {
            qWarning() << "Failed to disable autostart";
            autostartAction->setChecked(true);
        }
    }
}

bool TrayManager::isAutostartEnabled() const
{
    QString appPath = QCoreApplication::applicationFilePath();
    QString appName = QFileInfo(appPath).baseName();

#ifdef Q_OS_WIN
    QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    QVariant v = reg.value(appName);
    if (!v.isNull()) return true;
    return false;
#elif defined(Q_OS_MACOS)
    // check presence of plist in ~/Library/LaunchAgents/
    QString plistName = QStringLiteral("%1.plist").arg(appName);
    QString launchDir = QDir::homePath() + "/Library/LaunchAgents";
    return QFile::exists(launchDir + "/" + plistName);
#else
    // freedesktop autostart .desktop
    QString autostartDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/autostart";
    QString desktopName = QStringLiteral("%1.desktop").arg(appName);
    return QFile::exists(autostartDir + "/" + desktopName);
#endif
}


bool TrayManager::enableAutostart()
{
    QString appPath = QCoreApplication::applicationFilePath();
    QString appName = QFileInfo(appPath).baseName();

#ifdef Q_OS_WIN
    // Write HKCU\...\Run\<appName> = "<appPath>" <args optional>
    QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    reg.setValue(appName, QDir::toNativeSeparators(appPath));
    return true;
#elif defined(Q_OS_MACOS)
    // Create a LaunchAgent plist in ~/Library/LaunchAgents/
    QString launchDir = QDir::homePath() + "/Library/LaunchAgents";
    QDir dir;
    if (!dir.exists(launchDir)) {
        if (!dir.mkpath(launchDir)) {
            qWarning() << "Cannot create LaunchAgents directory:" << launchDir;
            return false;
        }
    }
    QString plistPath = launchDir + "/" + QStringLiteral("%1.plist").arg(appName);
    QString plistContent = QString(
                               "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                               "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
                               "<plist version=\"1.0\">\n"
                               "<dict>\n"
                               "  <key>Label</key>\n"
                               "  <string>%1</string>\n"
                               "  <key>ProgramArguments</key>\n"
                               "  <array>\n"
                               "    <string>%2</string>\n"
                               "  </array>\n"
                               "  <key>RunAtLoad</key>\n"
                               "  <true/>\n"
                               "</dict>\n"
                               "</plist>\n").arg(appName, appPath);

    QFile f(plistPath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Failed to open plist for writing:" << plistPath;
        return false;
    }
    qint64 written = f.write(plistContent.toUtf8());
    f.close();
    if (written == -1) {
        qWarning() << "Failed to write plist:" << plistPath;
        return false;
    }
    return true;
#else
    // freedesktop .desktop in ~/.config/autostart/
    QString autostartDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/autostart";
    QDir dir;
    if (!dir.exists(autostartDir)) {
        if (!dir.mkpath(autostartDir)) {
            qWarning() << "Cannot create autostart dir:" << autostartDir;
            return false;
        }
    }
    QString desktopPath = autostartDir + "/" + QStringLiteral("%1.desktop").arg(appName);

    // Create a simple .desktop file
    QString desktopContent = QString(
                                 "[Desktop Entry]\n"
                                 "Type=Application\n"
                                 "Name=%1\n"
                                 "Exec=%2\n"
                                 "X-GNOME-Autostart-enabled=true\n"
                                 "NoDisplay=false\n"
                                 ).arg(appName, QDir::toNativeSeparators(appPath));

    QFile f(desktopPath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Failed to open desktop file for writing:" << desktopPath;
        return false;
    }
    qint64 written = f.write(desktopContent.toUtf8());
    f.close();
    if (written == -1) {
        qWarning() << "Failed to write desktop file:" << desktopPath;
        return false;
    }

    return true;
#endif
}

bool TrayManager::disableAutostart()
{
    QString appPath = QCoreApplication::applicationFilePath();
    QString appName = QFileInfo(appPath).baseName();

#ifdef Q_OS_WIN
    QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    reg.remove(appName);
    return true;
#elif defined(Q_OS_MACOS)
    QString plistPath = QDir::homePath() + "/Library/LaunchAgents/" + QStringLiteral("%1.plist").arg(appName);
    if (QFile::exists(plistPath)) {
        if (!QFile::remove(plistPath)) {
            qWarning() << "Failed to remove plist:" << plistPath;
            return false;
        }
    }
    // Optionally: QProcess::execute("launchctl", QStringList() << "unload" << plistPath);
    return true;
#else
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/autostart/" + QStringLiteral("%1.desktop").arg(appName);
    if (QFile::exists(desktopPath)) {
        if (!QFile::remove(desktopPath)) {
            qWarning() << "Failed to remove desktop file:" << desktopPath;
            return false;
        }
    }
    return true;
#endif
}
