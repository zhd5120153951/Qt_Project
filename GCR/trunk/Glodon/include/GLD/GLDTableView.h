/*!
 *@file GLDTableView.h
 *@brief {网格编辑控件}
 *
 *@remarks {}
 *Copyright (c) 1998-2015 Glodon Corporation
 */

#pragma once

#include "GLDTableViewBasic.h"

const GString c_sTableViewQssFile = ":/qsses/GLDTableView.qss";

class GlodonTableViewPrivate;
class GlodonTreeDrawInfo;

class GLDTABLEVIEWSHARED_EXPORT GlodonTableView :  public GLDTableView
{
    Q_OBJECT

public slots:
    /*!
     *隐藏一行
     *@param[in]  row  逻辑行号
     *@return 无
     */
    virtual void hideRow(int row);
    void groupChanged(QVector<int> newGroup);

public:
    explicit GlodonTableView(QWidget *parent = 0);
    ~GlodonTableView();
    /*!
     *树形模式下，展开某一行，可指定是否发送信号
     *@param[in]  row         逻辑行号
     *@param[in]  emitSignal  是否发送信号
     *@return 无
     */
    void expand(int row, bool emitSignal);

    /*!
     *树形模式下，折叠某一行，可指定是否发送信号
     *@param[in]  row         逻辑行号
     *@param[in]  emitSignal  是否发送信号
     *@return 无
     */
    void collapse(int row, bool emitSignal);

    /*!
     *树形模式下，判断某一行是否展开
     *@param[in]  row  逻辑行号
     *@return bool
     */
    bool isRowExpanded(int row);

    /*!
     *树形模式下，展开全部节点
     *@return 无
     */
    virtual void expandAll();

    /*!
     *树形模式下，折叠全部节点
     *@return 无
     */
    virtual void collapseAll();

    /*!
     *设置根节点，通常都是QModelIndex()
     *@param[in]  index  一般为QModelIndex()
     *@return 无
     *@see 参见GlodonAbstractItemView::setRootIndex(const QModelIndex &index)
     */
    void setRootIndex(const QModelIndex &index);

    /*!
     *获取格子对应的treeModel中的index
     *@return 无
     */
    virtual QModelIndex treeIndex(const QModelIndex &index) const;

    /*!
     *获取格子对应的dataModel中的index
     *@return 无
     */
    virtual QModelIndex dataIndex(const QModelIndex &index) const;

    /*!
     *树形显示情况下，获取当前格子对应的dataModel中的index
     *@return 无
     */
    QModelIndex currentDataIndex() const;

    /**
     * @brief 设置是否树形结构显示
     *        在setModel后使用
     *        调用该方法，会导致之前设置的行高不起作用
     *        建议先调用该方法，再设置行高
     * @param value
     */
    void setIsTree(bool value);
    bool isTree() const;

    /**
     * @brief 设置是否分组模式显示
     * @param value
     */
    void setIsGroupMode(bool groupModeEnable);

    bool isGroupMode() const;

    /**
     * @brief 设置树形显示下，展开/折叠图标的样式
     * @param style
     */
    void setTreeDecorationStyle(TreeDecorationStyle style);
    TreeDecorationStyle treeDecorationStyle() const;

    void setTreeDrawInfo(GlodonTreeDrawInfo *tableViewDrawInfo);
    GlodonTreeDrawInfo *treeDrawInfo();

    /**
     * @brief 在树形结构时，选择折叠的父，是否同时选择子
     * @return
     */
    bool isAddChildToSelection();
    void setAddChildToSelection(bool value);

    /**
     * @brief 设置树形结构所在的列号
     * @param column
     */
    void setTreeColumn(int column);
    int treeColumn();

protected slots:
    void reBuildTree();

    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight,
                             const QVector<int> &roles);

protected:
    explicit GlodonTableView(GlodonTableViewPrivate &dd, QWidget *parent = 0);

    void doSetIsTree(bool value);
    virtual void doSetModel(QAbstractItemModel *model);

    /*!
     * \brief cellShowTreeWidthOffset
     * \return 为了显示树形的格子的文字区将进行的水平方向的偏移(主要是树干的细线和branch图标占去的宽度)
     */
    int treeCellDisplayHorizontalOffset(int row, int col, bool isOldMinTextHeightCalWay = true) const;

    virtual void onBoolCellPress(QMouseEvent *event);
    virtual void onMousePress(QMouseEvent *event);
    virtual bool event(QEvent *event);

    virtual QAbstractItemModel* itemModel();

private:
    /**
     * @brief 如果当前节点为折叠状态，那么当选择它时，把它所有的子都增加到selection中
     * @param topLeft
     * @param bottomLeft
     * @param selection 当前需要添加到selectionModel中的selection
     * @param isRowSelect 如果是行选状态，则不需要判断该行是否处于折叠
     */
    void setChildSelection(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                            QItemSelection &selection, bool isRowSelect);
											
    void doGMExpand(GLDEvent *event);

    void doGMQueryExpand(GLDEvent *event);

    void doGMCollapse(GLDEvent *event);

    void doGMQueryCollapse(GLDEvent *event);

    void doGMExpandAll(GLDEvent *event);

    void doGMQueryExpandAll(GLDEvent *event);

    void doGMCollapseAll(GLDEvent *event);

    void doGMQueryCollapseAll(GLDEvent *event);

private:
    Q_DECLARE_PRIVATE(GlodonTableView)
    Q_DISABLE_COPY(GlodonTableView)
};
