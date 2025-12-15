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
#include <QNetworkAccessManager>
#include <QNetworkReply>

#ifdef Q_OS_WIN
#include <QSettings>
#include <windows.h>
#endif

//#define BASE_URL "https://photo.nakyss.fr/users"
#define BASE_URL "http://localhost:59875/users"

TrayManager::TrayManager(PhotoBooth *mainWindow, QObject *parent)
    : QObject(parent),
    trayIcon(nullptr),
    trayMenu(nullptr),
    mainWindow(mainWindow),
    autostartAction(nullptr),
    clientCountAction(nullptr),
    netManager(nullptr),
    clientCountApiUrl(QStringLiteral(BASE_URL))
{
    // Vérifie la disponibilité du tray
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << "System tray not available on this platform.";
        return;
    }

    netManager = new QNetworkAccessManager(this);
    connect(netManager, &QNetworkAccessManager::finished, this, &TrayManager::onClientCountReply);

    QIcon icon(":/icons/data/icons/icon.png");
    if (icon.isNull()) {
        qWarning() << "Icon not found, tray icon may not appear.";
    }

    trayIcon = new QSystemTrayIcon(icon, this);

    // Menu contextuel
    trayMenu = new QMenu();

    QAction *sendImagesAction = new QAction(tr("Envoyer des images"), this);
    autostartAction = new QAction(tr("Lancer au démarrage"), this);
    autostartAction->setCheckable(true);

    ttsAction = new QAction(tr("TextToSpeech"), this);
    ttsAction->setCheckable(true);

    clientCountAction = new QAction(tr("Clients connectés : —"), this);
    clientCountAction->setEnabled(false); // juste pour afficher l'info

    QAction *quitAction = new QAction(tr("Quitter"), this);

    trayMenu->addAction(sendImagesAction);
    trayMenu->addAction(autostartAction);
    trayMenu->addAction(ttsAction);
    trayMenu->addSeparator();
    trayMenu->addAction(clientCountAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayMenu);

    // Connexions
    connect(trayIcon, &QSystemTrayIcon::activated, this, &TrayManager::onActivated);

    connect(autostartAction, &QAction::toggled, this, &TrayManager::onToggleAutostart);

    connect(ttsAction, &QAction::toggled, this, &TrayManager::onToggleTTS);

    // Ouvre la page https://photo.nakyss.fr dans le navigateur par défaut
    connect(sendImagesAction, &QAction::triggered, this, &TrayManager::onSendImages);

    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    connect(trayMenu, &QMenu::aboutToShow, this, &TrayManager::refreshClientCount);

    bool enabled = isAutostartEnabled();
    autostartAction->setChecked(enabled);
    ttsAction->setChecked(mainWindow->getIsTTSEnabled());

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
       // onShowHide();
    }
    // Clique droit ouvre le menu par défaut (Qt le gère déjà),
    // double-clic pourrait aussi afficher la fenêtre :
    if (reason == QSystemTrayIcon::DoubleClick) {
        //onShowHide();
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

void TrayManager::onToggleTTS(bool checked)
{
    mainWindow->setIsTTSEnabled(checked);
    ttsAction->setChecked(checked);
    qDebug() <<"TTS Enabled:"<< mainWindow->getIsTTSEnabled();
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


void TrayManager::setConnected(bool connected)
{
    if (!trayIcon) return;

    // chemins d'icônes attendus dans le qrc (à adapter si tu as d'autres noms)
    const QString iconOkPath   = QStringLiteral(":/icons/data/icons/icon.png");
    const QString iconErrorPath= QStringLiteral(":/icons/data/icons/icon-red.png"); // icône rouge

    QIcon ic;

    if (connected) {
        if (QFile::exists(iconOkPath)) {
            ic = QIcon(iconOkPath);
        } else {
            // fallback sur le thème du système
            ic = QIcon::fromTheme(QStringLiteral("network-transmit-receive"));
        }
        trayIcon->setToolTip(tr("PhotoBooth — connecté"));
    } else {
        if (QFile::exists(iconErrorPath)) {
            ic = QIcon(iconErrorPath);
        } else {
            // fallback sur le thème du système
            ic = QIcon::fromTheme(QStringLiteral("network-error"));
        }
        trayIcon->setToolTip(tr("PhotoBooth — déconnecté"));
    }

    if (!ic.isNull()) {
        trayIcon->setIcon(ic);
    } else {
        qWarning() << "TrayManager::setConnected : aucune icône trouvée pour état" << connected;
    }
}


void TrayManager::refreshClientCount()
{
    if (!netManager) return;

    // Mettre le texte en mode chargement
    if (clientCountAction) {
        clientCountAction->setText(tr("Clients connectés : …"));
    }

    QUrl url(clientCountApiUrl);
    QNetworkRequest req(url);
    netManager->get(req);
}

void TrayManager::onClientCountReply(QNetworkReply* reply)
{
    if (!reply) return;

    QUrl requestUrl = reply->request().url();

    // lecture
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Client count request failed:" << reply->errorString();
        if (clientCountAction) clientCountAction->setText(tr("Clients connectés : ? (erreur)"));
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    int count = -1;


    bool ok = false;
    QString s = QString::fromUtf8(data).trimmed();
    int maybe = s.toInt(&ok);
    if (ok) {
        count = maybe;
    }

    if (ok && clientCountAction) {
        clientCountAction->setText(tr("Clients connectés : %1").arg(QString::number(count)));
    } else {
        if (clientCountAction) clientCountAction->setText(tr("Clients connectés : ?"));
        qWarning() << "Could not parse client count response from" << requestUrl << "data:" << QString::fromUtf8(data);
    }
}
