
add_library(Qt5::QQmlInspectorServiceFactory MODULE IMPORTED)


if(EXISTS "${_qt5Qml_install_prefix}/plugins/qmltooling/qmldbg_inspector.dll")
    _populate_Qml_plugin_properties(QQmlInspectorServiceFactory RELEASE "qmltooling/qmldbg_inspector.dll" FALSE)
endif()
if(EXISTS "${_qt5Qml_install_prefix}/debug/plugins/qmltooling/qmldbg_inspectord.dll")
    _populate_Qml_plugin_properties(QQmlInspectorServiceFactory DEBUG "qmltooling/qmldbg_inspectord.dll" FALSE)
endif()

list(APPEND Qt5Qml_PLUGINS Qt5::QQmlInspectorServiceFactory)
set_property(TARGET Qt5::Qml APPEND PROPERTY QT_ALL_PLUGINS_qmltooling Qt5::QQmlInspectorServiceFactory)
set_property(TARGET Qt5::QQmlInspectorServiceFactory PROPERTY QT_PLUGIN_TYPE "qmltooling")
set_property(TARGET Qt5::QQmlInspectorServiceFactory PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QQmlInspectorServiceFactory PROPERTY QT_PLUGIN_CLASS_NAME "QQmlInspectorServiceFactory")
