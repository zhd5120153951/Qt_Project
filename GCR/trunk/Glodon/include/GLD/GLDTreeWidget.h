#ifndef GLDTREEWIDGET_H
#define GLDTREEWIDGET_H

#include <QTreeWidget>
#include "GLDTableView_Global.h"
 
class GLDTABLEVIEWSHARED_EXPORT GlodonTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    inline GlodonTreeWidget(QWidget *parent = 0) : QTreeWidget(parent) {}
};

#endif // GLDTREEWIDGET_H
