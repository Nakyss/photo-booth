#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>  // Added for reconnection timer

class WebSocketClient : public QObject
{
    Q_OBJECT

public:
    explicit WebSocketClient(const QUrl &url, QObject *parent = nullptr);

signals:
    void connected();
    void disconnected();
    void imageReceived(const QString &imageUrl);

private slots:
    void onConnected();
    void onTextMessageReceived(const QString &message);
    void onDisconnected();
    void attemptReconnect();

private:
    QWebSocket m_webSocket;
    QUrl m_url;
    QTimer *m_reconnectTimer;
    int m_reconnectAttempts;
    int m_maxReconnectAttempts;
};

#endif // WEBSOCKETCLIENT_H
