#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include <nextcloud/NextcloudClient.h>
#include <blender/BlenderClient.h>
#include <logging/LogClient.h>
#include <queue/QueueClient.h>
#include <utility>

// KF headers
#include <KXmlGui/KXmlGuiWindow>
#include <KXmlGui/KActionCollection>
#include <KXmlGui/KToolBar>

#include <KConfigWidgets/KConfigDialog>

// Qt headers
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>

#include <ui_settings.h>
#include <AppSettings.h>
#include "MainView.h"

/**
 * This class serves as the main window for gui.  It handles the
 * menus, toolbars and status bars.
 *
 * @short Main window class
 * @author Imanuel Ulbricht <imanuel.ulbricht@outlook.com>
 * @version 0.1
 */
class MainWindow : public KXmlGuiWindow {
Q_OBJECT
public:
    /**
     * Default Constructor
     */
    MainWindow();

private slots:

    void connectToServer();

    void settingsConfigure();

    void startRendering();

    void addToRenderQueue();

    void pauseRendering();

    void stopRendering();

    void startRenderFinished();

    void stopRenderFinished();

    void pauseRenderFinished();

    void queueAddFinished();

    void queueRemoveFinished();

    void queueGetFinished(QList<QueueEntry> entries);

    void listFiles();

    void syncFiles();

    void listFilesFinished(QList<File> files);

    void httpError(QString message);

public slots:

    void quitApplication();

    void writeLog(const QString &message, const QString &file);

private:
    // this is the name of the root widget inside our Ui file
    // you can rename it in designer and then change it here
    Ui::Settings m_settings;
    MainView *m_mainView;

    NextcloudClient *nextcloudClient;
    BlenderClient *blenderClient;
    LogClient *logClient;
    QueueClient *queueClient;

    QLabel *statusbarFilename;
    QLabel *statusbarMessage;

    QProgressBar *statusbarProgress;

    QString *currentFilename;

    QAction *nextcloudSyncAction;
    QAction *nextcloudListAction;
    QAction *blenderStartRenderAction;
    QAction *blenderPauseRenderAction;
    QAction *blenderStopRenderAction;
    QAction *queueAddAction;
    QAction *queueRemoveAction;

    QSystemTrayIcon *trayIcon;

    void removeFromRenderQueue();

    void handleSettingsChanged();

};

#endif // GUIWINDOW_H
