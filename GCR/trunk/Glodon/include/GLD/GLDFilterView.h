/*!
 *@file gfilterview.h
 *@brief {筛选行}
 *用于筛选表格中的数据
 *@author Gaosy
 *@date 2012.9.7
 *@remarks {位置在水平表头之下，目前仅支持单行}
 *Copyright (c) 1998-2012 Glodon Corporation
 */

#ifndef GLDFILTERVIEW_H
#define GLDFILTERVIEW_H

#include "GLDHeaderView.h"
#include <QComboBox>

class GFilterViewPrivate;

/*!   
 *@class: GFilterView
 *@brief {为Grid提供筛选行的功能，可自定义筛选方式，默认为ComboBox方式}
 *@author Gaosy 
 *@date 2012.9.7
 */
class GLDTABLEVIEWSHARED_EXPORT GFilterView : public GlodonHeaderView
{
    Q_OBJECT
public:
    /*!
     *GFilterView构造方法
     *@param[in]  parent  父widget
     *@return 无
     */
    GFilterView(QWidget *parent);

    /*!
     *获取logicalIndex这一列的筛选控件的内容
     *@param[in]  logicalIndex  需要获取筛选控件内容的逻辑列号（Model中的列号）
     *@return QString 
     */
    QString filterData(int logicalIndex);

    /*!
     *为GFilterView设置model，仅仅是为了控制显示的列数，没有其他意义
     *@param[in]  model  
     *@return 无
     */
    void setModel(QAbstractItemModel *model);

    /*!
     *使GFilterView获取TableView中使用的Model，用于默认筛选方式的建立
     *@param[in]  filterModel  TableView中使用的Model
     *@return 无
     */
    void setFilterDataModel(QAbstractItemModel *filterModel);

    /*!
     *获取index所在格子的大小
     *@param[in]  index  
     *@return QRect
     */
    QRect visualRect(const QModelIndex &index) const;

    /*!
     *GFilterView的绘制方法
     *@param[in]  e  
     *@return 无
     */
    void paintEvent(QPaintEvent *e);

    /*!
     *获取控件的高度
     *@return int
     */
    int height() const;

public:
    //TableView中的数据Model，用于Filter的筛选行初始化
    QAbstractItemModel *m_pFilterDataModel;
    void resetFilter(int col);
public Q_SLOTS:
    /*!
     *用于同步resize动作
     *@param[in]  logicalIndex  resize动作发生的逻辑列（Model中的列）
     *@param[in]  oldSize       resize前该列的宽度
     *@param[in]  size          resize后该列的宽度
     *@return 无
     */
    void resizeSection(int logicalIndex, int oldSize, int size);

    /*!
     *用于同步列移动动作
     *@param[in]  logicalIndex  列移动动作发生的逻辑列（Model中的列）
     *@param[in]  oldSize       列移动前该列所处的位置
     *@param[in]  size          列移动后该列所处的位置
     *@return 无
     */
    void moveSection(int logicalIndex, int from, int to);

    /*!
     *处理筛选控件的值的改变
     *@return 无
     */
    void filterChanged();

    /*!
     *设置应为隐藏状态的子widget为隐藏状态
     *@return 无
     */
    void hideHiddenSection(int hbarPos);

    void filterDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void initFilterWidget();

Q_SIGNALS:
    void onFilterChanged(int column);
    QWidget* onQueryFilterWidget(int logicalIndex);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    void paintSection(int beginToHideIndex) const;
    void keyPressEvent(QKeyEvent *event);
    mutable bool m_bHideFlag;
private:
    Q_DECLARE_PRIVATE(GFilterView)
};

#endif // GLDFILTERVIEW_H
