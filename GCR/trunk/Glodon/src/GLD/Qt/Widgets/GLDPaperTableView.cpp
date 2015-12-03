#include "GLDPaperTableView.h"
#include "GLDPaperTableView_p.h"
#include "GLDAbstractItemModel.h"
#include "GLDGlobal.h"

GlodonPaperTableView::GlodonPaperTableView(QWidget *parent)
    : GlodonTableView(*new GlodonPaperTableViewPrivate, parent)
{

}

GlodonPaperTableView::GlodonPaperTableView(GlodonPaperTableViewPrivate &dd, QWidget *parent)
    : GlodonTableView(dd, parent)
{

}

void GlodonPaperTableView::showSectionResizingInfoFrame(
        const QPoint &mousePosition, Qt::Orientation direction, GlodonHeaderView::ResizeState state)
{
    Q_D(GlodonPaperTableView);

    GlodonTableView::showSectionResizingInfoFrame(mousePosition, direction, state);

    d->m_paperState = state;
}

void GlodonPaperTableView::columnResized(int column, int oldWidth, int newWidth, bool isManual)
{
    Q_D(GlodonPaperTableView);

    if (GlodonHeaderView::Press == d->m_paperState || GlodonHeaderView::Finish == d->m_paperState)
    {
        GlodonTableView::columnResized(column, oldWidth, newWidth, isManual);
        return;
    }

    const double c_fixedWidth = this->width() - oldWidth;

    if (newWidth >= width() || c_fixedWidth <= 0 || newWidth <= 0)
    {
        d->m_horizontalHeader->resizeSection(column, oldWidth, false, false);
        return;
    }

    if (d->m_bIsInAdjustFitCols == true)
    {
        return;
    }

    d->m_bIsInAdjustFitCols = true;

    const int c_colCount = d->m_horizontalHeader->count();

    if (0 == c_colCount)
    {
        return;
    }

    emit enableFitColWidths(false);

    GIntList *originColWidths = new GIntList();
//    QList<double> *originColRate = new QList<double>();

    for (int i = 0; i < c_colCount; ++i)
    {
        if (columnWidth(i) > 0)
        {
            originColWidths->push_back(columnWidth(i));
//            originColRate->push_back(columnWidth(i) / c_fixedWidth);
        }
        else
        {
            QModelIndex modelIndex = model()->index(0, i);
            QVariant colWidth = modelIndex.data(gidrColWidthRole);

            if (colWidth.isValid())
            {
                originColWidths->push_back(colWidth.toInt());
//                originColRate->push_back(colWidth.toInt() / c_fixedWidth);
            }
            else
            {
                G_ASSERT(false);
//                qDebug() << "GLDTableView::columnResized on paper mode problem";
            }
        }
    }

    GIntList newWidths(*originColWidths);
    {
        int nNext = column + 1;

        if (nNext > c_colCount - 1)
        {
            nNext = column - 1;
        }

        int nNextWidth = newWidths[nNext] + oldWidth - newWidth;
        const int c_fontSize = horizontalHeader()->fontInfo().pixelSize();
        nNextWidth = (nNextWidth > c_fontSize) ? nNextWidth : c_fontSize;
        int nNewWidth = newWidths[nNext] + oldWidth - nNextWidth;
        newWidths[column] = nNewWidth;
        newWidths[nNext] = nNextWidth;
    }

    for (int i = 0; i < newWidths.size(); ++i)
    {
        if (columnWidth(i) != newWidths.at(i))
        {
            setColumnWidth(i, newWidths.at(i));
        }
    }

    for (int i = 0; i < d->m_horizontalHeader->count(); ++i)
    {
        QModelIndex modelIndex = model()->index(0, i);
        model()->setData(modelIndex, newWidths.at(i), gidrColWidthRole);
    }

    emit columnNewWidths(&newWidths);
    d->m_bIsInAdjustFitCols = false;
    emit enableFitColWidths(true);

    freeAndNil(originColWidths);

    // Timer Ë¢ÐÂ´¦Àí
    d->m_columnsToUpdate.append(column);

    if (d->m_columnResizeTimerID == 0)
    {
        d->m_columnResizeTimerID = startTimer(0);
    }
//    freeAndNil(originColRate);
}

