/*!
*@file     
*@brief    GLD程序翻译配置文件加载
*@author   cuidc
*@date     2014年3月11日
*@remarks  
*@version  3.0
*Copyright (c) 1998-2014 Glodon Corporation
*/
#ifndef GLDTRANSLATIONS_H
#define GLDTRANSLATIONS_H

#include <QObject>
#include <QTranslator>
#include "GLDCommon_Global.h"
#include "GLDString.h"
#include "GLDXMLTypes.h"

class GLDTranslationsPrivate;
class GLDCOMMONSHARED_EXPORT GLDTranslations : public QObject
{
	Q_OBJECT
public:
    GLDTranslations(QObject *parent = NULL);
    ~GLDTranslations();
	// 加载翻译配置文件
    bool loadConfigFile(const GString filePath, const GString configfileName = "TranslationsConfig.xml");
	// 加载翻译文件
    bool installTranslators();
private:
    Q_DECLARE_PRIVATE(GLDTranslations);
};

#endif // GLDTRANSLATIONS_H
