GLDRS = $$quote($$(GLDRS))

DEFINES += QUAZIP_STATIC
INCLUDEPATH += $$GLDRS/ThirdPart/quazip

CONFIG(debug, debug|release) {

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$GLDRS/ThirdPart/quazip/lib/Debug/X64
    } else {
        LIBS += -L$$GLDRS/ThirdPart/quazip/lib/Debug/X86
    }

    win32:LIBS += -lquazipd
    unix:LIBS += -lquazip_debug
}

CONFIG(release, debug|release) {

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$GLDRS/ThirdPart/quazip/lib/Release/X64
    } else {
        LIBS += -L$$GLDRS/ThirdPart/quazip/lib/Release/X86
    }

    LIBS += -lquazip
}

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003|win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012|win32-msvc2013 {
    include($$GLDRS/Glodon/shared/ZLIB.pri)
    include($(GLDRS)/ThirdPart/lzma/C/lzma.pri)
}
else {
#    include($$GLDRS/ThirdPart/zlib/zlib.pri)
#    include($(GLDRS)/ThirdPart/lzma/C/lzma.pri)
}
