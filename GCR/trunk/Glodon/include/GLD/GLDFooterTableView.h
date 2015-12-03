/*!
 *@file glodonfootertableview.h
 *@brief {带有筛选行及合计行功能的Grid}
 *
 *@author Gaosy
 *@date 2012.9.7
 *@remarks {remarks}
 *Copyright (c) 1998-2012 Glodon Corporation
 */

#ifndef GLDFOOTERTABLEVIEW_H
#define GLDFOOTERTABLEVIEW_H

#include "GLDFilterTableView.h"
#include "GLDFooterView.h"
#include "GLDFooterModel.h"

class GlodonFooterTableViewPrivate;

/*!   
 *@class: GlodonFooterGrid
 *@brief {带有筛选行及合计行功能的Grid，筛选行可以隐藏，合计行需要单独setModel}
 *@author Gaosy 
 *@date 2012.9.10
 */
class GLDTABLEVIEWSHARED_EXPORT GlodonFooterTableView : public GlodonFilterTableView
{
    Q_OBJECT

public:
    /*!
     *GlodonFooterGrid构造方法
     *@param[in]  parent  父widget
     *@return 无
     */
    GlodonFooterTableView(QWidget *parent);

    /*!
     *设置GlodonFooterGrid中水平表头、竖直表头、表体、筛选行、合计行的位置，
     *将合计行置于表体的下方
     *@return 无
     */
    void updateGeometries();

    /*!
     *为合计行设置model
     *@param[in]  model  合计行显示和编辑需要的model
     *@return 无
     */
    void setFooterModel(QAbstractItemModel *model);

    /*!
     *设置水平表头，将footer（合计行）与表头的resize、moveSection信号连接
     *@param[int]  header  水平表头
     *@return 无
     */
    void setHorizontalHeader(GlodonHeaderView *header);
    void setModel(QAbstractItemModel *model);

    bool totalRowAtFooter() const;
    void setTotalRowAtFooter(bool value);

    void setTotalRowCount(int value);
    void paintEvent(QPaintEvent *e);
    void setGridColor(QColor value);
    void setGridLineColor(QColor value);
    void setFixedColCount(int fixedColCount);
public:
    //合计行
    GFooterView *footer();

protected Q_SLOTS:
    void beforeReset();
    void doReset();
    void afterReset();

protected:
    GlodonFooterTableView(GlodonFooterTableViewPrivate &dd, QWidget *parent);
    void resetModel();
    void createModel();

protected:
    void updateFooterGeometry(int nVerticalHeaderWidth);

private:
    Q_DECLARE_PRIVATE(GlodonFooterTableView)

    void changeVerticalHeaderDrawWidth();

protected Q_SLOTS:
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void columnResized(int column, int oldWidth, int newWidth, bool isManual = false);
    void setCurrentIndex(const QModelIndex &dataIndex);
    void setCurrentIndexForFooter(const QModelIndex &current);
};

#endif // GLDFOOTERTABLEVIEW_H
