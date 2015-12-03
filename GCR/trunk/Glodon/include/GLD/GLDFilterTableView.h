/*!
 *@file glodonfiltertableview.h
 *@brief {提供筛选功能的Grid}
 *
 *@author Gaosy
 *@date 2012.9.7
 *@remarks {remarks}
 *Copyright (c) 1998-2012 Glodon Corporation
 */

#ifndef GLDFILTERTABLEVIEW_H
#define GLDFILTERTABLEVIEW_H

#include "GLDTableView.h"
#include "GLDFilterView.h"
#include "GLDGlobal.h"

class GlodonFilterTableViewPrivate;

/*!
 *@class: GlodonFilterTableView
 *@brief {提供筛选行的Grid，通过对筛选行设置筛选方式，可自定义筛选方式，默认为ComboBox}
 *@author Gaosy
 *@date 2012.9.10
 */
class GLDTABLEVIEWSHARED_EXPORT GlodonFilterTableView : public GlodonTableView
{
    Q_OBJECT

public:
    /*!
        *GlodonFilterTableView构造方法
        *@param[in]  parent  父widget
        *@return 无
        */
    GlodonFilterTableView(QWidget *parent);

    /*!
        *为GlodonFilterTableView设置model,用于数据显示和编辑，同时为filter（筛选行）设置model
        *@param[in]  model
        *@return 无
        */
    void setModel(QAbstractItemModel *model);

    /*!
        *GlodonFilterTableView的绘制方法，调用父类GlodonTableView的绘制方法
        *@param[in]  e
        *@return 无
        *@see 参见GlodonTableView::paintEvent(QPaintEvent *e)
        */
    void paintEvent(QPaintEvent *e);

    /*!
        *设置水平表头，将filter（筛选行）与表头的resize、moveSection信号连接
        *@param[in]  header  水平表头
        *@return 无
        */
    void setHorizontalHeader(GlodonHeaderView *header);

    /*!
        *设置是否显示filter（筛选行）
        *@param[in]  hide  true为隐藏filter（筛选行），false为显示filter（筛选行）
        *@return 无
        *@warning 如果使用的是GSPDataSource，必须在GSPDataSource被setActive后，再设置过滤行可见
        */
    void setIsDisplayFilter(bool show);
    /**
        * @brief 是否显示过滤行
        * @return
        */
    bool isDisplayFilter();

    /*!
        *设置GlodonFilterTableView中水平表头、竖直表头、表体、筛选行的位置，
        *将筛选行置于水平表头和表体之间
        *@return 无
        */
    void updateGeometries();

    /*!
        *设置固定可编辑列数
        *@param[in]  fixedColCount  固定可编辑列数
        *@return 无
        */
    void setFixedColCount(int fixedColCount);

    //筛选行
    GFilterView *filter();
    void setGridColor(QColor value);
    void setGridLineColor(QColor value);

public Q_SLOTS:
    /*!
        *处理默认情况下的筛选
        *@param[in]  column  The memory area to copy from.
        *@return 无
        */
    void changeViewItemsForFilter(int column);

protected Q_SLOTS:
    void columnResized(int column, int oldWidth, int newWidth, bool isManual = false);

Q_SIGNALS:
    bool onFilterData(int row);

protected:
    GlodonFilterTableView(GlodonFilterTableViewPrivate &dd, QWidget *parent);
    void timerEvent(QTimerEvent *event);
    void resetModel(QAbstractItemModel *dataModel);

    virtual void resetEllipsisButtonLocation();

    bool isFilterHide();

protected:
    // 重构updateGeometries
    void updateFilterViewGeometry(int nFilterViewHeight);
    void updateHorizontalHeaderGeometry(int nHorizontalHeaderHeight, int nFilterViewHeight);

private:
    bool m_bDisplayFilter;
    Q_DECLARE_PRIVATE(GlodonFilterTableView)
};

#endif // GLDFILTERTABLEVIEW_H
