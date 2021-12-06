
add_library(Qt5::AudioCaptureServicePlugin MODULE IMPORTED)


if(EXISTS "${_qt5Multimedia_install_prefix}/plugins/mediaservice/qtmedia_audioengine.dll")
    _populate_Multimedia_plugin_properties(AudioCaptureServicePlugin RELEASE "mediaservice/qtmedia_audioengine.dll" FALSE)
endif()
if(EXISTS "${_qt5Multimedia_install_prefix}/debug/plugins/mediaservice/qtmedia_audioengined.dll")
    _populate_Multimedia_plugin_properties(AudioCaptureServicePlugin DEBUG "mediaservice/qtmedia_audioengined.dll" FALSE)
endif()

list(APPEND Qt5Multimedia_PLUGINS Qt5::AudioCaptureServicePlugin)
set_property(TARGET Qt5::Multimedia APPEND PROPERTY QT_ALL_PLUGINS_mediaservice Qt5::AudioCaptureServicePlugin)
set_property(TARGET Qt5::AudioCaptureServicePlugin PROPERTY QT_PLUGIN_TYPE "mediaservice")
set_property(TARGET Qt5::AudioCaptureServicePlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::AudioCaptureServicePlugin PROPERTY QT_PLUGIN_CLASS_NAME "AudioCaptureServicePlugin")
