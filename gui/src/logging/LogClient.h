//
// Created by imanuel on 21.04.19.
//

#ifndef BLENDER_RENDER_CONTROL_CENTER_LOGCLIENT_H
#define BLENDER_RENDER_CONTROL_CENTER_LOGCLIENT_H

#include <QtCore/QObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <QtWebSockets/QWebSocket>

#include <AppSettings.h>

class LogClient : public QObject {
Q_OBJECT
public:
    LogClient();

public slots:

    void openSocket();

    void closeSocket();

private:
    void onConnected();

    void onTextMessageReceived(const QString &message);

    QWebSocket socketClient;

signals:

    void connected();

    void logReceived(const QString &message, const QString &file);
};


#endif //BLENDER_RENDER_CONTROL_CENTER_LOGCLIENT_H
