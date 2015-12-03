#include "GLDTableViewPlugin.h"
#include "GLDFilterTableViewPlugin.h"
#include "GLDWidgets.h"

GlodonWidgets::GlodonWidgets(QObject *parent)
    : QObject(parent)
{
    m_widgets.append(new GlodonTableViewPlugin(this));
    m_widgets.append(new GlodonFilterTableViewPlugin(this));
    
}

QList<QDesignerCustomWidgetInterface*> GlodonWidgets::customWidgets() const
{
    return m_widgets;
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2(glodonwidgets, GlodonWidgets)
#endif
