#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include <nextcloud/NextcloudClient.h>
#include <nextcloud/BlenderClient.h>
#include <utility>

#include <KXmlGuiWindow>
#include <QStatusBar>
#include <QtWidgets/QProgressBar>

#include "ui_settings.h"
#include "guiSettings.h"
#include "MainView.h"

class guiView;

/**
 * This class serves as the main window for gui.  It handles the
 * menus, toolbars and status bars.
 *
 * @short Main window class
 * @author Imanuel Ulbricht <imanuel.ulbricht@outlook.com>
 * @version 0.1
 */
class GuiWindow : public KXmlGuiWindow {
Q_OBJECT
public:
    /**
     * Default Constructor
     */
    GuiWindow();

    /**
     * Default Destructor
     */
    ~GuiWindow() override;

private slots:

    void connectToServer();

    void settingsConfigure();

    void startRendering();

    void pauseRendering();

    void stopRendering();

public slots:

    void writeNextcloudLog(const QString &message);

    void writeBlenderLog(const QString &message, const QString &file);

    void blenderConnected();

    void nextcloudConnected();

private:
    // this is the name of the root widget inside our Ui file
    // you can rename it in designer and then change it here
    Ui::Settings m_settings;
    MainView *m_mainView;
    NextcloudClient *nextcloudClient;
    BlenderClient *blenderClient;
    QLabel *statusbarFilename;
    QLabel *statusbarMessage;
    QProgressBar *statusbarProgress;
    QString *currentFilename;

    void handleSettingsChanged();

    QAction *nextcloudSyncAction;
    QAction *nextcloudStopSyncAction;
    QAction *nextcloudListAction;
    QAction *blenderStartRenderAction;
    QAction *blenderPauseRenderAction;
    QAction *blenderStopRenderAction;
};

#endif // GUIWINDOW_H
