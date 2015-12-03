#ifndef GLDDEFAULTITEMDELEGATEFACTORY_H
#define GLDDEFAULTITEMDELEGATEFACTORY_H

#include <QObject>
#include "GLDTableView_Global.h"

class GlodonDefaultItemDelegate;

class GLDTABLEVIEWSHARED_EXPORT GlodonDefaultItemDelegateFactory : public QObject
{
    Q_OBJECT
public:
    explicit GlodonDefaultItemDelegateFactory(/*QObject *parent = 0*/);
    virtual ~GlodonDefaultItemDelegateFactory();
    virtual GlodonDefaultItemDelegate *createDelegate() = 0;
signals:

public slots:

};

#endif // GLDDEFAULTITEMDELEGATEFACTORY_H
