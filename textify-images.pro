QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = textify-images
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++14

SOURCES += \
        main.cpp \
        widget.cpp

HEADERS += \
        widget.h

FORMS += \
        widget.ui
