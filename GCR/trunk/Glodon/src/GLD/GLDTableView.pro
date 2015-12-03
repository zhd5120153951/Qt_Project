#-------------------------------------------------
#
# Project created by QtCreator 2012-09-21T09:14:54
#
#-------------------------------------------------
QT += xml
QT += printsupport core
greaterThan(QT_MAJOR_VERSION, 4): QT +=  core-private widgets-private

TARGET = GLDTableView
TEMPLATE = lib

CONFIG +=  debug_and_release

unix:QMAKE_CXXFLAGS += -std=c++11

CONFIG(debug, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        DESTDIR = ../../bin/Debug/X64
    } else {
        DESTDIR = ../../bin/Debug/X86
    }
    unix:TARGET=$$join(TARGET,,,_debug)
    win32:TARGET=$$join(TARGET,,,d)
}

CONFIG(release, debug|release){
    contains(QMAKE_HOST.arch, x86_64) {
        DESTDIR = ../../bin/Release/X64
    } else {
        DESTDIR = ../../bin/Release/X86
    }
}

QMAKE_CXXFLAGS_RELEASE += /Zi
QMAKE_LFLAGS_RELEASE += /DEBUG

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += NOMINMAX

DEFINES += GLODONCOMMON_LIBRARY \
    QUAZIP_STATIC \
    INITGUID \
    GDP_QT \
    GLDTABLEVIEW_LIBRARY

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003|win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012|win32-msvc2013 {
    include($(GLDRS)/Glodon/shared/GLDCommon.pri)
    include($(GLDRS)/Glodon/shared/GLDXLS.pri)
    include($(GLDRS)/Glodon/shared/GLDXML.pri)
    include($(GLDRS)/Glodon/shared/GLDWidget.pri)
}

INCLUDEPATH += $(GLDRS)/ThirdPart/quazip \
    $(GLDRS)/ThirdPart/RichText/include \
    $(GLDRS)/ThirdPart/libxl/include_cpp \
    ../../include/GLD

HEADERS += \
    ../../include/GLD/GLDTreeModel.h \
    ../../include/GLD/GLDCommentFrameEx.h \
    ../../include/GLD/GLDCustomCommentFrame.h \
    ../../include/GLD/GLDTreeView.h \
    ../../include/GLD/GLDTreeWidget.h \
    ../../include/GLD/GLDAbstractItemView.h \
    ../../include/GLD/GLDAbstractItemView_p.h \
    ../../include/GLD/GLDHeaderView.h \
    ../../include/GLD/GLDHeaderView_p.h \
    ../../include/GLD/GLDMultiHeaderView.h \
    ../../include/GLD/GLDMultiHeaderView_p.h \
    ../../include/GLD/GLDGroupHeaderView.h \
    ../../include/GLD/GLDGroupHeaderView_p.h \
    ../../include/GLD/GLDTableView.h \
    ../../include/GLD/GLDTableViewBasic.h \
    ../../include/GLD/GLDTableView_p.h \
    ../../include/GLD/GLDTableViewBasic_p.h \
    ../../include/GLD/GLDFilterView.h \
    ../../include/GLD/GLDFilterView_p.h \
    ../../include/GLD/GLDFilterTableView.h \
    ../../include/GLD/GLDFilterTableView_p.h \
    ../../include/GLD/GLDFooterView.h \
    ../../include/GLD/GLDFooterView_p.h \
    ../../include/GLD/GLDFooterTableView.h \
    ../../include/GLD/GLDFooterTableView_p.h \
    ../../include/GLD/GLDDefaultItemDelegate.h \
    ../../include/GLD/GLDGridSetting.h \
    ../../include/GLD/GLDGridSettingXMLBuilder.h \
    ../../include/GLD/GLDExtPropDefs.h \
    ../../include/GLD/GLDDocView.h \
    ../../include/GLD/GLDDrawSymbol.h \
    ../../include/GLD/GLDExcelGridIterator.h \
    ../../include/GLD/GLDExcelGridIntf.h \
    ../../include/GLD/GLDExcelIteratorTableView.h \
    ../../include/GLD/GLDTableViewExport.h \
    ../../include/GLD/GLDDefaultItemDelegateFactory.h \
    ../../include/GLD/GLDPaperWidget.h \
    ../../include/GLD/GLDPaperTableView.h \
    ../../include/GLD/GLDPaperWidgetModel.h \
    ../../include/GLD/GLDTreeViewEx.h \
    ../../include/GLD/GLDTableView_Global.h \
    ../../include/GLD/GLDXLSModel.h \
    ../../include/GLD/GLDXLS.h

SOURCES += \ 
    Qt/Components/GLDTreeModel.cpp \
    Qt/Widgets/GLDAbstractItemView.cpp \
    Qt/Widgets/GLDCommentFrameEx.cpp \
    Qt/Widgets/GLDCustomCommentFrame.cpp \
    Qt/Widgets/GLDDefaultItemDelegate.cpp \
    Qt/Widgets/GLDDefaultItemDelegateFactory.cpp \
    Qt/Widgets/GLDDocView.cpp \
    Qt/Widgets/GLDFilterTableView.cpp \
    Qt/Widgets/GLDFilterView.cpp \
    Qt/Widgets/GLDFooterTableView.cpp \
    Qt/Widgets/GLDFooterView.cpp \
    Qt/Widgets/GLDGroupHeaderView.cpp \
    Qt/Widgets/GLDHeaderView.cpp \
    Qt/Widgets/GLDMultiHeaderView.cpp \
    Qt/Widgets/GLDPaperWidget.cpp \
    Qt/Widgets/GLDPaperTableView.cpp \
    Qt/Widgets/GLDPaperWidgetModel.cpp \
    Qt/Widgets/GLDTableView.cpp \
    Qt/Widgets/GLDTableViewBasic.cpp \
    Qt/Widgets/GLDTreeDrawInfo.cpp \
    Qt/Widgets/GLDTreeView.cpp \
    Qt/Widgets/GLDTreeViewEx.cpp \
    Base/GLDDrawSymbol.cpp \
    Base/GLDExcelGridIterator.cpp \
    Base/GLDExcelIteratorTableView.cpp \
    Base/GLDExtPropDefs.cpp \
    Base/GLDGridSetting.cpp \
    Base/GLDGridSettingXMLBuilder.cpp \
    Base/GLDTableViewExport.cpp \
    Base/GLDXLS.cpp \
    Qt/Components/GLDXLSModel.cpp

RESOURCES += \
    Res/ico/GLDIcons.qrc \
    Res/ini/GLDInis.qrc \
    Res/qss/GLDQsses.qrc

TRANSLATIONS += GLD_zh_CN.ts

RC_FILE = Res/rc/GLDTableView.rc
