
GLDRS = $$quote($$(GLDRS))

INCLUDEPATH += $$GLDRS/GLDTechPlatform/include \
               $$GLDRS/GLDTechPlatform/include/Intf \
               $$GLDRS/GLDTechPlatform/include/Impl


CONFIG(debug, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$GLDRS/GLDTechPlatform/lib/Debug/X64
    } else {
        LIBS += -L$$GLDRS/GLDTechPlatform/lib/Debug/X86
    }
    LIBS += -lGLDTechPlatformWithGSPd
}

CONFIG(release, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$GLDRS/GLDTechPlatform/lib/Release/X64
    } else {
        LIBS += -L$$GLDRS/GLDTechPlatform/lib/Release/X86
    }
    LIBS += -lGLDTechPlatformWithGSP
}
