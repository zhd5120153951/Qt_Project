#include <QStack>
#include <QDebug>
#include "GLDGlobal.h"
#include "GLDTreeDrawInfo.h"
#include "GLDAbstractItemModel.h"

void GlodonTreeDrawInfo::setModel(QAbstractItemModel *newModel)
{
    model = newModel;
    init();
}

int GlodonTreeDrawInfo::rowNo(QModelIndex index)
{
    if (!index.isValid())
    {
        return -1;
    }

    const int nCount = m_viewItems.count();
    for (int i = 0; i < nCount; ++i)
    {
        if (m_viewItems.at(i).modelIndex == index)
        {
            return i;
        }
    }

    return -2;
}

void GlodonTreeDrawInfo::setTreeColumn(int value)
{
    m_treeColumn = value;
}

int GlodonTreeDrawInfo::treeColumn()
{
    return m_treeColumn;
}

int GlodonTreeDrawInfo::lastChildRow(int parentRow)
{
    int nRow = parentRow;

    const int nCount = m_viewItems.count();
    do
    {
        ++nRow;
    }
    while ((nRow < nCount) && (m_viewItems.at(nRow).treeLevel > m_viewItems.at(parentRow).treeLevel));

    return nRow - 1;
}

void GlodonTreeDrawInfo::reset()
{
    m_viewItems.clear();
    m_viewItems.resize(modelSectionCount());

    transformModelFromTreeToLine();
    buildTree(true);
}

GlodonTreeDrawInfo::~GlodonTreeDrawInfo()
{
}

int GlodonTreeDrawInfo::modelSectionCount() const
{
    return model->rowCount() + childRowCount();
}

void GlodonTreeDrawInfo::init(bool expandAllLevel)
{
    int nOldCount = m_viewItems.count();
    int nNewCount = modelSectionCount();

    if (m_header != NULL && nNewCount < nOldCount)
    {
        m_header->resetForGroup();
    }

    m_viewItems.clear();
    m_viewItems.resize(nNewCount);

    transformModelFromTreeToLine();

    buildTree(expandAllLevel, nNewCount >= nOldCount && !expandAllLevel);

    if (!expandAllLevel)
    {
        QVector<int> hiddenNodeIndexs;

        const int nCount = m_viewItems.count();
        for (int i = 0; i < nCount; ++i)
        {
            if (0 == m_viewItems.at(i).hidden)
            {
                hiddenNodeIndexs.append(i);
            }
        }

        if (m_header != NULL)
        {
            m_header->resetAfterTreeBuild(hiddenNodeIndexs);
        }
    }

    if (m_header != NULL)
    {
        m_header->viewport()->update();
    }
}

void GlodonTreeDrawInfo::expandAll()
{
    const int nCount = m_viewItems.count();
    for (int row = 0; row < nCount; ++row)
    {
        if (!m_viewItems.at(row).expanded)
        {
            expandItem(row);
        }
    }
}

void GlodonTreeDrawInfo::collapseAll()
{
    const int nCount = m_viewItems.count();
    for (int row = 0; row < nCount; ++row)
    {
        if (m_viewItems.at(row).expanded)
        {
            collapseItem(row);
        }
    }
}

int GlodonTreeDrawInfo::childRowCount(QModelIndex parentIndex) const
{
    int nSumCount = 0;

    int nChildRowCount = model->rowCount(parentIndex);

    for (int i = 0; i < nChildRowCount; ++i)
    {
        QModelIndex childIndex = model->index(i, m_treeColumn, parentIndex);

        nSumCount += model->rowCount(childIndex) + childRowCount(childIndex);
    }

    return nSumCount;
}

void GlodonTreeDrawInfo::transformModelFromTreeToLine()
{
    QModelIndex rootIndex;

    int nIndex = 0;
    int nModelSectionCount = modelSectionCount();

    while (nIndex < nModelSectionCount)
    {
        transformTreeByLevel(nIndex, rootIndex, 0);
    }
}

void GlodonTreeDrawInfo::transformTreeByLevel(int &nIndex, const QModelIndex &parentIndex, int sectionLevel)
{
    int nChildRowCount = model->rowCount(parentIndex);

    for (int i = 0; i < nChildRowCount; i++)
    {
        QModelIndex index = model->index(i, m_treeColumn, parentIndex);

        m_viewItems[nIndex].treeLevel = sectionLevel;
        m_viewItems[nIndex].modelIndex = index;

        ++nIndex;

        if (model->rowCount(index) > 0)
        {
            transformTreeByLevel(nIndex, index, sectionLevel + 1);
        }
    }
}

void GlodonTreeDrawInfo::switchNodeExpandState(
        bool buildHeaderLater, bool shouldExpandAllLevel, int i,
        QSet<int> &hiddenSections, QSet<int> &visibleSections)
{
    if (shouldExpandAllLevel)
    {
        if (m_header != NULL && !buildHeaderLater)
        {
            visibleSections.insert(i);
        }

        m_viewItems[i].expanded = true;
    }
    else
    {
        m_viewItems[i].expanded = false;

        if (m_viewItems.at(i).treeLevel == 0)
        {
            if (m_header != NULL && !buildHeaderLater)
            {
                visibleSections.insert(i);
            }
        }
        else
        {
            if (m_header != NULL && !buildHeaderLater)
            {
                hiddenSections.insert(i);
            }

            m_viewItems[i].hidden = true;
        }
    }
}

void GlodonTreeDrawInfo::buildTree(bool shouldExpandAllLevel, bool buildHeaderLater)
{
    if (m_viewItems.count() <= 1)
    {
        return;
    }

    QSet<int> oHiddenSections;
    QSet<int> oVisibleSections;

    QStack<int> parentStack;

    const int nCount = m_viewItems.count();
    for (int i = 0; i < nCount - 1; ++i)
    {
        Q_ASSERT(m_viewItems.at(i + 1).treeLevel <= (m_viewItems.at(i).treeLevel + 1));

        switchNodeExpandState(buildHeaderLater, shouldExpandAllLevel, i, oHiddenSections, oVisibleSections);

        if (parentStack.count() == 0)
        {
            m_viewItems[i].parentIndex = -1;
        }
        else
        {
            m_viewItems[i].parentIndex = parentStack.last();
        }

        if (m_viewItems.at(i).treeLevel < m_viewItems.at(i + 1).treeLevel)
        {
            m_viewItems[i].hasChildren = true;

            parentStack.push_back(i);
        }
        else if (m_viewItems.at(i).treeLevel == m_viewItems.at(i + 1).treeLevel)
        {
            m_viewItems[i].hasMoreSiblings = true;
        }
        else
        {
            int nindex = parentStack.last();

            while (true)
            {
                if (m_viewItems.at(nindex).treeLevel == m_viewItems.at(i + 1).treeLevel)
                {
                    m_viewItems[nindex].hasMoreSiblings = true;
                    m_viewItems[nindex].childCount = i - nindex;
                    parentStack.pop_back();
                    break;
                }
                else if (m_viewItems.at(nindex).treeLevel > m_viewItems.at(i + 1).treeLevel)
                {
                    m_viewItems[nindex].hasMoreSiblings = false;
                    m_viewItems[nindex].childCount = i - nindex;
                    parentStack.pop_back();
                    nindex = parentStack.last();
                }
            }
        }
    }

    int nlast = m_viewItems.count() - 1;

    m_viewItems[nlast].expanded = shouldExpandAllLevel;

    if (parentStack.count() == 0)
    {
        m_viewItems[nlast].parentIndex = -1;
    }
    else
    {
        m_viewItems[nlast].parentIndex = parentStack.last();

        if (shouldExpandAllLevel)
        {
            switchNodeExpandState(buildHeaderLater, shouldExpandAllLevel, nlast, oHiddenSections, oVisibleSections);
        }
        else
        {
            m_viewItems[nlast].hidden = true;

            if (m_header != NULL && !buildHeaderLater)
            {
                oHiddenSections.insert(nlast);
            }
        }

        while (parentStack.count() != 0)
        {
            m_viewItems[parentStack.last()].childCount = nlast - parentStack.last();
            parentStack.pop_back();
        }
    }

    m_header->batchSetSectionHidden(oHiddenSections, true);
    m_header->batchSetSectionHidden(oVisibleSections, false);
}

void GlodonTreeDrawInfo::doExpandItem(int row, QSet<int> &oExpandRows)
{
    QVector<int> vecIndexRecord;
    int nItem = row;
    while (true)
    {
        vecIndexRecord.append(nItem);
        nItem = m_viewItems.at(nItem).parentIndex;
        if (-1 == nItem || m_viewItems.at(nItem).expanded)
        {
            break;
        }
    }

    for (int nTemp = vecIndexRecord.count() - 1; nTemp >= 0; --nTemp)
    {
        m_viewItems[vecIndexRecord[nTemp]].expanded = true;
        if (m_viewItems.at(vecIndexRecord[nTemp]).hasChildren)
        {
            // TODO，wangdd-a：觉得不应该同步展开子，递归遍历中会设置Section的显示隐藏，影响效率
            setChildItemsExpand(oExpandRows, vecIndexRecord[nTemp]);
        }
    }
}

void GlodonTreeDrawInfo::expandItem(int row)
{
    QSet<int> oExpandRows;

    doExpandItem(row, oExpandRows);

    if (m_header != NULL)
    {
        m_header->batchSetSectionHidden(oExpandRows, false);
    }
}

int GlodonTreeDrawInfo::doCollapseItem(int row, QSet<int> &oCollapseRows)
{
    m_viewItems[row].expanded = false;
    int result = row + 1;

    if (m_viewItems.at(row).hasChildren)
    {
        // TODO，wangdd-a：觉得不应该同步折叠子，递归遍历中会设置Section的显示隐藏，影响效率
        result = setChildItemsCollapse(row, oCollapseRows);
    }

    return result;
}

int GlodonTreeDrawInfo::collapseItem(int row)
{
    QSet<int> oCollapseRows;

    int result = doCollapseItem(row, oCollapseRows);

    if (m_header != NULL)
    {
        m_header->batchSetSectionHidden(oCollapseRows, true);
    }
    return result;
}

void GlodonTreeDrawInfo::setChildItemsExpand(QSet<int> &oExpandRows, int row)
{
    int nChildRow = row + 1;

    while (nChildRow < m_viewItems.count())
    {
        if (m_viewItems.at(nChildRow).treeLevel <= m_viewItems.at(row).treeLevel)
        {
            break;
        }
        else if (m_viewItems.at(nChildRow).expanded && m_viewItems.at(row).hasChildren)
        {
            bool bExpanded = model->data(m_viewItems[nChildRow].modelIndex, gidrRowExpandedRole).toBool();

            if (bExpanded)
            {
                setChildItemsExpand(oExpandRows, nChildRow);
            }
            else
            {
                m_viewItems[nChildRow].expanded = false;
            }
        }

        m_viewItems[nChildRow].hidden = false;

        oExpandRows << nChildRow;

        nChildRow += m_viewItems[nChildRow].childCount + 1;
    }
}

int GlodonTreeDrawInfo::setChildItemsCollapse(int row, QSet<int> &oCollapseRows)
{
    int nChildRow;

    for (nChildRow = row + 1; nChildRow < m_viewItems.count(); ++nChildRow)
    {
        if (m_viewItems.at(nChildRow).treeLevel <= m_viewItems.at(row).treeLevel)
        {
            break;
        }

        m_viewItems[nChildRow].hidden = true;

        if (m_header != NULL)
        {
            oCollapseRows << nChildRow;
        }
    }

    return nChildRow;
}

int GlodonTreeDrawInfo::insertViewItem(int parent, QModelIndex index)
{
    int nchildRowNo = 0;
    int npos = parent + 1;

    while (true)
    {
        if (nchildRowNo == index.row())
        {
            break;
        }
        else
        {
            npos += m_viewItems[npos].childCount + 1;
            nchildRowNo++;
        }
    }

    GTreeViewItem item;
    item.parentIndex = parent;
    item.childCount = 0;
    item.modelIndex = index;
    m_viewItems.insert(npos, item);

    if (-1 == parent)
    {
        m_viewItems[npos].treeLevel = 0;
        m_viewItems[npos].hasMoreSiblings = (index.row() < model->rowCount() - 1);
    }
    else
    {
        m_viewItems[npos].hasMoreSiblings = (index.row() < model->rowCount(m_viewItems[parent].modelIndex) - 1);
        m_viewItems[npos].treeLevel = m_viewItems[parent].treeLevel + 1;

        int temp = parent;
        while (-1 != temp)
        {
            ++m_viewItems[temp].childCount;
            temp = m_viewItems[temp].parentIndex;
        }

        m_viewItems[parent].hasChildren = true;
    }

    for (int i = npos + 1; i < m_viewItems.count(); i++)
    {
        if (m_viewItems.at(i).parentIndex >= npos)
        {
            m_viewItems[i].parentIndex++;
        }
    }

    int ni = parent + 1;

    while (ni < npos)
    {
        if ((m_viewItems.at(ni).parentIndex == parent) && (0 == m_viewItems.at(ni).hasMoreSiblings))
        {
            m_viewItems[ni].hasMoreSiblings = true;
        }

        ni += (m_viewItems[ni].childCount + 1);
    }

    refreshModelIndexAfterInsertOrRemove(npos, parent, true);

    return npos;
}

void GlodonTreeDrawInfo::removeViewItem(int row)
{
    int ndelCount = m_viewItems[row].childCount + 1;
    int noldParent = m_viewItems[row].parentIndex;

    for (int i = row + 1; i < m_viewItems.count(); i++)
    {
        if (m_viewItems.at(i).parentIndex > row)
        {
            m_viewItems[i].parentIndex = m_viewItems[i].parentIndex - ndelCount;
        }
    }

    int ni = noldParent;

    while (ni != -1)
    {
        m_viewItems[ni].childCount = m_viewItems[ni].childCount - ndelCount;

        if (m_viewItems.at(ni).childCount == 0)
        {
            m_viewItems[ni].hasChildren = false;
        }

        ni = m_viewItems[ni].parentIndex;
    }

    ni = noldParent + 1;

    if (0 == m_viewItems.at(row).hasMoreSiblings)
    {
        while (true)
        {
            if ((ni + (uint)m_viewItems.at(ni).childCount + 1) >= (uint)row)
            {
                break;
            }

            ni += (m_viewItems[ni].childCount + 1);
        }

        m_viewItems[ni].hasMoreSiblings = false;
    }

    m_viewItems.remove(row, m_viewItems[row].childCount + 1);

    refreshModelIndexAfterInsertOrRemove(row, noldParent, false);
}

void GlodonTreeDrawInfo::moveViewItem(int parent, int first, int last, int destinationParent, int row)
{
    if (parent == destinationParent && first == row)
    {
        return;
    }

    int nparentLevel = (parent == -1) ? -1 : m_viewItems[parent].treeLevel;
    int ndestinationParentLevel = (destinationParent == -1) ? -1 : m_viewItems[destinationParent].treeLevel;

    int nfirstRow = parent + 1;
    int ni = 0;

    while (ni != first)
    {
        nfirstRow += m_viewItems[nfirstRow].childCount + 1;
        ni++;
    }

    int nlastRow = nfirstRow;
    ni = first;

    while (ni != last)
    {
        nlastRow += m_viewItems[nlastRow].childCount + 1;
        ni++;
    }

    int nlastMovedRow = nlastRow + m_viewItems[nlastRow].childCount;

    int ndestinationRow = destinationParent + 1;
    ni = 0;

    while (ni != row)
    {
        ndestinationRow += m_viewItems[ndestinationRow].childCount + 1;
        ni++;
    }

    Q_ASSERT(ndestinationRow < nfirstRow || ndestinationRow > nlastMovedRow);

    if (destinationParent != -1)
    {
        if (0 == m_viewItems.at(destinationParent).hasChildren)
        {
            m_viewItems[destinationParent].hasChildren = true;
        }
        else if (ndestinationRow > destinationParent + (int)m_viewItems.at(destinationParent).childCount)
        {
            int nlastRowBeforeMove = destinationParent + 1;
            ni = 0;

            while (ni != row - 1)
            {
                nlastRowBeforeMove += m_viewItems[nlastRowBeforeMove].childCount + 1;
                ni++;
            }

            m_viewItems[nlastRowBeforeMove].hasMoreSiblings = true;
        }
        else if (destinationParent == parent)
        {
            int nlastRowBeforeMove = destinationParent + 1;

            while (nlastRowBeforeMove < m_viewItems.count())
            {
                if (0 == m_viewItems.at(nlastRowBeforeMove).hasMoreSiblings)
                {
                    break;
                }

                nlastRowBeforeMove += m_viewItems[nlastRowBeforeMove].childCount + 1;
            }

            if (nlastRowBeforeMove < ndestinationRow)
            {
                m_viewItems[nlastRowBeforeMove].hasMoreSiblings = true;
            }
        }

        if (destinationParent != parent)
        {
            m_viewItems[destinationParent].childCount += (nlastMovedRow + m_viewItems[nlastMovedRow].childCount - nfirstRow + 1);
        }
    }
    else
    {
        int nlastRowBeforeMove = destinationParent + 1;

        while (nlastRowBeforeMove < m_viewItems.count())
        {
            if (0 == m_viewItems.at(nlastRowBeforeMove).hasMoreSiblings)
            {
                break;
            }

            nlastRowBeforeMove += m_viewItems[nlastRowBeforeMove].childCount + 1;
        }

        if (ndestinationRow > nlastRowBeforeMove)
        {
            m_viewItems[nlastRowBeforeMove].hasMoreSiblings = true;
        }
    }

    if (parent != -1)
    {
        m_viewItems[parent].childCount -= (nlastMovedRow - nfirstRow + 1);

        if (m_viewItems.at(parent).childCount == 0)
        {
            m_viewItems[parent].hasChildren = false;
        }
        else
        {
            if (0 == m_viewItems.at(nlastRow).hasMoreSiblings)
            {
                int newLastRow = parent + 1;
                ni = 0;

                while (ni != first - 1)
                {
                    newLastRow += m_viewItems[newLastRow].childCount + 1;
                    ni++;
                }

                m_viewItems[newLastRow].hasMoreSiblings = false;
            }
        }
    }
    else
    {
        int nlastRowBeforeMove = parent + 1;

        while (nlastRowBeforeMove < m_viewItems.count())
        {
            if (0 == m_viewItems.at(nlastRowBeforeMove).hasMoreSiblings)
            {
                break;
            }

            nlastRowBeforeMove += m_viewItems[nlastRowBeforeMove].childCount + 1;
        }

        if (ndestinationRow > nlastRowBeforeMove)
        {
            m_viewItems[nlastRowBeforeMove].hasMoreSiblings = true;
        }
    }

    if (ndestinationRow < nfirstRow)
    {
        for (int i = 0; i <= nlastMovedRow - nfirstRow; i++)
        {
            GTreeViewItem item = m_viewItems.at(nfirstRow + i);
            m_viewItems.remove(nfirstRow + i);
            m_viewItems.insert(ndestinationRow + i, item);

            if (m_viewItems.at(ndestinationRow + i).parentIndex == parent)
            {
                m_viewItems[ndestinationRow + i].parentIndex = destinationParent;
            }
            else
            {
                m_viewItems[ndestinationRow + i].parentIndex -= (nfirstRow - ndestinationRow);
            }

            m_viewItems[ndestinationRow + i].treeLevel -= (nparentLevel - ndestinationParentLevel);
        }

        if (ndestinationRow + nlastMovedRow - nfirstRow + 1 >= m_viewItems.count())
        {
            m_viewItems[ndestinationRow + nlastRow - nfirstRow].hasMoreSiblings = false;
        }
        else if (m_viewItems.at(ndestinationRow + nlastMovedRow - nfirstRow + 1).treeLevel == ndestinationParentLevel + 1)
        {
            m_viewItems[ndestinationRow + nlastRow - nfirstRow].hasMoreSiblings = true;
        }
        else
        {
            m_viewItems[ndestinationRow + nlastRow - nfirstRow].hasMoreSiblings = false;
        }

        for (int i = ndestinationRow + nlastMovedRow - nfirstRow + 1; i <= nlastMovedRow; i++)
        {
            if (i >= m_viewItems.count())
            {
                break;
            }

            if (m_viewItems.at(i).parentIndex != -1)
            {
                m_viewItems[i].parentIndex += (nlastMovedRow - nfirstRow + 1);
            }
        }
    }
    else
    {
        for (int i = 0; i <= nlastMovedRow - nfirstRow; i++)
        {
            GTreeViewItem item = m_viewItems.at(nfirstRow);
            m_viewItems.remove(nfirstRow);
            m_viewItems.insert(ndestinationRow - 1, item);

            if (m_viewItems.at(ndestinationRow - 1).parentIndex == parent)
            {
                m_viewItems[ndestinationRow - 1].parentIndex = destinationParent;
            }
            else
            {
                m_viewItems[ndestinationRow - 1].parentIndex -= (nfirstRow - ndestinationRow + 2);
            }

            m_viewItems[ndestinationRow - 1].treeLevel -= (nparentLevel - ndestinationParentLevel);
        }

        if (ndestinationRow == m_viewItems.count())
        {
            m_viewItems[ndestinationRow + nlastRow - nlastMovedRow - 1].hasMoreSiblings = false;
        }
        else if (m_viewItems.at(ndestinationRow + nlastRow - nlastMovedRow - 1).treeLevel == ndestinationParentLevel + 1)
        {
            m_viewItems[ndestinationRow + nlastRow - nlastMovedRow - 1].hasMoreSiblings = true;
        }
        else
        {
            m_viewItems[ndestinationRow + nlastRow - nlastMovedRow - 1].hasMoreSiblings = false;
        }

        for (int i = nfirstRow; i < ndestinationRow; i++)
        {
            if (i >= m_viewItems.count())
            {
                break;
            }

            if (m_viewItems.at(i).parentIndex != -1)
            {
                m_viewItems[i].parentIndex -= (nlastMovedRow - nfirstRow + 1);
            }
        }
    }

    //TODO:优化刷新
    refreshModelIndexAfterMove(parent, nfirstRow);
    refreshModelIndexAfterMove(destinationParent, ndestinationRow);
}

void GlodonTreeDrawInfo::refreshModelIndexAfterInsertOrRemove(int row, int parent, bool insert)
{
    int nindex = parent + 1;
    int ncurrNo = 0;

    while (nindex < m_viewItems.count())
    {
        if (nindex < row || (insert && nindex == row))
        {
            nindex += m_viewItems[nindex].childCount + 1;
            ncurrNo++;
            continue;
        }

        if (parent != -1)
        {
            if (m_viewItems.at(nindex).treeLevel <= m_viewItems.at(parent).treeLevel)
            {
                break;
            }
            else
            {
                m_viewItems[nindex].modelIndex = model->index(ncurrNo, 0, m_viewItems[parent].modelIndex);

                if (0 != m_viewItems.at(nindex).hasChildren)
                {
                    refreshChildModelIndex(nindex);
                }

                nindex += m_viewItems[nindex].childCount + 1;
                ncurrNo++;
            }
        }
        else
        {
            m_viewItems[nindex].modelIndex = model->index(ncurrNo, 0);

            if (0 != m_viewItems.at(nindex).hasChildren)
            {
                refreshChildModelIndex(nindex);
            }

            nindex += m_viewItems[nindex].childCount + 1;
            ncurrNo++;
        }
    }
}

void GlodonTreeDrawInfo::refreshChildModelIndex(int parent)
{
    int nindex = parent + 1;
    int ncurrNo = 0;

    while (nindex < m_viewItems.count())
    {
        if (parent != -1)
        {
            if (m_viewItems.at(nindex).treeLevel <= m_viewItems.at(parent).treeLevel)
            {
                break;
            }
            else
            {
                m_viewItems[nindex].modelIndex = model->index(ncurrNo, 0, m_viewItems[parent].modelIndex);

                if (0 != m_viewItems.at(nindex).hasChildren)
                {
                    refreshChildModelIndex(nindex);
                }

                nindex += m_viewItems[nindex].childCount + 1;
                ncurrNo++;
            }
        }
        else
        {
            m_viewItems[nindex].modelIndex = model->index(ncurrNo, 0);

            if (0 != m_viewItems.at(nindex).hasChildren)
            {
                refreshChildModelIndex(nindex);
            }

            nindex += m_viewItems[nindex].childCount + 1;
            ncurrNo++;
        }
    }
}

void GlodonTreeDrawInfo::refreshModelIndexAfterMove(int parent, int row)
{
    int nindex = parent + 1;
    int ncurrNo = 0;

    while (nindex < m_viewItems.count())
    {
        if (nindex < row)
        {
            nindex += m_viewItems[nindex].childCount + 1;
            ncurrNo++;
            continue;
        }

        if (parent != -1)
        {
            if (m_viewItems.at(nindex).treeLevel <= m_viewItems.at(parent).treeLevel)
            {
                break;
            }
            else
            {
                m_viewItems[nindex].modelIndex = model->index(ncurrNo, 0, m_viewItems[parent].modelIndex);

                if (0 != m_viewItems.at(nindex).hasChildren)
                {
                    refreshChildModelIndex(nindex);
                }

                nindex += m_viewItems[nindex].childCount + 1;
                ncurrNo++;
            }
        }
        else
        {
            m_viewItems[nindex].modelIndex = model->index(ncurrNo, 0);

            if (0 != m_viewItems.at(nindex).hasChildren)
            {
                refreshChildModelIndex(nindex);
            }

            nindex += m_viewItems[nindex].childCount + 1;
            ncurrNo++;
        }
    }
}
