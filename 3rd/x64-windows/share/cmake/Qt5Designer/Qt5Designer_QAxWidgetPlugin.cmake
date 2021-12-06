
add_library(Qt5::QAxWidgetPlugin MODULE IMPORTED)


if(EXISTS "${_qt5Designer_install_prefix}/plugins/designer/qaxwidget.dll")
    _populate_Designer_plugin_properties(QAxWidgetPlugin RELEASE "designer/qaxwidget.dll" FALSE)
endif()
if(EXISTS "${_qt5Designer_install_prefix}/debug/plugins/designer/qaxwidgetd.dll")
    _populate_Designer_plugin_properties(QAxWidgetPlugin DEBUG "designer/qaxwidgetd.dll" FALSE)
endif()

list(APPEND Qt5Designer_PLUGINS Qt5::QAxWidgetPlugin)
set_property(TARGET Qt5::Designer APPEND PROPERTY QT_ALL_PLUGINS_designer Qt5::QAxWidgetPlugin)
set_property(TARGET Qt5::QAxWidgetPlugin PROPERTY QT_PLUGIN_TYPE "designer")
set_property(TARGET Qt5::QAxWidgetPlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QAxWidgetPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QAxWidgetPlugin")
