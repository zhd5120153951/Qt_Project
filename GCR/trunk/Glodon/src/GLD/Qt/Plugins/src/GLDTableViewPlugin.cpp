#include "GLDTableView.h"
#include "GLDTableViewPlugin.h"

#include <QtPlugin>

GlodonTableViewPlugin::GlodonTableViewPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void GlodonTableViewPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;
    
    // Add extension registrations, etc. here
    
    m_initialized = true;
}

bool GlodonTableViewPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *GlodonTableViewPlugin::createWidget(QWidget *parent)
{
    return new GlodonTableView(parent);
}

QString GlodonTableViewPlugin::name() const
{
    return QLatin1String("GLDTableView");
}

QString GlodonTableViewPlugin::group() const
{
    return QLatin1String("Glodon");
}

QIcon GlodonTableViewPlugin::icon() const
{
    return QIcon(QLatin1String(":/ico/GlodonTableView.ico"));
}

QString GlodonTableViewPlugin::toolTip() const
{
    return QLatin1String("GLDTableView");
}

QString GlodonTableViewPlugin::whatsThis() const
{
    return QLatin1String("GLDTableView");
}

bool GlodonTableViewPlugin::isContainer() const
{
    return false;
}

QString GlodonTableViewPlugin::domXml() const
{
    return QLatin1String("<widget class=\"GLDTableView\" name=\"gldTableView\">\n</widget>\n");
}

QString GlodonTableViewPlugin::includeFile() const
{
    return QLatin1String("GLDTableView.h");
}

