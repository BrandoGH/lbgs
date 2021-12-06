
add_library(Qt5::QQuickWidgetPlugin MODULE IMPORTED)


if(EXISTS "${_qt5Designer_install_prefix}/plugins/designer/qquickwidget.dll")
    _populate_Designer_plugin_properties(QQuickWidgetPlugin RELEASE "designer/qquickwidget.dll" FALSE)
endif()
if(EXISTS "${_qt5Designer_install_prefix}/debug/plugins/designer/qquickwidgetd.dll")
    _populate_Designer_plugin_properties(QQuickWidgetPlugin DEBUG "designer/qquickwidgetd.dll" FALSE)
endif()

list(APPEND Qt5Designer_PLUGINS Qt5::QQuickWidgetPlugin)
set_property(TARGET Qt5::Designer APPEND PROPERTY QT_ALL_PLUGINS_designer Qt5::QQuickWidgetPlugin)
set_property(TARGET Qt5::QQuickWidgetPlugin PROPERTY QT_PLUGIN_TYPE "designer")
set_property(TARGET Qt5::QQuickWidgetPlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QQuickWidgetPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QQuickWidgetPlugin")
