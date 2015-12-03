#include "GLDGroupModel.h"
#include "GLDGlobal.h"

#include <QColor>

GlodonGroupModel::GlodonGroupModel(QAbstractItemModel *pModel, QObject *parent) :
    QAbstractItemModel(parent)
{
    m_model = pModel;
    m_root = new GlodonGroupModelGroupItem();
    m_treeCol = MaxInt;
    buildModel();
    connect(pModel, SIGNAL(modelReset()), this, SLOT(onResetModel()));
}

GlodonGroupModel::~GlodonGroupModel()
{
    delete m_root;
}

QModelIndex GlodonGroupModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        if (row < 0 || column < 0 || row > m_model->rowCount() - 1 || column > m_model->columnCount() - 1)
        {
            return QModelIndex();
        }

        return createIndex(row, column, m_root->child.at(row));
    }

    if (GlodonGroupModelAbstractItem * parentItem
            = static_cast<GlodonGroupModelAbstractItem *>(parent.internalPointer()))
    {
        if (row < 0 || column < 0)
        {
            return QModelIndex();
        }
        else
        {
            return createIndex(row, column, parentItem->child.at(row));
        }
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex GlodonGroupModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    if (GlodonGroupModelAbstractItem *item = static_cast<GlodonGroupModelAbstractItem *>(child.internalPointer()))
    {
        if (item->parent == m_root)
        {
            return QModelIndex();
        }

        GlodonGroupModelAbstractItem *parentItem = item->parent->parent;
        int row = 0;

        for (row = 0; row < parentItem->child.count(); row++)
        {
            if (parentItem->child.indexOf(item) >= 0)
            {
                return createIndex(row, 0, item->parent);
            }
        }
    }

    return QModelIndex();
}

int GlodonGroupModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        return m_root->child.count();
    }

    if (GlodonGroupModelAbstractItem *item = static_cast<GlodonGroupModelAbstractItem *>(parent.internalPointer()))
    {
        return item->child.count();
    }

    return 0;
}

int GlodonGroupModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_model->columnCount();
}

QVariant GlodonGroupModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    GlodonGroupModelAbstractItem *item = static_cast<GlodonGroupModelAbstractItem *>(index.internalPointer());

    if (item == NULL)
    {
        return QVariant();
    }

    if (item->type() == GlodonGroupModelAbstractItem::CXGroup)
    {
        return groupData(index, role, item);
    }
    else
    {
        return itemData(index, role, item);
    }

    return QVariant();
}

bool GlodonGroupModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
    {
        return false;
    }

    if (GlodonGroupModelAbstractItem *item = static_cast<GlodonGroupModelAbstractItem *>(index.internalPointer()))
    {
        if (item->type() == GlodonGroupModelAbstractItem::CXGroup)
        {
            return false;
        }
        else
        {
            GlodonGroupModelDataItem *dataItem = dynamic_cast<GlodonGroupModelDataItem *>(item);
            QModelIndex indexForData = m_model->index(dataItem->index(), index.column(), dataItem->parentIndex());
            return m_model->setData(indexForData, value, role);
        }
    }

    return false;
}

QVariant GlodonGroupModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return m_model->headerData(section, orientation, role);
}

bool GlodonGroupModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    return m_model->setHeaderData(section, orientation, value, role);
}

Qt::ItemFlags GlodonGroupModel::flags(const QModelIndex &index) const
{
    if (GlodonGroupModelAbstractItem *item = static_cast<GlodonGroupModelAbstractItem *>(index.internalPointer()))
    {
        if (item->type() == GlodonGroupModelAbstractItem::CXData)
        {
            GlodonGroupModelDataItem *dataItem = dynamic_cast<GlodonGroupModelDataItem *>(item);
            QModelIndex indexForData = m_model->index(dataItem->index(), index.column(), dataItem->parentIndex());
            return m_model->flags(indexForData);
        }
        else
        {
            return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
        }
    }

    return Qt::NoItemFlags;
}

void GlodonGroupModel::buildModel(bool reset)
{
    if (reset)
    {
        resetModel();
    }

    //QTime startBuildModel = QTime::currentTime();

    m_root->child.clear();

    if (m_groupCols.count() == 0)
    {
        for (int i = 0; i < m_model->rowCount(); i++)
        {
            GlodonGroupModelDataItem *item;
            QModelIndex index = m_model->index(i, 0);
            item = new GlodonGroupModelDataItem(index.parent(), m_root);
            item->setIndex(i);
            LoadChildModelIndex(item, index);
            m_root->child.append(item);
        }
    }
    else
    {
        int nCurrCol = m_groupCols[0];
        ValueRowInfo valueInfo;

        //QTime start = QTime::currentTime();

        for (int i = 0; i < m_model->rowCount(); i++)
        {
            QString strValue = m_model->data(m_model->index(i, nCurrCol)).toString();
            valueInfo.insertMulti(strValue, i);
        }

        QList<QString> keys = valueInfo.uniqueKeys();

        for (int i = 0; i < keys.count(); i++)
        {
            GlodonGroupModelGroupItem *item = new GlodonGroupModelGroupItem(m_root);
            QList<int> value = valueInfo.values(keys[i]);
            item->setData(m_model->headerData(nCurrCol, Qt::Horizontal).toString() + ": " + keys[i]);
            loadChildItems(1, value, item);
            m_root->child.append(item);
        }
    }

    emit modelRebuild();
}

QAbstractItemModel *GlodonGroupModel::model() const
{
    return m_model;
}

void GlodonGroupModel::setModel(QAbstractItemModel *model)
{
    if (model == m_model)
    {
        return;
    }

    m_model = model;
    buildModel(true);
}

QModelIndex GlodonGroupModel::dataIndex(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    if (GlodonGroupModelAbstractItem *item = static_cast<GlodonGroupModelAbstractItem *>(index.internalPointer()))
    {
        if (item->type() == GlodonGroupModelAbstractItem::CXGroup)
        {
            return QModelIndex();
        }
        else
        {
            GlodonGroupModelDataItem *dataItem = dynamic_cast<GlodonGroupModelDataItem *>(item);
            QModelIndex indexForData = m_model->index(dataItem->index(), index.column(), dataItem->parentIndex());
            return indexForData;
        }
    }

    return QModelIndex();
}

QModelIndex GlodonGroupModel::groupIndex(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

void GlodonGroupModel::groupChanged(QVector<int> newGroup)
{
    m_groupCols.clear();
    m_groupCols = newGroup;
    m_treeCol = 0;

    // 得到当前的树形所在的列
    for (int nCol = 0; nCol < model()->columnCount(); ++nCol)
    {
        for (int nGroup = 0; nGroup < m_groupCols.size(); ++nGroup)
        {
            if (nCol == m_groupCols.at(nGroup) && nCol <= m_treeCol)
            {
                m_treeCol++;
                break;
            }
        }
    }

    buildModel();
}

void GlodonGroupModel::onResetModel()
{
    buildModel(true);
}

void GlodonGroupModel::resetModel()
{
    m_groupCols.clear();
    m_valueInfo.clear();
}

void GlodonGroupModel::loadChildItems(int index, const QList<int> &rows, GlodonGroupModelAbstractItem *parent)
{
    if (index == m_groupCols.count())
    {
        for (int i = 0; i < rows.count(); i++)
        {
            int nRow = rows[i];
            QModelIndex dataIndex = m_model->index(nRow, 0);
            GlodonGroupModelDataItem *item = new GlodonGroupModelDataItem(dataIndex.parent(), parent);
            item->setIndex(nRow);

            if (m_model->hasChildren(dataIndex))
            {
                LoadChildModelIndex(item, dataIndex);
            }

            parent->child.append(item);
        }
    }
    else
    {
        int nCurrCol = m_groupCols[index];

        ValueRowInfo valueInfo;

        //QTime start = QTime::currentTime();
        for (int i = 0; i < rows.count(); i++)
        {
            QString strValue = m_model->data(m_model->index(rows[i], nCurrCol)).toString();
            valueInfo.insertMulti(strValue, rows[i]);
        }

        QList<QString> keys = valueInfo.uniqueKeys();

        for (int i = 0; i < keys.count(); i++)
        {
            GlodonGroupModelGroupItem *item = new GlodonGroupModelGroupItem(parent);
            QList<int> value = valueInfo.values(keys[i]);
            item->setData(m_model->headerData(nCurrCol, Qt::Horizontal).toString() + ": " + keys[i]);
            loadChildItems(index + 1, value, item);
            parent->child.append(item);
        }
    }
}

void GlodonGroupModel::LoadChildModelIndex(GlodonGroupModelAbstractItem *parentItem, QModelIndex parentIndex)
{
    for (int nRow = 0; nRow < m_model->rowCount(parentIndex); nRow++)
    {
        QModelIndex dataIndex = m_model->index(nRow, 0, parentIndex);
        GlodonGroupModelDataItem *item = new GlodonGroupModelDataItem(parentIndex, parentItem);
        item->setIndex(nRow);

        if (m_model->hasChildren(dataIndex))
        {
            LoadChildModelIndex(item, dataIndex);
        }

        parentItem->child.append(item);
    }
}

void GlodonGroupModel::initColumnValueInfo(int col)
{
    ValueRowInfo rowInfo = m_valueInfo.value(col);

    for (int row = 0; row < m_model->rowCount(); row++)
    {
        QModelIndex index = m_model->index(row, col);
        QString strData = m_model->data(index).toString();
        rowInfo.insertMulti(strData, row);
    }

    m_valueInfo.insert(col, rowInfo);
}

QVariant GlodonGroupModel::groupData(const QModelIndex &index, int role, GlodonGroupModelAbstractItem *item) const
{
    if (index.column() <= m_treeCol || role == Qt::BackgroundColorRole)
    {
        switch (role)
        {
            case Qt::TextAlignmentRole:
            {
                return Qt::AlignVCenter;
            }

            case Qt::DisplayRole:
            {
                return static_cast<GlodonGroupModelGroupItem *>(item)->data();
            }

            case Qt::BackgroundColorRole:
            {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                // todo qt5
                return QColor(Qt::gray);
#else
                return Qt::gray;
#endif
            }
        }
    }

    return QVariant();
}

QVariant GlodonGroupModel::itemData(const QModelIndex &index, int role, GlodonGroupModelAbstractItem *item) const
{
    if (role == 91)
    {
        return item->type();
    }

    GlodonGroupModelDataItem *dataItem = dynamic_cast<GlodonGroupModelDataItem *>(item);
    QModelIndex indexForData = m_model->index(dataItem->index(), index.column(), dataItem->parentIndex());
    return indexForData.data(role);
}

GlodonGroupModelAbstractItem::GlodonGroupModelAbstractItem(GlodonGroupModelAbstractItem *parentItem)
{
    parent = parentItem;
}

GlodonGroupModelAbstractItem::~GlodonGroupModelAbstractItem()
{
    child.clear();
}

GlodonGroupModelDataItem::GlodonGroupModelDataItem(QModelIndex parentIndex, GlodonGroupModelAbstractItem *parentItem)
    : GlodonGroupModelAbstractItem(parentItem)
{
    m_index = parentIndex;
}

GlodonGroupModelGroupItem::GlodonGroupModelGroupItem(GlodonGroupModelAbstractItem *parentItem)
    : GlodonGroupModelAbstractItem(parentItem)
{
}
