//
// Created by imanuel on 06.04.19.
//

#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QFileInfo>
#include <AppSettings.h>
#include <QtNetwork/QNetworkAccessManager>
#include "BlenderClient.h"

BlenderClient::BlenderClient() : socketClient() {
    connect(&socketClient, &QWebSocket::connected, this, &BlenderClient::onConnected);
}

void BlenderClient::startRender() {
    auto socketState = socketClient.state();

    if (socketState == QAbstractSocket::ConnectedState) {
        auto data = QJsonObject();

        data["action"] = 0;

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

void BlenderClient::addToQueue(const QStringList &files) {
    auto socketState = socketClient.state();

    if (socketState == QAbstractSocket::ConnectedState) {
        auto data = QJsonObject();
        auto filesArray = QJsonArray::fromStringList(files);

        data["action"] = 3;
        data["files"] = filesArray;

        auto document = QJsonDocument();
        document.setObject(data);

        auto jsonData = document.toJson(QJsonDocument::Compact);
        socketClient.sendTextMessage(jsonData);
    }
}

void BlenderClient::getQueue() {
    auto accessManager = new QNetworkAccessManager();
    auto request = QNetworkRequest();
    request.setUrl(
            QStringLiteral("http://%1:%2/show-queue").arg(AppSettings::serverHost()).arg(AppSettings::serverPort()));
    connect(accessManager, &QNetworkAccessManager::finished, this, &BlenderClient::onFinished);

    accessManager->get(request);
}

void BlenderClient::onFinished(QNetworkReply *reply) {
    auto files = QList<QString>();

    if (reply->error()) {
        emit logReceived("", reply->errorString());
    } else {
        auto data = reply->readAll();
        auto logData = data.toStdString();
        auto document = QJsonDocument::fromJson(data);
        if (!document.isEmpty() && document.isArray()) {
            auto arrayData = document.array();

            for (auto i = 0; i < arrayData.count(); ++i) {
                files.append(arrayData[i].toString());
            }
        }

        emit queueListed(files);
    }
}

void BlenderClient::removeFromQueue(const QStringList &files) {
    auto socketState = socketClient.state();

    if (socketState == QAbstractSocket::ConnectedState) {
        auto data = QJsonObject();
        auto filesArray = QJsonArray::fromStringList(files);

        data["action"] = 4;
        data["files"] = filesArray;

        auto document = QJsonDocument();
        document.setObject(data);

        auto jsonData = document.toJson(QJsonDocument::Compact);
        socketClient.sendTextMessage(jsonData);
    }
}
