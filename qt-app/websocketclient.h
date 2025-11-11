#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>

class WebSocketClient : public QObject
{
    Q_OBJECT

public:
    explicit WebSocketClient(const QUrl &url, QObject *parent = nullptr);

signals:
    void imageReceived(const QString &imageUrl);

private slots:
    void onConnected();
    void onTextMessageReceived(const QString &message);
    void onDisconnected();

private:
    QWebSocket m_webSocket;
    QUrl m_url;
};

#endif // WEBSOCKETCLIENT_H
