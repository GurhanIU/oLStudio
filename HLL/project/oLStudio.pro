#-------------------------------------------------
#
# Project created by QtCreator 2021-02-18T19:57:26
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

UNDERSCORE = "_"
DOT = "."
BLANK = " "
SERIES = "OnLab Studio"
V_MAJOR = "V0"
V_MINOR = "0"
V_BUILD = "0"
V_STABLE = "b.11"

BUILD_VERSION = $$V_MAJOR$$DOT$$V_MINOR$$DOT$$V_BUILD$$V_STABLE
#APP_NAME = $$SERIES$$BLANK$$DEVICE$$MODEL$$BLANK$$BUILD_VERSION
APP_NAME = $$SERIES$$BLANK$$BUILD_VERSION
NAME     = $$APP_NAME

TARGET = $$NAME

INCLUDEPATH += \
    forms

SOURCES += \
    src/forms/entrytoolbox.cpp \
    src/forms/settingsmodbusrtu.cpp \
    src/forms/settings.cpp \
    src/forms/about.cpp \
    src/forms/toptoolbox.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/modbuscommsettings.cpp \
    src/objects/ebusdata.cpp \
    src/objects/ebusdataentries.cpp \
    src/objects/editablesqlmodel.cpp \
    src/objects/ticker.cpp \
    src/pages/WdgTest.cpp

HEADERS += \ version.h \
    src/forms/entrytoolbox.h \
    src/forms/settingsmodbusrtu.h \
    src/forms/settings.h \
    src/forms/about.h \
    src/forms/toptoolbox.h \
    src/mainwindow.h \
    src/modbuscommsettings.h \
    src/initdb.h \
    src/objects/ebusdata.h \
    src/objects/ebusdataentries.h \
    src/objects/editablesqlmodel.h \
    src/objects/ticker.h \
    src/pages/WdgTest.h

FORMS += \
    src/forms/about.ui \
    src/forms/entrytoolbox.ui \
    src/forms/settingsmodbusrtu.ui \
    src/forms/settings.ui \
    src/forms/toptoolbox.ui \
    src/mainwindow.ui \
    src/pages/WdgTest.ui

RESOURCES += \
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
