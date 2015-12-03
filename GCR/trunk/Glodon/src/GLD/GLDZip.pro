#-------------------------------------------------
#
# Project created by QtCreator 2012-09-21T09:14:54
#
#-------------------------------------------------
greaterThan(QT_MAJOR_VERSION, 4): QT += core-private

TARGET = GLDZip
TEMPLATE = lib

CONFIG +=  debug_and_release

unix:QMAKE_CXXFLAGS += -std=c++11

CONFIG(debug, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        DESTDIR = ../../bin/Debug/X64
    } else {
        DESTDIR = ../../bin/Debug/X86
    }
    unix:TARGET=$$join(TARGET,,,_debug)
    win32:TARGET=$$join(TARGET,,,d)
}

CONFIG(release, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        DESTDIR = ../../bin/Release/X64
    } else {
        DESTDIR = ../../bin/Release/X86
    }
}

QMAKE_CXXFLAGS_RELEASE += /Zi
QMAKE_LFLAGS_RELEASE += /DEBUG

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += NOMINMAX

DEFINES += GLODONCOMMON_LIBRARY \
    QUAZIP_STATIC \
    INITGUID \
    GDP_QT \
    GLDZIP_LIBRARY

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003|win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012|win32-msvc2013 {
   include($(GLDRS)/ThirdPart/zlib/zlib.pri)
   include($(GLDRS)/ThirdPart/lzma/C/lzma.pri)
   include($(GLDRS)/Glodon/shared/GLDCommon.pri)
}

INCLUDEPATH +=  \
    ../../include/GLD \
    $(GLDRS)/ThirdPart/zlib

HEADERS += \
    ../../include/GLD/GLDZipFile.h \
    ../../include/GLD/GLDZipEnc.h \
    ../../include/GLD/GLDZlib.h \
    ../../include/GLD/GLDZip_Global.h
 
SOURCES += \
    Base/GLDZipFile.cpp \
    Base/GLDZipEnc.cpp \
    Base/GLDZlib.cpp
   
RESOURCES += \
    Res/ico/GLDIcons.qrc \
    Res/ini/GLDInis.qrc \
    Res/qss/GLDQsses.qrc

TRANSLATIONS += GLD_zh_CN.ts

RC_FILE = Res/rc/GLDZip.rc
