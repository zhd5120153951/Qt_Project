
GLDRS = $$quote($$(GLDRS))
Glodon= $$(Glodon)

isEmpty(Glodon){
    Glodon = $$GLDRS/Glodon
}

DEFINES += GDP_QT

INCLUDEPATH += $$Glodon/include/GUC

include($$Glodon/shared/GLD.pri)

CONFIG(debug, debug|release) {

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$Glodon/lib/Debug/X64
    } else {
        LIBS += -L$$Glodon/lib/Debug/X86
    }

    win32:LIBS += -lGUCd
    unix:LIBS += -lGUC_debug
}

CONFIG(release, debug|release) {

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$Glodon/lib/Release/X64
    } else {
        LIBS += -L$$Glodon/lib/Release/X86
    }

    LIBS += -lGUC
}

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003|win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012 {
    LIBS += -loleaut32 -lole32 -lshell32
greaterThan(QT_MAJOR_VERSION, 4){
    greaterThan(QT_MINOR_VERSION, 1){
#    QT += winextras
    }
}
}

win32-g++ {
    LIBS += -loleaut32 -lole32 -luuid -lnetapi32
}
