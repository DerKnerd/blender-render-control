//
// Created by imanuel on 06.04.19.
//

#ifndef GUI_BLENDERCLIENT_H
#define GUI_BLENDERCLIENT_H


#include <QtCore/QArgument>
#include <QtWebSockets/QWebSocket>

class BlenderClient : public QObject {
Q_OBJECT
public:
    BlenderClient();

public slots:

    void startRender(const QStringList &files);

    void stopRender();

    void pauseRender();

    void openSocket(const QString &url);

    void closeSocket();

private:
    void onConnected();

    void onTextMessageReceived(const QString &message);

    QWebSocket socketClient;

signals:

    void connected();

    void logReceived(const QString &message, const QString &file);
};


#endif //GUI_BLENDERCLIENT_H
