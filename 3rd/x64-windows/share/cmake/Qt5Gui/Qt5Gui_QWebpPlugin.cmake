
add_library(Qt5::QWebpPlugin MODULE IMPORTED)


if(EXISTS "${_qt5Gui_install_prefix}/plugins/imageformats/qwebp.dll")
    _populate_Gui_plugin_properties(QWebpPlugin RELEASE "imageformats/qwebp.dll" FALSE)
endif()
if(EXISTS "${_qt5Gui_install_prefix}/debug/plugins/imageformats/qwebpd.dll")
    _populate_Gui_plugin_properties(QWebpPlugin DEBUG "imageformats/qwebpd.dll" FALSE)
endif()

list(APPEND Qt5Gui_PLUGINS Qt5::QWebpPlugin)
set_property(TARGET Qt5::Gui APPEND PROPERTY QT_ALL_PLUGINS_imageformats Qt5::QWebpPlugin)
set_property(TARGET Qt5::QWebpPlugin PROPERTY QT_PLUGIN_TYPE "imageformats")
set_property(TARGET Qt5::QWebpPlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QWebpPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QWebpPlugin")
