#include "GLDFooterView.h"
#include "GLDFooterView_p.h"
#include "GLDTreeModel.h"
#include <QScrollBar>

GFooterView::GFooterView(QWidget *parent) : GlodonTableView(*new GFooterViewPrivate, parent)
{
    horizontalHeader()->hide();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(NoFrame);
}

void GFooterView::setModel(QAbstractItemModel *model)
{
    GlodonTableView::setModel(model);
}

int GFooterView::height() const
{
    Q_D(const GFooterView);
    int nHeight = 0;
    for (int i = 0; i < d->m_model->rowCount(); i++)
    {
        nHeight += d->m_verticalHeader->sectionSize(i);
    }
    return nHeight;
}

void GFooterView::paintEvent(QPaintEvent *e)
{
    GlodonTableView::paintEvent(e);
}

void GFooterView::resizeSection(int logicalIndex, int oldSize, int size)
{
    Q_UNUSED(oldSize);
    Q_D(GFooterView);
    d->m_horizontalHeader->resizeSection(logicalIndex, size);
    d->viewport->update();
}

void GFooterView::moveSection(int logicalIndex, int from, int to)
{
    Q_UNUSED(logicalIndex);
    Q_D(GFooterView);
    d->m_horizontalHeader->moveSection(from, to);
    d->viewport->update();
}

void GFooterView::setOffset(int offset)
{
    Q_D(GFooterView);
    d->m_horizontalHeader->setOffset(offset);
    d->viewport->update();
}

void GFooterView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)
    emit currentIndexChanged(current);
}

void GFooterView::resetCurrentIndex(const QModelIndex &current)
{
    if (current.column() != currentIndex().column())
    {
        int row = qMax(0, currentIndex().row());
        QAbstractItemModel *curModel = model();
        GlodonTreeModel *treeModel = dynamic_cast<GlodonTreeModel *>(curModel);
        QModelIndex newIndex = curModel->index(row, current.column());
        if (treeModel)
            newIndex = treeModel->dataIndex(row, current.column());
        setCurrentIndex(newIndex);
        update(newIndex);
    }
}

void GFooterView::setVerticalHeaderDrawWidth( int width )
{
    Q_D(GFooterView);
    d->m_verticalHeader->setDrawWidth(width);
    updateGeometries();
}

GFooterViewDelegate::GFooterViewDelegate(QObject *parent) : GlodonDefaultItemDelegate(parent)
{
}

GEditStyle GFooterViewDelegate::editStyle(const QModelIndex &index, bool &readOnly) const
{
    return esNone;
    Q_UNUSED(index);
    Q_UNUSED(readOnly);
}
