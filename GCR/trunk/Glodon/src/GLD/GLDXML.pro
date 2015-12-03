#-------------------------------------------------
#
# Project created by QtCreator 2012-09-21T09:14:54
#
#-------------------------------------------------
QT += xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets core-private


TARGET = GLDXML
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

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += NOMINMAX

QMAKE_CXXFLAGS_RELEASE += /Zi
QMAKE_LFLAGS_RELEASE += /DEBUG

DEFINES += GLODONCOMMON_LIBRARY \
    QUAZIP_STATIC \
    INITGUID \
    GDP_QT \
    GLDXML_LIBRARY

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003|win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012|win32-msvc2013 {
    LIBS += -loleaut32 -lole32
    include($(GLDRS)/Glodon/shared/GLDCommon.pri)
}

INCLUDEPATH += $(GLDRS)/ThirdPart/RichText/include \
    $(GLDRS)/ThirdPart/libxl/include_cpp \
    ../../include/GLD \

HEADERS += \
    ../../include/GLD/GLDXMLUtils.h \
    ../../include/GLD/GLDXMLWriter.h \
    ../../include/GLD/GLDXMLSAXUtils.h \
    ../../include/GLD/GLDXMLBuilder.h \
    ../../include/GLD/GLDXMLCore.h \
    ../../include/GLD/GLDXMLDoc.h \
    ../../include/GLD/GLDXMLInterface.h \
    ../../include/GLD/GLDXMLParser.h \
    ../../include/GLD/GLDXML_Global.h

SOURCES += \
    Base/GLDXMLWriter.cpp \
    Base/GLDXMLUtils.cpp \
    Base/GLDXMLSAXUtils.cpp \
    Base/GLDXMLBuilder.cpp \
    Base/GLDXMLCore.cpp \
    Base/GLDXMLDoc.cpp \
    Base/GLDXMLParser.cpp

RESOURCES += \
    Res/ico/GLDIcons.qrc \
    Res/ini/GLDInis.qrc \
    Res/qss/GLDQsses.qrc

TRANSLATIONS += GLD_zh_CN.ts

RC_FILE = Res/rc/GLDXML.rc
