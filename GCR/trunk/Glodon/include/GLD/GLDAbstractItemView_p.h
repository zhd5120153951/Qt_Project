#ifndef GLDABSTRACTITEMVIEW_P_H
#define GLDABSTRACTITEMVIEW_P_H

#include <private/qabstractscrollarea_p.h>
#include "GLDAbstractItemView.h"

class GLDTABLEVIEWSHARED_EXPORT GlodonAbstractItemViewPrivate : public QAbstractScrollAreaPrivate
{
    Q_DECLARE_PUBLIC(GlodonAbstractItemView)

public:
    GlodonAbstractItemViewPrivate();
    virtual ~GlodonAbstractItemViewPrivate();

public:
    void init();

    virtual void _q_rowsRemoved(const QModelIndex &parent, int start, int end);
    virtual void _q_rowsInserted(const QModelIndex &parent, int start, int end);
    virtual void _q_columnsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    virtual void _q_columnsRemoved(const QModelIndex &parent, int start, int end);
    virtual void _q_columnsInserted(const QModelIndex &parent, int start, int end);
    virtual void _q_modelDestroyed();
    virtual void _q_layoutChanged();
    void _q_headerDataChanged()
    {
        doDelayedItemsLayout();
    }

    void fetchMore();

    bool shouldEdit(GlodonAbstractItemView::EditTrigger trigger, const QModelIndex &index) const;
    bool shouldForwardEvent(GlodonAbstractItemView::EditTrigger trigger, const QEvent *event) const;
    bool shouldAutoScroll(const QPoint &pos) const;
    void doDelayedItemsLayout(int delay = 0);
    void doWheel();
    void interruptDelayedItemsLayout() const;

    void startAutoScroll()
    {
        int nScrollInterval = (m_verticalScrollMode == GlodonAbstractItemView::ScrollPerItem) ? 150 : 50;
        m_autoScrollTimer.start(nScrollInterval, q_func());
        m_autoScrollCount = 0;
    }

    void stopAutoScroll()
    {
        m_autoScrollTimer.stop();
        m_autoScrollCount = 0;
    }

#ifndef QT_NO_DRAGANDDROP
    virtual bool dropOn(QDropEvent *event, int *row, int *col, QModelIndex *index);
#endif
    bool droppingOnItself(QDropEvent *event, const QModelIndex &index);

    QWidget *editor(const QModelIndex &index, const QStyleOptionViewItem &options);
    bool sendDelegateEvent(const QModelIndex &index, QEvent *event) const;
    bool openEditor(const QModelIndex &index, QEvent *event);
    void updateEditorData(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    QItemSelectionModel::SelectionFlags multiSelectionCommand(const QModelIndex &index,
            const QEvent *event) const;
    QItemSelectionModel::SelectionFlags extendedSelectionCommand(const QModelIndex &index,
            const QEvent *event) const;
    QItemSelectionModel::SelectionFlags contiguousSelectionCommand(const QModelIndex &index,
            const QEvent *event) const;
    virtual void selectAll(QItemSelectionModel::SelectionFlags command);

    void setHoverIndex(const QPersistentModelIndex &index);

    void checkMouseMove(const QPersistentModelIndex &index);
    inline void checkMouseMove(const QPoint &pos)
    {
        checkMouseMove(q_func()->indexAt(pos));
    }

    QItemSelectionModel::SelectionFlags selectionBehaviorFlags() const;

#ifndef QT_NO_DRAGANDDROP
    virtual GlodonAbstractItemView::DropIndicatorPosition position(const QPoint &pos,
            const QRect &rect,
            const QModelIndex &idx) const;

    bool canDecode(QDropEvent *e) const;

    void paintDropIndicator(QPainter *painter);

#endif
    virtual GItemViewPaintPairs draggablePaintPairs(const QModelIndexList &indexes, QRect *r) const;

    virtual void adjustViewOptionsForIndex(QStyleOptionViewItem *, const QModelIndex &) const {}

    void releaseEditor(QWidget *editor) const;

    void executePostedLayout() const
    {
        if (m_delayedPendingLayout && m_state != GlodonAbstractItemView::CollapsingState)
        {
            interruptDelayedItemsLayout();
            const_cast<GlodonAbstractItemView *>(q_func())->doItemsLayout();
        }
    }

    inline void setDirtyRegion(const QRegion &visualRegion)
    {
        m_updateRegion += visualRegion;

        if (!m_updateTimer.isActive())
        {
            m_updateTimer.start(0, q_func());
        }
    }

    inline void scrollDirtyRegion(int dx, int dy)
    {
        m_scrollDelayOffset = QPoint(-dx, -dy);
        updateDirtyRegion();
        m_scrollDelayOffset = QPoint(0, 0);
    }

    inline void scrollContentsBy(int dx, int dy)
    {
        scrollDirtyRegion(dx, dy);
        viewport->scroll(dx, dy);
    }

    inline void scrollContentsBy(int dx, int dy, QRect rect)
    {
        scrollDirtyRegion(dx, dy);
        viewport->scroll(dx, dy, rect);
    }

    void updateDirtyRegion()
    {
        m_updateTimer.stop();
        viewport->update(m_updateRegion);
        m_updateRegion = QRegion();
    }

    void clearOrRemove();
    void checkPersistentEditorFocus();

    QPixmap renderToPixmap(const QModelIndexList &indexes, QRect *r) const;

    inline QPoint offset() const
    {
        Q_Q(const GlodonAbstractItemView);
        return QPoint(q->isRightToLeft() ? -q->horizontalOffset()
                      : q->horizontalOffset(), q->verticalOffset());
    }

    const GEditorInfo &editorForIndex(const QModelIndex &index) const;
    inline bool hasEditor(const QModelIndex &index) const
    {
        return m_indexEditorHash.find(index) != m_indexEditorHash.constEnd();
    }

    QModelIndex indexForEditor(QWidget *editor) const;
    void addEditor(const QModelIndex &index, QWidget *editor, bool isStatic);
    void removeEditor(QWidget *editor);

    inline bool isAnimating() const
    {
        return m_state == GlodonAbstractItemView::AnimatingState;
    }

    inline GlodonDefaultItemDelegate *delegateForIndex(const QModelIndex &index) const
    {
        GlodonDefaultItemDelegate *del;

        if ((del = m_rowDelegates.value(index.row(), 0)))
        {
            return del;
        }

        if ((del = m_columnDelegates.value(index.column(), 0)))
        {
            return del;
        }

        return m_itemDelegate;
    }

    inline bool isIndexValid(const QModelIndex &index) const
    {
        return (index.row() >= 0) && (index.column() >= 0) && (index.model() == m_model);
    }
    inline bool isIndexSelectable(const QModelIndex &index) const
    {
        return (Qt::NoItemFlags != (m_model->flags(index) & Qt::ItemIsSelectable));
    }
    inline bool isIndexEnabled(const QModelIndex &index) const
    {
        return (Qt::NoItemFlags != (m_model->flags(index) & Qt::ItemIsEnabled));
    }
    inline bool isIndexDropEnabled(const QModelIndex &index) const
    {
        return (Qt::NoItemFlags != (m_model->flags(index) & Qt::ItemIsDropEnabled));
    }
    inline bool isIndexDragEnabled(const QModelIndex &index) const
    {
        return (Qt::NoItemFlags != (m_model->flags(index) & Qt::ItemIsDragEnabled));
    }

    virtual bool selectionAllowed(const QModelIndex &index) const
    {
        return isIndexValid(index) && isIndexSelectable(index);
    }

    virtual QPoint contentsOffset() const
    {
        Q_Q(const GlodonAbstractItemView);
        return QPoint(q->horizontalOffset(), q->verticalOffset());
    }

    /**
     * For now, assume that we have few editors, if we need a more efficient implementation
     * we should add a QMap<GlodonDefaultItemDelegate*, int> member.
     */
    int delegateRefCount(const GlodonDefaultItemDelegate *delegate) const;

    /**
     * return true if the index is registered as a QPersistentModelIndex
     */
    inline bool isPersistent(const QModelIndex &index) const;

    QModelIndexList selectedDraggableIndexes() const;

    QStyleOptionViewItem viewOptionsV4() const;

    void doDelayedReset();

    void updateNoStaticEditorData(const QModelIndex &editorIndex);

private:
    // extendedSelectionCommand重构
    bool mouseMoveEventFlag(Qt::KeyboardModifiers &modifiers, const QEvent *event, QItemSelectionModel::SelectionFlags &selectionFlags) const;
    bool mouseButtonPressEventFlag(Qt::KeyboardModifiers &modifiers, const QModelIndex &index, const QEvent *event, QItemSelectionModel::SelectionFlags &selectionFlags) const;
    QItemSelectionModel::SelectionFlags mouseButtonReleaseEventFlag(
            Qt::KeyboardModifiers &modifiers, const QModelIndex &index, const QEvent *event) const;
    bool keyPressEventFlag(Qt::KeyboardModifiers &modifiers, const QEvent *event, QItemSelectionModel::SelectionFlags &selectionFlags) const;
    bool isNewSelect(Qt::KeyboardModifiers &modifiers, const QModelIndex &index, const QEvent *event) const;

public:
    QAbstractItemModel *m_model;
    QAbstractItemModel *m_dataModel;

    QPointer<GlodonDefaultItemDelegate> m_itemDelegate;
    QMap<int, QPointer<GlodonDefaultItemDelegate> > m_rowDelegates;
    QMap<int, QPointer<GlodonDefaultItemDelegate> > m_columnDelegates;
    QPointer<QItemSelectionModel> m_selectionModel;
    QItemSelectionModel::SelectionFlag m_ctrlDragSelectionFlag;
    bool m_noSelectionOnMousePress;

    GlodonAbstractItemView::SelectionMode m_selectionMode;
    GlodonAbstractItemView::SelectionBehavior m_selectionBehavior;

    GEditorIndexHash m_editorIndexHash;
    GIndexEditorHash m_indexEditorHash;
    QSet<QWidget *> m_persistent;
    QWidget *m_pCurCommittingEditor;

    QPersistentModelIndex m_enteredIndex;
    QPersistentModelIndex m_pressedIndex;
    Qt::KeyboardModifiers m_pressedModifiers;
    QPoint m_pressedPosition;
    bool m_pressedAlreadySelected;

    //forces the next mouseMoveEvent to send the viewportEntered signal
    //if the mouse is over the viewport and not over an item
    bool m_viewportEnteredNeeded;
    GlodonAbstractItemView::State m_state;
    GlodonAbstractItemView::State m_stateBeforeAnimation;
    GlodonAbstractItemView::EditTriggers m_editTriggers;
    GlodonAbstractItemView::EditTrigger m_lastTrigger;

    QPersistentModelIndex m_root;
    QPersistentModelIndex m_hover;

    bool m_bTabKeyNavigation;

#ifndef QT_NO_DRAGANDDROP
    bool m_bShowDropIndicator;
    QRect m_dropIndicatorRect;
    bool m_bDragEnabled;
    GlodonAbstractItemView::DragDropMode m_dragDropMode;
    bool m_bDragDropOverwrite;
    GlodonAbstractItemView::DropIndicatorPosition m_dropIndicatorPosition;
    Qt::DropAction m_defaultDropAction;
#endif

#ifdef QT_SOFTKEYS_ENABLED
    QAction *doneSoftKey;
#endif

    QString m_keyboardInput;
    QElapsedTimer m_keyboardInputTime;

    bool m_bAutoScroll;
    QBasicTimer m_autoScrollTimer;
    int m_nAutoScrollMargin;
    int m_autoScrollCount;
    bool m_shouldScrollToCurrentOnShow; //used to know if we should scroll to current on show event
    bool m_shouldClearStatusTip; //if there is a statustip currently shown that need to be cleared when leaving.

    bool m_bAlternatingColors;

    QSize m_oIconSize;
    Qt::TextElideMode m_textElideMode;

    QRegion m_updateRegion; // used for the internal update system
    QPoint m_scrollDelayOffset;

    QBasicTimer m_updateTimer;
    QBasicTimer m_delayedAutoScroll; //used when an item is clicked
//    QBasicTimer delayedDragSelect; //used when mouse is moving fo multi selecting
    QBasicTimer m_delayedReset;

    QBasicTimer m_delayedShowToolTip;
    int m_nDelayedHintTime;

    GlodonAbstractItemView::ScrollMode m_verticalScrollMode;
    GlodonAbstractItemView::ScrollMode m_horizontalScrollMode;

    bool m_bCurrentIndexSet;

    bool m_bWrapItemText;
    mutable bool m_delayedPendingLayout;
    bool m_moveCursorUpdatedView;
    bool m_bGridTextIncludeLeading; //格子区域中的文字绘制时是否用包含行间距
    bool m_bScrollBarTracking;   //鼠标拖拽滚动条时，是否在松开滚动条时刻才进行表格区域的滚动
    bool m_bAllowSelectAll;//允许全选
    bool m_isSearchModel;//是否允许当满足定位时，进行内容定位
    bool m_bMouseMoveRefresh;//鼠标移动时，是否刷新鼠标当前覆盖的格子和之前的格子

    QModelIndex m_oEditorIndex;
    QColor m_oGridColor;

    mutable QBasicTimer m_delayedLayout;
    mutable QBasicTimer m_fetchMoreTimer;

    GToolTipFrame *m_pToolTipFrame;
    GlodonAbstractItemView::EnterMovingState m_prevState;

    bool m_bShowIndexContent;//是否显示格子内容
    bool m_ignoreActiveWindowFocusReason;
    bool m_alwaysShowEditorPro;//总是处于编辑状态，一开始就创建出全部的控件

    QWidget *m_currentEditor;//配合alwaysShowEditorPro使用，每次创建之后，用该变量存储创建出来的editor，并且加到m_persisten里面

                             // 标识当前tableView是否有多块选择区域，在有合并格或者行列移动时，
    bool m_bIsInMultiSelect; // selectionModel中的selectionRange个数不能作为选择区域个数的标识

};

#endif // GLDABSTRACTITEMVIEW_P_H
