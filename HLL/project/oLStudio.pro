#-------------------------------------------------
#
# Project created by QtCreator 2021-02-18T09:57:26
#
# @author M.Gurhan SANCAR
# @project OnLabStudio
#-------------------------------------------------


QT += core gui sql xml concurrent serialbus serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

QMAKE_CXXFLAGS += -Wno-ignored-qualifiers
QMAKE_CXXFLAGS += -std=c++11
#QMAKE_CXXFLAGS += -std=gnu++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

UNDERSCORE = "_"
DOT = "."
BLANK = " "
SERIES = "OnLab Studio"
V_MAJOR = "V0"
V_MINOR = "0"
V_BUILD = "0"
V_STABLE = "b.10"

BUILD_VERSION = $$V_MAJOR$$DOT$$V_MINOR$$DOT$$V_BUILD$$V_STABLE
#APP_NAME = $$SERIES$$BLANK$$DEVICE$$MODEL$$BLANK$$BUILD_VERSION
APP_NAME = $$SERIES$$BLANK$$BUILD_VERSION
NAME     = $$APP_NAME

TARGET = $$NAME

INCLUDEPATH += \
    forms \
    shared \
#    uilib \
    abstractions \
    components

include(src/shared/shared.pri)
#include(src/uilib/uilib.pri)
include(src/abstractions/abstractions.pri)
include(src/components/components.pri)



SOURCES += \
    src/forms/dlgactualeventlog.cpp \
    src/forms/dlgalarmconfig.cpp \
    src/forms/dlgmenu.cpp \
    src/forms/dlgmodbusfunction.cpp \
    src/forms/dlgpageutil.cpp \
    src/forms/dlgpairmenupage.cpp \
    src/forms/dlgpairregisterpage.cpp \
    src/forms/dlgpairtyperegister.cpp \
    src/forms/dlgparameter.cpp \
    src/forms/dlgregister.cpp \
    src/forms/settingsmodbusrtu.cpp \
    src/forms/settings.cpp \
    src/forms/about.cpp \
#    src/etreewidget/abstractformeditor.cpp \
#    src/etreewidget/abstractwidgetbox.cpp \
#    src/etreewidget/abstractwidgetdatabase.cpp \
#    src/etreewidget/edesigner_utils.cpp \
#    src/etreewidget/edesignerwidgetbox.cpp \
#    src/etreewidget/filterwidget.cpp \
#    src/etreewidget/iconloader.cpp \
#    src/etreewidget/sheet_delegate.cpp \
#    src/etreewidget/widgetbox.cpp \
#    src/etreewidget/widgetboxcategorylistview.cpp \
#    src/etreewidget/widgetboxtreewidget.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/modbuscommsettings.cpp \
    src/objects/deviceevent.cpp \
    src/objects/ebusdata.cpp \
    src/objects/ebusdataentries.cpp \
    src/objects/editablesqlmodel.cpp \
    src/objects/eventdata.cpp \
    src/objects/ticker.cpp \
    src/objects/unit.cpp \
    src/pages/WdgTest.cpp

HEADERS += \ version.h \
    src/forms/dlgactualeventlog.h \
    src/forms/dlgalarmconfig.h \
    src/forms/dlgmenu.h \
    src/forms/dlgmodbusfunction.h \
    src/forms/dlgpageutil.h \
    src/forms/dlgpairmenupage.h \
    src/forms/dlgpairregisterpage.h \
    src/forms/dlgpairtyperegister.h \
    src/forms/dlgparameter.h \
    src/forms/dlgregister.h \
    src/forms/settingsmodbusrtu.h \
    src/forms/settings.h \
    src/forms/about.h \
    src/mainwindow.h \
#    src/etreewidget/abstractdnditem.h \
#    src/etreewidget/abstractformeditor.h \
#    src/etreewidget/abstractwidgetbox.h \
#    src/etreewidget/abstractwidgetdatabase.h \
#    src/etreewidget/edesigner_utils_p.h \
#    src/etreewidget/edesigner_widgetbox_p.h \
#    src/etreewidget/filterwidget_p.h \
#    src/etreewidget/iconloader_p.h \
#    src/etreewidget/sheet_delegate_p.h \
#    src/etreewidget/widgetbox.h \
#    src/etreewidget/widgetboxcategorylistview.h \
#    src/etreewidget/widgetboxtreewidget.h \
    src/modbuscommsettings.h \
    src/initdb.h \
    src/objects/deviceevent.h \
    src/objects/ebusdata.h \
    src/objects/ebusdataentries.h \
    src/objects/edata.h \
    src/objects/editablesqlmodel.h \
    src/objects/eventdata.h \
    src/objects/ticker.h \
    src/objects/unit.h \
    src/pages/WdgTest.h

FORMS += \
    src/forms/dlgactualeventlog.ui \
    src/forms/dlgalarmconfig.ui \
    src/forms/dlgmenu.ui \
    src/forms/about.ui \
    src/forms/dlgmodbusfunction.ui \
    src/forms/dlgpageutil.ui \
    src/forms/dlgpairmenupage.ui \
    src/forms/dlgpairregisterpage.ui \
    src/forms/dlgpairtyperegister.ui \
    src/forms/dlgparameter.ui \
    src/forms/dlgregister.ui \
    src/forms/settingsmodbusrtu.ui \
    src/forms/settings.ui \
    src/mainwindow.ui \
    src/pages/WdgTest.ui

RESOURCES += \
#    src/etreewidget/formeditor.qrc \
    src/icons/icons.qrc

win32:LIBS += -lsetupapi -lwsock32 -lws2_32

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src


TRANSLATIONS += \
    src/tr_TR.ts

win32 {
   RC_FILE = app.rc
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
