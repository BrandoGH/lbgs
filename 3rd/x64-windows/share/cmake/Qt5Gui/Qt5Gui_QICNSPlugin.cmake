
add_library(Qt5::QICNSPlugin MODULE IMPORTED)


if(EXISTS "${_qt5Gui_install_prefix}/plugins/imageformats/qicns.dll")
    _populate_Gui_plugin_properties(QICNSPlugin RELEASE "imageformats/qicns.dll" FALSE)
endif()
if(EXISTS "${_qt5Gui_install_prefix}/debug/plugins/imageformats/qicnsd.dll")
    _populate_Gui_plugin_properties(QICNSPlugin DEBUG "imageformats/qicnsd.dll" FALSE)
endif()

list(APPEND Qt5Gui_PLUGINS Qt5::QICNSPlugin)
set_property(TARGET Qt5::Gui APPEND PROPERTY QT_ALL_PLUGINS_imageformats Qt5::QICNSPlugin)
set_property(TARGET Qt5::QICNSPlugin PROPERTY QT_PLUGIN_TYPE "imageformats")
set_property(TARGET Qt5::QICNSPlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QICNSPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QICNSPlugin")
