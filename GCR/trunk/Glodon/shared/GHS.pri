
GLDRS = $$quote($$(GLDRS))
Glodon= $$(Glodon)

isEmpty(Glodon){
    Glodon = $$GLDRS/Glodon
}

INCLUDEPATH += \
    $$Glodon/include/GHS

CONFIG(debug, debug|release){
    LIBS += -L$$Glodon/lib/Debug/X86 \
        -lGLDd -lGHSd
    LIBS += -L$(GLDRS)/ThirdPart/curl/build/lib/Debug \
        -llibcurl_imp
}

CONFIG(release, debug|release){
    LIBS += -L$$Glodon/lib/Release/X86 \
        -lGLD -lGHS
    LIBS += -L$(GLDRS)/ThirdPart/curl/build/lib/Release \
        -llibcurl_imp
}

# vista and above only
LIBS += -lWs2_32
