set(gui_ICONS
  16-apps-gui.png
  22-apps-gui.png
  32-apps-gui.png
  48-apps-gui.png
  sc-apps-gui.svgz
)

ecm_install_icons(ICONS ${gui_ICONS} DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor)
