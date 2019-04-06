// application headers
#include "guiwindow.h"

#include "MainView.h"
#include "guidebug.h"

// KF headers
#include <KActionCollection>
#include <KConfigDialog>
#include <KToolBar>

GuiWindow::GuiWindow() : KXmlGuiWindow() {
    m_mainView = new MainView(this);
    nextcloudClient = new NextcloudClient();
    blenderClient = new BlenderClient();
    currentFilename = new QString("");

    statusbarFilename = new QLabel(statusBar());
    statusbarMessage = new QLabel(statusBar());
    statusbarProgress = new QProgressBar(statusBar());
    statusbarProgress->setFixedWidth(150);
    statusbarProgress->hide();

    setCentralWidget(m_mainView);

    statusBar()->addWidget(statusbarFilename);
    statusBar()->addWidget(statusbarMessage, 1);
    statusBar()->addWidget(statusbarProgress);

    auto nextcloudToolbar = addToolBar(i18n("Nextcloud Toolbar"));
    nextcloudToolbar->setObjectName(QStringLiteral("nextcloudToolbar"));
    nextcloudToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    auto blenderToolbar = addToolBar(i18n("Blender Toolbar"));
    blenderToolbar->setObjectName(QStringLiteral("blenderToolbar"));
    blenderToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    nextcloudSyncAction = nextcloudToolbar->addAction(QStringLiteral("nextcloud_sync_files"));
    nextcloudSyncAction->setText(i18n("Sync Files"));
    nextcloudSyncAction->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    nextcloudSyncAction->setEnabled(false);

    nextcloudListAction = nextcloudToolbar->addAction(QStringLiteral("nextcloud_list_files"));
    nextcloudListAction->setText(i18n("List .blend Files"));
    nextcloudListAction->setIcon(QIcon::fromTheme("edit-find"));
    nextcloudListAction->setEnabled(false);

    blenderStartRenderAction = blenderToolbar->addAction(QStringLiteral("blender_start_render"));
    blenderStartRenderAction->setText(i18n("Render selected files"));
    blenderStartRenderAction->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    blenderStartRenderAction->setEnabled(false);

    blenderPauseRenderAction = blenderToolbar->addAction(QStringLiteral("blender_pause_render"));
    blenderPauseRenderAction->setText(i18n("Pause rendering"));
    blenderPauseRenderAction->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    blenderPauseRenderAction->setEnabled(false);

    blenderStopRenderAction = blenderToolbar->addAction(QStringLiteral("blender_stop_render"));
    blenderStopRenderAction->setText(i18n("Stop rendering"));
    blenderStopRenderAction->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    blenderStopRenderAction->setEnabled(false);

    connect(nextcloudSyncAction, &QAction::triggered, nextcloudClient, &NextcloudClient::startFileSync);
    connect(nextcloudListAction, &QAction::triggered, nextcloudClient, &NextcloudClient::listBlendFiles);

    connect(nextcloudClient, &NextcloudClient::logReceived, this, &GuiWindow::writeNextcloudLog);
    connect(nextcloudClient, &NextcloudClient::filesReceived, m_mainView, &MainView::onFilesReceived);
    connect(nextcloudClient, &NextcloudClient::connected, this, &GuiWindow::nextcloudConnected);

    connect(blenderStartRenderAction, &QAction::triggered, this, &GuiWindow::startRendering);
    connect(blenderPauseRenderAction, &QAction::triggered, this, &GuiWindow::pauseRendering);
    connect(blenderStopRenderAction, &QAction::triggered, this, &GuiWindow::stopRendering);

    connect(blenderClient, &BlenderClient::logReceived, this, &GuiWindow::writeBlenderLog);
    connect(blenderClient, &BlenderClient::connected, this, &GuiWindow::blenderConnected);

    KActionCollection *actionCollection = this->actionCollection();
    KStandardAction::preferences(this, SLOT(settingsConfigure()), actionCollection);

    setupGUI();

    if (!guiSettings::serverHost().trimmed().isEmpty()) {
        connectToServer();
    }
}

GuiWindow::~GuiWindow() {
}

void GuiWindow::connectToServer() {
    auto port = guiSettings::serverPort();
    auto host = guiSettings::serverHost().trimmed();

    auto nextcloudEndpoint = QStringLiteral("ws://%1:%2/nextcloud-control").arg(host).arg(port);
    auto blenderEndpoint = QStringLiteral("ws://%1:%2/blender-control").arg(host).arg(port);

    nextcloudClient->openSocket(nextcloudEndpoint);
    blenderClient->openSocket(blenderEndpoint);
}

void GuiWindow::settingsConfigure() {
    if (KConfigDialog::showDialog(QStringLiteral("settings"))) {
        return;
    }

    QWidget *generalSettingsPage = new QWidget;
    m_settings.setupUi(generalSettingsPage);
    KConfigDialog *dialog = new KConfigDialog(this, QStringLiteral("settings"), guiSettings::self());
    auto page = dialog->addPage(generalSettingsPage, i18n("General"), QStringLiteral("package_setting"));
    page->setIcon(QIcon::fromTheme("systemsettings"));
    connect(dialog, &KConfigDialog::settingsChanged, this, &GuiWindow::handleSettingsChanged);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void GuiWindow::handleSettingsChanged() {
    nextcloudClient->closeSocket();
    blenderClient->closeSocket();

    connectToServer();
}

void GuiWindow::writeNextcloudLog(const QString &message) {
    m_mainView->onLogReceived(message);
    statusbarMessage->setText(message);
}

void GuiWindow::startRendering() {
    auto files = m_mainView->getFiles();
    blenderClient->startRender(files);
    m_mainView->uncheckAllFiles();
}

void GuiWindow::pauseRendering() {
    blenderClient->pauseRender();

    statusbarProgress->setRange(0, 0);
}

void GuiWindow::stopRendering() {
    blenderClient->stopRender();

    m_mainView->onLogReceived(i18n("Stopped rendering"));
}

void GuiWindow::writeBlenderLog(const QString &message, const QString &file) {
    blenderStopRenderAction->setEnabled(true);
    blenderPauseRenderAction->setEnabled(true);

    auto fileInfo = QFileInfo(file);
    auto filenameWithoutExtension = fileInfo.fileName().section(".", 0, 0);

    if (currentFilename->compare(file) == 0) {
        m_mainView->onLogReceived(QStringLiteral("%1:").arg(file));
    }

    m_mainView->onLogReceived(message);

    statusbarFilename->setText(filenameWithoutExtension);
    statusbarMessage->setText(message);

    auto splittedMessage = message.split(QStringLiteral("|"));
    auto tilesProgress = splittedMessage.last().trimmed();

    tilesProgress = tilesProgress.remove(0, QStringLiteral("Rendered").length());
    tilesProgress = tilesProgress.remove(tilesProgress.indexOf(QStringLiteral("Tiles")), tilesProgress.length());

    auto progress = tilesProgress.split(QStringLiteral("/"));

    if (progress.length() >= 2) {
        auto renderedTiles = progress[0].toInt();
        auto totalTiles = progress[1].toInt();

        statusbarProgress->setMaximum(totalTiles);
        statusbarProgress->setValue(renderedTiles);
        if (statusbarProgress->isHidden()) {
            statusbarProgress->show();
        }
    }

    if (message.compare("Blender quit") == 0) {
        blenderStopRenderAction->setEnabled(false);
        blenderPauseRenderAction->setEnabled(false);
        statusbarProgress->hide();
    }
}

void GuiWindow::blenderConnected() {
    blenderStartRenderAction->setEnabled(true);
}

void GuiWindow::nextcloudConnected() {
    nextcloudSyncAction->setEnabled(true);
    nextcloudListAction->setEnabled(true);
}
