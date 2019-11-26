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

OBJECTS_DIR = build/obj
MOC_DIR     = build/moc
UI_DIR      = build/ui
RCC_DIR     = build/rc

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    buildstepconditiondelegate.cpp \
    buildstepslist.cpp \
    dialogconfigurationrename.cpp \
    dialoglinkordereditor.cpp \
    dialoglisteditor.cpp \
    listeditor.cpp \
    parser/buildstep.cpp \
    parser/buildsteplist.cpp \
    parser/configsettings.cpp \
    parser/export/abstractprojectexport.cpp \
    parser/export/projectexportccs3.cpp \
    parser/fileoptions.cpp \
    parser/projectparser.cpp \
    parser/projectreader.cpp \
    parser/projectsettings.cpp \
    parser/utils.cpp

HEADERS += \
    mainwindow.h \
    buildstepconditiondelegate.h \
    buildstepslist.h \
    dialogconfigurationrename.h \
    dialoglinkordereditor.h \
    dialoglisteditor.h \
    listeditor.h \
    parser/buildstep.h \
    parser/buildsteplist.h \
    parser/configsettings.h \
    parser/export/abstractprojectexport.h \
    parser/export/projectexportccs3.h \
    parser/fileoptions.h \
    parser/projectparser.h \
    parser/projectreader.h \
    parser/projectsettings.h \
    parser/utils.h

FORMS += \
    buildstepslist.ui \
    dialogconfigurationrename.ui \
    dialoglinkordereditor.ui \
    dialoglisteditor.ui \
    listeditor.ui \
    mainwindow.ui

unix: target.path = /usr/local/bin
!isEmpty(target.path): INSTALLS += target

unix: desktop.path = /usr/share/applications
unix: desktop.files = ccs-pjt-editor-qt.desktop

!isEmpty(desktop.path): INSTALLS += desktop

unix: mimeinfo.path = /usr/share/mime/packages
unix: mimeinfo.files = text-x-ccs-pjt.xml

!isEmpty(mimeinfo.path): INSTALLS += mimeinfo
