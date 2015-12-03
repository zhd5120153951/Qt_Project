
GLDRS = $$quote($$(GLDRS))

INCLUDEPATH += $$GLDRS/ThirdPart/lua/include

CONFIG(debug, debug|release) {
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$GLDRS/ThirdPart/lua/bin/Debug/X64
    } else {
        LIBS += -L$$GLDRS/ThirdPart/lua/bin/Debug/X86
    }
    win32:LIBS += -lluad
    #unix:LIBS += -llua
}

CONFIG(release, debug|release) {
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$GLDRS/ThirdPart/lua/bin/Release/X64
    } else {
        LIBS += -L$$GLDRS/ThirdPart/lua/bin/Release/X86
    }
    win32:LIBS += -llua
    #unix:LIBS += -llua
}
