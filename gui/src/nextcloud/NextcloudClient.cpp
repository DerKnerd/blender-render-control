//
// Created by imanuel on 04.04.19.
//

#include "NextcloudClient.h"

NextcloudClient::NextcloudClient() {
    accessManager = new QNetworkAccessManager();
}

void NextcloudClient::startSync() {
    connect(accessManager, &QNetworkAccessManager::finished, this, &NextcloudClient::onStartSyncFinished);

    auto data = QByteArray();
    auto request = this->getRequest();

    accessManager->post(request, data);
}

void NextcloudClient::listFiles() {
    connect(accessManager, &QNetworkAccessManager::finished, this, &NextcloudClient::onListFilesFinished);

    auto request = this->getRequest();

    accessManager->get(request);
}

void NextcloudClient::onStartSyncFinished(QNetworkReply *reply) {
    disconnect(accessManager, &QNetworkAccessManager::finished, this, &NextcloudClient::onStartSyncFinished);

    parseError(reply, [this]() {
        emit startSyncFinished();
    });
}

void NextcloudClient::onListFilesFinished(QNetworkReply *reply) {
    disconnect(accessManager, &QNetworkAccessManager::finished, this, &NextcloudClient::onListFilesFinished);

    parseError(reply, [this, reply]() {
        auto files = new QList<File>();
        auto data = reply->readAll();
        auto jsonDoc = QJsonDocument::fromJson(data);

        auto responseArray = jsonDoc.array();
        for (auto item = responseArray.constBegin(); item < responseArray.constEnd(); ++item) {
            auto file = new File();
            auto obj = item->toObject();
            file->setPath(obj["path"].toString());
            file->setSize(obj["size"].toInt());
            file->setName(obj["name"].toString());
            file->setModifiedDate(QDateTime::fromString(obj["modified_date"].toString(), Qt::DateFormat::ISODate));
            files->append(*file);
        }

        emit listFilesFinished(*files);
    });
}

void NextcloudClient::parseError(QNetworkReply *reply, const function<void()> &action) {
    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        auto data = reply->readAll();
        auto jsonDoc = QJsonDocument::fromJson(data);

        if (jsonDoc.isObject()) {
            emit httpError(jsonDoc.object()["error"].toString());
        } else {
            emit httpError(data);
        }
    } else {
        action();
    }
}

QNetworkRequest NextcloudClient::getRequest() {
    auto request = QNetworkRequest();
    request.setUrl(QStringLiteral("http://%1:%2/nextcloud")
                           .arg(AppSettings::serverHost())
                           .arg(AppSettings::serverPort()));

    return request;
}
