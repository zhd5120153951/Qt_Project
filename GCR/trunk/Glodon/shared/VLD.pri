
GLDRS = $$quote($$(GLDRS))

INCLUDEPATH += $$GLDRS/ThirdPart/VLD/include

contains(QMAKE_HOST.arch, x86_64) {
    LIBS += -L$$GLDRS/ThirdPart/VLD/lib/Win64
} else {
    LIBS += -L$$GLDRS/ThirdPart/VLD/lib/Win32
}

CONFIG(debug, debug|release) {
    win32:LIBS += -lvld
}
