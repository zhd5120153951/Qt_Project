#-------------------------------------------------
#
# Project created by QtCreator 2012-09-21T09:14:54
#
#-------------------------------------------------
greaterThan(QT_MAJOR_VERSION, 4): QT += core-private widgets xml

TARGET = GLDCommon
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
    GLD_LIBRARY \
    GLDCOMMON_LIBRARY

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003|win32-msvc2005|win32-msvc2008|win32-msvc2010|win32-msvc2012|win32-msvc2013 {
     LIBS += -loleaut32 -lole32 -lShell32
}

INCLUDEPATH += ../../include/GLD

HEADERS += \
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
    ../../include/GLD/GLDStream.h \
    ../../include/GLD/GLDEvent.h \  
    ../../include/GLD/GLDGlobal.h \
    ../../include/GLD/GLDSysUtils.h \
    ../../include/GLD/GLDStrUtils.h \
    ../../include/GLD/GLDStreamUtils.h \  
    ../../include/GLD/GLDFileUtils.h \
    ../../include/GLD/GLDMathUtils.h \
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
    ../../include/GLD/GLDClassFactory.h \ 
    ../../include/GLD/GLDException.h \   
    ../../include/GLD/GLDRBTree.h \
    ../../include/GLD/GLDStringObjectList.h \
    ../../include/GLD/GLDSortUtils.h \  
    ../../include/GLD/GLDIniFiles.h \     
    ../../include/GLD/GLDSingleInstance.h \   
    ../../include/GLD/GLDStrings.h \     
    ../../include/GLD/GLDFloatFormating.h \
    ../../include/GLD/GLDJSONTypes.h \
    ../../include/GLD/GLDJSONUtils.h \
    ../../include/GLD/GLDAbstractItemModel_p.h \
    ../../include/GLD/GLDAbstractItemModel.h \
    ../../include/GLD/GLDGroupModel.h \
    ../../include/GLD/GLDFooterModel.h \
    ../../include/GLD/GLDCommon_Global.h \
    ../../include/GLD/GLDUIUtils.h \
    ../../include/GLD/GLDXMLTypes.h \
    ../../include/GLD/GLDTranslations.h \

SOURCES += \
    Base/GLDEvent.cpp \
    Base/GLDGlobal.cpp \
    Base/GLDObject.cpp \
    Base/GLDStringObjectList.cpp \
    Base/GLDStream.cpp \ 
    Base/GLDIniFiles.cpp \    
    Base/GLDSysUtils.cpp \
    Base/GLDStrUtils.cpp \
    Base/GLDMathUtils.cpp \
    Base/GLDFileUtils.cpp \
    Base/GLDStreamUtils.cpp \
    Base/GLDSortUtils.cpp \   
    Base/GLDSingleInstance.cpp \  
    Base/GLDStrings.cpp \    
    Base/GLDFloatFormating.cpp \
    Qt/Components/GLDAbstractItemModel.cpp \
    Base/GLDJSONUtils.cpp \
    Qt/Components/GLDFooterModel.cpp \
    Qt/Components/GLDGroupModel.cpp \
    Base/GLDUIUtils.cpp \
    Base/GLDTranslations.cpp

RESOURCES += \
    Res/ico/GLDIcons.qrc \
    Res/ini/GLDInis.qrc \
    Res/qss/GLDQsses.qrc

TRANSLATIONS += GLD_zh_CN.ts

RC_FILE = Res/rc/GLDCommon.rc
