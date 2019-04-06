/*
Copyright (C) 2019 by Imanuel Ulbricht <imanuel.ulbricht@outlook.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy 
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// application header
#include "guiwindow.h"
#include "guidebug.h"

// KF headers
#include <KCrash>
#include <KDBusService>
#include <KLocalizedString>

// Qt headers
#include <QApplication>
#include <QtCore/QCommandLineParser>
#include <KAboutData>

int main(int argc, char **argv) {
    QApplication application(argc, argv);

    KLocalizedString::setApplicationDomain("blender-render-control");
    KCrash::initialize();

    KAboutData aboutData( QStringLiteral("gui"),
                          i18n("gui"),
                          QStringLiteral("0.1"),
                          i18n("A Simple Application written with KDE Frameworks"),
                          KAboutLicense::Custom,
                          i18n("Copyright 2019, Imanuel Ulbricht <imanuel.ulbricht@outlook.com>"));

    aboutData.addAuthor(i18n("Imanuel Ulbricht"),i18n("Author"), QStringLiteral("imanuel.ulbricht@outlook.com"));
    aboutData.setOrganizationDomain("example.org");
    aboutData.setDesktopFileName(QStringLiteral("org.example.gui"));

    KAboutData::setApplicationData(aboutData);
    application.setWindowIcon(QIcon::fromTheme(QStringLiteral("gui")));

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    parser.process(application);
    aboutData.processCommandLine(&parser);

    auto *window = new GuiWindow;
    window->show();

    return QApplication::exec();
}
