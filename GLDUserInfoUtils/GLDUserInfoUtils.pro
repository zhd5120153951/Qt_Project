#-------------------------------------------------
#
# Project created by QtCreator 2016-01-14T09:36:13
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = GLDUserInfoUtils
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    GLDProcessUtils.cpp \
    GLDSysInfoUtils.h.cpp \
    GLDDiskInfoUtils.cpp \
    GLDMemoryInfoUtils.cpp

HEADERS += \
    GLDProcessUtils.h \
    GLDSysInfoUtils.h \
    GLDDiskInfoUtils.h \
    GLDMemoryInfoUtils.h
