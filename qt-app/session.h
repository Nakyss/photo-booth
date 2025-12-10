#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QUrl>
#include <QTimer>
#include <QPixmap>
#include "filedownloader.h" // adapte le nom si différent
#include "overlay.h"
#include "overlay_mac.h"
#include <QTextToSpeech>

class Session : public QObject
{
    Q_OBJECT
public:
    explicit Session(const QUrl &imageUrl, const QString &message, int displayMs = 2500, QObject *parent = nullptr);
    ~Session();

    void start();

signals:
    void finished(Session* self); // émis quand la session se termine (pour que PhotoBooth enlève la session)

private slots:
    void onDownloaded();
    void onTimeout();

private:
    QUrl m_url;
    QString message;
    FileDownloader *m_downloader = nullptr;
    #ifdef Q_OS_MAC
        OverlayMac *mac_overlay = nullptr;
    #else
        Overlay *m_overlay = nullptr;
    #endif
    QPixmap *m_pixmap = nullptr;
    int m_displayMs;
    QTextToSpeech *speech;
};

#endif // SESSION_H
