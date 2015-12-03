GLDRS = $$quote($$(GLDRS))
Glodon= $$(Glodon)

isEmpty(Glodon){
    Glodon = $$GLDRS/Glodon
}

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003|win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012|win32-msvc2013 {
}

DEFINES += GDP_QT

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += NOMINMAX

INCLUDEPATH += $$Glodon/include/GSP

CONFIG(debug, debug|release) {

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$Glodon/bin/Debug/X64
    } else {
        LIBS += -L$$Glodon/bin/Debug/X86
    }
    win32:LIBS += -lGSPd
    unix:LIBS += -lGSP_debug
}

CONFIG(release, debug|release) {

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$Glodon/bin/Release/X64
    } else {
        LIBS += -L$$Glodon/bin/Release/X86
    }
    LIBS += -lGSP
}
