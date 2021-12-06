
add_library(Qt5::QQmlDebugServerFactory MODULE IMPORTED)


if(EXISTS "${_qt5Qml_install_prefix}/plugins/qmltooling/qmldbg_server.dll")
    _populate_Qml_plugin_properties(QQmlDebugServerFactory RELEASE "qmltooling/qmldbg_server.dll" FALSE)
endif()
if(EXISTS "${_qt5Qml_install_prefix}/debug/plugins/qmltooling/qmldbg_serverd.dll")
    _populate_Qml_plugin_properties(QQmlDebugServerFactory DEBUG "qmltooling/qmldbg_serverd.dll" FALSE)
endif()

list(APPEND Qt5Qml_PLUGINS Qt5::QQmlDebugServerFactory)
set_property(TARGET Qt5::Qml APPEND PROPERTY QT_ALL_PLUGINS_qmltooling Qt5::QQmlDebugServerFactory)
set_property(TARGET Qt5::QQmlDebugServerFactory PROPERTY QT_PLUGIN_TYPE "qmltooling")
set_property(TARGET Qt5::QQmlDebugServerFactory PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QQmlDebugServerFactory PROPERTY QT_PLUGIN_CLASS_NAME "QQmlDebugServerFactory")
