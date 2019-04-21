//
// Created by imanuel on 04.04.19.
//

#ifndef GUI_NEXTCLOUDCLIENT_H
#define GUI_NEXTCLOUDCLIENT_H

#include <QtCore/QObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <AppSettings.h>
#include <types/File.h>

using namespace std;

class NextcloudClient : public QObject {
Q_OBJECT
public:
    NextcloudClient();

public slots:

    void startSync();

    void listFiles();

private:
    void onStartSyncFinished(QNetworkReply *reply);

    void onListFilesFinished(QNetworkReply *reply);

    void parseError(QNetworkReply *reply, const function<void()> &action);

    QNetworkRequest getRequest();

    QNetworkAccessManager *accessManager;

signals:

    void startSyncFinished();

    void listFilesFinished(QList<File> &files);

    void httpError(QString message);
};


#endif //GUI_NEXTCLOUDCLIENT_H
