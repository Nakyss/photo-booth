#include "websocketclient.h"
#include <QDebug>

WebSocketClient::WebSocketClient(const QUrl &url, QObject *parent)
    : QObject(parent), m_url(url)
{
    connect(&m_webSocket, &QWebSocket::connected,
            this, &WebSocketClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected,
            this, &WebSocketClient::onDisconnected);

    qDebug() << "Connecting to" << url.toString() << "...";
    m_webSocket.open(m_url);
}

void WebSocketClient::onConnected()
{
    qDebug() << "Connected to" << m_url.toString();

    connect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &WebSocketClient::onTextMessageReceived);
}

void WebSocketClient::onTextMessageReceived(const QString &message)
{
    qDebug() << "Message reçu:" << message;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "Message JSON invalide.";
        return;
    }

    QJsonObject obj = doc.object();
    QString event = obj.value("event").toString();

    if (event == "photo:broadcast") {
        QJsonObject data = obj.value("data").toObject();
        QString image = "https://photo.nakyss.fr" + data.value("image").toString();
        QString messageText = data.value("message").toString();

        qDebug() << "Une image a été envoyée!";
        qDebug() << "Lien:" << image;
        qDebug() << "Message:" << messageText;

        emit imageReceived(image);
    }
}

void WebSocketClient::onDisconnected()
{
    qWarning() << "Déconnecté du serveur WebSocket.";
    m_webSocket.open(m_url);
}
