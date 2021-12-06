
add_library(Qt5::QJp2Plugin MODULE IMPORTED)


if(EXISTS "${_qt5Gui_install_prefix}/plugins/imageformats/qjp2.dll")
    _populate_Gui_plugin_properties(QJp2Plugin RELEASE "imageformats/qjp2.dll" FALSE)
endif()
if(EXISTS "${_qt5Gui_install_prefix}/debug/plugins/imageformats/qjp2d.dll")
    _populate_Gui_plugin_properties(QJp2Plugin DEBUG "imageformats/qjp2d.dll" FALSE)
endif()

list(APPEND Qt5Gui_PLUGINS Qt5::QJp2Plugin)
set_property(TARGET Qt5::Gui APPEND PROPERTY QT_ALL_PLUGINS_imageformats Qt5::QJp2Plugin)
set_property(TARGET Qt5::QJp2Plugin PROPERTY QT_PLUGIN_TYPE "imageformats")
set_property(TARGET Qt5::QJp2Plugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QJp2Plugin PROPERTY QT_PLUGIN_CLASS_NAME "QJp2Plugin")
