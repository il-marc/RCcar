#-------------------------------------------------
#
# Project created by QtCreator 2014-01-13T16:42:27
#
#-------------------------------------------------

QT       += serialport core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = remote
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    portdialog.cpp

HEADERS  += mainwindow.h \
    portdialog.h

FORMS    += mainwindow.ui \
    portdialog.ui

RESOURCES += \
    states.qrc
