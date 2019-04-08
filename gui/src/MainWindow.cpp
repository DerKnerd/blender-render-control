#include "MainWindow.h"

MainWindow::MainWindow() : KXmlGuiWindow() {
    m_mainView = new MainView(this);
    nextcloudClient = new NextcloudClient();
    blenderClient = new BlenderClient();
    currentFilename = new QString("");
    trayIcon = new QSystemTrayIcon();
    trayIcon->setIcon(QIcon::fromTheme(QStringLiteral("blender")));
    trayIcon->show();

    statusbarFilename = new QLabel(statusBar());
    statusbarMessage = new QLabel(statusBar());
    statusbarProgress = new QProgressBar(statusBar());
    statusbarProgress->setFixedWidth(200);
    statusbarProgress->hide();

    setCentralWidget(m_mainView);
    setWindowTitle(i18n("Blender Render Control Center"));

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

    blenderAddToQueueAction = blenderToolbar->addAction(QStringLiteral("blender_add_to_queue_files"));
    blenderAddToQueueAction->setText(i18n("Add to render queue"));
    blenderAddToQueueAction->setIcon(QIcon::fromTheme("list-add"));
    blenderAddToQueueAction->setVisible(false);

    blenderStartRenderAction = blenderToolbar->addAction(QStringLiteral("blender_start_render"));
    blenderStartRenderAction->setText(i18n("Render selected files"));
    blenderStartRenderAction->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    blenderStartRenderAction->setEnabled(false);

    blenderRemoveFromQueueAction = blenderToolbar->addAction(QStringLiteral("blender_remove_from_queue_files"));
    blenderRemoveFromQueueAction->setText(i18n("Remove from render queue"));
    blenderRemoveFromQueueAction->setIcon(QIcon::fromTheme("list-remove"));
    blenderRemoveFromQueueAction->setEnabled(false);

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

    connect(nextcloudClient, &NextcloudClient::logReceived, this, &MainWindow::writeNextcloudLog);
    connect(nextcloudClient, &NextcloudClient::filesReceived, m_mainView, &MainView::onFilesReceived);
    connect(nextcloudClient, &NextcloudClient::connected, this, &MainWindow::nextcloudConnected);

    connect(blenderStartRenderAction, &QAction::triggered, this, &MainWindow::startRendering);
    connect(blenderAddToQueueAction, &QAction::triggered, this, &MainWindow::addToRenderQueue);
    connect(blenderRemoveFromQueueAction, &QAction::triggered, this, &MainWindow::removeFromRenderQueue);
    connect(blenderPauseRenderAction, &QAction::triggered, this, &MainWindow::pauseRendering);
    connect(blenderStopRenderAction, &QAction::triggered, this, &MainWindow::stopRendering);

    connect(blenderClient, &BlenderClient::logReceived, this, &MainWindow::writeBlenderLog);
    connect(blenderClient, &BlenderClient::connected, this, &MainWindow::blenderConnected);
    connect(blenderClient, &BlenderClient::queueListed, this, &MainWindow::blenderQueueListed);

    KActionCollection *actionCollection = this->actionCollection();
    KStandardAction::preferences(this, SLOT(settingsConfigure()), actionCollection);

    auto quitAction = KStandardAction::quit(this, SLOT(quitApplication()), actionCollection);
    auto contextMenu = new QMenu();
    contextMenu->addAction(quitAction);
    trayIcon->setContextMenu(contextMenu);

    setupGUI();

    if (!AppSettings::serverHost().trimmed().isEmpty()) {
        connectToServer();
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::connectToServer() {
    auto port = AppSettings::serverPort();
    auto host = AppSettings::serverHost().trimmed();

    auto nextcloudEndpoint = QStringLiteral("ws://%1:%2/nextcloud-control").arg(host).arg(port);
    auto blenderEndpoint = QStringLiteral("ws://%1:%2/blender-control").arg(host).arg(port);

    nextcloudClient->openSocket(nextcloudEndpoint);
    blenderClient->openSocket(blenderEndpoint);
    blenderClient->getQueue();
}

void MainWindow::settingsConfigure() {
    if (KConfigDialog::showDialog(QStringLiteral("settings"))) {
        return;
    }

    QWidget *generalSettingsPage = new QWidget;
    m_settings.setupUi(generalSettingsPage);
    KConfigDialog *dialog = new KConfigDialog(this, QStringLiteral("settings"),
                                              AppSettings::self());
    auto page = dialog->addPage(generalSettingsPage, i18n("General"), QStringLiteral("package_setting"));
    page->setIcon(QIcon::fromTheme("systemsettings"));
    connect(dialog, &KConfigDialog::settingsChanged, this, &MainWindow::handleSettingsChanged);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::handleSettingsChanged() {
    nextcloudClient->closeSocket();
    blenderClient->closeSocket();

    connectToServer();
}

void MainWindow::writeNextcloudLog(const QString &message) {
    m_mainView->onLogReceived(message);
    statusbarMessage->setText(message);
}

void MainWindow::startRendering() {
    auto files = m_mainView->getFiles();
    trayIcon->showMessage(i18n("Start rendering"),
                          i18n("Added %1 file(s) into the queue and started rendering", files.length()));
    blenderClient->addToQueue(files);
    blenderClient->startRender();
    m_mainView->uncheckAllFiles();
}

void MainWindow::pauseRendering() {
    blenderClient->pauseRender();

    statusbarProgress->setRange(0, 0);
}

void MainWindow::stopRendering() {
    blenderClient->stopRender();

    m_mainView->onLogReceived(i18n("Stopped rendering"));
}

void MainWindow::writeBlenderLog(const QString &message, const QString &file) {
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
        blenderAddToQueueAction->setVisible(false);
        blenderStartRenderAction->setVisible(true);
        statusbarProgress->hide();
        blenderClient->getQueue();

        trayIcon->showMessage(filenameWithoutExtension, i18n("The file %1 was rendered", filenameWithoutExtension));
    } else if (message.compare("Added files to queue") == 0) {
        blenderClient->getQueue();
        trayIcon->showMessage("Added files to queue", i18n("Added selected files to rendering queue"));
    } else if (message.compare("Removed files from queue") == 0) {
        blenderClient->getQueue();
        trayIcon->showMessage("Removed files from queue", i18n("Removed selected files from rendering queue"));
    } else {
        blenderAddToQueueAction->setVisible(true);
        blenderStartRenderAction->setVisible(false);
    }
}

void MainWindow::blenderConnected() {
    blenderStartRenderAction->setEnabled(true);
}

void MainWindow::nextcloudConnected() {
    nextcloudSyncAction->setEnabled(true);
    nextcloudListAction->setEnabled(true);
}

void MainWindow::addToRenderQueue() {
    auto files = m_mainView->getFiles();
    blenderClient->addToQueue(files);

    blenderClient->getQueue();
}

void MainWindow::removeFromRenderQueue() {
    auto files = m_mainView->getSelectedFilesFromQueue();
    blenderClient->removeFromQueue(files);

    blenderClient->getQueue();
}

void MainWindow::blenderQueueListed(const QList<QString> &files) {
    blenderRemoveFromQueueAction->setEnabled(files.count() > 0);

    m_mainView->updateQueue(files);
}

void MainWindow::quitApplication() {
    trayIcon->hide();
    QApplication::closeAllWindows();
}
