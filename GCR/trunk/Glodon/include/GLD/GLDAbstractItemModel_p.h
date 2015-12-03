#ifndef GLDABSTRACTITEMMODEL_P_H
#define GLDABSTRACTITEMMODEL_P_H

#include <QAbstractItemModel>
#include <QSet>
#include <private/qabstractitemmodel_p.h>
#include "GLDAbstractItemModel.h"

class GLDCOMMONSHARED_EXPORT GlodonAbstractItemModelPrivate : public QAbstractItemModelPrivate
{
    Q_DECLARE_PUBLIC(GlodonAbstractItemModel)

public:
    GlodonAbstractItemModelPrivate();
    virtual ~GlodonAbstractItemModelPrivate();
public:
    bool m_showCheckbox;
    bool m_recursiveCheck;
    bool m_isTristate;
    QSet<QModelIndex> m_checkedIndexes;
    QSet<QModelIndex> m_partiallyCheckedIndexes;
    QSet<QModelIndex> m_forbiddenCheckedIndexes;
};

#endif // GLDABSTRACTITEMMODEL_P_H
