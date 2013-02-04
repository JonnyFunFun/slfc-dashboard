#-------------------------------------------------
#
# Project created by QtCreator 2013-01-15T08:26:36
#
#-------------------------------------------------

QT       += core gui webkit network xml multimedia

TARGET = slfcdashboard
TEMPLATE = app

DEFINES += NETWORKACCESS

SOURCES += main.cpp\
        mainwindow.cpp \
    scrolltext.cpp \
    googlecalendar.cpp \
    calendarevent.cpp \
    rfc3339.cpp \
    qhttpserver.cpp \
    qhttpresponse.cpp \
    qhttprequest.cpp \
    qhttpconnection.cpp \
    tickerhttpserver.cpp \
    http-parser/http_parser.c \
    httpswebview.cpp

HEADERS  += mainwindow.h++ \
    scrolltext.h++ \
    googlecalendar.h++ \
    calendarevent.h++ \
    rfc3339.h++ \
    qhttpserver.h \
    qhttpresponse.h \
    qhttprequest.h \
    qhttpconnection.h \
    tickerhttpserver.h++ \
    http-parser/http_parser.h \
    httpswebview.h++

FORMS    += mainwindow.ui

CONFIG += console

OTHER_FILES += \
    config.xml
