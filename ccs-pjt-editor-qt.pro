#-------------------------------------------------
#
# Project created by QtCreator 2019-06-30T16:33:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ccs-pjt-editor-qt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    parser/configsettings.cpp \
    parser/projectparser.cpp \
    parser/projectreader.cpp \
    parser/projectsettings.cpp \
    parser/utils.cpp \
    listeditor.cpp \
    dialoglisteditor.cpp \
    buildstepconditiondelegate.cpp \
    buildstepslist.cpp \
    dialogconfigurationrename.cpp \
    parser/export/abstractprojectexport.cpp \
    parser/export/projectexportccs3.cpp

HEADERS += \
        mainwindow.h \
    parser/configsettings.h \
    parser/projectparser.h \
    parser/projectreader.h \
    parser/projectsettings.h \
    parser/utils.h \
    listeditor.h \
    dialoglisteditor.h \
    buildstepconditiondelegate.h \
    buildstepslist.h \
    dialogconfigurationrename.h \
    parser/export/abstractprojectexport.h \
    parser/export/projectexportccs3.h

FORMS += \
        mainwindow.ui \
    listeditor.ui \
    dialoglisteditor.ui \
    buildstepslist.ui \
    dialogconfigurationrename.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
