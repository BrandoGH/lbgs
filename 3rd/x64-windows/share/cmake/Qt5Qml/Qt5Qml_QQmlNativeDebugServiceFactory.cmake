
add_library(Qt5::QQmlNativeDebugServiceFactory MODULE IMPORTED)


if(EXISTS "${_qt5Qml_install_prefix}/plugins/qmltooling/qmldbg_nativedebugger.dll")
    _populate_Qml_plugin_properties(QQmlNativeDebugServiceFactory RELEASE "qmltooling/qmldbg_nativedebugger.dll" FALSE)
endif()
if(EXISTS "${_qt5Qml_install_prefix}/debug/plugins/qmltooling/qmldbg_nativedebuggerd.dll")
    _populate_Qml_plugin_properties(QQmlNativeDebugServiceFactory DEBUG "qmltooling/qmldbg_nativedebuggerd.dll" FALSE)
endif()

list(APPEND Qt5Qml_PLUGINS Qt5::QQmlNativeDebugServiceFactory)
set_property(TARGET Qt5::Qml APPEND PROPERTY QT_ALL_PLUGINS_qmltooling Qt5::QQmlNativeDebugServiceFactory)
set_property(TARGET Qt5::QQmlNativeDebugServiceFactory PROPERTY QT_PLUGIN_TYPE "qmltooling")
set_property(TARGET Qt5::QQmlNativeDebugServiceFactory PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QQmlNativeDebugServiceFactory PROPERTY QT_PLUGIN_CLASS_NAME "QQmlNativeDebugServiceFactory")
