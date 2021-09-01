#-------------------------------------------------
#
# Project created by QtCreator 2021-07-02T21:12:26
#
# @author M.Gurhan SANCAR
# @project oLStudio
#-------------------------------------------------


QT += core gui sql xml concurrent serialbus serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

QMAKE_CXXFLAGS += -Wno-ignored-qualifiers
QMAKE_CXXFLAGS += -std=c++11
#QMAKE_CXXFLAGS += -std=gnu++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    objects \
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
    src/main.cpp \
    src/MainWindow.cpp \
    src/forms/dlgaddress.cpp \
    src/forms/dlgparameter.cpp \
    src/forms/dlgmenu.cpp \
    src/forms/dlgmodbusfunction.cpp \
    src/forms/dlgpageutil.cpp \
    src/forms/dlgpairmenupage.cpp \
    src/forms/dlgpairregisterpage.cpp \
    src/forms/settingsrtu.cpp \
    src/forms/settings.cpp \
    src/forms/about.cpp \
    src/objects/commsettings.cpp \
    src/objects/ebusdata.cpp \
    src/objects/ebusdataentries.cpp \
    src/objects/editablesqlmodel.cpp \
    src/objects/modbusdata.cpp \
    src/objects/modbusdataentries.cpp \
    src/objects/onbusmaster.cpp \
    src/objects/responsepacket.cpp \
    src/pages/WdgTest.cpp

HEADERS += \
    src/initdb.h \
    src/MainWindow.h \
    src/forms/dlgaddress.h \
    src/forms/dlgparameter.h \
    src/forms/dlgmenu.h \
    src/forms/dlgmodbusfunction.h \
    src/forms/dlgpageutil.h \
    src/forms/dlgpairmenupage.h \
    src/forms/dlgpairregisterpage.h \
    src/forms/settingsrtu.h \
    src/forms/settings.h \
    src/forms/about.h \
    src/objects/commsettings.h \
    src/objects/ebusdata.h \
    src/objects/ebusdataentries.h \
    src/objects/edata.h \
    src/objects/editablesqlmodel.h \
    src/objects/modbusdata.h \
    src/objects/modbusdataentries.h \
    src/objects/onbusmaster.h \
    src/objects/responsepacket.h \
    src/pages/WdgTest.h

FORMS += \
    src/MainWindow.ui \
    src/forms/dlgaddress.ui \
    src/forms/dlgparameter.ui \
    src/forms/settingsrtu.ui \
    src/forms/settings.ui \
    src/forms/dlgmenu.ui \
    src/forms/about.ui \
    src/forms/dlgmodbusfunction.ui \
    src/forms/dlgpageutil.ui \
    src/forms/dlgpairmenupage.ui \
    src/forms/dlgpairregisterpage.ui \
    src/pages/WdgTest.ui

RESOURCES += \
    src/icons/icons.qrc

win32:LIBS += -lsetupapi -lwsock32 -lws2_32

INCLUDEPATH += $$PWD/src $$PWD/src/objects $$PWD/src/forms
DEPENDPATH += $$PWD/src $$PWD/src/objects $$PWD/src/forms


TRANSLATIONS += \
    src/app_tr_TR.ts

win32 {
   RC_FILE      = app.rc
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
