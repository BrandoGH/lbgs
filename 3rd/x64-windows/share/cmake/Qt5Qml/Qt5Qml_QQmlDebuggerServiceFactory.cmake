
add_library(Qt5::QQmlDebuggerServiceFactory MODULE IMPORTED)


if(EXISTS "${_qt5Qml_install_prefix}/plugins/qmltooling/qmldbg_debugger.dll")
    _populate_Qml_plugin_properties(QQmlDebuggerServiceFactory RELEASE "qmltooling/qmldbg_debugger.dll" FALSE)
endif()
if(EXISTS "${_qt5Qml_install_prefix}/debug/plugins/qmltooling/qmldbg_debuggerd.dll")
    _populate_Qml_plugin_properties(QQmlDebuggerServiceFactory DEBUG "qmltooling/qmldbg_debuggerd.dll" FALSE)
endif()

list(APPEND Qt5Qml_PLUGINS Qt5::QQmlDebuggerServiceFactory)
set_property(TARGET Qt5::Qml APPEND PROPERTY QT_ALL_PLUGINS_qmltooling Qt5::QQmlDebuggerServiceFactory)
set_property(TARGET Qt5::QQmlDebuggerServiceFactory PROPERTY QT_PLUGIN_TYPE "qmltooling")
set_property(TARGET Qt5::QQmlDebuggerServiceFactory PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QQmlDebuggerServiceFactory PROPERTY QT_PLUGIN_CLASS_NAME "QQmlDebuggerServiceFactory")
