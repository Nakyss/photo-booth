#include "websocketclient.h"
#include <QDebug>

#define BASE_URL "https://photo.nakyss.fr"
//#define BASE_URL "http://localhost:59875"

const int PING_INTERVAL_MS = 10000;   // Envoyer un ping toutes les 10 secondes
const int PONG_TIMEOUT_MS = 3000;     // Attendre le pong max 3 secondes

WebSocketClient::WebSocketClient(const QUrl &url, QObject *parent)
    : QObject(parent), m_url(url), m_reconnectAttempts(0), m_maxReconnectAttempts(10)
{
    connect(&m_webSocket, &QWebSocket::connected,
            this, &WebSocketClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected,
            this, &WebSocketClient::onDisconnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &WebSocketClient::onTextMessageReceived);  // Moved here to avoid duplicates

    // Gestion des erreurs (Important pour détecter les problèmes réseau rapidement)
    connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
            this, &WebSocketClient::onError);

    // Gestion du Pong (réponse du serveur au Ping)
    connect(&m_webSocket, &QWebSocket::pong,
            this, &WebSocketClient::onPong);

    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &WebSocketClient::attemptReconnect);

    // Timer pour envoyer les Pings périodiques
    m_pingTimer = new QTimer(this);
    m_pingTimer->setInterval(PING_INTERVAL_MS);
    connect(m_pingTimer, &QTimer::timeout, this, &WebSocketClient::sendPing);

    // Timer pour surveiller l'absence de réponse (Timeout)
    m_pongTimeoutTimer = new QTimer(this);
    m_pongTimeoutTimer->setSingleShot(true);
    m_pongTimeoutTimer->setInterval(PONG_TIMEOUT_MS);
    connect(m_pongTimeoutTimer, &QTimer::timeout, this, &WebSocketClient::onPongTimeout);

    qDebug() << "Connecting to" << url.toString() << "...";
    m_webSocket.open(m_url);
}

void WebSocketClient::onConnected()
{
    qDebug() << "Connected to" << m_url.toString();
    m_reconnectAttempts = 0;

    m_pingTimer->start();
    emit connected();
}

void WebSocketClient::sendPing()
{
    m_webSocket.ping();

    m_pongTimeoutTimer->start();
}

void WebSocketClient::onPong(quint64 elapsedTime, const QByteArray &payload)
{
    Q_UNUSED(elapsedTime)
    Q_UNUSED(payload)

    m_pongTimeoutTimer->stop();
}

void WebSocketClient::onPongTimeout()
{
    qWarning() << "Timeout Ping/Pong : Le serveur ne répond plus.";

    m_webSocket.close();
}

void WebSocketClient::onTextMessageReceived(const QString &message)
{
    // (Votre code existant pour le parsing JSON...)
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "Message JSON invalide.";
        return;
    }

    QJsonObject obj = doc.object();
    QString event = obj.value("event").toString();

    // qDebug() << "Message reçu:" << event; // Commenté pour éviter le spam

    if (event == "photo:broadcast") {
        QJsonObject data = obj.value("data").toObject();
        QString image = BASE_URL + data.value("image").toString();
        QString messageText = data.value("message").toString();

        qDebug() << "Image reçue:" << image;
        qDebug() << "Message: " << messageText;
        emit imageReceived(image);
    }
}

void WebSocketClient::onDisconnected()
{
    qWarning() << "Déconnecté du serveur WebSocket. Raison:" << m_webSocket.closeReason();
    emit disconnected();  // Optional: emit signal

    m_pingTimer->stop();
    m_pongTimeoutTimer->stop();

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

void WebSocketClient::onError(QAbstractSocket::SocketError error)
{
    // Cette fonction capture les erreurs réseau (ex: HostNotFoundError, ConnectionRefused)
    // qui ne déclenchent pas toujours immédiatement disconnected()
    qWarning() << "Erreur WebSocket:" << error << m_webSocket.errorString();

    // Généralement, une erreur critique entraînera une fermeture,
    // donc onDisconnected sera appelé ensuite par Qt.
}
