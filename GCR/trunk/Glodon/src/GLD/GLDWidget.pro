#-------------------------------------------------
#
# Project created by QtCreator 2012-09-21T09:14:54
#
#-------------------------------------------------
#QT += gui  //去掉也能编译过
QT += core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets core-private widgets-private

TARGET = GLDWidget
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
    GLDWIDGET_LIBRARY

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003|win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012|win32-msvc2013 {
    include($(GLDRS)/Glodon/shared/GLDCommon.pri)
    LIBS += user32.lib
}

INCLUDEPATH += $(GLDRS)/ThirdPart/quazip \
    $(GLDRS)/ThirdPart/RichText/include \
    $(GLDRS)/ThirdPart/libxl/include_cpp \
    ../../include/GLD \
    ../../include/GLD/GLDDockPanel

HEADERS += \    
    ../../include/GLD/GLDApplication.h \
    ../../include/GLD/GLDImageEditor.h \
    ../../include/GLD/GLDApplication.h \
    ../../include/GLD/GLDColorList.h \
    ../../include/GLD/GLDClassFactory.h \
    ../../include/GLD/GLDEllipsis.h \
    ../../include/GLD/GLDException.h \
    ../../include/GLD/GLDRBTree.h \
    ../../include/GLD/GLDIniFiles.h \
    ../../include/GLD/GLDFontList.h \
    ../../include/GLD/GLDStylePaintUtils.h \
    ../../include/GLD/GLDWindowComboBox.h \
    ../../include/GLD/GLDDockContainer.h \
    ../../include/GLD/GLDSpinBox.h \
    ../../include/GLD/GLDToolBox.h \
    ../../include/GLD/GLDShellWidgets.h \
    ../../include/GLD/GLDFileSystemModel.h \
    ../../include/GLD/GLDAbstractBtnEdit.h \
    ../../include/GLD/GLDListView.h \
    ../../include/GLD/GLDComboBox.h \
    ../../include/GLD/GLDNetScapeSplitter.h \
    ../../include/GLD/GLDDateTimeEdit.h \
    ../../include/GLD/GLDTextEdit.h \
    ../../include/GLD/GLDWAbstractspinbox_p.h \
    ../../include/GLD/GLDWAbstractspinbox.h \
    ../../include/GLD/GLDFilterWidget.h \    
    ../../include/GLD/GLDClassicsOutLookBar.h \
    ../../include/GLD/GLD360MainWindow.h \
    ../../include/GLD/GLDCommentFrame.h \
    ../../include/GLD/GLDSplitter.h \
    ../../include/GLD/GLDSplitter_p.h \
    ../../include/GLD/GLDSplitterHandle.h \
    ../../include/GLD/GLDNetScapeSplitterEx.h \
    ../../include/GLD/GLDNetScapeSplitterHandle.h \
    ../../include/GLD/GLDCustomWaitingBox.h \
    ../../include/GLD/GLDDockWidget.h \
    ../../include/GLD/GLDTabDockContainer.h \
    ../../include/GLD/GLDScrollStyle.h \
    ../../include/GLD/GLDShadow.h \
    ../../include/GLD/GLDWindowComboBoxEx.h \
    ../../include/GLD/GLDLineWidthComboBoxEx.h \
    ../../include/GLD/GLDFontListEx.h \
    ../../include/GLD/GLDColorListEx.h \
    ../../include/GLD/GLDWebLogin.h \
    ../../include/GLD/GLDKeyboardInput.h \
    ../../include/GLD/GLDKeyboardButton.h \
    ../../include/GLD/GLDShellComboBoxEx.h \
    ../../include/GLD/GLDDateTimeEditEx.h \
    ../../include/GLD/GLDShellTreeViewEx.h \
    ../../include/GLD/GLDSearchEdit.h \
    #../../include/GLD/GLDFileDialog.h \
    #../../include/GLD/GLDMutilFileModel.h \
    ../../include/GLD/GLDCalendarWidget.h\
    ../../include/GLD/GLDProgressBar.h \
    ../../include/GLD/GLDProgressBarEvent.h \
    Qt/Widgets/GLDDockPanel/GLDDockTabWidget.h \
    Qt/Widgets/GLDDockPanel/GLDDockTabBar.h \
    Qt/Widgets/GLDDockPanel/GLDDockPanelComponents.h \
    Qt/Widgets/GLDDockPanel/GLDDockPanel.h \
    Qt/Widgets/GLDDockPanel/GLDDockNode.h \
    Qt/Widgets/GLDDockPanel/GLDDockMaskWidget.h \
    Qt/Widgets/GLDDockPanel/GLDDockDataBuilder.h \
    Qt/Widgets/GLDDockPanel/GLDDockArrows.h \
    ../../include/GLD/GLDDockPanel/GLDDockManager.h \
    ../../include/GLD/GLDDockPanel/GLDDockFrame.h \
    ../../include/GLD/GLDDockPanel/GLDDockCommon.h \
    ../../include/GLD/GLDTableCornerButton.h \
    ../../include/GLD/GLDWidget_Global.h

SOURCES += \ 
    #Qt/Components/GLDMutilFileModel.cpp \
    Qt/Widgets/GLDApplication.cpp \
    Qt/Widgets/GLDImageEditor.cpp \
    Qt/Widgets/GLDApplication.cpp \
    Qt/Widgets/GLDFontList.cpp \
    Qt/Widgets/GLDColorList.cpp \
    Qt/Widgets/GLDEllipsis.cpp \
    Qt/Widgets/GLDStylePaintUtils.cpp \
    Qt/Widgets/GLDWindowComboBox.cpp \
    Qt/Widgets/GLDSpinBox.cpp \
    Qt/Widgets/GLDToolBox.cpp \
    Qt/Widgets/GLDShellWidgets.cpp \
    Qt/Widgets/GLDFileSystemModel.cpp \
    Qt/Widgets/GLDAbstractBtnEdit.cpp \
    Qt/Widgets/GLDListView.cpp \
    Qt/Widgets/GLDComboBox.cpp \
    Qt/Widgets/GLDNetScapeSplitter.cpp \
    Qt/Widgets/GLDNetScapeSplitterHandle.cpp \
    Qt/Widgets/GLDDockContainer.cpp \
    Qt/Widgets/GLDDateTimeEdit.cpp \
    Qt/Widgets/GLDTextEdit.cpp \
    Qt/Widgets/GLDWAbstractspinbox.cpp \
    Qt/Widgets/GLDFilterWidget.cpp \
    Qt/Widgets/GLD360MainWindow.cpp \
    Qt/Widgets/GLDClassicsOutLookBar.cpp \
    Qt/Widgets/GLDCommentFrame.cpp \
    Qt/Widgets/GLDSplitter.cpp \
    Qt/Widgets/GLDSplitterHandle.cpp \
    Qt/Widgets/GLDNetScapeSplitterEx.cpp \
    Qt/Widgets/GLDCustomWaitingBox.cpp \
    Qt/Widgets/GLDDockWidget.cpp \  
    Qt/Widgets/GLDTabDockContainer.cpp \
    Qt/Widgets/GLDScrollStyle.cpp \
    Qt/Widgets/GLDShadow.cpp \
    Qt/Widgets/GLDWindowComboBoxEx.cpp \
    Qt/Widgets/GLDLineWidthComboBoxEx.cpp \
    Qt/Widgets/GLDFontListEx.cpp \
    Qt/Widgets/GLDColorListEx.cpp \
    Qt/Widgets/GLDWebLogin.cpp \
    Qt/Widgets/GLDKeyboardInput.cpp \
    Qt/Widgets/GLDKeyboardButton.cpp \
    Qt/Widgets/GLDShellComboBoxEx.cpp \
    Qt/Widgets/GLDDateTimeEditEx.cpp \
    Qt/Widgets/GLDShellTreeViewEx.cpp \
    Qt/Widgets/GLDSearchEdit.cpp \
    #Qt/Widgets/GLDFileDialog.cpp \
    Qt/Widgets/GLDCalendarWidget.cpp\
    Qt/Widgets/GLDProgressBar.cpp \
    Qt/Widgets/GLDProgressBarEvent.cpp \
    Qt/Widgets/GLDDockPanel/GLDDockTabWidget.cpp \
    Qt/Widgets/GLDDockPanel/GLDDockTabBar.cpp \
    Qt/Widgets/GLDDockPanel/GLDDockPanelComponents.cpp \
    Qt/Widgets/GLDDockPanel/GLDDockPanel.cpp \
    Qt/Widgets/GLDDockPanel/GLDDockNode.cpp \
    Qt/Widgets/GLDDockPanel/GLDDockMaskWidget.cpp \
    Qt/Widgets/GLDDockPanel/GLDDockManager.cpp \
    Qt/Widgets/GLDDockPanel/GLDDockFrame.cpp \
    Qt/Widgets/GLDDockPanel/GLDDockDataBuilder.cpp \
    Qt/Widgets/GLDDockPanel/GLDDockArrows.cpp \
    Qt/Widgets/GLDTableCornerButton.cpp

RESOURCES += \
    Res/ico/GLDIcons.qrc \
    Res/ini/GLDInis.qrc \
    Res/qss/GLDQsses.qrc

TRANSLATIONS += GLD_zh_CN.ts

RC_FILE = Res/rc/GLDWidget.rc
