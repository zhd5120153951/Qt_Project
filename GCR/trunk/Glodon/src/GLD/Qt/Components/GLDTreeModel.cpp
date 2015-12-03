#include "GLDTreeModel.h"

#include "GLDAbstractItemModel.h"

GlodonTreeModel::GlodonTreeModel(QAbstractItemModel *pModel, QObject *parent) :
    GlodonAbstractItemModel(parent)
{
    model = pModel;

    connect(pModel, SIGNAL(rowsInserted(QModelIndex, int, int)),
            this, SLOT(onRowInserted(QModelIndex, int, int)));
    connect(pModel, SIGNAL(rowsRemoved(QModelIndex, int, int)),
            this, SLOT(onRowRemoved(QModelIndex, int, int)));
    connect(pModel, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)),
            this, SLOT(onRowMoved(QModelIndex, int, int, QModelIndex, int)));
    connect(pModel, SIGNAL(modelReset()),
            this, SLOT(onResetModel()));
    connect(pModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)),
            this, SLOT(onUpdate(QModelIndex, QModelIndex, QVector<int>)));

    root = QModelIndex();
}

QModelIndex GlodonTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (row < drawInfo->m_viewItems.count())
    {
        return createIndex(row, column);
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex GlodonTreeModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int GlodonTreeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return drawInfo->m_viewItems.count();
}

int GlodonTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return model->columnCount();
}

QVariant GlodonTreeModel::data(const QModelIndex &index, int role) const
{
    QModelIndex data = dataIndex(index);
    return model->data(data, role);
}

bool GlodonTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    QModelIndex data = dataIndex(index);
    return model->setData(data, value, role);
}

QMap<int, QVariant> GlodonTreeModel::itemData(const QModelIndex &index) const
{
    QModelIndex data = dataIndex(index);
    return model->itemData(data);
}

bool GlodonTreeModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    QModelIndex data = dataIndex(index);
    return model->setItemData(data, roles);
}

QVariant GlodonTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        return model->headerData(section, orientation, role);
    }
    else
    {
        QModelIndex oIndex = dataIndex(index(section, 0));

        switch (role)
        {
            case Qt::TextAlignmentRole:
            {
                return model->data(oIndex, gidrHeaderTextAlignmentRole);
            }

            case Qt::DisplayRole:
            {
                if (oIndex.parent().isValid())
                {
                    return model->data(oIndex, gidrRowHeaderData);
                }
                else
                {
                    return model->headerData(oIndex.row(), orientation, role);
                }
            }

            case Qt::DecorationRole:
            {
                return model->data(oIndex, gidrHeaderDecorationRole);
            }

            case Qt::FontRole:
            {
                return model->data(oIndex, gidrHeaderFontRole);
            }
            
            case Qt::DecorationPropertyRole:
            {
                return model->data(oIndex, Qt::DecorationPropertyRole);
            }
					  
            default:
            {
                return model->headerData(oIndex.row(), orientation, role);
            }
        }
    }
}

bool GlodonTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    return model->setHeaderData(section, orientation, value, role);
}

Qt::ItemFlags GlodonTreeModel::flags(const QModelIndex &index) const
{
    return model->flags(dataIndex(index));
}

QStringList GlodonTreeModel::mimeTypes() const
{
    return model->mimeTypes();
}

QMimeData *GlodonTreeModel::mimeData(const QModelIndexList &indexes) const
{
    QModelIndexList dataIndexes;

    for (int i = 0; i < indexes.count(); ++i)
    {
        dataIndexes.append(dataIndex(indexes.at(i)));
    }

    return model->mimeData(dataIndexes);
}

bool GlodonTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row,
                                   int column, const QModelIndex &parent)
{
    return model->dropMimeData(data, action, row, column, dataIndex(parent));
}

bool GlodonTreeModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                      const QModelIndex &parent) const
{
    return model->canDropMimeData(data, action, row, column, dataIndex(parent));
}

QModelIndex GlodonTreeModel::dataIndex(int row, int column) const
{
    if (row < 0 || column < 0 || row >= drawInfo->m_viewItems.count())
    {
        return QModelIndex();
    }

    int nParentItem = drawInfo->m_viewItems[row].parentIndex;

    if (nParentItem == -1)
    {
        return model->index(drawInfo->m_viewItems[row].modelIndex.row(),
                            column);
    }
    else
    {
        return model->index(drawInfo->m_viewItems[row].modelIndex.row(),
                            column,
                            drawInfo->m_viewItems[nParentItem].modelIndex);
    }
}

QModelIndex GlodonTreeModel::dataIndex(const QModelIndex &index) const
{
    if (index.isValid())
    {
        return dataIndex(index.row(), index.column());
    }
    else
    {
        return QModelIndex();
    }
}

void GlodonTreeModel::onRowRemoved(const QModelIndex &parent, int first, int last)
{
    int nRow = drawInfo->rowNo(parent);

    if (nRow < -1)
    {
        return;
    }

    int nStart = visualRowNo(parent, first);
    int nEnd = visualRowNo(parent, last);

    int nLastDelRow = nEnd + drawInfo->m_viewItems[nEnd].childCount;

    for (int i = nEnd; i >= nStart; i--)
    {
        if (nRow == drawInfo->m_viewItems.at(i).parentIndex)
        {
            drawInfo->removeViewItem(i);
        }
    }

    beginRemoveRows(root, nStart, nLastDelRow);
    endRemoveRows();
//    onResetModel();
}

void GlodonTreeModel::onRowInserted(const QModelIndex &parent, int first, int last)
{
    int row = drawInfo->rowNo(parent);

    if (row < -1)
    {
        return;
    }

    QVector<int> insertedRows;

    for (int i = first; i <= last; i++)
    {
        QModelIndex dataIndex = model->index(i, 0, parent);
        int nCurr = drawInfo->insertViewItem(row, dataIndex);

        if (model->hasChildren(dataIndex))
        {
            onRowInserted(dataIndex, 0, model->rowCount(dataIndex) - 1);
        }

        if (nCurr >= 0)
        {
            insertedRows.append(nCurr);
        }
    }

    int nIndex = 0;

    while (nIndex < insertedRows.count())
    {
        int ninsertedFirst = insertedRows.at(nIndex);
        int ncount = 1;

        while (true)
        {
            if (insertedRows.last() == ninsertedFirst + ncount - 1)
            {
                beginInsertRows(root, ninsertedFirst, ninsertedFirst + ncount - 1);
                endInsertRows();
                return;
            }

            if (insertedRows.at(nIndex) == insertedRows.at(nIndex + 1) - 1)
            {
                nIndex++;
                ncount++;
            }
            else
            {
                beginInsertRows(root, ninsertedFirst, ninsertedFirst + ncount - 1);
                endInsertRows();
                nIndex++;
                break;
            }
        }
    }
}

void GlodonTreeModel::onRowMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    Q_UNUSED(destination);
    Q_UNUSED(row);

    onResetModel();
}

void GlodonTreeModel::onUpdate(const QModelIndex &topLeft,
                               const QModelIndex &bottomRight,
                               const QVector<int> &roles)
{
    int ntopRowNo = visualRowNo(topLeft.parent(), topLeft.row());
    int nbottomRowNo = visualRowNo(bottomRight.parent(), bottomRight.row());

    if (ntopRowNo > nbottomRowNo)
    {
        int ntemp = ntopRowNo;
        ntopRowNo = nbottomRowNo;
        nbottomRowNo = ntemp;
    }

    emit dataChanged(index(ntopRowNo, topLeft.column()),
                     index(nbottomRowNo, bottomRight.column()),
                     roles);
}

void GlodonTreeModel::onResetModel()
{
    drawInfo->reset();

    beginResetModel();
    endResetModel();
}

int GlodonTreeModel::visualRowNo(const QModelIndex &parent, int row) const
{
    int nParentRow = drawInfo->rowNo(parent);

    int nIndex = nParentRow + 1;

    while (row != 0)
    {
        if (nIndex < 0)
        {
            break;
        }

        if (nIndex >= drawInfo->m_viewItems.count())
        {
            break;
        }

        nIndex += (drawInfo->m_viewItems[nIndex].childCount + 1);
        row--;
    }

    return nIndex;
}

QModelIndex GlodonTreeModel::treeIndex(const QModelIndex &dataIndex) const
{
    if (!dataIndex.isValid())
    {
        return QModelIndex();
    }

    Q_ASSERT(dataIndex.model() == model);

    int row = visualRowNo(dataIndex.parent(), dataIndex.row());

    return index(row, dataIndex.column());
}

void GlodonTreeModel::sort(int column, Qt::SortOrder order)
{
    model->sort(column, order);
}
