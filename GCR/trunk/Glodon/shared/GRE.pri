
GLDRS = $$quote($$(GLDRS))
Glodon= $$(Glodon)

isEmpty(Glodon){
    Glodon = $$GLDRS/Glodon
}

DEFINES += GDP_QT

INCLUDEPATH += $$Glodon/include/GRE

CONFIG(debug, debug|release) {

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$Glodon/lib/Debug/X64
    } else {
        LIBS += -L$$Glodon/lib/Debug/X86
    }

    win32:LIBS += -lGREd
    unix:LIBS += -lGRE_debug
}

CONFIG(release, debug|release) {

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$Glodon/lib/Release/X64
    } else {
        LIBS += -L$$Glodon/lib/Release/X86
    }

    LIBS += -lGRE
}
