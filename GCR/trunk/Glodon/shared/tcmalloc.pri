
GLDRS = $$quote($$(GLDRS))

INCLUDEPATH += $$GLDRS/ThirdPart/gperftools/include

CONFIG(debug, debug|release) {
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$GLDRS/ThirdPart/gperftools/Debug/X64
    } else {
        LIBS += -L$$GLDRS/ThirdPart/gperftools/Debug/X86
    }
    win32:LIBS += -llibtcmalloc_minimal-debug
    #unix:LIBS += -libtcmalloc_minimal-debug
}

CONFIG(release, debug|release) {
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$GLDRS/ThirdPart/gperftools/Release/X64
    } else {
        LIBS += -L$$GLDRS/ThirdPart/gperftools/Release/X86
    }
    win32:LIBS += -llibtcmalloc_minimal
    #unix:LIBS += -llibtcmalloc_minimal
}


