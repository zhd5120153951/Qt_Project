GLDRS = $$quote($$(GLDRS))

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003|win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012|win32-msvc2013 {
#    include($$GLDRS/Glodon/shared/QuaZip.pri)
}
else {
    include($$GLDRS/Glodon/shared/QuaZip.pri)
}

INCLUDEPATH += $$GLDRS/ThirdPart/RichText/include

CONFIG(debug, debug|release) {

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$GLDRS/ThirdPart/RichText/lib/Debug/X64
    } else {
        LIBS += -L$$GLDRS/ThirdPart/RichText/lib/Debug/X86
    }

    win32:LIBS += -lRichTextd
    unix:LIBS += -lRichText_debug
}

CONFIG(release, debug|release) {

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$GLDRS/ThirdPart/RichText/lib/Release/X64
    } else {
        LIBS += -L$$GLDRS/ThirdPart/RichText/lib/Release/X86
    }

    LIBS += -lRichText
}
