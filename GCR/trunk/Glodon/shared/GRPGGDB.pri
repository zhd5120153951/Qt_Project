
GLDRS = $$quote($$(GLDRS))
Glodon= $$(Glodon)

isEmpty(Glodon){
    Glodon = $$GLDRS/Glodon
}

DEFINES += GDP_QT 
greaterThan(QT_MAJOR_VERSION, 4): DEFINES += NOMINMAX 

INCLUDEPATH += $$Glodon/include/GRPGGDB

include($$Glodon/shared/GRP7.pri)

LIBS += -L$$Glodon/lib/Debug/X86 \
        GGDBd.lib \
        MDCommond.lib \
        Commond.lib \
        Geometryd.lib \
        MDCmdLogd.lib \
        MDCached.lib \
        MDScriptd.lib
CONFIG(debug, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$Glodon/lib/Debug/X64
    } else {
        LIBS += -L$$Glodon/lib/Debug/X86
    }
    win32:LIBS += -lGRPGGDBd
    unix:LIBS += -lGRPGGDBd_debug
}


CONFIG(release, debug|release) {
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$Glodon/lib/Release/X64
    } else {
        LIBS += -L$$Glodon/lib/Release/X86
    }

    LIBS += -lGRPGGDB 
	   
}
