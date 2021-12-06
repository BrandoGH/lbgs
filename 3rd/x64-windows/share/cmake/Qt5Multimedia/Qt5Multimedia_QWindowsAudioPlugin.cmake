
add_library(Qt5::QWindowsAudioPlugin MODULE IMPORTED)


if(EXISTS "${_qt5Multimedia_install_prefix}/plugins/audio/qtaudio_windows.dll")
    _populate_Multimedia_plugin_properties(QWindowsAudioPlugin RELEASE "audio/qtaudio_windows.dll" FALSE)
endif()
if(EXISTS "${_qt5Multimedia_install_prefix}/debug/plugins/audio/qtaudio_windowsd.dll")
    _populate_Multimedia_plugin_properties(QWindowsAudioPlugin DEBUG "audio/qtaudio_windowsd.dll" FALSE)
endif()

list(APPEND Qt5Multimedia_PLUGINS Qt5::QWindowsAudioPlugin)
set_property(TARGET Qt5::Multimedia APPEND PROPERTY QT_ALL_PLUGINS_audio Qt5::QWindowsAudioPlugin)
set_property(TARGET Qt5::QWindowsAudioPlugin PROPERTY QT_PLUGIN_TYPE "audio")
set_property(TARGET Qt5::QWindowsAudioPlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QWindowsAudioPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QWindowsAudioPlugin")
