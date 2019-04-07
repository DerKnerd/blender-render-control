//
// Created by imanuel on 06.04.19.
//

#ifndef GUI_BLENDERCLIENT_H
#define GUI_BLENDERCLIENT_H


#include <QtCore/QArgument>
#include <QtWebSockets/QWebSocket>
#include <QNetworkReply>
#include <types/File.h>

class BlenderClient : public QObject {
Q_OBJECT
public:
    BlenderClient();

public slots:

    void startRender();

    void stopRender();

    void pauseRender();

    void openSocket(const QString &url);

    void closeSocket();

    void addToQueue(const QStringList &files);

    void removeFromQueue(const QStringList &files);

    void getQueue();

private:
    void onConnected();

    void onTextMessageReceived(const QString &message);

    void onFinished(QNetworkReply *reply);

    QWebSocket socketClient;

signals:

    void connected();

    void queueListed(const QList<QString> &files);

    void logReceived(const QString &message, const QString &file);
};


#endif //GUI_BLENDERCLIENT_H
