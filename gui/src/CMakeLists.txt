set(blender_render_control_center_SRCS
        main.cpp
        MainWindow.cpp
        MainView.cpp
        MainView.h
        nextcloud/NextcloudClient.cpp
        nextcloud/NextcloudClient.h
        types/File.cpp
        types/File.h
        blender/BlenderClient.cpp
        blender/BlenderClient.h
        )

ecm_qt_declare_logging_category(blender_render_control_center_SRCS
        HEADER guidebug.h
        IDENTIFIER BLENDER_RENDER_CONTROL_APP
        CATEGORY_NAME "blender render control app"
        )
kconfig_add_kcfg_files(blender_render_control_center_SRCS AppSettings.kcfgc)
ki18n_wrap_ui(blender_render_control_center_SRCS
        MainView.ui
        settings.ui
        )

add_executable(blender_render_control_center ${blender_render_control_center_SRCS})

target_link_libraries(blender_render_control_center
        KF5::CoreAddons
        KF5::I18n
        KF5::XmlGui
        KF5::ConfigWidgets
        KF5::DBusAddons
        KF5::Crash
        Qt5::Widgets
        Qt5::WebSockets
        )

install(TARGETS blender_render_control_center ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS codes.ulbricht.imanuel.blender-render-control-center.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES codes.ulbricht.imanuel.blender-render-control-center.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
