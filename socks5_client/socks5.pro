#-------------------------------------------------
#
# Project created by QtCreator 2019-01-29T11:18:03
#
#-------------------------------------------------

QT       += core gui network
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = socks5
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    server.cpp \
    thread.cpp \
    analysis.cpp \
    udp.cpp \
    qaeswrap.cpp \
    aes.c

HEADERS += \
        mainwindow.h \
    server.h \
    thread.h \
    analysis.h \
    udp.h \
    aes.h \
    qaeswrap.h

FORMS += \
        mainwindow.ui
