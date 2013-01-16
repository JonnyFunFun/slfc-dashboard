#-------------------------------------------------
#
# Project created by QtCreator 2013-01-15T08:26:36
#
#-------------------------------------------------

QT       += core gui webkit network xml

TARGET = slfcdashboard
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    scrolltext.cpp \
    googlecalendar.cpp \
    calendarevent.cpp \
    rfc3339.cpp

HEADERS  += mainwindow.h++ \
    scrolltext.h++ \
    googlecalendar.h++ \
    calendarevent.h++ \
    rfc3339.h++

FORMS    += mainwindow.ui

CONFIG += console

OTHER_FILES += \
    config.xml
