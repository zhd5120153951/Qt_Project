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
    GLDDiskInfoUtils.cpp \
    GLDMemoryInfoUtils.cpp \
    GLDCpuInfoUtils.h.cpp

HEADERS += \
    GLDProcessUtils.h \
    GLDDiskInfoUtils.h \
    GLDMemoryInfoUtils.h \
    GLDCpuInfoUtils.h
