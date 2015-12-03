#include "GLDGroupHeaderView.h"
#include "GLDGroupHeaderView_p.h"

#include <QDrag>

GlodonGroupHeaderView::GlodonGroupHeaderView(QWidget *parent) : GlodonHeaderView(Qt::Horizontal, parent)
{
    cgFrame = new GlodonColumnGroupFrame(this);
    connect(cgFrame, SIGNAL(cgFrameChanged(int, bool)), this, SLOT(cgFrameChanged(int, bool)));
    connect(cgFrame, SIGNAL(groupChanged()), this, SLOT(onGroupChanged()));
    cgFrame->setVisible(true);
    setAcceptDrops(true);
    sectionLabel = 0;
    updateGeometries();
}

int GlodonGroupHeaderView::drawWidth() const
{
    return cgFrame->frameHeight() + GlodonHeaderView::drawWidth();
}

void GlodonGroupHeaderView::setModel(QAbstractItemModel *model)
{
    GlodonHeaderView::setModel(model);
    updateGeometries();
}

void GlodonGroupHeaderView::mousePressEvent(QMouseEvent *e)
{
    Q_D(GlodonGroupHeaderView);

    if (d->m_state != GlodonHeaderViewPrivate::NoState || e->button() != Qt::LeftButton)
    {
        return;
    }

    int npos = e->x();
    int nhandle = -1;

    if (e->pos().y() > 0)
    {
        nhandle = d->sectionHandleAt(npos);
        d->originalSize = -1;

        if (nhandle == -1)
        {
            d->pressed = logicalIndexAt(npos);

            if (d->clickableSections)
            {
                emit sectionPressed(d->pressed);
            }

            d->section = d->target = d->pressed;

            if (d->section == -1)
            {
                return;
            }

            d->m_state = GlodonHeaderViewPrivate::CXMoveSection;
            setupSectionLabel(d->section);
        }
        else if (resizeMode(nhandle) == Interactive)
        {
            d->originalSize = sectionSize(nhandle);
            d->m_state = GlodonHeaderViewPrivate::ResizeSection;
            d->section = nhandle;
        }

        d->firstPos = npos;
        d->lastPos = npos;

        d->clearCascadingSections();
    }
}

void GlodonGroupHeaderView::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(GlodonGroupHeaderView);

    int npos = e->x();

    if (npos < 0)
    {
        return;
    }

    if (e->buttons() == Qt::NoButton)
    {
        d->m_state = GlodonHeaderViewPrivate::NoState;
        d->pressed = -1;
    }

    switch (d->m_state)
    {
        case GlodonHeaderViewPrivate::NoState:
        {
#ifndef QT_NO_CURSOR
            int nhandle = -1;

            if (e->pos().y() > 0)
            {
                nhandle = d->sectionHandleAt(npos);
            }

            bool bhasCursor = testAttribute(Qt::WA_SetCursor);

            if (nhandle != -1 && (resizeMode(nhandle) == Interactive))
            {
                if (!bhasCursor)
                {
                    setCursor(Qt::SplitHCursor);
                }
            }
            else if (bhasCursor)
            {
                unsetCursor();
            }

#endif
            return;
        }

        default:
            break;
    }

    GlodonHeaderView::mouseMoveEvent(e);
}

void GlodonGroupHeaderView::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(GlodonGroupHeaderView);

    if (d->m_state == GlodonHeaderViewPrivate::CXMoveSection)
    {
        d->m_state = GlodonHeaderViewPrivate::NoState;
    }
    else
    {
        GlodonHeaderView::mouseReleaseEvent(e);
    }
}

void GlodonGroupHeaderView::dragEnterEvent(QDragEnterEvent *dee)
{
    if (/*GColumnGroupLabel *label = */dynamic_cast<GColumnGroupLabel *>(dee->source()))
    {
        dee->acceptProposedAction();
    }
}

void GlodonGroupHeaderView::dragMoveEvent(QDragMoveEvent *dme)
{
    if (/*GColumnGroupLabel *label = */dynamic_cast<GColumnGroupLabel *>(dme->source()))
    {
        dme->acceptProposedAction();
    }
}

void GlodonGroupHeaderView::dropEvent(QDropEvent *de)
{
    Q_D(GlodonGroupHeaderView);

    if (GColumnGroupLabel *label = dynamic_cast<GColumnGroupLabel *>(de->source()))
    {
        if (de->pos().y() > 0)
        {
            label->draggedLeaveFrame();
        }
    }

    if (d->m_state == GlodonHeaderViewPrivate::CXMoveSection)
    {
        d->m_state = GlodonHeaderViewPrivate::NoState;
    }
}

void GlodonGroupHeaderView::setupSectionLabel(int section)
{
    Q_D(GlodonGroupHeaderView);

    if (sectionLabel == NULL)
    {
        sectionLabel = new GColumnGroupLabel(section, d->m_model->headerData(section, Qt::Horizontal).toString(), this);
    }
    else
    {
        sectionLabel->column = section;
        sectionLabel->setText(d->m_model->headerData(section, Qt::Horizontal).toString());
    }

    QDrag *drag = new QDrag(sectionLabel);
    QMimeData *data = new QMimeData();
    data->setText(sectionLabel->text());
    drag->setMimeData(data);

    QPixmap pix(sectionLabel->width(), sectionLabel->height());
    pix.fill(QColor(0, 0, 0, 45));
    QRect rect(0, 0, sectionLabel->width(), sectionLabel->height());

    QPainter painter(&pix);
    painter.setOpacity(0.75);
    painter.drawText(rect, sectionLabel->text(), QTextOption(Qt::AlignHCenter));
    painter.end();

    drag->setPixmap(pix);

    drag->exec();
}

QVector<int> GlodonGroupHeaderView::currentGroups()
{
    QVector<int> result;

    for (int i = 0; i < cgFrame->labels.count(); i++)
    {
        result << cgFrame->labels[i]->column;
    }

    return result;
}

void GlodonGroupHeaderView::updateGeometries()
{
    Q_D(GlodonGroupHeaderView);
    setViewportMargins(0, cgFrame->frameHeight(), 0, 0);
    cgFrame->setGeometry(0, 0, d->viewport->width(), cgFrame->frameHeight());
    repaint();
}

void GlodonGroupHeaderView::cgFrameChanged(int col, bool dragIn)
{
    if (dragIn)
    {
        showSection(col);
    }
    else
    {
        hideSection(col);
    }

    updateGeometries();
    emit cgFrameChanged();
}

void GlodonGroupHeaderView::onGroupChanged()
{
    emit groupChanged(currentGroups());
}

GlodonColumnGroupFrame::GlodonColumnGroupFrame(QWidget *parent) :
    QFrame(parent)
{
    setAcceptDrops(true);
    moveStart = -1;
    movePos = -1;
}

void GlodonColumnGroupFrame::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::gray);

    for (int i = 0; i < labels.count(); i++)
    {
        QRect rect = labelRect(i);
        labels[i]->setGeometry(rect);
        drawBranch(i, rect);
    }

    QFrame::paintEvent(e);
}

void GlodonColumnGroupFrame::dragEnterEvent(QDragEnterEvent *dee)
{
    if (GColumnGroupLabel *label = dynamic_cast<GColumnGroupLabel *>(dee->source()))
    {
        dee->acceptProposedAction();
        moveStart = labels.indexOf(label);
    }
}

void GlodonColumnGroupFrame::dropEvent(QDropEvent *de)
{
    if (GColumnGroupLabel *label = dynamic_cast<GColumnGroupLabel *>(de->source()))
    {
        movePos = moveTarget(de->pos());
        moveStart = labels.indexOf(label);

        if (moveStart != -1)
        {
            labels[moveStart]->setStyleSheet("GColumnGroupLabel {background:rgb(255, 255, 255); color:rgb(0, 0, 0)}");

            if (movePos != moveStart)
            {
                labels.move(moveStart, movePos);
            }
            else
            {
                moveStart = -1;
                movePos = -1;
                repaint();
                return;
            }
        }
        else
        {
            GColumnGroupLabel *newLabel = new GColumnGroupLabel(label->column, label->text(), this);
            connect(newLabel, SIGNAL(leaveFrame()), this, SLOT(onLabelDragOut()));
            labels.append(newLabel);
            newLabel->setVisible(true);
            labels.move(labels.count() - 1, movePos);
            emit cgFrameChanged(label->column, false);
        }

        setGeometry(pos().x(), pos().y(), width(), frameHeight());
        emit groupChanged();
        moveStart = -1;
        movePos = -1;
        repaint();
    }
}

void GlodonColumnGroupFrame::dragMoveEvent(QDragMoveEvent *dme)
{
    if (dynamic_cast<GColumnGroupLabel *>(dme->source()))
    {
        dme->acceptProposedAction();
        int nold = movePos;
        movePos = moveTarget(dme->pos());

        if (nold != movePos)
        {
            if (nold >= moveStart)
            {
                update(branchRect(nold));
            }
            else
            {
                update(branchRect(nold - 1));
            }

            if (movePos >= moveStart)
            {
                update(branchRect(movePos));
            }
            else
            {
                update(branchRect(movePos - 1));
            }
        }
    }
}

void GlodonColumnGroupFrame::dragLeaveEvent(QDragLeaveEvent *)
{
    if (moveStart >= 0)
    {
        labels[moveStart]->setStyleSheet("GColumnGroupLabel {background:rgb(0, 0, 0); color:rgb(255, 255, 255)}");
    }
}

int GlodonColumnGroupFrame::frameHeight() const
{
    return qMax(35, (labels.count() + 1) * 11 + 24);
}

void GlodonColumnGroupFrame::onLabelDragOut()
{
    GColumnGroupLabel *label = dynamic_cast<GColumnGroupLabel *>(sender());
    label->setVisible(false);
    labels.removeOne(label);
    emit cgFrameChanged(label->column, true);
    emit groupChanged();
    moveStart = -1;
    movePos = -1;
    repaint();
}

QRect GlodonColumnGroupFrame::labelRect(int level)
{
    if (level > labels.count() - 1 || level < 0)
    {
        return QRect();
    }

    int nleft = 12;

    for (int i = 0; i < level; i++)
    {
        nleft += labels[i]->width() + 5;
    }

    int ntop = 12;

    for (int i = 0; i < level; i++)
    {
        ntop += 11;
    }

    return QRect(nleft, ntop, labels[level]->width(), labels[level]->height());
}

void GlodonColumnGroupFrame::drawBranch(int level, QRect labelRect)
{
    if (level >= labels.count() - 1 || level < 0)
    {
        return;
    }

    QPainter painter(this);
    QPen old = painter.pen();
    int nleft = labelRect.right() - 5;

    if ((movePos >= moveStart && level == movePos) || (movePos < moveStart && level == movePos - 1))
    {
        painter.setPen(Qt::green);
        painter.drawLine(nleft, labelRect.bottom(), nleft, labelRect.bottom() + 4);
        painter.drawLine(nleft, labelRect.bottom() + 4, nleft + 10, labelRect.bottom() + 4);
        painter.setPen(old);
    }
    else
    {
        painter.drawLine(nleft, labelRect.bottom(), nleft, labelRect.bottom() + 4);
        painter.drawLine(nleft, labelRect.bottom() + 4, nleft + 10, labelRect.bottom() + 4);
    }
}

int GlodonColumnGroupFrame::moveTarget(QPoint pos)
{
    for (int i = 0; i < labels.count(); i++)
    {
        if (pos.x() <= labels[i]->pos().x())
        {
            if (i <= moveStart || moveStart == -1)
            {
                return i;
            }
            else
            {
                return i - 1;
            }
        }
    }

    if (moveStart == -1)
    {
        return labels.count();
    }
    else
    {
        return labels.count() - 1;
    }
}

QRect GlodonColumnGroupFrame::branchRect(int level)
{
    if (level < 0 || level >= labels.count() - 1)
    {
        return QRect();
    }

    QRect rect = labelRect(level);
    return QRect(rect.right() - 6, rect.bottom() - 1, rect.width() + 2, rect.height() + 2);
}

GColumnGroupLabel::GColumnGroupLabel(int col, QString text, QWidget *parent) :
    QLabel(parent)
{
    setStyleSheet("GColumnGroupLabel {background:rgb(255, 255, 255)}");
    setFrameShape(Panel);
    setText(text);
    adjustSize();
    column = col;
    setFixedSize(qMax(width(), 60), 22);
}

void GColumnGroupLabel::mousePressEvent(QMouseEvent *e)
{
    QDrag *drag = new QDrag(this);
    QMimeData *data = new QMimeData();
    data->setText(text());
    drag->setMimeData(data);

    QPixmap pix(width(), height());
    pix.fill(QColor(0, 0, 0, 45));
    QRect rect(0, 0, width(), height());

    QPainter painter(&pix);
    painter.setOpacity(0.75);
    painter.drawText(rect, text(), QTextOption(Qt::AlignHCenter));
    painter.end();

    drag->setPixmap(pix);
    drag->setHotSpot(QPoint(e->pos()));

    drag->exec(Qt::MoveAction);
}

void GColumnGroupLabel::draggedLeaveFrame()
{
    emit leaveFrame();
}
