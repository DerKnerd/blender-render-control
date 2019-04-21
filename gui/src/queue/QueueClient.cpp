//
// Created by imanuel on 21.04.19.
//

#include "QueueClient.h"

QueueClient::QueueClient() {
    accessManager = new QNetworkAccessManager();
}

void QueueClient::get() {
    connect(accessManager, &QNetworkAccessManager::finished, this, &QueueClient::onGetFinished);
    auto request = getRequest();

    accessManager->get(request);
}

void QueueClient::add(const QList<QueueEntry> &entries) {
    connect(accessManager, &QNetworkAccessManager::finished, this, &QueueClient::onAddFinished);
    auto request = getRequest();
    auto jsonData = QJsonArray();

    for (auto entry = entries.constBegin(); entry < entries.constEnd(); ++entry) {
        auto item = new QJsonObject();
        item->insert("path", entry->getPath());
        item->insert("height", entry->getHeight());
        item->insert("width", entry->getWidth());
        jsonData.append(*item);
    }

    auto jsonDoc = QJsonDocument(jsonData);
    auto body = jsonDoc.toJson(QJsonDocument::Compact);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    accessManager->post(request, body);
}

void QueueClient::remove(const QStringList &files) {
    connect(accessManager, &QNetworkAccessManager::finished, this, &QueueClient::onRemoveFinished);

    auto request = getRequest();
    auto jsonData = QJsonArray();

    for (auto file = files.constBegin(); file < files.constEnd(); ++file) {
        jsonData.append(*file);
    }

    accessManager->put(request, QJsonDocument(jsonData).toJson(QJsonDocument::JsonFormat::Compact));
}

void QueueClient::onGetFinished(QNetworkReply *reply) {
    disconnect(accessManager, &QNetworkAccessManager::finished, this, &QueueClient::onGetFinished);

    parseError(reply, [this, reply]() {
        auto entries = QList<QueueEntry>();
        auto data = reply->readAll();
        auto jsonDoc = QJsonDocument::fromJson(data);

        auto responseArray = jsonDoc.array();
        for (auto item = responseArray.constBegin(); item < responseArray.constEnd(); ++item) {
            auto entry = new QueueEntry();
            auto obj = item->toObject();
            entry->setHeight(obj["height"].toInt());
            entry->setWidth(obj["width"].toInt());
            entry->setPath(obj["path"].toString());

            entries.append(*entry);
        }

        emit getFinished(entries);
    });
}

void QueueClient::onAddFinished(QNetworkReply *reply) {
    disconnect(accessManager, &QNetworkAccessManager::finished, this, &QueueClient::onAddFinished);

    parseError(reply, [this]() {
        emit addFinished();
    });
}

void QueueClient::onRemoveFinished(QNetworkReply *reply) {
    disconnect(accessManager, &QNetworkAccessManager::finished, this, &QueueClient::onRemoveFinished);

    parseError(reply, [this]() {
        emit removeFinished();
    });
}

void QueueClient::parseError(QNetworkReply *reply, const function<void()> &action) {
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

QNetworkRequest QueueClient::getRequest() {
    auto request = QNetworkRequest();
    request.setUrl(QStringLiteral("http://%1:%2/queue")
                           .arg(AppSettings::serverHost())
                           .arg(AppSettings::serverPort()));

    return request;
}