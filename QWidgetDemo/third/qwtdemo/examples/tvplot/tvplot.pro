TARGET      = tvplot
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

HEADERS += tvplot.h
SOURCES += tvplot.cpp
SOURCES += main.cpp

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../../../3rd_qwt
include ($$PWD/../../../3rd_qwt/3rd_qwt.pri)
