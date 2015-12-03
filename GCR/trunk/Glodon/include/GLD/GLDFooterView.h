/*!
 *@file gfooterview.h
 *@brief {合计行}
 *固定的显示在TableView的下方
 *@author Gaosy
 *@date 2012.9.7
 *@remarks {支持多行，可进行合并，需要单独setModel}
 *Copyright (c) 1998-2012 Glodon Corporation
 */

#ifndef GLDFOOTERVIEW_H
#define GLDFOOTERVIEW_H

#include "GLDTableView.h"

class GFooterViewPrivate;

/*!   
 *@class: GFooterView
 *@brief {合计行，为Grid提供合计行功能，支持多行和合并，需要额外setModel}
 *@author Gaosy 
 *@date 2012.9.7
 */
class GLDTABLEVIEWSHARED_EXPORT GFooterView : public GlodonTableView
{
    Q_OBJECT

public:
    /*!
     *GFooterView构造方法
     *@param[in]  parent  父widget
     *@return 无
     */
    GFooterView(QWidget *parent);

    /*!
     *为GFooterView设置model,用于合计行的数据显示和编辑，与所处的TableView表体数据独立
     *@param[in]  model  
     *@return 无
     */
    void setModel(QAbstractItemModel *model);

    /*!
     *获取控件的高度
     *@return int
     */
    int height() const;
    void paintEvent(QPaintEvent *e);
    void resetCurrentIndex(const QModelIndex &current);

    void setVerticalHeaderDrawWidth(int width);

Q_SIGNALS:
    void currentIndexChanged(const QModelIndex &current);

public Q_SLOTS:
    /*!
     *用于同步resize动作
     *@param[in]  logicalIndex  resize动作发生的逻辑列（Model中的列）
     *@param[in]  oldSize       resize前该列的宽度
     *@param[in]  size          resize后该列的宽度
     *@return 无
     */
    void resizeSection(int logicalIndex, int oldSize,int size);
    /*!
     *用于同步列移动动作
     *@param[in]  logicalIndex  列移动动作发生的逻辑列（Model中的列）
     *@param[in]  oldSize       列移动前该列所处的位置
     *@param[in]  size          列移动后该列所处的位置
     *@return 无
     */
    void moveSection(int logicalIndex, int from, int to);
    void setOffset(int offset);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
private:
    Q_DECLARE_PRIVATE(GFooterView)
};

/**
 * @brief FooterView的Delegate, 不能编辑
 */
class GLDTABLEVIEWSHARED_EXPORT GFooterViewDelegate : public GlodonDefaultItemDelegate
{
public:
    explicit GFooterViewDelegate(QObject *parent = 0);
    GEditStyle editStyle(const QModelIndex &index, bool &readOnly) const;
};

#endif // GLDFOOTERVIEW_H

