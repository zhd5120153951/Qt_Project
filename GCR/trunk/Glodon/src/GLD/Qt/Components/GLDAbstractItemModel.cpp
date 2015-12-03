#include "GLDAbstractItemModel.h"
#include "GLDAbstractItemModel_p.h"

GlodonAbstractItemModel::GlodonAbstractItemModel(QObject *parent)
    : QAbstractItemModel(*new GlodonAbstractItemModelPrivate, parent)
{
}

GlodonAbstractItemModel::~GlodonAbstractItemModel()
{
}

QVariant GlodonAbstractItemModel::data(const QModelIndex &index, int role) const
{
    Q_D(const GlodonAbstractItemModel);

    if (d->m_showCheckbox)
    {
        if (Qt::CheckStateRole == role)
        {
            if (d->m_checkedIndexes.contains(index) && !d->m_partiallyCheckedIndexes.contains(index) && !d->m_forbiddenCheckedIndexes.contains(index))
            {
                return Qt::Checked;
            }

            if (d->m_forbiddenCheckedIndexes.contains(index))
            {
                return Qt::Unchecked;    // 为了使不可点击的视觉效果明显
            }

            if (d->m_partiallyCheckedIndexes.contains(index))
            {
                return Qt::PartiallyChecked;
            }

            return Qt::Unchecked;
        }
        else if (gidrCheckUnEnableRole == role)
        {
            return d->m_forbiddenCheckedIndexes.contains(index) ? true : false;
        }
    }

    return GVariant();
}

bool GlodonAbstractItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(GlodonAbstractItemModel);

    if (! d->indexValid(index))
    {
        return false;
    }

    if (Qt::CheckStateRole == role)
    {
        bool result = true;

        if (Qt::Checked == value)
        {
            d->m_checkedIndexes.insert(index);
        }
        else
        {
            d->m_checkedIndexes.remove(index);
        }

        d->m_partiallyCheckedIndexes.remove(index);
        emit dataChanged(index, index);

        if (d->m_recursiveCheck && hasChildren(index))
        {
            result &= recursiveCheck(index, value);
        }

        if (d->m_isTristate)
        {
            result &= recursiveCheckParent(index, value);
        }

        return result;
    }
    else if (gidrAddUnEnableRole == role)
    {
        d->m_forbiddenCheckedIndexes.insert(index);
        d->m_partiallyCheckedIndexes.remove(index);
        d->m_checkedIndexes.remove(index);
        return true;
    }
    else if (gidrRemoveUnEnableRole == role)
    {
        d->m_forbiddenCheckedIndexes.remove(index);
        return true;
    }

    return false;
}

Qt::ItemFlags GlodonAbstractItemModel::flags(const QModelIndex &index) const
{
    Q_D(const GlodonAbstractItemModel);

    if (!d->indexValid(index))
    {
        return 0;
    }

    Qt::ItemFlags oFlags = QAbstractItemModel::flags(index);

    if (d->m_showCheckbox)
    {
        oFlags |= Qt::ItemIsUserCheckable;
    }

    if (d->m_isTristate)
    {
        oFlags |= Qt::ItemIsTristate;
    }

    return oFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

bool GlodonAbstractItemModel::showCheckBox()
{
    Q_D(GlodonAbstractItemModel);
    return d->m_showCheckbox;
}

void GlodonAbstractItemModel::setShowCheckBox(const bool &value)
{
    Q_D(GlodonAbstractItemModel);
    d->m_showCheckbox = value;
}

bool GlodonAbstractItemModel::isItemIsTristate()
{
    Q_D(GlodonAbstractItemModel);
    return d->m_isTristate;
}

void GlodonAbstractItemModel::setIsTristate(const bool &value)
{
    Q_D(GlodonAbstractItemModel);
    d->m_isTristate = value;
}

bool GlodonAbstractItemModel::isRecursiveCheck()
{
    Q_D(GlodonAbstractItemModel);
    return d->m_showCheckbox;
}

void GlodonAbstractItemModel::setRecursiveCheck(const bool &value)
{
    Q_D(GlodonAbstractItemModel);
    d->m_recursiveCheck = value;
}

void GlodonAbstractItemModel::afterCurrentChanged(const QModelIndex &index)
{
    Q_UNUSED(index);
}

/*!
  \internal
*/
GlodonAbstractItemModel::GlodonAbstractItemModel(GlodonAbstractItemModelPrivate &dd, QObject *parent)
    : QAbstractItemModel(dd, parent)
{
}

bool GlodonAbstractItemModel::recursiveCheck(const QModelIndex &index, const GVariant &value)
{
    int nChildCount = rowCount(index);
    QModelIndex child;
    bool result = true;

    for (int i = 0; i < nChildCount; i++)
    {
        child = this->index(i, 0, index);
        result &= setData(child, value, Qt::CheckStateRole);
    }

    return result;
}

bool GlodonAbstractItemModel::recursiveCheckParent(const QModelIndex &index, const GVariant &value)
{
    Q_D(GlodonAbstractItemModel);
    QModelIndex oParentIndex = index.parent();

    if (! oParentIndex.isValid())
    {
        return false;
    }

    int nChildCount = rowCount(oParentIndex);
    QModelIndex oChild;
    Qt::CheckState oState = (Qt::CheckState)value.toInt();
    bool bSameCheck = true;

    if (Qt::PartiallyChecked == oState)
    {
        recursiveCheckParentPartCheck(oParentIndex);
        return true;
    }
    else if (Qt::Checked == oState)
    {
        for (int i = 0; i < nChildCount; i++)
        {
            oChild = this->index(i, 0, oParentIndex);

            if (!d->m_checkedIndexes.contains(oChild))
            {
                recursiveCheckParentPartCheck(oParentIndex);
                bSameCheck = false;
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < nChildCount; i++)
        {
            oChild = this->index(i, 0, oParentIndex);

            if (d->m_checkedIndexes.contains(oChild))
            {
                recursiveCheckParentPartCheck(oParentIndex);
                bSameCheck = false;
                break;
            }
            if(d->m_partiallyCheckedIndexes.contains(oChild))
            {
                bSameCheck = false;
                break;
            }
        }
    }

    if (bSameCheck)
    {
        if (Qt::Checked == oState)
        {
            d->m_checkedIndexes.insert(oParentIndex);
        }
        else
        {
            d->m_checkedIndexes.remove(oParentIndex);
        }

        d->m_partiallyCheckedIndexes.remove(oParentIndex);
        emit dataChanged(oParentIndex, oParentIndex);
        recursiveCheckParent(oParentIndex, oState);
    }

    return true;
}

void GlodonAbstractItemModel::recursiveCheckParentPartCheck(const QModelIndex &index)
{
    if (! index.isValid())
    {
        return;
    }

    Q_D(GlodonAbstractItemModel);
    QModelIndex oIndex = index;

    while (oIndex.isValid())
    {
        d->m_partiallyCheckedIndexes.insert(oIndex);
        d->m_checkedIndexes.remove(oIndex);
        emit dataChanged(oIndex, oIndex);
        oIndex = oIndex.parent();
    }
}

/* GlodonAbstractItemModelPrivate */

GlodonAbstractItemModelPrivate::GlodonAbstractItemModelPrivate()
    : QAbstractItemModelPrivate() , m_showCheckbox(false), m_recursiveCheck(false),
      m_isTristate(false)
{
}

GlodonAbstractItemModelPrivate::~GlodonAbstractItemModelPrivate()
{
}
