#include "GLDFilterView.h"
#include "GLDFilterView_p.h"

#include <QStandardItemModel>
#include <QStyleFactory>

GFilterView::GFilterView(QWidget *parent): GlodonHeaderView(Qt::Horizontal, parent), m_bHideFlag(false)
{
}

QString GFilterView::filterData(int logicalIndex)
{
    if (QComboBox *comboBox = dynamic_cast<QComboBox *>(indexWidget(model()->index(0, logicalIndex))))
    {
        return comboBox->currentText();
    }
    else
    {
        return "NULL";
    }
}

void GFilterView::setModel(QAbstractItemModel *model)
{
    m_pFilterDataModel = model;

    int nCount = model->columnCount();
    if (nCount < 0)
        return;

    QStandardItemModel *filterModel = new QStandardItemModel(1, nCount, this);
    QList<QStandardItem *> items;
    for (int i = 0; i < nCount; i++)
    {
        QStandardItem *item = new QStandardItem("");
        items.append(item);
    }
    filterModel->appendRow(items);

    setFilterDataModel(filterModel);
}

void GFilterView::setFilterDataModel(QAbstractItemModel *filterModel)
{
    Q_ASSERT(m_pFilterDataModel->columnCount() == filterModel->columnCount());

    GlodonHeaderView::setModel(filterModel);
    initFilterWidget();
    connect(filterModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(filterDataChanged(QModelIndex,QModelIndex)));
}

QRect GFilterView::visualRect(const QModelIndex &index) const
{
    Q_D(const GFilterView);
    int nLeft = sectionViewportPosition(index.column());
    return QRect(nLeft, 0, sectionSize(index.column()), d->viewport->height());
}

void GFilterView::paintEvent(QPaintEvent * e)
{
    GlodonHeaderView::paintEvent(e);
}

int GFilterView::height() const
{
    if (isHidden())
    {
        return 0;
    }
    return qMax(drawWidth(), 20);
}

void GFilterView::resizeSection(int logicalIndex, int oldSize, int size)
{
    Q_UNUSED(oldSize);
    Q_D(GFilterView);
    int nOffset = size - oldSize;
    //no need to resize
    if (0 == nOffset)
        return;
    bool beginUseOffset = false;
    for (int i = 0; i < d->m_model->columnCount(); i++)
    {
        if (i == logicalIndex)
        {
            beginUseOffset = true;
        }

        if (QComboBox *comboBox = dynamic_cast<QComboBox *>(indexWidget(model()->index(0, i))))
        {
            QRect rect = visualRect(d->m_model->index(0, i));
            //calculate the new rect from logicalIndex column
            QRect rectOffset = beginUseOffset?
                        ( (i == logicalIndex) ?
                              QRect(rect.left(), 0, rect.width() + nOffset, rect.height()) :
                              QRect(rect.left() + nOffset,0,rect.width(), rect.height())) :
                        rect;
            comboBox->setGeometry(rectOffset);
        }
    }
    GlodonHeaderView::resizeSection(logicalIndex, size);
}

void GFilterView::moveSection(int logicalIndex, int from, int to)
{
    Q_UNUSED(logicalIndex);
    GlodonHeaderView::moveSection(from, to);
}

void GFilterView::filterChanged()
{
    Q_D(GFilterView);
    QObject *obj = QObject::sender();
    if (QWidget *widget = dynamic_cast<QWidget *>(obj))
    {
        QModelIndex index = d->indexForEditor(widget);
        emit onFilterChanged(index.column());
    }
}

void GFilterView::hideHiddenSection(int hbarPos)
{
    Q_UNUSED(hbarPos);
    m_bHideFlag = true;
    this->viewport()->update();
}

void GFilterView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    Q_D(const GFilterView);
    QStyleOptionHeader opt;
    initStyleOption(&opt);
    QStyle::State state = QStyle::State_None;

    if (isEnabled())
    {
        state |= QStyle::State_Enabled;
    }
    if (window()->isActiveWindow())
    {
        state |= QStyle::State_Active;
    }

    QVariant textAlignment = d->m_model->data(d->m_model->index(0, logicalIndex), Qt::TextAlignmentRole);

    opt.rect = rect;
    opt.section = logicalIndex;
    opt.state |= state;
    opt.textAlignment = Qt::Alignment(textAlignment.isValid()
                                      ? Qt::Alignment(textAlignment.toInt())
                                      : d->defaultAlignment);

    opt.iconAlignment = Qt::AlignVCenter;
    opt.text = d->m_model->data(d->m_model->index(0, logicalIndex), Qt::DisplayRole).toString();

    if (d->m_textElideMode != Qt::ElideNone)
    {
        opt.text = opt.fontMetrics.elidedText(opt.text, d->m_textElideMode , rect.width() - 4);
    }

    QVariant variant = d->m_model->data(d->m_model->index(0, logicalIndex), Qt::DecorationRole);

    opt.icon = qvariant_cast<QIcon>(variant);
    if (opt.icon.isNull())
    {
        opt.icon = qvariant_cast<QPixmap>(variant);
    }

    QVariant foregroundBrush = d->m_model->data(d->m_model->index(0, logicalIndex), Qt::ForegroundRole);

    if (foregroundBrush.canConvert<QBrush>())
    {
        opt.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));
    }

    QPointF oldBO = painter->brushOrigin();
    QVariant backgroundBrush = d->m_model->data(d->m_model->index(0, logicalIndex), Qt::BackgroundRole);

    if (backgroundBrush.canConvert<QBrush>())
    {
        opt.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));
        opt.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));
        painter->setBrushOrigin(opt.rect.topLeft());
    }

    int nVisual = visualIndex(logicalIndex);
    Q_ASSERT(nVisual != -1);
    if (count() == 1)
    {
        opt.position = QStyleOptionHeader::OnlyOneSection;
    }
    else if (nVisual == 0)
    {
        opt.position = QStyleOptionHeader::Beginning;
    }
    else if (nVisual == count() - 1)
    {
        opt.position = QStyleOptionHeader::End;
    }
    else
    {
        opt.position = QStyleOptionHeader::Middle;
    }
    opt.orientation = d->orientation;

    style()->drawControl(QStyle::CE_Header, &opt, painter, this);
    painter->setBrushOrigin(oldBO);


    if (QWidget *widget = indexWidget(d->m_model->index(0, logicalIndex)))
    {
        int nFixedWidth = 0;
        if (d->fixedCount > 0)
        {
            if (d->orientation == Qt::Horizontal)
            {
                for (int i = 0; i < d->fixedCount; i++)
                {
                    nFixedWidth += sectionSize(i);
                }
            }
        }
        QRect rect = visualRect(d->m_model->index(0, logicalIndex));
        if ((QStyleFactory::keys().contains(qApp->style()->objectName(), Qt::CaseInsensitive)))
            rect.adjust(0, -1, 0, 0);
        rect.adjust(-1, -1, 0, 0);
        if (d->fixedCount > 0)
        {
            if (rect.right() - 2 > nFixedWidth && rect.left() < nFixedWidth - 2)
            {
                if (logicalIndex >= d->fixedCount)
                    rect.setLeft(nFixedWidth);
            }
            else if (logicalIndex < d->fixedCount && rect.right() < nFixedWidth + 4)
            {
                rect = widget->geometry();
            }
        }
//        QRect indexRect = this->visualRect(d->indexForEditor(widget));
//        int indexWidth = indexRect.width(); 未使用变量indexWidth
        widget->setGeometry(rect);
        //        widget->setGeometry(visualRect(d->model->index(0, logicalIndex)));
        if (d->fixedCount > 0 && logicalIndex >= d->fixedCount && widget->geometry().right() <= nFixedWidth + 4)
            widget->hide();
        else
            widget->show();
    }

//    if (QWidget *widget = indexWidget(d->model->index(0, logicalIndex)))
//    {
//        QRect rect = visualRect(d->model->index(0, logicalIndex));
//        if ((QStyleFactory::keys().contains(qApp->style()->objectName(), Qt::CaseInsensitive)))
//            rect.adjust(0, -1, 0, 0);
//        rect.adjust(-1, -1, 0, 0);
//        widget->setGeometry(rect);
////        widget->setGeometry(visualRect(d->model->index(0, logicalIndex)));
//        widget->show();
    //    }
}

void GFilterView::paintSection(int beginToHideIndex) const
{
    Q_D(const GFilterView);

    if (m_bHideFlag)
    {
        m_bHideFlag = false;

        for (int i = beginToHideIndex; i < d->sectionCount(); ++i)
        {
            if (QWidget *widget = indexWidget(d->m_model->index(0, i)))
            {
                widget->hide();
            }
        }
    }
}

void GFilterView::keyPressEvent(QKeyEvent *event)
{
    event->accept();
}

void GFilterView::filterDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    for (int i = topLeft.column(); i <= bottomRight.column(); i++)
    {
        resetFilter(i);
    }
}

void GFilterView::initFilterWidget()
{
    Q_D(GFilterView);
    for (int col = 0; col < d->m_model->columnCount(); col++)
    {
        QComboBox *comboBox = new QComboBox(d->viewport);
        QStringList list;
        list.append("NULL");
        for (int row = 0; row < m_pFilterDataModel->rowCount(); row++)
        {
            QModelIndex index = m_pFilterDataModel->index(row, col);
            if (index.isValid())
            {
                QString strValue = index.data().toString();
                if (!list.contains(strValue))
                {
                    list.append(strValue);
                }
            }
        }
        comboBox->addItems(list);
        comboBox->setEditable(true);
        comboBox->setGeometry(visualRect(d->m_model->index(0, col)));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
        setIndexWidget(d->m_model->index(0, col), comboBox);
    }
}

void GFilterView::resetFilter(int col)
{
    Q_D(GFilterView);
    QComboBox *comboBox = new QComboBox(d->viewport);
    QStringList list;
    list.append("NULL");
    for (int row = 0; row < m_pFilterDataModel->rowCount(); row++)
    {
        QModelIndex index = m_pFilterDataModel->index(row, col);
        if (index.isValid())
        {
            QString strValue = index.data().toString();
            if (!list.contains(strValue))
            {
                list.append(strValue);
            }
        }
    }
    comboBox->addItems(list);
    comboBox->setEditable(true);
    comboBox->setGeometry(visualRect(d->m_model->index(0, col)));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
    setIndexWidget(d->m_model->index(0, col), comboBox);
}
