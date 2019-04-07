//
// Created by imanuel on 06.04.19.
//

#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QFileInfo>
#include "BlenderClient.h"

BlenderClient::BlenderClient() : socketClient() {
    connect(&socketClient, &QWebSocket::connected, this, &BlenderClient::onConnected);
}

void BlenderClient::startRender(const QStringList &files) {
    auto socketState = socketClient.state();

    if (socketState == QAbstractSocket::ConnectedState) {
        auto data = QJsonObject();
        auto filesArray = QJsonArray::fromStringList(files);

        data["action"] = 0;
        data["files"] = filesArray;

        auto document = QJsonDocument();
        document.setObject(data);

        auto jsonData = document.toJson(QJsonDocument::Compact);
        socketClient.sendTextMessage(jsonData);
    }
}

void BlenderClient::stopRender() {
    auto socketState = socketClient.state();

    if (socketState == QAbstractSocket::ConnectedState) {
        auto data = QJsonObject();
        data["action"] = 2;

        auto document = QJsonDocument();
        document.setObject(data);

        auto jsonData = document.toJson(QJsonDocument::Compact);
        socketClient.sendTextMessage(jsonData);
    }
}

void BlenderClient::pauseRender() {
    auto socketState = socketClient.state();

    if (socketState == QAbstractSocket::ConnectedState) {
        auto data = QJsonObject();
        data["action"] = 1;

        auto document = QJsonDocument();
        document.setObject(data);

        auto jsonData = document.toJson(QJsonDocument::Compact);
        socketClient.sendTextMessage(jsonData);
    }
}

void BlenderClient::onConnected() {
    connect(&socketClient, &QWebSocket::textMessageReceived, this, &BlenderClient::onTextMessageReceived);

    emit connected();
}

void BlenderClient::onTextMessageReceived(const QString &message) {
    auto response = QJsonDocument::fromJson(message.toUtf8());
    auto responseMessage = response.object()["message"].toString();
    auto responseFile = response.object()["file"].toString();

    emit logReceived(responseMessage, responseFile);
}

void BlenderClient::openSocket(const QString &url) {
    socketClient.open(QUrl(url));
}

void BlenderClient::closeSocket() {
    socketClient.close(QWebSocketProtocol::CloseCodeNormal);
}