#ifndef GLDFOOTERMODEL_H
#define GLDFOOTERMODEL_H

#include "GLDAbstractItemModel.h"

class GLDCOMMONSHARED_EXPORT GlodonFooterBodyModel : public GlodonAbstractItemModel
{
    Q_OBJECT

public:
    GlodonFooterBodyModel(QAbstractItemModel *model, QObject *parent = 0);
public:
    QModelIndex index(const QModelIndex &dataIndex) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                       int role = Qt::EditRole);
    void sort(int column, Qt::SortOrder order);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row,
                         int column, const QModelIndex &parent) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                              int row, int column, const QModelIndex &parent);

public:
    QModelIndex dataIndex(const QModelIndex &footerIndex) const;
    QModelIndex footerBodyIndex(const QModelIndex &dataIndex) const;
    inline int footerRowCount() const { return m_footerRowCount; }
    inline void setFooterRowCount(int value) { m_footerRowCount = value; }
private:
    QAbstractItemModel *m_model;
    int m_footerRowCount;

private Q_SLOTS:
    void onRowRemoved(const QModelIndex &parent, int first, int last);
    void onRowInserted(const QModelIndex &parent, int first, int last);
    void onRowMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void onUpdate(QModelIndex &topLeft);
    void onResetModel();
    void onDataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                      const QVector<int>&roles = QVector<int>());
};

class GLDCOMMONSHARED_EXPORT GlodonFooterModel : public GlodonAbstractItemModel
{
    Q_OBJECT

public:
    GlodonFooterModel(QAbstractItemModel *model, QObject *parent = 0);
public:
    QModelIndex index(const QModelIndex &dataIndex) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
public:
    QModelIndex dataIndex(const QModelIndex &footerIndex) const;
    QModelIndex footerIndex(const QModelIndex &dataIndex) const;
    inline int footerRowCount() const { return m_footerRowCount; }
    inline void setFooterRowCount(int value) { m_footerRowCount = value; }
private Q_SLOTS:
    void onDataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                      const QVector<int>&roles = QVector<int>());
private:
    QAbstractItemModel *m_model;
    int m_footerRowCount;

private Q_SLOTS:
    void onResetModel();
};

#endif // GLDFOOTERMODEL_H
