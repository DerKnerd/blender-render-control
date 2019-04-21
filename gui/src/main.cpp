/*
Copyright (C) 2019 by Imanuel Ulbricht <imanuel.ulbricht@outlook.com>
*/

// application header
#include "MainWindow.h"

// KF headers
#include <KCrash/KCrash>

#include <KI18n/KLocalizedString>

#include <KCoreAddons/KAboutData>

// Qt headers
#include <QtCore/QCommandLineParser>

#include <QtWidgets/QApplication>

int main(int argc, char **argv) {
    QApplication application(argc, argv);

    KLocalizedString::setApplicationDomain("blender-render-control");
    KCrash::initialize();

    auto aboutData = KAboutData(i18n("Blender Render Control Center"),
                                i18n("Blender Render Control Center"),
                                QStringLiteral("1.0"),
                                i18n("A simple application to control the blender render control server"),
                                KAboutLicense::Custom,
                                i18n("Copyright 2019, Imanuel Ulbricht <imanuel.ulbricht@outlook.com>"));

    aboutData.addAuthor(i18n("Imanuel Ulbricht"), i18n("Author"), QStringLiteral("imanuel.ulbricht@outlook.com"));
    aboutData.setOrganizationDomain("imanuel.ulbricht.codes");
    aboutData.setDesktopFileName(QStringLiteral("codes.ulbricht.imanuel.blender-render-control"));

    KAboutData::setApplicationData(aboutData);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("blender")));
    QApplication::setApplicationDisplayName(i18n("Blender Render Control Center"));
    QApplication::setApplicationName(i18n("Blender Render Control Center"));

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    parser.process(application);
    aboutData.processCommandLine(&parser);

    auto *window = new MainWindow();
    window->show();

    QObject::connect(&application, &QApplication::aboutToQuit, window, &MainWindow::quitApplication);

    return QApplication::exec();
}
