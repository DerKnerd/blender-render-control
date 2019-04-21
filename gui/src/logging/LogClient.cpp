//
// Created by imanuel on 21.04.19.
//

#include "LogClient.h"

LogClient::LogClient() : socketClient() {
    connect(&socketClient, &QWebSocket::connected, this, &LogClient::onConnected);
}

void LogClient::onConnected() {
    connect(&socketClient, &QWebSocket::textMessageReceived, this, &LogClient::onTextMessageReceived);

    emit connected();
}

void LogClient::onTextMessageReceived(const QString &message) {
    auto response = QJsonDocument::fromJson(message.toUtf8());

    if (response.isObject()) {
        auto responseMessage = response.object()["message"].toString();
        auto responseFile = response.object()["file"].toString();

        emit logReceived(responseMessage, responseFile);
    } else if (response.isNull() || response.isEmpty()) {
        if (!message.startsWith("null")) {
            auto msg = message;
            msg = msg.remove(0, 1);
            msg = msg.remove(msg.length() - 2, msg.length() - 1);
            emit logReceived(msg, "");
        }
    }
}

void LogClient::openSocket() {
    auto port = AppSettings::serverPort();
    auto host = AppSettings::serverHost().trimmed();
    auto logEndpoint = QStringLiteral("ws://%1:%2/log").arg(host).arg(port);

    socketClient.open(QUrl(logEndpoint));
}

void LogClient::closeSocket() {
    socketClient.close(QWebSocketProtocol::CloseCodeNormal);
}
