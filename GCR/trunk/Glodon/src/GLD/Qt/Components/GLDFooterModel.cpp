#include "GLDFooterModel.h"
#include "GLDGlobal.h"
#include "GLDStringList.h"

class QModelIndexHack
{
public:
    int r, c;
    quintptr i;
    QAbstractItemModel *m;
};

/* GlodonFooterModel */

GlodonFooterModel::GlodonFooterModel(QAbstractItemModel *pModel, QObject *parent) :
    GlodonAbstractItemModel(parent), m_model(pModel), m_footerRowCount(1)
{
    connect(m_model, SIGNAL(modelReset()), this, SLOT(onResetModel()));
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(onDataChange(QModelIndex,QModelIndex,QVector<int>)));
}

QModelIndex GlodonFooterModel::index(const QModelIndex &dataIndex) const
{
    return index(dataIndex.row(), dataIndex.column(), dataIndex.parent());
}

QModelIndex GlodonFooterModel::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex oIndex = m_model->index(row + m_model->rowCount() - m_footerRowCount, column, parent);
    return createIndex(row, column, oIndex.internalPointer());
}

QModelIndex GlodonFooterModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
    Q_UNUSED(child)
}

int GlodonFooterModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return m_footerRowCount;
}

int GlodonFooterModel::columnCount(const QModelIndex &parent) const
{
    return m_model->columnCount(dataIndex(parent));
}

bool GlodonFooterModel::hasChildren(const QModelIndex &parent) const
{
    return false;
    Q_UNUSED(parent)
}

QVariant GlodonFooterModel::data(const QModelIndex &index, int role) const
{
    return m_model->data(dataIndex(index), role);
}

bool GlodonFooterModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return m_model->setData(dataIndex(index), value, role);
}

QModelIndex GlodonFooterModel::dataIndex(const QModelIndex &footerIndex) const
{
    if (footerIndex.isValid())
    {
        int nRow = m_model->rowCount() - m_footerRowCount;
        return m_model->index(nRow + footerIndex.row(), footerIndex.column(), footerIndex.parent());
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex GlodonFooterModel::footerIndex(const QModelIndex &dataIndex) const
{
    if (!dataIndex.isValid())
        return QModelIndex();
    QModelIndex parent = dataIndex.parent();
    if (!parent.isValid())
    {
        int nRowCount = m_model->rowCount(parent);
        return index(dataIndex.row() - (nRowCount - m_footerRowCount),
                           dataIndex.column(), dataIndex.parent());
    }
    else
        return QModelIndex();
}

void GlodonFooterModel::onDataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                     const QVector<int> &roles)
{
    emit dataChanged(footerIndex(topLeft), footerIndex(bottomRight), roles);
}

void GlodonFooterModel::onResetModel()
{
    beginResetModel();
    endResetModel();
}

/*GlodonFooterBodyModel*/

GlodonFooterBodyModel::GlodonFooterBodyModel(QAbstractItemModel *model, QObject *parent) :
    GlodonAbstractItemModel(parent), m_model(model), m_footerRowCount(1)
{
    connect(m_model, SIGNAL(rowsInserted(const QModelIndex &,int,int)), this,
            SLOT(onRowInserted(const QModelIndex &,int,int)));
    connect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(onRowRemoved(QModelIndex,int,int)));
    connect(m_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
            this, SLOT(onRowMoved(QModelIndex,int,int,QModelIndex,int)));
    connect(m_model, SIGNAL(modelReset()), this, SLOT(onResetModel()));
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(onDataChange(QModelIndex,QModelIndex,QVector<int>)));
}

QModelIndex GlodonFooterBodyModel::index(const QModelIndex &dataIndex) const
{
    return createIndex(dataIndex.row(), dataIndex.column(), dataIndex.internalPointer());
}

QModelIndex GlodonFooterBodyModel::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex oIndex = m_model->index(row, column, dataIndex(parent));
    if (!oIndex.isValid())
    {
        return QModelIndex();
    }
    return createIndex(oIndex.row(), oIndex.column(), oIndex.internalPointer());
}

QModelIndex GlodonFooterBodyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();
    return footerBodyIndex(m_model->parent(dataIndex(child)));
}

int GlodonFooterBodyModel::rowCount(const QModelIndex &parent) const
{
    QModelIndex oParentDataIndex = dataIndex(parent);
    int nRowCount = m_model->rowCount(oParentDataIndex);
    if (parent.isValid())
    {
        return nRowCount;
    }
    else
    {
        if (nRowCount > 0)
            return nRowCount - m_footerRowCount;
        else
            return 0;
    }
}

int GlodonFooterBodyModel::columnCount(const QModelIndex &parent) const
{
    return m_model->columnCount(dataIndex(parent));
}

bool GlodonFooterBodyModel::hasChildren(const QModelIndex &parent) const
{
    return m_model->hasChildren(dataIndex(parent));
}

QVariant GlodonFooterBodyModel::data(const QModelIndex &index, int role) const
{
    return m_model->data(dataIndex(index), role);
}

bool GlodonFooterBodyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return m_model->setData(dataIndex(index), value, role);
}

QVariant GlodonFooterBodyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return m_model->headerData(section, orientation, role);
}

bool GlodonFooterBodyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    return m_model->setHeaderData(section, orientation, value, role);
}

void GlodonFooterBodyModel::sort(int column, Qt::SortOrder order)
{
    emit layoutAboutToBeChanged();

    m_model->sort(column, order);

    emit layoutChanged();
}

Qt::ItemFlags GlodonFooterBodyModel::flags(const QModelIndex &index) const
{
    return m_model->flags(dataIndex(index));
}

QStringList GlodonFooterBodyModel::mimeTypes() const
{
    return m_model->mimeTypes();
}

QMimeData *GlodonFooterBodyModel::mimeData(const QModelIndexList &indexes) const
{
    QModelIndexList dataIndexes;
    for (int i = 0; i < indexes.count(); ++i)
    {
        dataIndexes.append(dataIndex(indexes.at(i)));
    }
    return m_model->mimeData(dataIndexes);
}

bool GlodonFooterBodyModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                            const QModelIndex &parent) const
{
    return m_model->canDropMimeData(data, action, row, column, dataIndex(parent));
}

bool GlodonFooterBodyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                         const QModelIndex &parent)
{
    return m_model->dropMimeData(data, action, row, column, dataIndex(parent));
}

QModelIndex GlodonFooterBodyModel::dataIndex(const QModelIndex &footerIndex) const
{
    if (footerIndex.isValid())
    {
        QModelIndex result = footerIndex;
        reinterpret_cast<QModelIndexHack *>(&result)->m = m_model;
        return result;
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex GlodonFooterBodyModel::footerBodyIndex(const QModelIndex &dataIndex) const
{
    if (!dataIndex.isValid())
    {
        return QModelIndex();
    }
    return createIndex(dataIndex.row(), dataIndex.column(), dataIndex.internalPointer());
}

void GlodonFooterBodyModel::onRowRemoved(const QModelIndex &parent, int first, int last)
{
    beginRemoveRows(dataIndex(parent), first, last);
    endRemoveRows();
}

void GlodonFooterBodyModel::onRowInserted(const QModelIndex &parent, int first, int last)
{
    beginInsertRows(dataIndex(parent), first, last);
    endInsertRows();
}

void GlodonFooterBodyModel::onRowMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination,
                                       int row)
{
    beginMoveRows(dataIndex(parent), start, end, dataIndex(destination), row);
    endMoveRows();
}

void GlodonFooterBodyModel::onUpdate(QModelIndex &topLeft)
{
    topLeft = index(topLeft);
}

void GlodonFooterBodyModel::onResetModel()
{
    beginResetModel();
    m_footerRowCount = m_model->data(QModelIndex(), gidrFooterRowCount).toInt();
    endResetModel();
}

void GlodonFooterBodyModel::onDataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                         const QVector<int> &roles)
{
    emit dataChanged(index(topLeft), index(bottomRight), roles);
}
