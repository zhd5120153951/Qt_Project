#-------------------------------------------------
#
# Project created by QtCreator 2012-09-21T09:14:54
#
#-------------------------------------------------
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets core-private widgets-private

TARGET = GLDStaticLib
TEMPLATE = lib

CONFIG += staticlib debug_and_release

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

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += NOMINMAX

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003|win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012|win32-msvc2013 {
    DEFINES += ZLIB_WINAPI
}

DEFINES += \
    QUAZIP_STATIC \
    _UNICODE \
    INITGUID \
    GDP_QT \
    GLD_FULLSOURCE

INCLUDEPATH += $(GLDRS)/ThirdPart/zlib \
    $(GLDRS)/ThirdPart/quazip \
    $(GLDRS)/ThirdPart/RichText/include \
    $(GLDRS)/ThirdPart/libxl/include_cpp \
    $(GLDRS)/ThirdPart/lzma/C \
    $(GLDRS)/ThirdPart/cryptopp/Include \
    ../../include/GLD \
    ../../include/GLD/GLDDockPanel

HEADERS += \
    ../../include/GLD/GLDCommon_Global.h \
    ../../include/GLD/GLDWidget_Global.h \
    ../../include/GLD/GLDTableView_Global.h \
    ../../include/GLD/GLDXML_GLobal.h \
    ../../include/GLD/GLDCrypt_Global.h \
    ../../include/GLD/GLDZip_Global.h \
    ../../include/GLD/GLDNameSpace.h \
    ../../include/GLD/GLDWinErrorDef.h \
    ../../include/GLD/GLDChar.h \
    ../../include/GLD/GLDString.h \
    ../../include/GLD/GLDDateTime.h \
    ../../include/GLD/GLDUuid.h \
    ../../include/GLD/GLDByteArray.h \
    ../../include/GLD/GLDVariant.h \
    ../../include/GLD/GLDIntList.h \
    ../../include/GLD/GLDVariantList.h \
    ../../include/GLD/GLDStringList.h \
    ../../include/GLD/GLDSystem.h \
    ../../include/GLD/GLDEnum.h \
    ../../include/GLD/GLDResourceDef.h \
    ../../include/GLD/GLDIODevice.h \
    ../../include/GLD/GLDBuffer.h \
    ../../include/GLD/GLDFile.h \
    ../../include/GLD/GLDFileInfo.h \
    ../../include/GLD/GLDThread.h \
    ../../include/GLD/GLDMutex.h \
    ../../include/GLD/GLDRunnable.h \
    ../../include/GLD/GLDDir.h \
    ../../include/GLD/GLDSettings.h \
    ../../include/GLD/GLDTextStream.h \
    ../../include/GLD/GLDTypes.h \
    ../../include/GLD/GLDXMLTypes.h \
    ../../include/GLD/GLDUITypes.h \
    ../../include/GLD/GLDStream.h \
    ../../include/GLD/GLDEvent.h \
    ../../include/GLD/GLDAbstractItemModel_p.h \
    ../../include/GLD/GLDAbstractItemModel.h \
    ../../include/GLD/GLDTreeModel.h \
    ../../include/GLD/GLDGroupModel.h \
    ../../include/GLD/GLDFooterModel.h \
    ../../include/GLD/GLDTreeDrawInfo.h \
    ../../include/GLD/GLDGlobal.h \
    ../../include/GLD/GLDSysUtils.h \
    ../../include/GLD/GLDStrUtils.h \
    ../../include/GLD/GLDStreamUtils.h \
    ../../include/GLD/GLDXMLUtils.h \
    ../../include/GLD/GLDFileUtils.h \
    ../../include/GLD/GLDMathUtils.h \
    ../../include/GLD/GLDUIUtils.h \
    ../../include/GLD/GLDObjBase.h \
    ../../include/GLD/GLDObject.h \
    ../../include/GLD/GLDObjectList.h \
    ../../include/GLD/GLDVector.h \
    ../../include/GLD/GLDList.h \
    ../../include/GLD/GLDMap.h \
    ../../include/GLD/GLDStack.h \
    ../../include/GLD/GLDHash.h \
    ../../include/GLD/GLDGuidDef.h \
    ../../include/GLD/GLDUnknwn.h \
    ../../include/GLD/GLDComptr.h \
    ../../include/GLD/GLDComObject.h \
    ../../include/GLD/GLDComLoaderIntf.h \
    ../../include/GLD/GLDComHelper.h \
    ../../include/GLD/GLDComDLLMain.h \
    ../../include/GLD/GLDMacroUtils.h \
    ../../include/GLD/GLDInterfaceObject.h \
    ../../include/GLD/GLDApplication.h \
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
    ../../include/GLD/GLDImageEditor.h \
    ../../include/GLD/GLDColorList.h \
    ../../include/GLD/GLDClassFactory.h \
    ../../include/GLD/GLDEllipsis.h \
    ../../include/GLD/GLDGridSetting.h \
    ../../include/GLD/GLDGridSettingXMLBuilder.h \
    ../../include/GLD/GLDException.h \
    ../../include/GLD/GLDExtPropDefs.h \
    ../../include/GLD/GLDRBTree.h \
    ../../include/GLD/GLDStringObjectList.h \
    ../../include/GLD/GLDSortUtils.h \
    ../../include/GLD/GLDMD5.h \
    ../../include/GLD/GLDSHA1.h \
    ../../include/GLD/GLDDes.h \
    ../../include/GLD/GLDZlib.h \
    ../../include/GLD/GLDZipFile.h \
    ../../include/GLD/GLDZipEnc.h \
    ../../include/GLD/GLDXMLWriter.h \
    ../../include/GLD/GLDXMLSAXUtils.h \
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
    ../../include/GLD/GLDPaperTableView.h \
    ../../include/GLD/GLDPaperTableView_p.h \
    ../../include/GLD/GLDDocView.h \
    ../../include/GLD/GLDPaperWidgetModel.h \
    ../../include/GLD/GLDComboBox.h \
    ../../include/GLD/GLDNetScapeSplitter.h \
    ../../include/GLD/GLDXLS.h \
    ../../include/GLD/GLDXLSModel.h \
    ../../include/GLD/GLDExcelGridIterator.h \
    ../../include/GLD/GLDExcelGridIntf.h \
    ../../include/GLD/GLDExcelIteratorTableView.h \
    ../../include/GLD/GLDDateTimeEdit.h \
    ../../include/GLD/GLDTextEdit.h \
    ../../include/GLD/GLDTableViewExport.h \
    ../../include/GLD/GLDSingleInstance.h \
    ../../include/GLD/GLDWAbstractspinbox_p.h \
    ../../include/GLD/GLDWAbstractspinbox.h \
    ../../include/GLD/GLDDrawSymbol.h \
    ../../include/GLD/GLDFilterWidget.h \
    ../../include/GLD/GLDStrings.h \    
    ../../include/GLD/GLDClassicsOutLookBar.h \
    ../../include/GLD/GLDTranslations.h \
    ../../include/GLD/GLD360MainWindow.h \
    ../../include/GLD/GLDCommentFrame.h \
    ../../include/GLD/GLDFloatFormating.h \
    ../../include/GLD/GLDJSONTypes.h \
    ../../include/GLD/GLDJSONUtils.h \
    ../../include/GLD/GLDSplitter.h \
    ../../include/GLD/GLDSplitter_p.h \
    ../../include/GLD/GLDSplitterHandle.h \
    ../../include/GLD/GLDNetScapeSplitterEx.h \
    ../../include/GLD/GLDNetScapeSplitterHandle.h \
    ../../include/GLD/GLDCommentFrameEx.h \
    ../../include/GLD/GLDCustomWaitingBox.h \
    ../../include/GLD/GLDDockWidget.h \
    ../../include/GLD/GLDXMLBuilder.h \
    ../../include/GLD/GLDXMLCore.h \
    ../../include/GLD/GLDXMLDoc.h \
    ../../include/GLD/GLDXMLInterface.h \
    ../../include/GLD/GLDXMLParser.h \
    ../../include/GLD/GLDTabDockContainer.h \
    ../../include/GLD/GLDScrollStyle.h \
    ../../include/GLD/GLDShadow.h \
    ../../include/GLD/GLDWindowComboBoxEx.h \
    ../../include/GLD/GLDLineWidthComboBoxEx.h \
    ../../include/GLD/GLDFontListEx.h \
    ../../include/GLD/GLDColorListEx.h \
    ../../include/GLD/GLDDefaultItemDelegateFactory.h \
    ../../include/GLD/GLDPaperWidget.h \
    ../../include/GLD/GLDWebLogin.h \
    ../../include/GLD/GLDKeyboardInput.h \
    ../../include/GLD/GLDKeyboardButton.h \
    ../../include/GLD/GLDShellComboBoxEx.h \
    ../../include/GLD/GLDDateTimeEditEx.h \
    ../../include/GLD/GLDTreeViewEx.h \
    ../../include/GLD/GLDShellTreeViewEx.h \
    ../../include/GLD/GLDSearchEdit.h \
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
    ../../include/GLD/GLDTableCornerButton.h  \
    ../../include/GLD/GLDCrypt.h \
    ../../include/GLD/GLDCustomCommentFrame.h

SOURCES += \
    Base/GLDEvent.cpp \
    Base/GLDGlobal.cpp \
    Base/GLDObject.cpp \
    Base/GLDStringObjectList.cpp \
    Base/GLDStream.cpp \
    Base/GLDMD5.cpp \
    Base/GLDSHA1.cpp \
    Base/GLDDes.cpp \
    Base/GLDZlib.cpp \
    Base/GLDZipFile.cpp \
    Base/GLDZipEnc.cpp \
    Base/GLDIniFiles.cpp \
    Base/GLDExtPropDefs.cpp \
    Base/GLDGridSetting.cpp \
    Base/GLDGridSettingXMLBuilder.cpp \
    Base/GLDSysUtils.cpp \
    Base/GLDStrUtils.cpp \
    Base/GLDMathUtils.cpp \
    Base/GLDFileUtils.cpp \
    Base/GLDStreamUtils.cpp \
    Base/GLDSortUtils.cpp \
    Base/GLDXMLWriter.cpp \
    Base/GLDXMLUtils.cpp \
    Base/GLDXMLSAXUtils.cpp \
    Base/GLDUIUtils.cpp \
    Base/GLDXLS.cpp \
    Base/GLDExcelGridIterator.cpp  \
    Base/GLDExcelIteratorTableView.cpp \
    Base/GLDTranslations.cpp \
    Base/GLDCrypt.cpp \
    Qt/Components/GLDAbstractItemModel.cpp \
    Qt/Components/GLDFooterModel.cpp \
    Qt/Components/GLDTreeModel.cpp \
    Qt/Components/GLDGroupModel.cpp \
    Qt/Components/GLDXLSModel.cpp \
    Qt/Widgets/GLDApplication.cpp \
    Qt/Widgets/GLDTreeDrawInfo.cpp \
    Qt/Widgets/GLDTreeView.cpp \
    Qt/Widgets/GLDAbstractItemView.cpp \
    Qt/Widgets/GLDHeaderView.cpp \
    Qt/Widgets/GLDMultiHeaderView.cpp \
    Qt/Widgets/GLDGroupHeaderView.cpp \
    Qt/Widgets/GLDTableView.cpp \
    Qt/Widgets/GLDTableViewBasic.cpp \
    Qt/Widgets/GLDFilterView.cpp \
    Qt/Widgets/GLDFilterTableView.cpp \
    Qt/Widgets/GLDFooterView.cpp \
    Qt/Widgets/GLDFooterTableView.cpp \
    Qt/Widgets/GLDDefaultItemDelegate.cpp \
    Qt/Widgets/GLDImageEditor.cpp \
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
    Qt/Widgets/GLDDocView.cpp \
    Qt/Widgets/GLDPaperWidgetModel.cpp \
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
    Qt/Widgets/GLDCommentFrameEx.cpp \
    Qt/Widgets/GLDCustomCommentFrame.cpp \
    Base/GLDTableViewExport.cpp \
    Base/GLDSingleInstance.cpp \
    Base/GLDDrawSymbol.cpp \
    Base/GLDStrings.cpp \    
    Base/GLDFloatFormating.cpp \
    Base/GLDJSONUtils.cpp \
    Qt/Widgets/GLDCustomWaitingBox.cpp \
    Qt/Widgets/GLDDockWidget.cpp \
    Base/GLDXMLBuilder.cpp \
    Base/GLDXMLCore.cpp \
    Base/GLDXMLDoc.cpp \
    Base/GLDXMLParser.cpp \
    Qt/Widgets/GLDTabDockContainer.cpp \
    Qt/Widgets/GLDScrollStyle.cpp \
    Qt/Widgets/GLDShadow.cpp \
    Qt/Widgets/GLDWindowComboBoxEx.cpp \
    Qt/Widgets/GLDLineWidthComboBoxEx.cpp \
    Qt/Widgets/GLDFontListEx.cpp \
    Qt/Widgets/GLDColorListEx.cpp \
    Qt/Widgets/GLDDefaultItemDelegateFactory.cpp \
    Qt/Widgets/GLDPaperWidget.cpp \
    Qt/Widgets/GLDWebLogin.cpp \
    Qt/Widgets/GLDKeyboardInput.cpp \
    Qt/Widgets/GLDKeyboardButton.cpp \
    Qt/Widgets/GLDShellComboBoxEx.cpp \
    Qt/Widgets/GLDDateTimeEditEx.cpp \
    Qt/Widgets/GLDTreeViewEx.cpp \
    Qt/Widgets/GLDShellTreeViewEx.cpp \
    Qt/Widgets/GLDSearchEdit.cpp \
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
    Qt/Widgets/GLDTableCornerButton.cpp \
    Qt/Widgets/GLDPaperTableView.cpp

RESOURCES += \
    Res/ico/GLDIcons.qrc \
    Res/ini/GLDInis.qrc \
    Res/qss/GLDQsses.qrc

TRANSLATIONS += GLD_zh_CN.ts
