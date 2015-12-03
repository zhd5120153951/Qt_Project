
GLDRS = $$quote($$(GLDRS))

INCLUDEPATH += $$GLDRS/ThirdPart/cryptopp/include \
    $$GLDRS/ThirdPart/cryptopp/Addin

CONFIG(debug, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += $$GLDRS/ThirdPart/cryptopp/lib/Debug/X64/cryptoppd.lib
    } else {
        LIBS += $$GLDRS/ThirdPart/cryptopp/lib/Debug/X86/cryptoppd.lib
    }
}

CONFIG(release, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += $$GLDRS/ThirdPart/cryptopp/lib/Release/X64/cryptopp.lib
    } else {
        LIBS += $$GLDRS/ThirdPart/cryptopp/lib/Release/X86/cryptopp.lib
    }
}
