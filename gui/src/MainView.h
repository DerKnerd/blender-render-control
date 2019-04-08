//
// Created by imanuel on 04.04.19.
//

#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H


#include <QtCore/QArgument>
#include <QtCore/QFileInfo>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QPlainTextEdit>
#include <QTableWidgetItem>
#include <QScrollBar>

#include <types/File.h>
#include <algorithm>
#include "ui_MainView.h"
#include "AppSettings.h"

class MainView : public QWidget {
Q_OBJECT

public:
    explicit MainView(QWidget *parent);

    ~MainView() override;

public slots:

    void onLogReceived(const QString &qString);

    void onFilesReceived(QList<File> &data);

    QStringList getSelectedFilesFromQueue() const;

    QStringList getFiles() const;

    void uncheckAllFiles();

    void updateQueue(const QStringList &files);

private:
    Ui::MainView m_ui;
};


#endif //GUI_MAINWINDOW_H
