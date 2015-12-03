#对于需要公共资源所有模块的大项目，可以直接使用引用GLD.pri
#如果是用到GLD部分功能，推荐直接引用单独的子模块即可，不必要include所有模块。

GLDRS = $$quote($$(GLDRS))
Glodon= $$(Glodon)

isEmpty(Glodon){
    Glodon = $$GLDRS/Glodon
}

DEFINES += GDP_QT

INCLUDEPATH += $$Glodon/include/GLD

include($(GLDRS)/Glodon/shared/GLDCommon.pri)
include($(GLDRS)/Glodon/shared/GLDZip.pri)
include($(GLDRS)/Glodon/shared/GLDCrypt.pri)
include($(GLDRS)/Glodon/shared/GLDTableView.pri)
include($(GLDRS)/Glodon/shared/GLDWidget.pri)
include($(GLDRS)/Glodon/shared/GLDXML.pri)

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003 \
    |win32-msvc2005|win32-msvc2008|win32-msvc2010  \
    |win32-msvc2012|win32-msvc2013 {

    include($(GLDRS)/Glodon/shared/ZLIB.pri)
    include($$Glodon/shared/RichText.pri)
    include($$Glodon/shared/QuaZip.pri)
    include($(GLDRS)/ThirdPart/lzma/C/lzma.pri)
    include($(GLDRS)/Glodon/shared/Cryptopp.pri)
    include($(GLDRS)/Glodon/shared/GLDXLS.pri)
}
else {
    include($$Glodon/shared/RichText.pri)
    include($(GLDRS)/ThirdPart/zlib/zlib.pri)
}

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003 \
    |win32-msvc2005|win32-msvc2008|win32-msvc2010       \
    |win32-msvc2012|win32-msvc2013 {

    LIBS += -loleaut32 -lole32 -lshell32 -lUser32

    greaterThan(QT_MAJOR_VERSION, 4){
        greaterThan(QT_MINOR_VERSION, 1){
#           QT += winextras
        }
    }
}

win32-g++ {
    LIBS += -loleaut32 -lole32 -luuid -lnetapi32
}
