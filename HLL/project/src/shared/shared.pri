
INCLUDEPATH += $$PWD
contains(QT_CONFIG, script): QT += script

# When linking dynamically, use only the QKeySequenceEdit widget of
# the property browser solution as the other symbols of the property
# browser solution must not be duplicated (see e7762b60d51). When linking
# statically, add the property browser solution to make the library self-contained
# and use only the include path in the property editor (see propertyeditor.pri)

#contains(CONFIG, static) {
#    include(../../../../shared/qtpropertybrowser/qtpropertybrowser.pri)
#} else {
#   include(../../../../shared/qtpropertybrowser/qtpropertybrowserutils.pri)
#}
#include(../../../../shared/deviceskin/deviceskin.pri)
#include(../../../../../src/tools/rcc/rcc.pri)
#include(../../../../shared/findwidget/findwidget.pri)
#include(../../../../shared/qtgradienteditor/qtgradienteditor.pri)

# Input
#FORMS += \
#    $$PWD/addlinkdialog.ui \
#    $$PWD/orderdialog.ui \
#    $$PWD/newactiondialog.ui \
#    $$PWD/gridpanel.ui \
#    $$PWD/signalslotdialog.ui \
#    $$PWD/previewconfigurationwidget.ui \
#    $$PWD/qtresourceeditordialog.ui \
#    $$PWD/newformwidget.ui \
#    $$PWD/selectsignaldialog.ui \
#    $$PWD/formlayoutrowdialog.ui \
#    $$PWD/plugindialog.ui

HEADERS += \
#    $$PWD/shared_global_p.h \
#    $$PWD/spacer_widget_p.h \
#    $$PWD/layoutinfo_p.h \
#    $$PWD/layout_p.h \
#    $$PWD/connectionedit_p.h \
#    $$PWD/pluginmanager_p.h \
#    $$PWD/metadatabase_p.h \
#    $$PWD/edesigner_formeditorcommand_p.h \
#    $$PWD/edesigner_formwindowcommand_p.h \
#    $$PWD/edesigner_command_p.h \
#    $$PWD/morphmenu_p.h \
#    $$PWD/edesigner_command2_p.h \
#    $$PWD/edesigner_formbuilder_p.h \
#    $$PWD/edesigner_taskmenu_p.h \
#    $$PWD/formlayoutmenu_p.h \
#    $$PWD/edesigner_widget_p.h \
#    $$PWD/edesigner_propertysheet_p.h \
#    $$PWD/edesigner_membersheet_p.h \
#    $$PWD/edesigner_propertyeditor_p.h \
#    $$PWD/edesigner_objectinspector_p.h \
#    $$PWD/edesigner_integration_p.h \
#    $$PWD/invisible_widget_p.h \
#    $$PWD/elayout_widget_p.h \
    $$PWD/sheet_delegate_p.h \
#    $$PWD/edesigner_stackedbox_p.h \
#    $$PWD/edesigner_tabwidget_p.h \
#    $$PWD/edesigner_dockwidget_p.h \
#    $$PWD/edesigner_toolbox_p.h \
#    $$PWD/edesigner_dnditem_p.h \
#    $$PWD/esimpleresource_p.h \
    $$PWD/widgetfactory_p.h \
#    $$PWD/widgetdatabase_p.h \
#    $$PWD/edesigner_promotion_p.h \
#    $$PWD/edesigner_introspection_p.h \
#    $$PWD/promotionmodel_p.h \
#    $$PWD/edesigner_promotiondialog_p.h \
    $$PWD/iconloader_p.h \
#    $$PWD/richtexteditor_p.h \
#    $$PWD/plaintexteditor_p.h \
#    $$PWD/actioneditor_p.h \
#    $$PWD/actionrepository_p.h \
#    $$PWD/edesigner_toolbar_p.h \
#    $$PWD/edesigner_menubar_p.h \
#    $$PWD/edesigner_menu_p.h \
#    $$PWD/actionprovider_p.h \
#    $$PWD/orderdialog_p.h \
#    $$PWD/newactiondialog_p.h \
#    $$PWD/stylesheeteditor_p.h \
#    $$PWD/csshighlighter_p.h \
#    $$PWD/shared_enums_p.h \
#    $$PWD/textpropertyeditor_p.h \
#    $$PWD/propertylineedit_p.h \
#    $$PWD/promotiontaskmenu_p.h \
#    $$PWD/scripterrordialog_p.h \
#    $$PWD/scriptcommand_p.h \
#    $$PWD/scriptdialog_p.h \
#    $$PWD/escripthighlighter_p.h \
#    $$PWD/gridpanel_p.h \
#    $$PWD/grid_p.h \
#    $$PWD/formwindowbase_p.h \
#    $$PWD/edesigner_utils_p.h \
    $$PWD/edesigner_menubox_p.h \
#    $$PWD/signalslotdialog_p.h \
#    $$PWD/extensionfactory_p.h \
#    $$PWD/dialoggui_p.h \
#    $$PWD/deviceprofile_p.h \
#    $$PWD/zoomwidget_p.h \
#    $$PWD/previewmanager_p.h \
#    $$PWD/previewconfigurationwidget_p.h \
#    $$PWD/codedialog_p.h \
#    $$PWD/qtresourceeditordialog_p.h \
#    $$PWD/qtresourcemodel_p.h \
#    $$PWD/qtresourceview_p.h \
#    $$PWD/iconselector_p.h \
#    $$PWD/htmlhighlighter_p.h \
#    $$PWD/edesigner_widgetitem_p.h \
#    $$PWD/edesigner_qsettings_p.h \
#    $$PWD/edesigner_formwindowmanager_p.h \
#    $$PWD/shared_settings_p.h \
#    $$PWD/newformwidget_p.h \
    $$PWD/filterwidget_p.h
#    $$PWD/plugindialog_p.h

SOURCES += \
#    $$PWD/spacer_widget.cpp \
#    $$PWD/layoutinfo.cpp \
#    $$PWD/layout.cpp \
#    $$PWD/connectionedit.cpp \
#    $$PWD/pluginmanager.cpp \
#    $$PWD/edesigner_formwindowcommand.cpp \
#    $$PWD/edesigner_formeditorcommand.cpp \
#    $$PWD/edesigner_command.cpp \
#    $$PWD/morphmenu.cpp \
#    $$PWD/edesigner_command2.cpp \
#    $$PWD/edesigner_propertycommand.cpp \
#    $$PWD/edesigner_formbuilder.cpp \
#    $$PWD/edesigner_taskmenu.cpp \
#    $$PWD/formlayoutmenu.cpp \
#    $$PWD/edesigner_widget.cpp \
#    $$PWD/edesigner_dockwidget.cpp \
#    $$PWD/edesigner_propertysheet.cpp \
#    $$PWD/edesigner_membersheet.cpp \
#    $$PWD/edesigner_propertyeditor.cpp \
#    $$PWD/edesigner_objectinspector.cpp \
#    $$PWD/edesigner_integration.cpp \
#    $$PWD/edesigner_dnditem.cpp \
#    $$PWD/esimpleresource.cpp \
#    $$PWD/invisible_widget.cpp \
#    $$PWD/elayout_widget.cpp \
    $$PWD/sheet_delegate.cpp \
#    $$PWD/metadatabase.cpp \
#    $$PWD/edesigner_stackedbox.cpp \
#    $$PWD/edesigner_tabwidget.cpp \
#    $$PWD/edesigner_toolbox.cpp \
    $$PWD/widgetfactory.cpp \
#    $$PWD/widgetdatabase.cpp \
#    $$PWD/edesigner_promotion.cpp \
#    $$PWD/edesigner_introspection.cpp \
#    $$PWD/promotionmodel.cpp \
#    $$PWD/edesigner_promotiondialog.cpp \
#    $$PWD/richtexteditor.cpp \
#    $$PWD/plaintexteditor.cpp \
#    $$PWD/actioneditor.cpp \
#    $$PWD/actionrepository.cpp \
#    $$PWD/edesigner_toolbar.cpp \
#    $$PWD/edesigner_menubar.cpp \
#    $$PWD/edesigner_menu.cpp \
#    $$PWD/orderdialog.cpp \
#    $$PWD/newactiondialog.cpp \
#    $$PWD/stylesheeteditor.cpp \
#    $$PWD/csshighlighter.cpp \
#    $$PWD/textpropertyeditor.cpp \
#    $$PWD/propertylineedit.cpp \
#    $$PWD/promotiontaskmenu.cpp \
#    $$PWD/scripterrordialog.cpp \
#    $$PWD/scriptcommand.cpp \
#    $$PWD/scriptdialog.cpp \
#    $$PWD/escripthighlighter.cpp\
#    $$PWD/gridpanel.cpp \
#    $$PWD/grid.cpp \
#    $$PWD/formwindowbase.cpp \
#    $$PWD/edesigner_utils.cpp \
    $$PWD/edesigner_menubox.cpp \
    $$PWD/iconloader.cpp \
#    $$PWD/signalslotdialog.cpp \
#    $$PWD/dialoggui.cpp \
#    $$PWD/deviceprofile.cpp \
#    $$PWD/zoomwidget.cpp \
#    $$PWD/previewmanager.cpp \
#    $$PWD/previewconfigurationwidget.cpp \
#    $$PWD/codedialog.cpp \
#    $$PWD/qtresourceeditordialog.cpp \
#    $$PWD/qtresourcemodel.cpp \
#    $$PWD/qtresourceview.cpp \
#    $$PWD/iconselector.cpp \
#    $$PWD/htmlhighlighter.cpp \
#    $$PWD/edesigner_widgetitem.cpp \
#    $$PWD/edesigner_qsettings.cpp \
#    $$PWD/edesigner_formwindowmanager.cpp \
#    $$PWD/shared_settings.cpp \
#    $$PWD/newformwidget.cpp \
    $$PWD/filterwidget.cpp
#    $$PWD/plugindialog.cpp

#RESOURCES += $$PWD/shared.qrc
