#ifndef GLDCXMODEL_H
#define GLDCXMODEL_H

#include <QHash>
#include <QAbstractItemModel>

#include "GLDObjectList.h"

class GlodonGroupModelAbstractItem;

typedef QHash<QString, int> ValueRowInfo;
typedef QHash<int, ValueRowInfo> ColumnValueInfo;
typedef QList<GlodonGroupModelAbstractItem *> GlodonCXModelItemList;

class GLDCOMMONSHARED_EXPORT GlodonGroupModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit GlodonGroupModel(QAbstractItemModel *pModel, QObject *parent = 0);
    ~GlodonGroupModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void buildModel(bool reset = false);
    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *model);

    QModelIndex dataIndex(const QModelIndex &index) const;
    QModelIndex groupIndex(const QModelIndex &index) const;

public Q_SLOTS:
    void groupChanged(QVector<int> newGroup);
    void onResetModel();
Q_SIGNALS:
    void modelRebuild();

private:
    QVector<int> m_groupCols;
    QAbstractItemModel *m_model;
    ColumnValueInfo m_valueInfo;
    GlodonGroupModelAbstractItem *m_root;
	int m_treeCol;

private:
    void resetModel();
    void loadChildItems(int index, const QList<int> &rows, GlodonGroupModelAbstractItem *parent);
    void LoadChildModelIndex(GlodonGroupModelAbstractItem *parentItem, QModelIndex parentIndex);
    void initColumnValueInfo(int col);
    QVariant groupData(const QModelIndex &index, int role, GlodonGroupModelAbstractItem *item) const;
    QVariant itemData(const QModelIndex &index, int role, GlodonGroupModelAbstractItem *item) const;
};

class GLDCOMMONSHARED_EXPORT GlodonGroupModelAbstractItem
{
public:
    enum CXType
    {
        CXGroup = 0,
        CXData = 1
    };

public:
    GlodonGroupModelAbstractItem(GlodonGroupModelAbstractItem *parentItem = 0);
    virtual ~GlodonGroupModelAbstractItem();
    virtual CXType type() const = 0;

public:
    GlodonGroupModelAbstractItem *parent;
    GObjectList<GlodonGroupModelAbstractItem *> child;
};

class GLDCOMMONSHARED_EXPORT GlodonGroupModelGroupItem : public GlodonGroupModelAbstractItem
{
public:
    GlodonGroupModelGroupItem(GlodonGroupModelAbstractItem *parentItem = 0);

    CXType type() const { return GlodonGroupModelAbstractItem::CXGroup; }

    inline QString data() { return m_data; }
    inline void setData(QString value) { m_data = value; }

private:
    QString m_data;
};

class GLDCOMMONSHARED_EXPORT GlodonGroupModelDataItem : public GlodonGroupModelAbstractItem
{
public:
    GlodonGroupModelDataItem(QModelIndex parentIndex = QModelIndex(), GlodonGroupModelAbstractItem *parentItem = 0);

    CXType type() const { return GlodonGroupModelAbstractItem::CXData; }

    inline void setIndex(int rowNo) { m_row = rowNo; }
    inline int index() const { return m_row; }
    inline void setPIndex(QModelIndex index) { m_index = index; }
    inline QModelIndex parentIndex() const { return m_index; }

private:
    int m_row;
    QModelIndex m_index;
};

#endif // GLDCXMODEL_H
