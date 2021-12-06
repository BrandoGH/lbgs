
add_library(Qt5::QWasapiPlugin MODULE IMPORTED)


if(EXISTS "${_qt5Multimedia_install_prefix}/plugins/audio/qtaudio_wasapi.dll")
    _populate_Multimedia_plugin_properties(QWasapiPlugin RELEASE "audio/qtaudio_wasapi.dll" FALSE)
endif()
if(EXISTS "${_qt5Multimedia_install_prefix}/debug/plugins/audio/qtaudio_wasapid.dll")
    _populate_Multimedia_plugin_properties(QWasapiPlugin DEBUG "audio/qtaudio_wasapid.dll" FALSE)
endif()

list(APPEND Qt5Multimedia_PLUGINS Qt5::QWasapiPlugin)
set_property(TARGET Qt5::Multimedia APPEND PROPERTY QT_ALL_PLUGINS_audio Qt5::QWasapiPlugin)
set_property(TARGET Qt5::QWasapiPlugin PROPERTY QT_PLUGIN_TYPE "audio")
set_property(TARGET Qt5::QWasapiPlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QWasapiPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QWasapiPlugin")
