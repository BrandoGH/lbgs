
add_library(Qt5::QTgaPlugin MODULE IMPORTED)


if(EXISTS "${_qt5Gui_install_prefix}/plugins/imageformats/qtga.dll")
    _populate_Gui_plugin_properties(QTgaPlugin RELEASE "imageformats/qtga.dll" FALSE)
endif()
if(EXISTS "${_qt5Gui_install_prefix}/debug/plugins/imageformats/qtgad.dll")
    _populate_Gui_plugin_properties(QTgaPlugin DEBUG "imageformats/qtgad.dll" FALSE)
endif()

list(APPEND Qt5Gui_PLUGINS Qt5::QTgaPlugin)
set_property(TARGET Qt5::Gui APPEND PROPERTY QT_ALL_PLUGINS_imageformats Qt5::QTgaPlugin)
set_property(TARGET Qt5::QTgaPlugin PROPERTY QT_PLUGIN_TYPE "imageformats")
set_property(TARGET Qt5::QTgaPlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QTgaPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QTgaPlugin")
