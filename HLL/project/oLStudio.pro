#-------------------------------------------------
#
# Project created by QtCreator 2021-06-16T00:01:26
#
# @author M.Gurhan SANCAR
# @project OnLab - oLStudio
#-------------------------------------------------

QT       += core gui sql serialbus serialport

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
    forms


SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/forms/settingsrtu.cpp \
    src/objects/commsettings.cpp \
    src/objects/masterthread.cpp

HEADERS += \
    src/mainwindow.h \
    src/forms/settingsrtu.h \
    src/objects/commsettings.h \
    src/objects/masterthread.h

FORMS += \
    src/mainwindow.ui \
    src/forms/settingsrtu.ui

RESOURCES += \
#    src/etreewidget/formeditor.qrc \
    src/icons/icons.qrc

win32:LIBS += -lsetupapi -lwsock32 -lws2_32

INCLUDEPATH += $$PWD/src $$PWD/src/objects $$PWD/src/forms
DEPENDPATH += $$PWD/src $$PWD/src/objects $$PWD/src/forms

win32 {
   RC_FILE      = oLStudio.rc
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
