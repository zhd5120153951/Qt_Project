
GLDRS = $$quote($$(GLDRS))

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003|win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012|win32-msvc2013 {

    DEFINES += ZLIB_WINAPI
}

INCLUDEPATH += $$GLDRS/ThirdPart/zlib

CONFIG(debug, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += $$GLDRS/ThirdPart/zlib/lib/Debug/X64/zlibd.lib
    } else {
        LIBS += $$GLDRS/ThirdPart/zlib/lib/Debug/X86/zlibd.lib
    }
}

CONFIG(release, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += $$GLDRS/ThirdPart/zlib/lib/Release/X64/zlib.lib
    } else {
        LIBS += $$GLDRS/ThirdPart/zlib/lib/Release/X86/zlib.lib
    }
}
