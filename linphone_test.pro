#-------------------------------------------------
#
# Project created by QtCreator 2013-10-16T09:16:05
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = linphone_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    linphonecall.cpp \
    callp.cpp

INCLUDEPATH += /usr/include/linphone/

LIBS += -L:/usr/local/lib/ \
        -llinphone -lortp -lasound -lmediastreamer_voip
#-L:/usr/lib/ \
#-llinphone -lmediastreamer -lortp

HEADERS += \
    linphonecall.h \
    callp.h

