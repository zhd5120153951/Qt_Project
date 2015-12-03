GLDRS = $$quote($$(GLDRS))
Glodon= $$(Glodon)

isEmpty(Glodon){
    Glodon = $$GLDRS/Glodon
}

DEFINES += GDP_QT
greaterThan(QT_MAJOR_VERSION, 4): DEFINES += NOMINMAX

INCLUDEPATH += $$Glodon/include/GRP7

RESOURCES += $$Glodon/src/GRP/GRP7/resDesigner/resDesigner.qrc


include($$Glodon/shared/GSP.pri)

CONFIG(debug, debug|release) {
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$Glodon/bin/Debug/X64
    } else {
        LIBS += -L$$Glodon/bin/Debug/X86
    }
    win32:LIBS += -lGRPCommond
    unix:LIBS += -lGRPCommon_debug
}

CONFIG(release, debug|release) {
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$Glodon/bin/Release/X64
    } else {
        LIBS += -L$$Glodon/bin/Release/X86
    }

    LIBS += -lGRPCommon
}

