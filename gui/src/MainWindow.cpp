#include "MainWindow.h"

MainWindow::MainWindow() : KXmlGuiWindow() {
    m_mainView = new MainView(this);
    nextcloudClient = new NextcloudClient();
    blenderClient = new BlenderClient();
    queueClient = new QueueClient();
    logClient = new LogClient();
    currentFilename = new QString("");

    setWindowIcon(QIcon::fromTheme(QStringLiteral("blender")));

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

    auto queueToolbar = addToolBar(i18n("Queue Toolbar"));
    queueToolbar->setObjectName(QStringLiteral("queueToolbar"));
    queueToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    nextcloudSyncAction = nextcloudToolbar->addAction(QStringLiteral("nextcloud_sync_files"));
    nextcloudSyncAction->setText(i18n("Sync Files"));
    nextcloudSyncAction->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

    nextcloudListAction = nextcloudToolbar->addAction(QStringLiteral("nextcloud_list_files"));
    nextcloudListAction->setText(i18n("List .blend Files"));
    nextcloudListAction->setIcon(QIcon::fromTheme("edit-find"));

    blenderStartRenderAction = blenderToolbar->addAction(QStringLiteral("blender_start_render"));
    blenderStartRenderAction->setText(i18n("Start rendering"));
    blenderStartRenderAction->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    blenderPauseRenderAction = blenderToolbar->addAction(QStringLiteral("blender_pause_render"));
    blenderPauseRenderAction->setText(i18n("Pause rendering"));
    blenderPauseRenderAction->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    blenderPauseRenderAction->setEnabled(false);

    blenderStopRenderAction = blenderToolbar->addAction(QStringLiteral("blender_stop_render"));
    blenderStopRenderAction->setText(i18n("Stop rendering"));
    blenderStopRenderAction->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    blenderStopRenderAction->setEnabled(false);

    queueAddAction = queueToolbar->addAction(QStringLiteral("queue_add"));
    queueAddAction->setText(i18n("Add to render queue"));
    queueAddAction->setIcon(QIcon::fromTheme("list-add"));

    queueRemoveAction = queueToolbar->addAction(QStringLiteral("queue_remove"));
    queueRemoveAction->setText(i18n("Remove from render queue"));
    queueRemoveAction->setIcon(QIcon::fromTheme("list-remove"));

    connect(nextcloudListAction, &QAction::triggered, this, &MainWindow::listFiles);
    connect(nextcloudSyncAction, &QAction::triggered, this, &MainWindow::syncFiles);

    connect(blenderStartRenderAction, &QAction::triggered, this, &MainWindow::startRendering);
    connect(blenderPauseRenderAction, &QAction::triggered, this, &MainWindow::pauseRendering);
    connect(blenderStopRenderAction, &QAction::triggered, this, &MainWindow::stopRendering);

    connect(queueAddAction, &QAction::triggered, this, &MainWindow::addToRenderQueue);
    connect(queueRemoveAction, &QAction::triggered, this, &MainWindow::removeFromRenderQueue);

    auto actionCollection = this->actionCollection();
    KStandardAction::preferences(this, SLOT(settingsConfigure()), actionCollection);

    auto quitAction = KStandardAction::quit(this, SLOT(quitApplication()), actionCollection);
    auto contextMenu = new QMenu();
    contextMenu->addAction(blenderStartRenderAction);
    contextMenu->addAction(blenderPauseRenderAction);
    contextMenu->addAction(blenderStopRenderAction);
    contextMenu->addSeparator();
    contextMenu->addAction(quitAction);

    trayIcon->setContextMenu(contextMenu);

    setupGUI();

    connect(logClient, &LogClient::logReceived, this, &MainWindow::writeLog);

    connect(blenderClient, &BlenderClient::startRenderFinished, this, &MainWindow::startRenderFinished);
    connect(blenderClient, &BlenderClient::stopRenderFinished, this, &MainWindow::stopRenderFinished);
    connect(blenderClient, &BlenderClient::pauseRenderFinished, this, &MainWindow::pauseRenderFinished);

    connect(nextcloudClient, &NextcloudClient::listFilesFinished, this, &MainWindow::listFilesFinished);

    connect(queueClient, &QueueClient::addFinished, this, &MainWindow::queueAddFinished);
    connect(queueClient, &QueueClient::removeFinished, this, &MainWindow::queueRemoveFinished);
    connect(queueClient, &QueueClient::getFinished, this, &MainWindow::queueGetFinished);

    connect(blenderClient, &BlenderClient::httpError, this, &MainWindow::httpError);
    connect(queueClient, &QueueClient::httpError, this, &MainWindow::httpError);
    connect(nextcloudClient, &NextcloudClient::httpError, this, &MainWindow::httpError);

    if (!AppSettings::serverHost().trimmed().isEmpty()) {
        connectToServer();
    }
}

void MainWindow::startRenderFinished() {
    statusbarMessage->setText(i18n("Started rendering"));
}

void MainWindow::pauseRenderFinished() {
    statusbarMessage->setText(i18n("Paused rendering"));
}

void MainWindow::stopRenderFinished() {
    statusbarMessage->setText(i18n("Stopped rendering"));
}

void MainWindow::connectToServer() {
    logClient->openSocket();
}

void MainWindow::settingsConfigure() {
    if (KConfigDialog::showDialog(QStringLiteral("settings"))) {
        return;
    }

    auto generalSettingsPage = new QWidget;
    m_settings.setupUi(generalSettingsPage);
    auto dialog = new KConfigDialog(this, QStringLiteral("settings"), AppSettings::self());
    auto page = dialog->addPage(generalSettingsPage, i18n("General"), QStringLiteral("package_setting"));
    page->setIcon(QIcon::fromTheme("systemsettings"));
    connect(dialog, &KConfigDialog::settingsChanged, this, &MainWindow::handleSettingsChanged);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::handleSettingsChanged() {
    logClient->closeSocket();

    connectToServer();
}

void MainWindow::startRendering() {
    blenderClient->startRender();

    blenderStartRenderAction->setEnabled(false);
    queueAddAction->setEnabled(true);
    blenderStopRenderAction->setEnabled(true);
    blenderPauseRenderAction->setEnabled(true);
}

void MainWindow::pauseRendering() {
    blenderClient->pauseRender();

    statusbarProgress->setRange(0, 0);

    blenderStartRenderAction->setEnabled(true);
    queueAddAction->setEnabled(true);
    blenderStopRenderAction->setEnabled(true);
    blenderPauseRenderAction->setEnabled(false);
}

void MainWindow::stopRendering() {
    blenderClient->stopRender();

    m_mainView->onLogReceived(i18n("Stopped rendering"));

    blenderStartRenderAction->setEnabled(true);
    queueAddAction->setEnabled(true);
    blenderStopRenderAction->setEnabled(false);
    blenderPauseRenderAction->setEnabled(false);
}

void MainWindow::writeLog(const QString &message, const QString &file) {
    m_mainView->onLogReceived(message);

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

    if (file != nullptr) {
        blenderStopRenderAction->setEnabled(true);
        blenderPauseRenderAction->setEnabled(true);

        auto fileInfo = QFileInfo(file);
        auto filenameWithoutExtension = fileInfo.fileName().section(".", 0, 0);

        if (currentFilename->compare(file) == 0) {
            m_mainView->onLogReceived(QStringLiteral("%1:").arg(file));
        }

        statusbarFilename->setText(filenameWithoutExtension);

        if (message.compare("Blender quit") == 0) {
            blenderStopRenderAction->setEnabled(false);
            blenderPauseRenderAction->setEnabled(false);
            blenderStartRenderAction->setVisible(true);
            statusbarProgress->hide();
            queueClient->get();

            trayIcon->showMessage(filenameWithoutExtension, i18n("The file %1 was rendered", filenameWithoutExtension));
        } else if (message.compare("Added files to queue") == 0) {
            queueClient->get();
            trayIcon->showMessage("Added files to queue", i18n("Added selected files to rendering queue"));
        } else if (message.compare("Removed files from queue") == 0) {
            queueClient->get();
            trayIcon->showMessage("Removed files from queue", i18n("Removed selected files from rendering queue"));
        } else {
            queueAddAction->setEnabled(true);
            blenderStartRenderAction->setEnabled(false);
        }
    }
}

void MainWindow::addToRenderQueue() {
    auto files = m_mainView->getFiles();

    queueClient->add(files);
    m_mainView->uncheckAllFiles();
}

void MainWindow::removeFromRenderQueue() {
    auto files = m_mainView->getSelectedFilesFromQueue();

    queueClient->remove(files);
}

void MainWindow::quitApplication() {
    logClient->closeSocket();
    trayIcon->hide();
    QApplication::closeAllWindows();
}

void MainWindow::listFilesFinished(QList<File> files) {
    m_mainView->onFilesReceived(files);
}

void MainWindow::listFiles() {
    nextcloudClient->listFiles();
}

void MainWindow::syncFiles() {
    nextcloudClient->startSync();
}

void MainWindow::httpError(QString message) {
    m_mainView->onLogReceived(message);
}

void MainWindow::queueAddFinished() {
    statusbarMessage->setText(i18n("Added files to queue"));
    queueClient->get();
}

void MainWindow::queueRemoveFinished() {
    statusbarMessage->setText(i18n("Removed files from queue"));
    queueClient->get();
}

void MainWindow::queueGetFinished(QList<QueueEntry> entries) {
    auto queue = QStringList();
    for (auto entry = entries.constBegin(); entry < entries.constEnd(); ++entry) {
        auto fileInfo = QFileInfo(entry->getPath());
        auto filenameWithoutExtension = fileInfo.fileName().section(".", 0, 0);
        queue.append(
                QStringLiteral("%1 (%2 x %3)")
                        .arg(filenameWithoutExtension)
                        .arg(entry->getWidth())
                        .arg(entry->getHeight())
        );
    }

    m_mainView->updateQueue(queue);
}
