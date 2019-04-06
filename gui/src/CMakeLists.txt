set(gui_SRCS
        main.cpp
        guiwindow.cpp
        MainView.cpp MainView.h nextcloud/NextcloudClient.cpp nextcloud/NextcloudClient.h types/File.cpp types/File.h nextcloud/BlenderClient.cpp nextcloud/BlenderClient.h)

ecm_qt_declare_logging_category(gui_SRCS
        HEADER guidebug.h
        IDENTIFIER BLENDER_RENDER_CONTROL_APP
        CATEGORY_NAME "blender render control app"
        )
kconfig_add_kcfg_files(gui_SRCS guiSettings.kcfgc)
ki18n_wrap_ui(gui_SRCS
        mainView.ui
        settings.ui
        )

add_executable(gui ${gui_SRCS})

target_link_libraries(gui
        KF5::CoreAddons
        KF5::I18n
        KF5::XmlGui
        KF5::ConfigWidgets
        KF5::DBusAddons
        KF5::Crash
        Qt5::Widgets
        Qt5::WebSockets
        )

install(TARGETS gui ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS org.example.gui.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.example.gui.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
