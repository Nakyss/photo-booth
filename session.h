#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QUrl>
#include <QTimer>
#include <QPixmap>
#include "filedownloader.h" // adapte le nom si différent
#include "overlay.h"
#include "overlay_mac.h"

class Session : public QObject
{
    Q_OBJECT
public:
    explicit Session(const QUrl &imageUrl, int displayMs = 2500, QObject *parent = nullptr);
    ~Session();

    void start();

signals:
    void finished(Session* self); // émis quand la session se termine (pour que PhotoBooth enlève la session)

private slots:
    void onDownloaded();
    void onTimeout();

private:
    QUrl m_url;
    FileDownloader *m_downloader = nullptr;
    Overlay *m_overlay = nullptr;
    OverlayMac *mac_overlay = nullptr;
    QPixmap *m_pixmap = nullptr;
    int m_displayMs;
};

#endif // SESSION_H
