//
// Created by imanuel on 06.04.19.
//

#include "BlenderClient.h"

BlenderClient::BlenderClient() {
    accessManager = new QNetworkAccessManager();
}

void BlenderClient::startRender() {
    connect(accessManager, &QNetworkAccessManager::finished, this, &BlenderClient::onStartRenderFinished);

    auto data = QByteArray();
    auto request = this->getRequest();

    accessManager->put(request, data);
}

void BlenderClient::stopRender() {
    connect(accessManager, &QNetworkAccessManager::finished, this, &BlenderClient::onStopRenderFinished);

    auto request = this->getRequest();
    auto urlQuery = QUrlQuery();

    urlQuery.addQueryItem("force", "true");
    request.url().setQuery(urlQuery);

    accessManager->deleteResource(request);
}

void BlenderClient::pauseRender() {
    connect(accessManager, &QNetworkAccessManager::finished, this, &BlenderClient::onPauseRenderFinished);

    auto request = this->getRequest();
    auto urlQuery = QUrlQuery();

    urlQuery.addQueryItem("force", "true");
    request.url().setQuery(urlQuery);

    accessManager->deleteResource(request);
}

void BlenderClient::onStartRenderFinished(QNetworkReply *reply) {
    disconnect(accessManager, &QNetworkAccessManager::finished, this, &BlenderClient::onStartRenderFinished);

    parseError(reply, [this]() {
        emit startRenderFinished();
    });
}

void BlenderClient::onStopRenderFinished(QNetworkReply *reply) {
    disconnect(accessManager, &QNetworkAccessManager::finished, this, &BlenderClient::onStopRenderFinished);

    parseError(reply, [this]() {
        emit stopRenderFinished();
    });
}

void BlenderClient::onPauseRenderFinished(QNetworkReply *reply) {
    disconnect(accessManager, &QNetworkAccessManager::finished, this, &BlenderClient::onPauseRenderFinished);

    parseError(reply, [this]() {
        emit pauseRenderFinished();
    });
}

void BlenderClient::parseError(QNetworkReply *reply, const function<void()> &action) {
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

QNetworkRequest BlenderClient::getRequest() {
    auto request = QNetworkRequest();
    request.setUrl(QStringLiteral("http://%1:%2/blender")
                           .arg(AppSettings::serverHost())
                           .arg(AppSettings::serverPort()));

    return request;
}
