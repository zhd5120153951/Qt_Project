GLDRS = $$quote($$(GLDRS))

INCLUDEPATH += $$GLDRS/ThirdPart/libxl/include_cpp

DEFINES += _UNICODE

CONFIG(debug, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += $$GLDRS/ThirdPart/libxl/lib64/libxl.lib
    } else {
        LIBS += $$GLDRS/ThirdPart/libxl/lib/libxl.lib
    }
}

CONFIG(release, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += $$GLDRS/ThirdPart/libxl/lib64/libxl.lib
    } else {
        LIBS += $$GLDRS/ThirdPart/libxl/lib/libxl.lib
    }
}
