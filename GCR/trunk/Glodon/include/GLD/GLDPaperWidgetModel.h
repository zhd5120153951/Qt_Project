/*!
 *@file GLDPaperWidgetModel.h
 *@brief {}
 *@author duanb
 *@date 2013.12.31
 *@remarks {}
 *Copyright (c) 1998-2013 Glodon Corporation
 */
#ifndef GLDPAPERWIDGETMODEL_H
#define GLDPAPERWIDGETMODEL_H

#include "GLDAbstractItemModel.h"
#include "GLDTableView_Global.h"

class GLDTABLEVIEWSHARED_EXPORT GLDPaperWidgetModel : public GlodonAbstractItemModel
{
    Q_OBJECT
public:
    explicit GLDPaperWidgetModel(QAbstractItemModel *pModel, int nCurPageNo, int startRow, int rowCount, QObject *parent = 0);
    ~GLDPaperWidgetModel();

public:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex dataIndex(const QModelIndex &index) const;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                       int role = Qt::EditRole);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());

    void setStartRow(int startRow);

    void setRowCountPerPage(int rowCount);

    int curPageNo() const;
    void setCurPageNo(int nCurPageNo);

Q_SIGNALS:
    void rowHeightChanged(const QModelIndex &index,const int &value);
    void columnWidthChanged(const QModelIndex &index,const int &value);

private:
    QAbstractItemModel *m_dataModel;
    int m_nStartRow;
    int m_nRowCountPerPage;
    int m_nCurPageNo;    // ´Ó0¿ªÊ¼
};

#endif // GLDPAPERWIDGETMODEL_H
