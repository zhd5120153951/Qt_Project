#ifndef GLODONWIDGETS_H
#define GLODONWIDGETS_H

#include <QtDesigner>
#include <qplugin.h>

class GlodonWidgets : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "org.devmachines.Qt.QDesignerCustomWidgetCollectionInterface")
#endif
    
public:
    explicit GlodonWidgets(QObject *parent = 0);
    
    virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;
    
private:
    QList<QDesignerCustomWidgetInterface*> m_widgets;
};

#endif
