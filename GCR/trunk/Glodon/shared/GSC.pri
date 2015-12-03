GLDRS = $$quote($$(GLDRS))
Glodon = $$(Glodon)

isEmpty(Glodon){
    Glodon = $$GLDRS/Glodon
}

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += NOMINMAX

INCLUDEPATH += $$Glodon/include/GSC

CONFIG(debug, debug|release) {

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$Glodon/lib/Debug/X64
    } else {
        LIBS += -L$$Glodon/lib/Debug/X86
    }

    win32:LIBS += -lGSCd -lGSCAppImpld -ladvapi32
    unix:LIBS += -lGSC_debug -lGSCAppImpld_debug
}

CONFIG(release, debug|release) {

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$Glodon/lib/Release/X64
    } else {
        LIBS += -L$$Glodon/lib/Release/X86
    }

    LIBS += -lGSC -lGSCAppImpl -ladvapi32
}
