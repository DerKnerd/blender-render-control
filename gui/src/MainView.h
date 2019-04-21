//
// Created by imanuel on 04.04.19.
//

#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QtWidgets/QScrollBar>

#include <QtCore/QFileInfo>

#include <types/File.h>
#include <algorithm>
#include <types/QueueEntry.h>
#include "ui_MainView.h"
#include "AppSettings.h"

using namespace std;

class MainView : public QWidget {
Q_OBJECT

public:
    explicit MainView(QWidget *parent);

public slots:

    void onLogReceived(const QString &qString);

    void onFilesReceived(QList<File> &data);

    QStringList getSelectedFilesFromQueue() const;

    QList<QueueEntry> getFiles() const;

    void uncheckAllFiles();

    void updateQueue(const QStringList &files);

private:
    Ui::MainView m_ui;
};


#endif //GUI_MAINWINDOW_H
