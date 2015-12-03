/*!
 *@file glodontreedrawinfo.h
 *@brief {辅助绘制树形结构}
 *
 *@author Gaosy
 *@date 2012.9.7
 *@remarks {remarks}
 *Copyright (c) 1998-2012 Glodon Corporation
 */

#ifndef GLDTREEDRAWINFO_H
#define GLDTREEDRAWINFO_H

#include <QVector>
#include <QAbstractItemModel>
#include "GLDHeaderView.h"
#include "GLDIntList.h"

/*!   
 *@struct: TreeViewItem
 *@brief {树形结构的一个节点，存储了这一行的所有树形结构相关信息，用于树形显示}
 *@author Gaosy 
 *@date 2012.9.10
 */
struct GTreeViewItem
{
    GTreeViewItem() : parentIndex(-1), expanded(true), hasChildren(false),
        hasMoreSiblings(false), childCount(0), hidden(0), treeLevel(0), modelIndex(QModelIndex()) {}

    int parentIndex; // parent item index in viewItems
    uint expanded : 1;
    uint hasChildren : 1; // if the item has visible children (even if collapsed)
    uint hasMoreSiblings : 1;
    uint childCount : 28; // total number of children
    uint hidden : 1;
    uint treeLevel : 16;
    QModelIndex modelIndex;
};

/*!   
 *@class: GlodonTreeDrawInfo
 *@brief {辅助绘制TableView的树形结构，记录了树形结构的全部信息}
 *@author Gaosy 
 *@date 2012.9.10
 */
class GLDTABLEVIEWSHARED_EXPORT GlodonTreeDrawInfo : public QObject
{
public:
    GlodonTreeDrawInfo(QObject *parent) : QObject(parent)
    {
        m_treeColumn = 0;
        m_header = NULL;
        model = NULL;
    }
    ~GlodonTreeDrawInfo();

    /*!
     *从model获取树形结构的全部数据的行数
     *@return int
     */
    int modelSectionCount() const;

    /*!
     *通过TableView中的数据model来初始化树形结构
     *@param[in]  newModel  TableView中的数据model
     *@return 无
     */
    void setModel(QAbstractItemModel *newModel);

    /*!
     *根据给定的index获取其对应的行号
     *@param[in]  index
     *@return int
     */
    int rowNo(QModelIndex index);
    /**
     * @brief 设置哪列是树形结显示列
     * @param value
     */
    void setTreeColumn(int value);
    int treeColumn();

    /**
     * @brief 传入一个界面上显示的Row，得到该节点最后一个属于它的节点的行
     * @param parentRow
     * @return
     */
    int lastChildRow(int parentRow);

    void reset();

    void calculateRollOut();
    /**
     * @brief 传入一个界面上显示的row，得到该节点的根节点
     * @param row
     * @return
     */
    int calculateRoot(int row);

public:

    //树形结构需要的所有节点的集合
    QVector<GTreeViewItem> m_viewItems;

    //TableView的竖直表头，用于树形结构的展开（折叠）动作发生时
    //显示（隐藏）被展开（折叠）的子行
    GlodonHeaderView *m_header;

    //TableView中的数据model
    QAbstractItemModel *model;

    void doExpandItem(int row, QSet<int> &oExpandRows);
    int doCollapseItem(int row, QSet<int> &oCollapseRows);

public Q_SLOTS:

    /*!
     *为parent行加入一个子行，并返回新插入行所在的行号
     *@param[in]  parent  
     *@return int
     */
    int insertViewItem(int parent, QModelIndex index);

    /*!
     *删除一行
     *@param[in]  row  要删除的行
     *@return 无
     */
    void removeViewItem(int row);

    /*!
     *移动一（多）行
     *@param[in]  parent             要移动的行的父节点行号
     *@param[in]  first              要移动的行在其父节点下的起始行号
     *@param[in]  last               要移动的行在其父节点下的末尾行号
     *@param[in]  destinationParent  移动目的地的父节点行号
     *@param[in]  row                移动目的地在其父节点下的位置
     *@return 无
     */
    void moveViewItem(int parent, int first, int last, int destinationParent, int row);

    /*!
     *展开一行
     *@param[in]  row
     *@return 无
     */
    void expandItem(int row);

    /*!
     *折叠一行
     *@param[in]  row  
     *@return 无
     */
    int collapseItem(int row);

    /*!
     *展开全部节点
     *@return 无
     */
    void expandAll();

    /*!
     *折叠全部节点
     *@return 无
     */
    void collapseAll();

    /*!
     *根据model中的父子关系，初始化整个树形结构
     *@param[in]  expandAll  默认全部展开
     *@return 无
     */
    void init(bool expandAllLevel = true);

private:
    void setChildItemsExpand(QSet<int> &oExpandRows, int row);

    /**
     * @brief 把树形结构前序遍历转储为线性结构
     */
    void transformTreeByLevel(int &index, const QModelIndex &parent, int level);
    void transformModelFromTreeToLine();

    void refreshModelIndexAfterMove(int parent, int row);
    void refreshModelIndexAfterInsertOrRemove(int row, int parent, bool insert);
    void refreshChildModelIndex(int parent);

    // Todo: 前序遍历和treeBuild应该合并
    void buildTree(bool shouldExpandAllLevel, bool buildHeaderLater = false);
    void switchNodeExpandState(
            bool shouldExpandAllLevel, bool buildHeaderLater, int i,
            QSet<int> &hiddenSections, QSet<int> &visibleSections);

    int setChildItemsCollapse(int row, QSet<int> &oCollapseRows);

    /**
     * @brief 递归获取所有行
     * @param index
     * @return 所有行，包括子节点的行
     */
    int childRowCount(QModelIndex index  = QModelIndex()) const;

private:
    int m_treeColumn;//显示树形结构的列
};

#endif // GLDTREEDRAWINFO_H
