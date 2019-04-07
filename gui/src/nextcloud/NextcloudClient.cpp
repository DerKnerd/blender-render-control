//
// Created by imanuel on 04.04.19.
//

#include "NextcloudClient.h"

NextcloudClient::NextcloudClient() : socketClient() {
    connect(&socketClient, &QWebSocket::connected, this, &NextcloudClient::onConnected);
}

void NextcloudClient::startFileSync() {
    auto socketState = socketClient.state();

    if (socketState == QAbstractSocket::ConnectedState) {
        auto data = QByteArray();
        data.append(char(0));
        socketClient.sendBinaryMessage(data);
    }
}

void NextcloudClient::onConnected() {
    connect(&socketClient, &QWebSocket::textMessageReceived, this, &NextcloudClient::onTextMessageReceived);
    connect(&socketClient, &QWebSocket::binaryMessageReceived, this, &NextcloudClient::onBinaryMessageReceived);

    emit connected();
}

void NextcloudClient::onTextMessageReceived(const QString &message) {
    emit logReceived(message);
}

void NextcloudClient::onBinaryMessageReceived(const QByteArray &data) {
    emit logReceived(data);
}

void NextcloudClient::openSocket(const QString &url) {
    socketClient.open(QUrl(url));
}

void NextcloudClient::closeSocket() {
    socketClient.close(QWebSocketProtocol::CloseCodeNormal);
}

void NextcloudClient::listBlendFiles() {
    auto accessManager = new QNetworkAccessManager();
    auto request = QNetworkRequest();
    request.setUrl(
            QStringLiteral("http://%1:%2/list-files").arg(AppSettings::serverHost()).arg(AppSettings::serverPort()));
    connect(accessManager, &QNetworkAccessManager::finished, this, &NextcloudClient::onFinished);

    accessManager->get(request);
}

void NextcloudClient::onFinished(QNetworkReply *reply) {
    auto files = QList<File>();

    if (reply->error()) {
        emit logReceived(reply->errorString());
    } else {
        auto data = reply->readAll();
        auto logData = data.toStdString();
        auto document = QJsonDocument::fromJson(data);
        if (!document.isEmpty() && document.isArray()) {
            auto arrayData = document.array();

            for (auto i = 0; i < arrayData.count(); ++i) {
                auto item = arrayData[i];
                if (item.isObject()) {
                    auto file = File();
                    auto object = item.toObject();
                    auto date = QDateTime::fromString(object["modified_date"].toString(), Qt::DateFormat::ISODate);

                    file.setModifiedDate(date);
                    file.setName(object[QStringLiteral("name")].toString());
                    file.setPath(object[QStringLiteral("path")].toString());
                    file.setSize(object[QStringLiteral("size")].toInt());
                    files.append(file);
                }
            }
        }

        emit filesReceived(files);
    }
}
