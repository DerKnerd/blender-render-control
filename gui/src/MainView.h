//
// Created by imanuel on 04.04.19.
//

#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H


#include <QtCore/QArgument>
#include <QtWidgets/QWidget>
#include <QPlainTextEdit>
#include <QTableWidgetItem>
#include <QScrollBar>
#include <types/File.h>
#include "ui_mainView.h"

class MainView : public QWidget {
Q_OBJECT

public:
    explicit MainView(QWidget *parent);

    ~MainView() override;

public slots:

    void onLogReceived(const QString &qString);

    void onFilesReceived(const QList<File> &data);

    QStringList getFiles();

    void uncheckAllFiles();

private:
    Ui::MainView m_ui;
};


#endif //GUI_MAINWINDOW_H
