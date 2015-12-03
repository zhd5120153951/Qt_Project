
GLDRS = $$quote($$(GLDRS))
INCLUDEPATH += \
    $(GLDRS)/ThirdPart/lzma/C/lzma \
    $(GLDRS)/ThirdPart/lzma/C

CONFIG(debug, debug|release) {
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$GLDRS/ThirdPart/lzma/C/lzma/lib/Debug/X64
    } else {
        LIBS += -L$$GLDRS/ThirdPart/lzma/C/lzma/lib/Debug/X86
    }

    win32:LIBS += -llzmad
    unix:LIBS += -llzma_debug
}

CONFIG(release, debug|release) {
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$GLDRS/ThirdPart/lzma/C/lzma/lib/Release/X64
    } else {
        LIBS += -L$$GLDRS/ThirdPart/lzma/C/lzma/lib/Release/X86
    }

    LIBS += -llzma
}


