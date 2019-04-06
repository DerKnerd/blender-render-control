//
// Created by imanuel on 04.04.19.
//

#ifndef GUI_NEXTCLOUDCLIENT_H
#define GUI_NEXTCLOUDCLIENT_H

#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QtXml/QtXml>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <types/File.h>
#include <iostream>
#include <guiSettings.h>
#include "guidebug.h"

class NextcloudClient : public QObject {
Q_OBJECT
public:
    NextcloudClient();

public slots:

    void startFileSync();

    void stopFileSync();

    void openSocket(const QString &url);

    void closeSocket();

    void listBlendFiles();

private:
    void onConnected();

    void onTextMessageReceived(const QString &message);

    void onBinaryMessageReceived(const QByteArray &data);

    void onFinished(QNetworkReply *reply);

    QWebSocket socketClient;

signals:

    void connected();

    void logReceived(const QString &message);

    void filesReceived(const QList<File> files);
};


#endif //GUI_NEXTCLOUDCLIENT_H
