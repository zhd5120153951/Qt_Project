CONFIG      += designer plugin debug_and_release
TARGET      = $$qtLibraryTarget(GLDWidgets)
TEMPLATE    = lib

greaterThan(QT_MAJOR_VERSION, 4): QT += core-private widgets-private

CONFIG += debug_and_release

CONFIG(debug, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        DESTDIR = ../../../../bin/Debug/X64
    } else {
        DESTDIR = ../../../../bin/Debug/X86
    }
    unix:TARGET=$$join(TARGET,,,_debug)
    win32:TARGET=$$join(TARGET,,,d)
}

CONFIG(release, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        DESTDIR = ../../../../bin/Release/X64
    } else {
        DESTDIR = ../../../../bin/Release/X86
    }
}

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003|win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012|win32-msvc2013 {
    LIBS += -lUser32
}

include(../../../../shared/GLD.pri)

INCLUDEPATH += include

RESOURCES   = ../../Res/ico/GLDIcons.qrc \

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target

SOURCES += \
    src/GLDWidgets.cpp \
    src/GLDTableViewPlugin.cpp \
    src/GLDFilterTableViewPlugin.cpp

HEADERS += \
    include/GLDWidgets.h \
    include/GLDTableViewPlugin.h \
    include/GLDFilterTableViewPlugin.h \
    ../../../../include/GLD/GLDTableView.h \
    ../../../../include/GLD/GLDFilterTableView.h
