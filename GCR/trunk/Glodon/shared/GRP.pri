Glodon= $$(Glodon)

isEmpty(Glodon){
    Glodon = $$GLDRS/Glodon
}

include($$Glodon/shared/GSP.pri)

INCLUDEPATH += $$Glodon/include/GRP

CONFIG(debug, debug|release) {
    LIBS += -lGRPd
}

CONFIG(release, debug|release) {
    LIBS += -lGRP
}
