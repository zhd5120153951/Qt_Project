#include "GLDPaperWidgetModel.h"

#include "GLDAbstractItemModel.h"

/* GLDPaperWidgetModel */
GLDPaperWidgetModel::GLDPaperWidgetModel(QAbstractItemModel *pModel,
        int nCurPageNo, int startRow, int rowCount,
        QObject *parent)
    : GlodonAbstractItemModel(parent), m_nCurPageNo(nCurPageNo)
{
    setObjectName("GLDPaperWidgetModel");
    m_dataModel = pModel;
    m_nStartRow = startRow;
    m_nRowCountPerPage = rowCount;
}

GLDPaperWidgetModel::~GLDPaperWidgetModel()
{

}

QModelIndex GLDPaperWidgetModel::index(int row,
                                       int column,
                                       const QModelIndex &parent) const
{
    QModelIndex dataIndex = m_dataModel->index(row + m_nStartRow, column, parent);
    return createIndex(row, column, dataIndex.internalPointer());
}

QModelIndex GLDPaperWidgetModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}

int GLDPaperWidgetModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_nRowCountPerPage;
}

int GLDPaperWidgetModel::columnCount(const QModelIndex &parent) const
{
    return m_dataModel->columnCount(parent);
}

QVariant GLDPaperWidgetModel::data(const QModelIndex &index, int role) const
{
    return m_dataModel->data(dataIndex(index), role);
}

bool GLDPaperWidgetModel::setData(const QModelIndex &index,
                                  const QVariant &value,
                                  int role)
{
    bool bSuccess = m_dataModel->setData(dataIndex(index), value, role);

    if (bSuccess)
    {
        switch (role)
        {
            case gidrColWidthRole:
                emit columnWidthChanged(index, value.toInt());
                break;

            case gidrRowHeightRole:
                emit rowHeightChanged(index, value.toInt());
                break;

            default:
                break;
        }
    }

    return bSuccess;
}

Qt::ItemFlags GLDPaperWidgetModel::flags(const QModelIndex &index) const
{
    return m_dataModel->flags(dataIndex(index));
}

QModelIndex GLDPaperWidgetModel::dataIndex(const QModelIndex &index) const
{
    return m_dataModel->index(index.row() + m_nStartRow, index.column(), index.parent());
}

QVariant GLDPaperWidgetModel::headerData(int section,
        Qt::Orientation orientation,
        int role) const
{
    //当竖直方向表头不是数字时，需要特殊处理
    return m_dataModel->headerData(section, orientation, role);
}

bool GLDPaperWidgetModel::setHeaderData(int section,
                                        Qt::Orientation orientation,
                                        const QVariant &value, int role)
{
    return m_dataModel->setHeaderData(section, orientation, value, role);
}

bool GLDPaperWidgetModel::insertRows(int row,
                                     int count,
                                     const QModelIndex &parent)
{
    return m_dataModel->insertRows(row, count, dataIndex(parent));
}

bool GLDPaperWidgetModel::insertColumns(int column,
                                        int count,
                                        const QModelIndex &parent)
{
    return m_dataModel->insertColumns(column, count, dataIndex(parent));
}

bool GLDPaperWidgetModel::removeRows(int row,
                                     int count,
                                     const QModelIndex &parent)
{
    return m_dataModel->removeRows(row, count, dataIndex(parent));
}

bool GLDPaperWidgetModel::removeColumns(int column,
                                        int count,
                                        const QModelIndex &parent)
{
    return m_dataModel->removeColumns(column, count, dataIndex(parent));
}

void GLDPaperWidgetModel::setStartRow(int startRow)
{
    m_nStartRow = startRow;
}

void GLDPaperWidgetModel::setRowCountPerPage(int rowCount)
{
    m_nRowCountPerPage = rowCount;
}

int GLDPaperWidgetModel::curPageNo() const
{
    return m_nCurPageNo;
}

void GLDPaperWidgetModel::setCurPageNo(int nCurPageNo)
{
    m_nCurPageNo = nCurPageNo;
}
