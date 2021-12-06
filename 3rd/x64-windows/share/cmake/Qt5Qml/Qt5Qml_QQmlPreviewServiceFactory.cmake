
add_library(Qt5::QQmlPreviewServiceFactory MODULE IMPORTED)


if(EXISTS "${_qt5Qml_install_prefix}/plugins/qmltooling/qmldbg_preview.dll")
    _populate_Qml_plugin_properties(QQmlPreviewServiceFactory RELEASE "qmltooling/qmldbg_preview.dll" FALSE)
endif()
if(EXISTS "${_qt5Qml_install_prefix}/debug/plugins/qmltooling/qmldbg_previewd.dll")
    _populate_Qml_plugin_properties(QQmlPreviewServiceFactory DEBUG "qmltooling/qmldbg_previewd.dll" FALSE)
endif()

list(APPEND Qt5Qml_PLUGINS Qt5::QQmlPreviewServiceFactory)
set_property(TARGET Qt5::Qml APPEND PROPERTY QT_ALL_PLUGINS_qmltooling Qt5::QQmlPreviewServiceFactory)
set_property(TARGET Qt5::QQmlPreviewServiceFactory PROPERTY QT_PLUGIN_TYPE "qmltooling")
set_property(TARGET Qt5::QQmlPreviewServiceFactory PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QQmlPreviewServiceFactory PROPERTY QT_PLUGIN_CLASS_NAME "QQmlPreviewServiceFactory")
