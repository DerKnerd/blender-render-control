//
// Created by imanuel on 21.04.19.
//

#ifndef BLENDER_RENDER_CONTROL_CENTER_QUEUECLIENT_H
#define BLENDER_RENDER_CONTROL_CENTER_QUEUECLIENT_H

#include <QtCore/QObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include <AppSettings.h>

#include "types/QueueEntry.h"

using namespace std;

class QueueClient : public QObject {
Q_OBJECT
public:
    QueueClient();

public slots:

    void get();

    void add(const QList<QueueEntry> &entries);

    void remove(const QStringList &files);

private:
    void onGetFinished(QNetworkReply *reply);

    void onAddFinished(QNetworkReply *reply);

    void onRemoveFinished(QNetworkReply *reply);

    void parseError(QNetworkReply *reply, const function<void()> &action);

    QNetworkRequest getRequest();

    QNetworkAccessManager *accessManager;

signals:

    void getFinished(QList<QueueEntry> entries);

    void addFinished();

    void removeFinished();

    void httpError(QString message);
};


#endif //BLENDER_RENDER_CONTROL_CENTER_QUEUECLIENT_H
