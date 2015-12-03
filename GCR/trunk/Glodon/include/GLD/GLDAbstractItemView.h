/*!
 *@file glodonabstractitemview.h
 *@brief {所有View的抽象}
 *定义了一些主要流程，如编辑、鼠标事件、setModel等
 *@author Gaosy
 *@date 2012.9.7
 *@remarks {remarks}
 *Copyright (c) 1998-2012 Glodon Corporation
 */

#ifndef GLDABSTRACTITEMVIEW_H
#define GLDABSTRACTITEMVIEW_H

#include <QObject>
#include <QPointer>
#include <QAbstractItemModel>
#include <QApplication>
#include <QEvent>
#include <QMimeData>
#include <QPainter>
#include <QPair>
#include <QRegion>
#include <QPainter>
#include <QBasicTimer>
#include <QElapsedTimer>
#include <QModelIndex>
#include <QDialog>
#include <QAbstractScrollArea>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include "GLDString.h"
#include "GLDEvent.h"
#include "GLDDefaultItemDelegate.h"
#include "GLDTextEdit.h"
#include "GLDCommentFrame.h"

class QLabel;
class QMenu;
class QDrag;
class QEvent;
class GlodonAbstractItemView;
class GlodonAbstractItemViewPrivate;

/*!   
 *@struct: GEditorInfo
 *@brief {缓存编辑方式，用于快速存取编辑所需控件}
 *@author Gaosy 
 *@date 2012.9.7
 */
struct GEditorInfo {
    GEditorInfo(QWidget *e, bool s): widget(QPointer<QWidget>(e)), isStatic(s) {}

    GEditorInfo(): isStatic(false) {}

    QPointer<QWidget> widget;
    bool isStatic;
};

typedef QHash<QWidget *, QPersistentModelIndex> GEditorIndexHash;
typedef QHash<QPersistentModelIndex, GEditorInfo> GIndexEditorHash;

typedef QPair<QRect, QModelIndex> GItemViewPaintPair;
typedef QList<GItemViewPaintPair> GItemViewPaintPairs;

/*!   
 *@class: GEmptyModel
 *@brief {空Model，没有数据，仅用于初始化时使用}
 *@author Gaosy 
 *@date 2012.9.7
 */
class GLDTABLEVIEWSHARED_EXPORT GEmptyModel : public QAbstractItemModel
{
public:
    explicit GEmptyModel(QObject *parent = 0) : QAbstractItemModel(parent) {}

    QModelIndex index(int, int, const QModelIndex &) const { return QModelIndex(); }

    QModelIndex parent(const QModelIndex &) const { return QModelIndex(); }

    int rowCount(const QModelIndex &) const { return 0; }

    int columnCount(const QModelIndex &) const { return 0; }

    bool hasChildren(const QModelIndex &) const { return false; }

    QVariant data(const QModelIndex &, int) const { return QVariant(); }
};

/*!   
 *@class: GlodonAbstractItemView
 *@brief {所有View的抽象父类，定义了数据编辑、鼠标事件、数据装载（setModel）、选择、移动焦点、设置编辑方式等功能（流程）}
 *@author Gaosy 
 *@date 2012.9.7
 */
class GLDTABLEVIEWSHARED_EXPORT GlodonAbstractItemView : public QAbstractScrollArea
{
    Q_OBJECT

    Q_ENUMS(SelectionMode SelectionBehavior ScrollHint ScrollMode DragDropMode)
    Q_FLAGS(EditTriggers)
    Q_PROPERTY(bool autoScroll READ hasAutoScroll WRITE setAutoScroll)
    Q_PROPERTY(int autoScrollMargin READ autoScrollMargin WRITE setAutoScrollMargin)
    Q_PROPERTY(EditTriggers editTriggers READ editTriggers WRITE setEditTriggers)
    Q_PROPERTY(bool tabKeyNavigation READ tabKeyNavigation WRITE setTabKeyNavigation)
#ifndef QT_NO_DRAGANDDROP
    Q_PROPERTY(bool showDropIndicator READ showDropIndicator WRITE setDropIndicatorShown)
    Q_PROPERTY(bool dragEnabled READ dragEnabled WRITE setDragEnabled)
    Q_PROPERTY(bool dragDropOverwriteMode READ dragDropOverwriteMode WRITE setDragDropOverwriteMode)
    Q_PROPERTY(DragDropMode dragDropMode READ dragDropMode WRITE setDragDropMode)
    Q_PROPERTY(Qt::DropAction defaultDropAction READ defaultDropAction WRITE setDefaultDropAction)
#endif
    Q_PROPERTY(bool alternatingRowColors READ alternatingRowColors WRITE setAlternatingRowColors)
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(SelectionBehavior selectionBehavior READ selectionBehavior WRITE setSelectionBehavior)
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
    Q_PROPERTY(Qt::TextElideMode textElideMode READ textElideMode WRITE setTextElideMode)
    Q_PROPERTY(ScrollMode verticalScrollMode READ verticalScrollMode WRITE setVerticalScrollMode)
    Q_PROPERTY(ScrollMode horizontalScrollMode READ horizontalScrollMode WRITE setHorizontalScrollMode)

public:
    /*!
     *@enum: SelectionMode
     *@brief {选择模式}
     *@author Gaosy
     *@date 2012.9.10
     */
    enum SelectionMode {
        NoSelection,
        SingleSelection,
        MultiSelection,
        ExtendedSelection,
        ContiguousSelection
    };

    /*!
     *@enum: SelectionBehavior
     *@brief {选择行为：选格子、选行、选列}
     *@author Gaosy
     *@date 2012.9.10
     */
    enum SelectionBehavior {
        SelectItems,
        SelectRows,
        SelectColumns
    };

    /*!
     *@enum: ScrollHint
     *@brief {滚动条滚动位置控制}
     *@author Gaosy
     *@date 2012.9.10
     */
    enum ScrollHint {
        EnsureVisible,
        PositionAtTop,
        PositionAtBottom,
        PositionAtCenter
    };

    /*!
     *@enum: EditTrigger
     *@brief {进入编辑状态的方式}
     *@author Gaosy
     *@date 2012.9.10
     */
    enum EditTrigger {
        NoEditTriggers = 0,
        CurrentChanged = 1,
        DoubleClicked = 2,
        SelectedClicked = 4,
        EditKeyPressed = 8,
        AnyKeyPressed = 16,
        AllEditTriggers = 31
    };

    Q_DECLARE_FLAGS(EditTriggers, EditTrigger)

    /*!
     *@enum: ScrollMode
     *@brief {滚动方式：逐个格子滚动、逐个像素滚动}
     *@author Gaosy
     *@date 2012.9.10
     */
    enum ScrollMode {
        ScrollPerItem,  // 滚没一格，即一格格子由可见变为不可见
        ScrollPerPixel
    };

    // 非编辑状态下，只要显示编辑方式，则显示即可操作，不需要获取焦点
    enum EditStyleDrawType {
        SdtNone = 0,         // 不显示编辑方式
        SdtCurrentCell = 1,  // 显示焦点所在格子的编辑方式
        SdtCurrentRow = 2,   // 显示焦点所在行的编辑方式
        SdtCurrentCol = 3,   // 显示焦点所在列的编辑方式
        SdtAll = 4           // 所有的编辑方式都显示
    };
    /*!
     *@enum: DragDropMode
     *@brief {拖拽方式}
     *@author Gaosy
     *@date 2012.9.10
     */
    enum DragDropMode {
        NoDragDrop,
        DragOnly,
        DropOnly,
        DragDrop,
        InternalMove
    };

    /*!
     *@enum: State
     *@brief {当前View的状态}
     *@author Gaosy
     *@date 2012.9.10
     */
    enum State {
        NoState,              // 处于默认状态
        DraggingState,        // 正在拖拽Item
        DragSelectingState,   // 正在选择Item
        EditingState,         // 正在编辑Item
        ExpandingState,       // 正在展开节点
        CollapsingState,      // 正在折叠节点
        AnimatingState,       // View处于动画状态
        RangeFillingState,    // 正在进行拖拉复制
        RangeMovingState,     // 正在进行框选拖拽
        DragResizingState     // 正在拖拽改变大小
    };

    /*!
     *@enum: State
     *@brief {跳格开关开启时当前格子的状态}
     *@author Chenq-a
     *@date 2013.9.5
     */
    enum EnterMovingState {
        FirstFocus,   // 第一次获得格子焦点
        Editting,     // 进入了编辑框
        LastFocus     // 从编辑框退出时获得焦点的临时状态
    };

    /*!
     *@enum: CursorAction
     *@brief {移动焦点的方向}
     *@author Gaosy
     *@date 2012.9.13
     */
    enum CursorAction {
        MoveUp,
        MoveDown,
        MoveLeft,
        MoveRight,
        MoveHome,
        MoveEnd,
        MovePageUp,
        MovePageDown,
        MoveNext,
        MovePrevious
    };

public:
    explicit GlodonAbstractItemView(QWidget *parent = 0);
    ~GlodonAbstractItemView();

public:
    // 一、Qt方法
    // 1. 功能选项
    /**
     * @brief QAbstractItemView方法，查找到最符合搜索值的Item，移动过去并选中。
     *        如果搜索的值为空，会重置搜索
     * @param search 要查找的值
     */
    virtual void keyboardSearch(const QString &search);

    /**
     * @brief QAbstractItemView方法，设置输入法操作
     * @param query 各种控制，例如字体等
     */
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

    /**
     * @brief QAbstractItemView方法，设置Item的Tab、backTab是有可用
     * @param enable
     */
    void setTabKeyNavigation(bool enable);
    bool tabKeyNavigation() const;

#ifndef QT_NO_DRAGANDDROP
    /**
     * @brief QAbstractItemView方法，设置ViewPort是否允许拖拽它的Item
     * @param enable
     */
    void setDragEnabled(bool enable);
    bool dragEnabled() const;

    /**
     * @brief QAbstractItemView方法，设置Drag\Drop时，ViewPort的操作
     * @param behavior
     */
    void setDragDropMode(DragDropMode behavior);
    DragDropMode dragDropMode() const;

    /**
     * @brief QAbstractItemView方法，控制ViewPort上的Item在拖动释放时的操作
     *        如果是true，则将原来的Item替换为拖动的Item
     *        如果是false，则插入一个新的Item
     * @param overwrite
     */
    void setDragDropOverwriteMode(bool overwrite);
    bool dragDropOverwriteMode() const;

    /**
     * @brief QAbstractItemView方法，设置默认的Drop操作
     * @param dropAction
     */
    void setDefaultDropAction(Qt::DropAction dropAction);
    Qt::DropAction defaultDropAction() const;

    /**
     * @brief QAbstractItemView方法，设置在拖拽Item和结束拖拽Item时，是否显示提示
     * @param enable
     */
    void setDropIndicatorShown(bool enable);
    bool showDropIndicator() const;
#endif

    // 2. 外观选项
    /**
     * @brief QAbstractItemView方法，返回QStyleOptionViewItem结构体，里面包含字体等
     */
    virtual QStyleOptionViewItem viewOptions() const;

    /**
     * @brief QAbstractItemView方法纯虚，将滚动条滚至给定的Index
     * @param index 目标Index
     * @param hint
     */
    virtual void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) = 0;

    /**
     * @brief 设置选择模式，可以为单选，多选等
     * @param mode
     */
    void setSelectionMode(GlodonAbstractItemView::SelectionMode mode);
    GlodonAbstractItemView::SelectionMode selectionMode() const;

    /**
     * @brief 设置选择模式，行选，或者列选
     * @param behavior 默认值 SelectItems
     */
    void setSelectionBehavior(GlodonAbstractItemView::SelectionBehavior behavior);
    GlodonAbstractItemView::SelectionBehavior selectionBehavior() const;

    /**
     * @brief 设置垂直滚动条的滚动模式（逐个格子滚动、逐个像素滚动）
     * @param mode
     */
    void setVerticalScrollMode(ScrollMode mode);
    ScrollMode verticalScrollMode() const;

    /**
     * @brief 设置水平滚动条的滚动模式（逐个格子滚动、逐个像素滚动）
     * @param mode
     */
    void setHorizontalScrollMode(ScrollMode mode);
    ScrollMode horizontalScrollMode() const;

    /*!
     *@brief 设置隔行背景色不同
     *@param[in]  enable
     *@return 无
     */
    void setAlternatingRowColors(bool enable);
    bool alternatingRowColors() const;

    /*!
     *@brief 设置Item图标大小
     *@param[in]  enable
     *@return 无
     */
    void setIconSize(const QSize &size);
    QSize iconSize() const;

    /**
     * @brief 获取/设置，自动滚动区域
     * @return
     */
    void setAutoScrollMargin(int margin);
    int autoScrollMargin() const;

    /**
     * @brief 点击右键的时候设置tooltip隐藏
     * @return 无
     */
    void setToolTipFrameHide();

public:
    // Glodon方法
    // 1. 功能选项
    /**
     * @brief 设置是否选择移动
     * @param value
     */
    virtual void setAllowRangeMoving(bool value);
    virtual bool allowRangeMoving() const;

    /**
     * @brief 是否能开始drag
     * @param indexes
     * @return
     */
    virtual bool canStartDrag(QModelIndexList indexes);

    // 2. 外观选项
    /**
     * @brief 设置是否允许全选
     * @return
     */
    void setAllowSelectAll(bool value);
    bool allowSelectAll() const;

    /**
     * @brief 格子区域中的文字绘制时是否用包含行间距
     * @return
     */
    void setGridTextIncludeLeading(bool value);
    bool gridTextIncludeLeading() const;

    /**
     * @brief 鼠标拖拽滚动条时，是否在松开滚动条时刻才进行表格区域的滚动
     * @param value
     */
    void setScrollBarTracking(bool value);
    bool scrollBarTracking() const;

    /**
     * @brief 设置表格ViewPort背景颜色
     * @return
     */
    virtual void setGridColor(QColor value);
    QColor gridColor() const;

    /**
     * @brief 设置当格子内容显示不全时，是否显示提示框
     * @param value
     * @param showIndexContext 如果toolTipRole没有值，是否显示格子中的值
     */
    void setIsShowHint(bool value, bool showIndexContent = false);
    void setShowHintDelay(int time);

    /**
     * @brief 获取/设置，竖直方向滚动条的值
     * @return
     */
    void setVerticalScrollBarValue(int value);
    int verticalScrollbarValue();

    /**
     * @brief 设置是否完全显示当前点击的格子，主要用于控制在当前格子部分显示时的点击行为，即:是否调整滚动条，使该格完全显示
     * @param value
     * @note 复杂layout情形下，没有滚动条时的显示不完全点击后不会有效果
     */
    void setShowPressIndex(bool value);

    /**
     * @brief 获取/设置，横向滚动条的值
     * @return
     */
    void setHorizontalScrollbarValue(int value);
    int horizontalScrollbarValue();

    /**
     * @brief 得到StyleOptionViewItem
     */
    QStyleOptionViewItem StyleOptionViewItem();

public:
    virtual QWidget *initCustomComment(const QModelIndex &index, GString &text);
    virtual QRect visualRect(const QModelIndex &index) const = 0;
    virtual QModelIndex indexAt(const QPoint &point) const = 0;
    GCommentFrame *commentFrame();

public:
    virtual void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    QSize sizeHintForIndex(const QModelIndex &index) const;
    virtual int sizeHintForRow(int row) const;
    virtual int sizeHintForColumn(int column) const;

    void setDataModel(QAbstractItemModel *dataModel);
    QAbstractItemModel *dataModel() const;

    /*!
     *设置QItemselectionModel，用于记录当前已选择区域
     *@param[in]  selectionModel
     *@return 无
     */
    virtual void setSelectionModel(QItemSelectionModel *selectionModel);
    QItemSelectionModel *selectionModel() const;

    /*!
     *设置QAbstractItemDelegate，即编辑方式，目前默认初始化为GlodonDefaultItemDelegate
     *如果需要自行设置编辑方式，请从GlodonDefaultItemDelegate派生
     *其优先级低于rowDelegate和columnDelegate，默认情况下rowDelegate和columnDelegate为空
     *@param[in]  delegate
     *@return 无
     */
    void setItemDelegate(GlodonDefaultItemDelegate *delegate);
    GlodonDefaultItemDelegate *itemDelegate() const;

    QModelIndex currentIndex() const;
    QModelIndex rootIndex() const;


    void openPersistentEditor(const QModelIndex &index);
    void closePersistentEditor(const QModelIndex &index);

    /*!
     *获取给定index的QAbstractItemDelegate，优先级顺序为
     *rowDelegate > columnDelegate > itemDelegate
     *@param[in]  index
     *@return QAbstractItemDelegate*
     */
    GlodonDefaultItemDelegate *itemDelegate(const QModelIndex &index) const;

    /**
     * @brief 正处于编辑状态的ModelIndex
     * @return
     */
    QModelIndex editorModelIndex();

    /**
     * @brief 设置进入编辑状态的方式
     * @param triggers
     */
    void setEditTriggers(EditTriggers triggers);
    EditTriggers editTriggers() const;

    void setAutoScroll(bool enable);
    bool hasAutoScroll() const;

    /*!
     *为给定的index设置持久性的编辑方式，与delegate的编辑方式的区别在于：
     *这种方式的编辑控件widget持续显示，而不会随着退出编辑状态而隐藏
     *@param[in]  index
     *@param[in]  widget
     *@return 无
     */
    void setIndexWidget(const QModelIndex &index, QWidget *widget);
    QWidget *indexWidget(const QModelIndex &index) const;

    /*!
     *为给定逻辑行设置编辑方式QAbstractItemDelegate，其优先级高于columnDelegate和itemDelegate
     *@param[in]  row       逻辑行号
     *@param[in]  delegate
     *@return 无
     */
    void setItemDelegateForRow(int row, GlodonDefaultItemDelegate *delegate);
    GlodonDefaultItemDelegate *itemDelegateForRow(int row) const;

    /*!
     *为给定逻辑列设置编辑方式QAbstractItemDelegate，其优先级低于rowDelegate但高于itemDelegate
     *@param[in]  column    逻辑列号
     *@param[in]  delegate
     *@return 无
     */
    void setItemDelegateForColumn(int column, GlodonDefaultItemDelegate *delegate);
    GlodonDefaultItemDelegate *itemDelegateForColumn(int column) const;

    /*!
     * \brief 设置总是处于编辑状态（调用完后，tableView会把所有的编辑方式都创建出来，请在setModel之后或dataSource激活后调用）
     * \return
     */
    bool alwaysShowEditorPro();
    void setAlwaysShowEditorPro(bool value);

    /**
     * @brief 是否允许搜索定位
     * @param value
     */
    void setIsSearchModel(bool value);
    bool isSearchModel();

    void setTextElideMode(Qt::TextElideMode mode);
    Qt::TextElideMode textElideMode() const;

#ifdef Q_NO_USING_KEYWORD
    inline void update() { QAbstractScrollArea::update(); }
#else
    using QAbstractScrollArea::update;
#endif

    /**
     * @brief 判断是否是拖拽选择状态
     * @return
     */
    bool dragSelectingState(){return state() == DragSelectingState;}

Q_SIGNALS:
    void pressed(const QModelIndex &index);
    void clicked(const QModelIndex &index);
    void doubleClicked(const QModelIndex &index);

    void activated(const QModelIndex &index);
    void entered(const QModelIndex &index);
    void viewportEntered();
    void onQueryImageRatioMode(const QModelIndex &index, GlodonDefaultItemDelegate::GAspectRatioMode &ratioMode);
    void onQueryFloatOrDoubleViewFormat(const QModelIndex &index, QString &text);
    void onQueryIndexDataType(const QModelIndex &index, GlodonDefaultItemDelegate::GDataType &dataType);
    void onCommitEditor(const QModelIndex &index, QVariant data, bool &commit);
    void onCloseEditor(const QModelIndex &index, QVariant data, bool &close);
    void onPressEnter(GlodonAbstractItemView::CursorAction &direction);
    void onbeforeMoveCurrent(const QModelIndex &oldIndex, const QModelIndex &newIndex,
                             QItemSelectionModel::SelectionFlags &command,
                             MoveReason moveReason, bool &canMove);
    void onAfterMoveCurrent(const QModelIndex &oldIndex, const QModelIndex &newIndex,
                            QItemSelectionModel::SelectionFlags &command, MoveReason moveReason);
    void canShowComment(const QModelIndex &index, bool &canShow);

    void onEditorCanCopy(bool yes);
    void onEditorCanCut(bool yes);
    void onEditorCanPaste(bool yes);
    void onEditorCanDelete(bool yes);
    void isInEditing(bool yes);

    void onEditorCopy();
    void onEditorCut();
    void onEditorPaste();
    void onEditorDelete();

public Q_SLOTS:
    /*!
     *重置整个View
     *@return 无
     */
    virtual void reset();

    virtual void setRootIndex(const QModelIndex &index);
    virtual void doItemsLayout();

    /*!
     *选中整个View
     *@return 无
     */
    virtual void selectAll();

    /*!
     *给定的index格子进入编辑状态
     *@param[in]  index
     *@return 无
     */
    virtual void edit(const QModelIndex &index);

    /*!
     *清除所有选择
     *@return 无
     */
    void clearSelection();

    /*!
     *设置给定的index为焦点
     *@param[in]  index
     *@return 无
     */
    virtual void setCurrentIndex(const QModelIndex &index);

    void scrollToTop();
    void scrollToBottom();

    /*!
     *更新index所在格子
     *@param[in]  index
     *@return 无
     */
    virtual void update(const QModelIndex &index);

    /**
     * @brief 提交数据并关闭编辑器
     */
    bool commitDataAndCloseEditor();

    void showToolTip(const QModelIndex &index);

    void editorCopy();
    void editorCut();
    void editorPaste();
    void editorDelete();

protected Q_SLOTS:
    virtual void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);
    virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    virtual void updateEditorData();
    virtual void updateEditorGeometries();
    virtual void updateGeometries();
    virtual void verticalScrollbarAction(int action);
    virtual void horizontalScrollbarAction(int action);
    virtual void verticalScrollbarValueChanged(int value);
    virtual void horizontalScrollbarValueChanged(int value);
    virtual void closeEditor(QWidget *editor, bool &canCloseEditor, GlodonDefaultItemDelegate::EndEditHint hint);
    virtual void commitData(QWidget *editor, bool &canCloseEditor);
    virtual void editorDestroyed(QObject *editor);

    virtual void doSelectionChanged();
    virtual void doEditorCopyAvailable(bool);
    virtual void doCursorPositionChanged();
    virtual void doCursorPositionChanged(int, int);

protected:
    GlodonAbstractItemView(GlodonAbstractItemViewPrivate &dd, QWidget *parent = 0);

    void setHorizontalStepsPerItem(int steps);
    int horizontalStepsPerItem() const;
    void setVerticalStepsPerItem(int steps);
    int verticalStepsPerItem() const;

    virtual bool moveCurrent(const QModelIndex &oldIndex, const QModelIndex &newIndex,
                             QItemSelectionModel::SelectionFlags command, MoveReason moveReason);

    virtual void beforeMoveCurrent(const QModelIndex &oldIndex, const QModelIndex &newIndex,
                                   QItemSelectionModel::SelectionFlags command, MoveReason moveReason, bool &canMove);
    virtual void doMoveCurrent(const QModelIndex &oldIndex, const QModelIndex &newIndex,
                               QItemSelectionModel::SelectionFlags command, MoveReason moveReason);
    virtual void afterMoveCurrent(const QModelIndex &oldIndex, const QModelIndex &newIndex,
                                  QItemSelectionModel::SelectionFlags command, MoveReason moveReason);

    virtual QModelIndex moveCursor(CursorAction cursorAction,
                                   Qt::KeyboardModifiers modifiers) = 0;

    virtual int horizontalOffset() const = 0;
    virtual int verticalOffset() const = 0;

    virtual bool isIndexHidden(const QModelIndex &index) const = 0;

    virtual void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command) = 0;
    virtual QRegion visualRegionForSelection(const QItemSelection &selection) const = 0;
    virtual QModelIndexList selectedIndexes() const;

    virtual bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event);

    virtual QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index,
                                                                 const QEvent *event = 0) const;
    virtual bool isLegalData();
    virtual void setLegalData(bool value);

#ifndef QT_NO_DRAGANDDROP
    virtual void startDrag(Qt::DropActions supportedActions);
#endif

    State state() const;
    void setState(State state);

    void scheduleDelayedItemsLayout();
    void executeDelayedItemsLayout();

    void setDirtyRegion(const QRegion &region);
    void scrollDirtyRegion(int dx, int dy);

    void startAutoScroll();
    void stopAutoScroll();
    void doAutoScroll();

    bool focusNextPrevChild(bool next);
    bool event(QEvent *event);
    bool viewportEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

#ifndef QT_NO_DRAGANDDROP
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
#endif

    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void timerEvent(QTimerEvent *event);
    void inputMethodEvent(QInputMethodEvent *event);

    bool inLinkCell(const QPoint &pos);

#ifndef QT_NO_DRAGANDDROP
    enum DropIndicatorPosition { OnItem, AboveItem, BelowItem, OnViewport };
    DropIndicatorPosition dropIndicatorPosition() const;
#endif

    bool setNewCurrentSelection(QKeyEvent *event, QPersistentModelIndex oNewCurrent);

    /**
     * @brief 同步一些标记到Delegate里面
     */
    virtual void syncStateToDelegate(GlodonDefaultItemDelegate *delegate);

    /*!
     *在重置View前的操作
     *@return 无
     */
    virtual void beforeReset();

    /*!
     *重置View
     *@return 无
     */
    virtual void doReset();

    /*!
     *执行View重置后的操作
     *@return 无
     */
    virtual void afterReset();

protected:
    void doGMFirst(GLDEvent *event);
    void doGMQueryFirst(GLDEvent *event);
    void doGMPrev(GLDEvent *event);
    void doGMQueryPrev(GLDEvent *event);
    void doGMNext(GLDEvent *event);
    void doGMQueryNext(GLDEvent *event);
    void doGMLast(GLDEvent *event);
    void doGMQueryLast(GLDEvent *event);
    void doGMSetSel(GLDEvent *event);
    void doGMQuerySetSel(GLDEvent *event);
    void doGMSetColRow(GLDEvent *event);

private:
    void controlEditorNormalActions(const QModelIndex &index);
    void setEditorActSignals(QWidget *editor);
    bool connectEditorSignal(QWidget *sender, const char *name,
                             QWidget *receiver, const char *slotName);
    bool editorHasSelText(QWidget *editor);
    bool editorReadOnly(QWidget *editor);
    void resetActAvailable(QWidget *editor);
    void disableEditorActions();

    bool isComment(const QPersistentModelIndex &index);

    /**
     * @brief 给特殊的edit(Edit内部还有其他的控件会印象eventFilter，比如viewport)安装EventFilter
     * @param widget
     * @param delegate
     */
    void installEventFilterWidthEdit(QWidget *widget, GlodonDefaultItemDelegate *delegate);

    // mousePressEvent重构
    void dealWithSelectionAndAutoScroll(QModelIndex index, QMouseEvent *event);

    // KeyPressEvent重构
#ifdef QT_KEYPAD_NAVIGATION
    void dealWithKeyPadNavigation(QKeyEvent *event);
#endif

    QPersistentModelIndex getNewCurrentAccordingToKeyPressOperation(QKeyEvent *event);
    void scrollToNewCurrent(QKeyEvent *event, QPersistentModelIndex &oNewCurrent);
    void dealWithEventAcception(QKeyEvent *event);

    // mouseMoveEvent重构
    void setSelectionAndScrollToOnMouseMove(
            const QModelIndex &oCurIndex, QMouseEvent *event, const QPoint &oPrePoint, const QPoint &oCurPoint);

    // TimerEvent重构
    void showCurIndexToolTip();
    void doLayoutAndScroll();
    // doAutoScroll重构
    void scrollVerticalScrollBar(int nVerticalValue, QScrollBar *verticalScroll);
    void scrollHorizontalScrollBar(int nHorizontalValue, QScrollBar *horizontalScroll);

    // CommitEditor重构
    void removeNonPersistentEditor(QWidget *editor);
    void setEditorFocus(QWidget *editor);
    void dealWithEndEditHint(GlodonDefaultItemDelegate::EndEditHint hint);

    void resetEditorFocus();

    /*!
     * \brief 处理提交数据和关闭编辑器,未能关闭编辑器时,则将编辑中文本全部选中
     * \param canCloseEditor
     */
    void doCommitDataAndCloseEditor(bool &canCloseEditor);

    /**
     * @brief 通过delegate向Model中写入数据
     * @param[in]  pDelegate
     * @param[in]  editor
     * @param[in]  index
     * @param[in]  canCloseEditor
     */
    void setModelData(GlodonDefaultItemDelegate *pDelegate, QWidget *editor,
                      const QModelIndex & index, bool &canCloseEditor);

    /**
     * @brief 设置数据的合法性
     * @param[in]  pDelegate
     * @param[in]  canCloseEditor
     */
    void setLegitimacyOfData(GlodonDefaultItemDelegate *pDelegate, bool & canCloseEditor);

private Q_SLOTS:
    void _q_columnsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void _q_columnsRemoved(const QModelIndex &index, int start, int end);
    void _q_columnsInserted(const QModelIndex &index, int start, int end);
    void _q_rowsInserted(const QModelIndex &index, int start, int end);
    void _q_rowsRemoved(const QModelIndex &index, int start, int end);
    void _q_modelDestroyed();
    void _q_layoutChanged();
    void _q_headerDataChanged();

private:
    Q_DECLARE_PRIVATE(GlodonAbstractItemView)
    Q_DISABLE_COPY(GlodonAbstractItemView)

    friend class QAccessibleItemRow;
    friend class QListModeViewBase;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GlodonAbstractItemView::EditTriggers)

class GLDTABLEVIEWSHARED_EXPORT GAbstractItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    friend class GlodonAbstractItemViewPrivate;
};

enum GLDScrollBarType
{
    gsbtAll = 0,
    gsbtVertical = 1,
    gsbtHorizon = 2
};

#endif // GLDABSTRACTITEMVIEW_H
