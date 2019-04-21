//
// Created by imanuel on 06.04.19.
//

#ifndef GUI_BLENDERCLIENT_H
#define GUI_BLENDERCLIENT_H

#include <QtCore/QObject>
#include <QtCore/QUrlQuery>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <AppSettings.h>

using namespace std;

class BlenderClient : public QObject {
Q_OBJECT
public:
    BlenderClient();

public slots:

    void startRender();

    void stopRender();

    void pauseRender();

private:
    void onStartRenderFinished(QNetworkReply *reply);

    void onStopRenderFinished(QNetworkReply *reply);

    void onPauseRenderFinished(QNetworkReply *reply);

    void parseError(QNetworkReply *reply, const function<void()> &action);

    QNetworkRequest getRequest();

    QNetworkAccessManager *accessManager;

signals:

    void startRenderFinished();

    void stopRenderFinished();

    void pauseRenderFinished();

    void httpError(QString message);
};


#endif //GUI_BLENDERCLIENT_H
