#-------------------------------------------------
#
# Project created by QtCreator 2012-09-21T09:14:54
#
#-------------------------------------------------

TARGET = GLDCrypt
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
    GLDCRYPT_LIBRARY

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003|win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012|win32-msvc2013 {

     include($(GLDRS)/Glodon/shared/GLDCommon.pri)
     include($(GLDRS)/Glodon/shared/Cryptopp.pri)
}

INCLUDEPATH +=  \
    ../../include/GLD

HEADERS += \
    ../../include/GLD/GLDMD5.h \
    ../../include/GLD/GLDSHA1.h \
    ../../include/GLD/GLDDes.h  \
    ../../include/GLD/GLDCrypt.h \
    ../../include/GLD/GLDCrypt_Global.h

SOURCES += \
    Base/GLDMD5.cpp \
    Base/GLDSHA1.cpp \
    Base/GLDDes.cpp \
    Base/GLDCrypt.cpp

RESOURCES += \
    Res/ico/GLDIcons.qrc \
    Res/ini/GLDInis.qrc \
    Res/qss/GLDQsses.qrc

TRANSLATIONS += GLD_zh_CN.ts

RC_FILE = Res/rc/GLDCrypt.rc
