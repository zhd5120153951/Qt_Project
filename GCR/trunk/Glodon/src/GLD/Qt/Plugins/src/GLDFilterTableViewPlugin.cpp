#include "GLDFilterTableView.h"
#include "GLDFilterTableViewPlugin.h"

#include <QtPlugin>

GlodonFilterTableViewPlugin::GlodonFilterTableViewPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void GlodonFilterTableViewPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;
    
    // Add extension registrations, etc. here
    
    m_initialized = true;
}

bool GlodonFilterTableViewPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *GlodonFilterTableViewPlugin::createWidget(QWidget *parent)
{
    return new GlodonFilterTableView(parent);
}

QString GlodonFilterTableViewPlugin::name() const
{
    return QLatin1String("GLDFilterTableView");
}

QString GlodonFilterTableViewPlugin::group() const
{
    return QLatin1String("Glodon");
}

QIcon GlodonFilterTableViewPlugin::icon() const
{
    return QIcon(QLatin1String(":/ico/GlodonFilterTableView.ico"));
}

QString GlodonFilterTableViewPlugin::toolTip() const
{
    return QLatin1String("GLDFilterTableView");
}

QString GlodonFilterTableViewPlugin::whatsThis() const
{
    return QLatin1String("GLDFilterTableView");
}

bool GlodonFilterTableViewPlugin::isContainer() const
{
    return false;
}

QString GlodonFilterTableViewPlugin::domXml() const
{
    return QLatin1String("<widget class=\"GLDFilterTableView\" name=\"gldFilterTableView\">\n</widget>\n");
}

QString GlodonFilterTableViewPlugin::includeFile() const
{
    return QLatin1String("GLDFilterTableView.h");
}

