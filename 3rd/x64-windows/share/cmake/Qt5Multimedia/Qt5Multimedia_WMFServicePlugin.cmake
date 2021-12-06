
add_library(Qt5::WMFServicePlugin MODULE IMPORTED)


if(EXISTS "${_qt5Multimedia_install_prefix}/plugins/mediaservice/wmfengine.dll")
    _populate_Multimedia_plugin_properties(WMFServicePlugin RELEASE "mediaservice/wmfengine.dll" FALSE)
endif()
if(EXISTS "${_qt5Multimedia_install_prefix}/debug/plugins/mediaservice/wmfengined.dll")
    _populate_Multimedia_plugin_properties(WMFServicePlugin DEBUG "mediaservice/wmfengined.dll" FALSE)
endif()

list(APPEND Qt5Multimedia_PLUGINS Qt5::WMFServicePlugin)
set_property(TARGET Qt5::Multimedia APPEND PROPERTY QT_ALL_PLUGINS_mediaservice Qt5::WMFServicePlugin)
set_property(TARGET Qt5::WMFServicePlugin PROPERTY QT_PLUGIN_TYPE "mediaservice")
set_property(TARGET Qt5::WMFServicePlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::WMFServicePlugin PROPERTY QT_PLUGIN_CLASS_NAME "WMFServicePlugin")
