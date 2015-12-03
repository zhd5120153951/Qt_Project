/*!
 *@file glodontreemodel.h
 *@brief {专门为树形结构显示时使用的中间态Model}
 *
 *@author gaosy-a
 *@date 2012.9.7
 *@remarks {remarks}
 *Copyright (c) 1998-2012 Glodon Corporation
 */

#ifndef GLDTREEMODEL_H
#define GLDTREEMODEL_H

#include <QAbstractItemModel>
#include <QMimeData>
#include "GLDTreeDrawInfo.h"
#include "GLDAbstractItemModel.h"
#include "GLDGlobal.h"

/*!   
 *@class: GlodonTreeModel
 *@brief {专门为TableView的树形结构显示而使用的中间态Model，将含有树形关系的Model转化去掉树形关系使用}
 *@author Gaosy 
 *@date 2012.9.10
 */
class GLDTABLEVIEWSHARED_EXPORT GlodonTreeModel : public GlodonAbstractItemModel
{
    Q_OBJECT
public:
    /*!
     *GlodonTreeModel的构造方法，将原始含有父子结构的model的父子结构去掉
     *@param[in]  pModel  含有父子结构的原始model
     *@param[in]  parent
     *@return 无
     */
    explicit GlodonTreeModel(QAbstractItemModel *pModel, QObject *parent = 0);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles);

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                       int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                              int row, int column, const QModelIndex &parent);
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row,
                         int column, const QModelIndex &parent) const;
    QModelIndex dataIndex(int row, int column) const;
    QModelIndex dataIndex(const QModelIndex &index) const;

    QModelIndex treeIndex(const QModelIndex &dataIndex) const;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

public:
    //存储树结构对应的信息，即每一行的level、是否有子节点、是否有父节点、是否展开、是否显示、是否有兄弟
    GlodonTreeDrawInfo *drawInfo;

    //原始含有父子结构的model
    QAbstractItemModel *model;

    //统一的根节点
    QModelIndex root;

private Q_SLOTS:
    void onRowRemoved(const QModelIndex &parent, int first, int last);
    void onRowInserted(const QModelIndex &parent, int first, int last);
    void onRowMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void onUpdate(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                  const QVector<int>&roles = QVector<int>());
    void onResetModel();

    int visualRowNo(const QModelIndex &parent, int row) const;
};

#endif // GLDTREEMODEL_H
