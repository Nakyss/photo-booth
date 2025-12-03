#include "websocketclient.h"
#include <QDebug>

#define BASE_URL "https://photo.nakyss.fr"
//#define BASE_URL "http://localhost:59875"

WebSocketClient::WebSocketClient(const QUrl &url, QObject *parent)
    : QObject(parent), m_url(url), m_reconnectAttempts(0), m_maxReconnectAttempts(10)
{
    connect(&m_webSocket, &QWebSocket::connected,
            this, &WebSocketClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected,
            this, &WebSocketClient::onDisconnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &WebSocketClient::onTextMessageReceived);  // Moved here to avoid duplicates

    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &WebSocketClient::attemptReconnect);

    qDebug() << "Connecting to" << url.toString() << "...";
    m_webSocket.open(m_url);
}

void WebSocketClient::onConnected()
{
    qDebug() << "Connected to" << m_url.toString();
    m_reconnectAttempts = 0;
    emit connected();
}

void WebSocketClient::onTextMessageReceived(const QString &message)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "Message JSON invalide.";
        return;
    }

    QJsonObject obj = doc.object();
    QString event = obj.value("event").toString();

    qDebug() << "Message reçu:" << event;

    if (event == "photo:broadcast") {
        QJsonObject data = obj.value("data").toObject();
        QString image = BASE_URL + data.value("image").toString();
        QString messageText = data.value("message").toString();

        qDebug() << "Une image a été envoyée!";
        qDebug() << "Lien:" << image;
        qDebug() << "Message:" << messageText;

        emit imageReceived(image);
    }
}

void WebSocketClient::onDisconnected()
{
    qWarning() << "Déconnecté du serveur WebSocket. Raison:" << m_webSocket.closeReason();
    emit disconnected();  // Optional: emit signal

    if (m_reconnectAttempts < m_maxReconnectAttempts) {
        m_reconnectAttempts++;
        int delay = qMin(1000 * (1 << (m_reconnectAttempts - 1)), 30000);  // Exponential backoff, max 30s
        qDebug() << "Tentative de reconnexion dans" << delay << "ms (tentative" << m_reconnectAttempts << ")";
        m_reconnectTimer->start(delay);
    } else {
        qWarning() << "Nombre maximum de tentatives de reconnexion atteint.";
    }
}

void WebSocketClient::attemptReconnect()
{
    qDebug() << "Tentative de reconnexion...";
    m_webSocket.open(m_url);
}
