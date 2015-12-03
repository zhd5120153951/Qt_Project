#include <QMouseEvent>
#include <QApplication>
#include <QPixmap>
#include <QPainter>
#include "GLDTreeView.h"
#include "GLDGlobal.h"
#include "GLDException.h"

GlodonTreeView::GlodonTreeView(QWidget *parent) : QTreeView(parent), m_rowDragable(false), m_ptStart(-1, -1),
    m_pIndicator(NULL), m_indexFrom(QModelIndexList()), m_dragStyle(DotLineStyle), m_state(NONE), m_canSectionMove(true),
    m_autoResetScrollBarAfterExpand(false), m_errCode(-1), m_errMessage("")
{
    init();
}

void GlodonTreeView::setAutoResetScrollBarAfterExpand(bool value)
{
    if (m_autoResetScrollBarAfterExpand == value)
    {
        return;
    }

    m_autoResetScrollBarAfterExpand = value;
    if (value)
    {
        connect(this, SIGNAL(expanded(QModelIndex)), this, SLOT(autoResetScrollBarOnExpand(QModelIndex)));
    }
    else
    {
        disconnect(this, SIGNAL(expanded(QModelIndex)), this, SLOT(autoResetScrollBarOnExpand(QModelIndex)));
    }
}

void GlodonTreeView::reset()
{
    QTreeView::reset();
    m_indexFrom = QModelIndexList();
    m_ptStart = QPoint(-1000, -1000);
}

bool GlodonTreeView::viewportEvent(QEvent *event)
{
    switch (event->type())
    {
        case QEvent::HoverEnter:
        case QEvent::HoverLeave:
        case QEvent::HoverMove:
        {
            if (m_state == DragSection)
            {
                return true;
            }
        }
        break;
        default:
            break;
    }
    return QAbstractItemView::viewportEvent(event);
}

void GlodonTreeView::mousePressEvent(QMouseEvent *event)
{
    QTreeView::mousePressEvent(event);
    if (m_rowDragable)
    {
        QModelIndex index  = indexAt(event->pos());
        if (index.isValid())
        {
            setupIndexIndicator(selectionModel()->selectedIndexes(), event->pos());
        }
        else
        {
            setupIndexIndicator(QModelIndexList(), QPoint(-1000, -1000));
        }
    }
}

void GlodonTreeView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_rowDragable)
    {
        startAutoScroll();
        bool bShouldUpdate = false;
        if (m_state == DragSection)
        {
            bShouldUpdate = true;
        }
        else if ((event->pos() - m_ptStart).manhattanLength() >= QApplication::startDragDistance())
        {
            if (NULL == m_pIndicator)
            {
                m_pIndicator = new QLabel(this);
                m_pIndicator->hide();
            }
            bShouldUpdate = true;
        }
        if (bShouldUpdate)
        {
            m_state = DragSection;
            updateIndexIndicator(event->pos());
            QModelIndex indexTo = indexAt(event->pos());
            m_canSectionMove = true;
            QCursor cursor;
            canDrag(m_indexFrom, indexTo, event->modifiers(), m_canSectionMove, cursor);
            setCursor(cursor);
        }
        event->accept();
        return;
    }
    QTreeView::mouseMoveEvent(event);
}

void GlodonTreeView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_rowDragable)
    {
        if (m_state == DragSection)
        {
            QModelIndex indexTo = indexAt(event->pos());

            if (m_canSectionMove)
            {
                moveIndex(m_indexFrom, indexTo, event->modifiers());
            }
            unsetCursor();
            m_ptStart = QPoint(-1, -1);
            m_pIndicator->hide();
            m_state = NONE;
        }
    }
    QTreeView::mouseReleaseEvent(event);
}

QStyleOptionViewItem GlodonTreeView::initOption()
{
    QStyleOptionViewItem option;
    option.init(this);
    option.state &= ~QStyle::State_MouseOver;
    option.font = font();

#ifndef Q_WS_MAC
    // On mac the focus appearance follows window activation
    // not widget activation
    if (!hasFocus())
        option.state &= ~QStyle::State_Active;
#endif

    option.state &= ~QStyle::State_HasFocus;
    if (iconSize().isValid())
    {
        option.decorationSize = iconSize();
    }
    else
    {
        int nPm = style()->pixelMetric(QStyle::PM_SmallIconSize, 0, this);
        option.decorationSize = QSize(nPm, nPm);
    }
    option.decorationPosition = QStyleOptionViewItem::Left;
    option.decorationAlignment = Qt::AlignCenter;
    option.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    option.textElideMode = textElideMode();
    option.rect = QRect();
    option.showDecorationSelected = (0 != style()->styleHint(QStyle::SH_ItemView_ShowDecorationSelected, 0, this));
    option.features = QStyleOptionViewItem::WrapText;
    option.locale = locale();
    option.locale.setNumberOptions(QLocale::OmitGroupSeparator);
    option.widget = this;
    return option;
}

void GlodonTreeView::afterExpandedChanged(const QModelIndex &index, bool expanded)
{
    G_UNUSED(index);
    G_UNUSED(expanded);
}

void GlodonTreeView::canDrag(QModelIndexList from, QModelIndex to,
                             Qt::KeyboardModifiers keyModifiers, bool &canMove, QCursor &cursor)
{
    emit canSectionMove(from, to, keyModifiers, canMove, cursor);
}

void GlodonTreeView::moveIndex(QModelIndexList from, QModelIndex to, Qt::KeyboardModifiers keyModifiers)
{
    emit sectionMove(from, to, keyModifiers);
    Q_UNUSED(from);
    Q_UNUSED(to);
    Q_UNUSED(keyModifiers);
    //todo 改变from对应的data中关于index的值到indexTo中对应的data的index
}

void GlodonTreeView::doExpanded(const QModelIndex &index)
{
//    if (!isExpanded(index))
//    {
//        expand(index);
        afterExpandedChanged(index, true);
//    }
}

void GlodonTreeView::doCollapsed(const QModelIndex &index)
{
//    if (isExpanded(index))
//    {
//        collapse(index);
        afterExpandedChanged(index, false);
//    }
}

void GlodonTreeView::autoResetScrollBarOnExpand(const QModelIndex &index)
{
    m_expandIndex = index;
    QModelIndex nextIndex = findLastModelIndex(index, index.parent(), 0);
    scrollTo(nextIndex);
}

void GlodonTreeView::afterCloseEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    G_UNUSED(editor);
    G_UNUSED(hint);
    if ((m_errCode > 0) || (m_errMessage.length() > 0))
    {
        QString strMessage = m_errMessage;
        int nErrCode = m_errCode;
        m_errCode = -1;
        m_errMessage = "";
        throw GLDException(strMessage, nErrCode);
    }
}

void GlodonTreeView::init()
{
    GLDTreeDefaultDelegaet *pDelegate = new GLDTreeDefaultDelegaet(this);
    setItemDelegate(pDelegate);
    connect(pDelegate, SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)), this, SLOT(afterCloseEditor(QWidget*, QAbstractItemDelegate::EndEditHint)));
    connect(this, SIGNAL(expanded(QModelIndex)), this, SLOT(doExpanded(QModelIndex)));
    connect(this, SIGNAL(collapsed(QModelIndex)), this, SLOT(doCollapsed(QModelIndex)));
}

void GlodonTreeView::setupIndexIndicator(QModelIndexList indexs, QPoint pt)
{
    m_indexFrom = indexs;
    m_ptStart = pt;
}

void GlodonTreeView::updateIndexIndicator(QPoint pt)
{
    QModelIndex targetIndex = indexAt(pt);
    if (m_pIndicator->isHidden())
    {
        QStyleOptionViewItem opt = initOption();
        opt.rect = visualRect(targetIndex);
        if (m_dragStyle == DotLineStyle)
        {
            QPixmap pm(this->viewport()->width(), 1);
            pm.fill(Qt::transparent);
            QPainter painter(&pm);
            QPen pen = painter.pen();
            pen.setWidth(1);
            pen.setColor(Qt::black);
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
            painter.drawLine(0, 0, pm.width(), pm.height());
            painter.end();
            m_pIndicator->setPixmap(pm);
        }
        else if (m_dragStyle == LabelStyle)
        {
            QPixmap pm(opt.rect.size());
            pm.fill(QColor(0, 0, 0, 45));
            opt.rect.adjust(0, -opt.rect.top(), 0, -opt.rect.top());

            QPainter painter(&pm);
            painter.setOpacity(0.6);

            int nPrevIndentation = indentation();
            if (nPrevIndentation > 0)
            {
                setIndentation(0);
            }
            drawRow(&painter, opt, m_indexFrom.at(0));
            if (nPrevIndentation > 0)
            {
                setIndentation(nPrevIndentation);
            }
            painter.end();

            m_pIndicator->resize(opt.rect.width(), opt.rect.height());
            m_pIndicator->setPixmap(pm);
        }
        m_pIndicator->show();
    }
    if (m_dragStyle == DotLineStyle)
    {
        QRect targetRect = visualRect(targetIndex);
        m_pIndicator->move(targetRect.bottomLeft());
    }
    else if (m_dragStyle == LabelStyle)
    {
        m_pIndicator->move(0, pt.y() - 10);
    }
}

QModelIndex GlodonTreeView::findLastModelIndex(const QModelIndex &index, const QModelIndex &parentIndex, int nCurrentHeight)
{
    //如果当前高度已经操作了viewport，就返回上一个节点
    if (nCurrentHeight + rowHeight(index) > viewport()->height())
    {
        return model()->index(index.row() - 1, 0, index.parent());
    }
    //如果当前节点已经是无效，且父是为即将要展开的节点，而且当前高度还超过viewport，那么就应该找当前展开节点的兄弟，或者父的兄弟
    else if (!index.isValid() && parentIndex == m_expandIndex
             && (nCurrentHeight < viewport()->height()))
    {
        QModelIndex nextValidSiblingIndex = index;
        QModelIndex currentParentIndex = parentIndex;
        while (true)
        {
            nextValidSiblingIndex = model()->index(currentParentIndex.row() + 1, 0, currentParentIndex.parent());
            currentParentIndex = currentParentIndex.parent();
            if (nextValidSiblingIndex.isValid() || currentParentIndex == QModelIndex())
            {
                break;
            }
        }
        if ((nCurrentHeight + rowHeight(nextValidSiblingIndex) > viewport()->height())
                || (!currentParentIndex.isValid() && !nextValidSiblingIndex.isValid()))
        {
            return model()->index(model()->rowCount(parentIndex) - 1, 0, parentIndex);
        }
        else
        {
            return nextValidSiblingIndex;
        }
    }
    else
    {
        //如果有子且为展开状态，则从第一个子开始找，否则，就找下一个兄弟
        if (model()->hasChildren(index))
        {
            if (isExpanded(index))
            {
                if (nCurrentHeight + rowHeight(index) > viewport()->height())
                {
                    return index;
                }
                else
                {
                    return findLastModelIndex(model()->index(0, 0, index), index, nCurrentHeight + rowHeight(index));
                }
            }
        }
        QModelIndex currentIndex = model()->index(index.row() + 1, 0, index.parent());
        return findLastModelIndex(currentIndex, index.parent(), rowHeight(index) + nCurrentHeight);
    }
}

void GlodonTreeView::setErrorMessage(int errCode, const QString &errMessage)
{
    m_errCode = errCode;
    m_errMessage = errMessage;
}

GLDTreeDefaultDelegaet::GLDTreeDefaultDelegaet(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void GLDTreeDefaultDelegaet::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    const_cast<GLDTreeDefaultDelegaet *>(this)->doSetModelData(editor, model, index);
}

/*!
 * \brief 在设置值的时候不允许抛出任何异常，暂时先将异常延后抛出。
 * \param editor
 * \param model
 * \param index
 */
void GLDTreeDefaultDelegaet::doSetModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index)
{
    try
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
    catch (const GLDException &e)
    {
        dynamic_cast<GlodonTreeView *>(parent())->setErrorMessage(e.errorCode(), e.message());
    }
    catch (...)
    {
    }
}
