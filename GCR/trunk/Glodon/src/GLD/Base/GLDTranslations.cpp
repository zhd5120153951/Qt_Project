#include <QLocale>
#include <QApplication>

#include "GLDGlobal.h"
#include "GLDFileUtils.h"
#include "GLDFile.h"
#include "GLDTranslations.h"

#include <private/qobject_p.h>

class GLDTranslationsPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(GLDTranslations)

private:
  QList<GString> *m_tsFileNames;
  QList<QTranslator *> *m_translators;
  GXMLDocument m_configDom;
};

GLDTranslations::GLDTranslations(QObject *parent)
    : QObject(*(new GLDTranslationsPrivate), parent)
{
    Q_D(GLDTranslations);
    d->m_tsFileNames = new QList<GString>;
    d->m_translators = new QList<QTranslator *>;
}

GLDTranslations::~GLDTranslations()
{
    Q_D(GLDTranslations);
    d->m_tsFileNames->clear();
    freeAndNil(d->m_tsFileNames);

    d->m_translators->clear();
    freeAndNil(d->m_translators);
}

bool GLDTranslations::loadConfigFile(const GString filePath, const GString configfileName)
{
    Q_D(GLDTranslations);
    if (0 == filePath.length()) return false;
    GString split = filePath.endsWith("/") ? "":(filePath.endsWith("\\") ? "" : (filePath.contains("/") ? "/" : "\\"));
    GString fileName = filePath + split + configfileName;
    if (0 == configfileName.length() || filePath.endsWith(".xml"))
    {
        fileName = filePath;
    }
    if (!fileName.endsWith("TranslationsConfig.xml"))
    {
        fileName += "TranslationsConfig.xml";
    }
    GFile file(fileName, this);
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

    if (!d->m_configDom.setContent(&file))
	{
		file.close();
		return false;
	}

    GXMLNode docElem = d->m_configDom.documentElement();
    GXMLNode elmtTRFiles = docElem.firstChildElement("TRFiles");
	QLocale defaultLocale;
	while (!elmtTRFiles.isNull())
	{
		GString strLanguage = elmtTRFiles.attribute("Language");

		if (strLanguage.compare(defaultLocale.name(), Qt::CaseInsensitive) == 0)
		{
			break;
		}
		elmtTRFiles = elmtTRFiles.nextSiblingElement("Language");
	}

	if (elmtTRFiles.isNull())
	{
		return false;
	}

	GXMLNode elmtTrFile = elmtTRFiles.firstChildElement("TRFile");
	while (!elmtTrFile.isNull())
	{
		GString strFileName = elmtTrFile.firstChild().nodeValue();
        d->m_tsFileNames->append(filePath + split + strFileName);
		elmtTrFile = elmtTrFile.nextSiblingElement("TRFile");
	}

	return true;
}

// ¼ÓÔØ·­ÒëÎÄ¼þ
bool GLDTranslations::installTranslators()
{
    Q_D(GLDTranslations);
    for (int i = 0; i < d->m_tsFileNames->count(); ++i)
	{
		QTranslator *translator = new QTranslator(this);
        GString strFileName = d->m_tsFileNames->value(i);
        GString strFileAbPath = /*exePath() + */strFileName;
		translator->load(strFileAbPath);
		bool isOK = qApp->installTranslator(translator);
		if (isOK)
		{
            d->m_translators->append(translator);
		}
		else
		{
			freeAndNil(translator);
		}
	}
	return true;
}
