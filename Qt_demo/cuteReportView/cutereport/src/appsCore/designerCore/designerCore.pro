! include( ../../../common.pri ) {
    error( Can not find the common.pri file! )
}

TARGET = CuteDesigner
TEMPLATE = lib

SOURCES += \
    core.cpp \
    moduleinterface.cpp \
    widgets/objectinspector/objectinspector.cpp \
    widgets/objectinspector/objectmodel.cpp \
    widgets/fancytabs.cpp \
    widgets/stylehelper.cpp \
    mainwindow.cpp \
    aboutdialog.cpp \
    storagesettingsdialog.cpp \
    optionsdialog.cpp \
    optionsdialogstoragepage.cpp \
    widgets/messagewidget.cpp \
    widgets/messagelist.cpp \
    widgets/messagemodel.cpp \
    widgets/messagedelegate.cpp \
    widgets/renamedialog.cpp \
    optionsdialogrendererpage.cpp \
    optionsdialogprinterpage.cpp \
    designeriteminterfaceobject.cpp \

HEADERS  += globals.h \
    core.h \
    moduleinterface.h \
    widgets/objectinspector/objectinspector.h \
    widgets/objectinspector/objectmodel.h \
    widgets/fancytabs.h \
    widgets/stylehelper.h \
    mainwindow.h \
    aboutdialog.h \
    storagesettingsdialog.h \
    optionsdialog.h \
    optionsdialogstoragepage.h \
    designer_globals.h \
    widgets/messagewidget.h \
    widgets/messagelist.h \
    widgets/messagemodel.h \
    widgets/messagedelegate.h \
    widgets/renamedialog.h \
    optionsdialogrendererpage.h \
    optionsdialogprinterpage.h \
    designeriteminterfaceobject.h \

RESOURCES += designerCore.qrc

FORMS += mainwindow.ui \
         aboutdialog.ui \
         storagesettingsdialog.ui \
         optionsdialog.ui \
         optionsdialogstoragepage.ui \
         widgets/messagelist.ui \
         widgets/renamedialog.ui \
         optionsdialogrendererpage.ui \
         optionsdialogprinterpage.ui \

INCLUDEPATH += ../../core \
            ../../widgets/widgets \
            ../../_common \
            ../../core/log \
            ../../thirdparty/propertyeditor/lib  \
            ../designer/ \
            widgets \
            widgets/objectinspector

DEPENDPATH += $$INCLUDEPATH


LIBS += -L../../../$$BUILD_DIR -lPropertyEditor -lCuteReport -lCuteReportWidgets

contains(DEFINES, SYSTEMINSTALL) {
    DESTDIR = ../../../$$BUILD_DIR
    target.path += $$REPORT_DESIGNER_LIBS_PATH
    INSTALLS += target
} else {
    DESTDIR = ../../../$$BUILD_DIR/$$REPORT_DESIGNER_LIBS_PATH
}


contains(DEFINES, STATIC_DESIGNER) {
    CONFIG += static
} else {
    CONFIG += shared
    win32: TARGET_EXT = .dll
}

win32 {
    DEFINES += CUTEREPORT_DESIGNER_EXPORTS
}
