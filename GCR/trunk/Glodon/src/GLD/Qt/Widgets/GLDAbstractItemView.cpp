#include "GLDAbstractItemView.h"
#include "GLDAbstractItemView_p.h"

#include <qapplication.h>
#include <qclipboard.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qdrag.h>
#include <qevent.h>
#include <qscrollbar.h>
#include <qwhatsthis.h>
#include <qtooltip.h>
#include <qdatetime.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qstyleditemdelegate.h>
#include <qstylepainter.h>
#include <private/qabstractitemmodel_p.h>
#include <qmetaobject.h>
#ifndef QT_NO_ACCESSIBILITY
#include <qaccessible.h>
#endif
//#include <private/qsoftkeymanager_p.h>
#include <QStyleOption>
#include <QLabel>
#include <QPaintEngine>

#include "GLDString.h"
#include "GLDGlobal.h"
#include "GLDAbstractItemModel.h"
#include "GLDEvent.h"
#include "GLDStrUtils.h"
#include "GLDWindowComboBox.h"
#include "GLDWindowComboBoxEx.h"
#include "GLDEllipsis.h"
#include "GLDHeaderView.h"

GlodonAbstractItemViewPrivate::GlodonAbstractItemViewPrivate()
    :   m_model(QAbstractItemModelPrivate::staticEmptyModel()),
        m_dataModel(0),
        m_itemDelegate(0),
        m_selectionModel(0),
        m_ctrlDragSelectionFlag(QItemSelectionModel::NoUpdate),
        m_noSelectionOnMousePress(false),
        m_selectionMode(GlodonAbstractItemView::ExtendedSelection),
        m_selectionBehavior(GlodonAbstractItemView::SelectItems),
        m_pCurCommittingEditor(0),
        m_pressedModifiers(Qt::NoModifier),
        m_pressedPosition(QPoint(-1, -1)),
        m_pressedAlreadySelected(false),
        m_viewportEnteredNeeded(false),
        m_state(GlodonAbstractItemView::NoState),
        m_stateBeforeAnimation(GlodonAbstractItemView::NoState),
        m_editTriggers(GlodonAbstractItemView::DoubleClicked | GlodonAbstractItemView::EditKeyPressed),
        m_lastTrigger(GlodonAbstractItemView::NoEditTriggers),
        m_bTabKeyNavigation(false),
#ifndef QT_NO_DRAGANDDROP
        m_bShowDropIndicator(true),
        m_bDragEnabled(false),
        m_dragDropMode(GlodonAbstractItemView::NoDragDrop),
        m_bDragDropOverwrite(false),
        m_dropIndicatorPosition(GlodonAbstractItemView::OnItem),
        m_defaultDropAction(Qt::IgnoreAction),
#endif
#ifdef QT_SOFTKEYS_ENABLED
        doneSoftKey(0),
#endif
        m_bAutoScroll(true),
        m_nAutoScrollMargin(16),
        m_autoScrollCount(0),
        m_shouldScrollToCurrentOnShow(false),
        m_shouldClearStatusTip(false),
        m_bAlternatingColors(true),
        m_textElideMode(Qt::ElideRight),
        m_nDelayedHintTime(1000),
        m_verticalScrollMode(GlodonAbstractItemView::ScrollPerItem),
        m_horizontalScrollMode(GlodonAbstractItemView::ScrollPerItem),
        m_bCurrentIndexSet(false),
        m_bWrapItemText(false),
        m_delayedPendingLayout(true),
        m_moveCursorUpdatedView(false),
        m_bGridTextIncludeLeading(true),
        m_bScrollBarTracking(true),
        m_bAllowSelectAll(true),
        m_isSearchModel(false),
        m_bMouseMoveRefresh(false),
        m_oGridColor(QColor()),
        m_pToolTipFrame(NULL),
        m_bShowIndexContent(false),
        m_ignoreActiveWindowFocusReason(false),
        m_alwaysShowEditorPro(false),
        m_currentEditor(NULL),
        m_bIsInMultiSelect(false)
{
    m_keyboardInputTime.invalidate();
}

GlodonAbstractItemViewPrivate::~GlodonAbstractItemViewPrivate()
{
    freeAndNil(m_pToolTipFrame);
}

void GlodonAbstractItemViewPrivate::init()
{
    Q_Q(GlodonAbstractItemView);
    q->setItemDelegate(new GlodonDefaultItemDelegate(q));

    vbar->setRange(0, 0);
    hbar->setRange(0, 0);

    QObject::connect(vbar, SIGNAL(actionTriggered(int)),
                     q, SLOT(verticalScrollbarAction(int)));
    QObject::connect(hbar, SIGNAL(actionTriggered(int)),
                     q, SLOT(horizontalScrollbarAction(int)));
    QObject::connect(vbar, SIGNAL(valueChanged(int)),
                     q, SLOT(verticalScrollbarValueChanged(int)));
    QObject::connect(hbar, SIGNAL(valueChanged(int)),
                     q, SLOT(horizontalScrollbarValueChanged(int)));

    viewport->setBackgroundRole(QPalette::Base);

    q->setAttribute(Qt::WA_InputMethodEnabled);

#ifdef QT_SOFTKEYS_ENABLED
    doneSoftKey = QSoftKeyManager::createKeyedAction(QSoftKeyManager::DoneSoftKey, Qt::Key_Back, q);
#endif
}

void GlodonAbstractItemViewPrivate::setHoverIndex(const QPersistentModelIndex &index)
{
    Q_Q(GlodonAbstractItemView);

    if (m_hover == index)
    {
        return;
    }

    if (m_bMouseMoveRefresh)
    {
        if (m_selectionBehavior != GlodonAbstractItemView::SelectRows)
        {
            q->update(m_hover); //update the old one
            q->update(index); //update the new one
        }
        else
        {
            QRect oldHoverRect = q->visualRect(m_hover);
            QRect newHoverRect = q->visualRect(index);
            viewport->update(QRect(0, newHoverRect.y(), viewport->width(), newHoverRect.height()));
            viewport->update(QRect(0, oldHoverRect.y(), viewport->width(), oldHoverRect.height()));
        }
    }

    m_hover = index;
}

void GlodonAbstractItemViewPrivate::checkMouseMove(const QPersistentModelIndex &index)
{
    //we take a persistent model index because the model might change by emitting signals
    Q_Q(GlodonAbstractItemView);
    setHoverIndex(index);

    if (m_viewportEnteredNeeded || m_enteredIndex != index)
    {
        m_viewportEnteredNeeded = false;

        if (index.isValid())
        {
            emit q->entered(index);
#ifndef QT_NO_STATUSTIP
            QString statustip = m_model->data(index, Qt::StatusTipRole).toString();

            if (parent && (m_shouldClearStatusTip || !statustip.isEmpty()))
            {
                QStatusTipEvent tip(statustip);
                QApplication::sendEvent(parent, &tip);
                m_shouldClearStatusTip = !statustip.isEmpty();
            }

#endif
        }
        else
        {
#ifndef QT_NO_STATUSTIP

            if (parent && m_shouldClearStatusTip)
            {
                QString emptyString;
                QStatusTipEvent tip(emptyString);
                QApplication::sendEvent(parent, &tip);
            }

#endif
            emit q->viewportEntered();
        }

        m_enteredIndex = index;
    }
}

QItemSelectionModel::SelectionFlags GlodonAbstractItemViewPrivate::selectionBehaviorFlags() const
{
    switch (m_selectionBehavior)
    {
        case GlodonAbstractItemView::SelectRows:
            return QItemSelectionModel::Rows;

        case GlodonAbstractItemView::SelectColumns:
            return QItemSelectionModel::Columns;

        case GlodonAbstractItemView::SelectItems:
        default:
            return QItemSelectionModel::NoUpdate;
    }
}

GlodonAbstractItemView::GlodonAbstractItemView(QWidget *parent)
    : QAbstractScrollArea(*(new GlodonAbstractItemViewPrivate), parent)
{
    setMouseTracking(true);
    d_func()->init();
}

GlodonAbstractItemView::GlodonAbstractItemView(GlodonAbstractItemViewPrivate &dd, QWidget *parent)
    : QAbstractScrollArea(dd, parent)
{
    setMouseTracking(true);
    d_func()->init();
}

GlodonAbstractItemView::~GlodonAbstractItemView()
{
    Q_D(GlodonAbstractItemView);
    freeAndNil(d->m_pToolTipFrame);
    // stop these timers here before ~QObject
    d->m_delayedReset.stop();
    d->m_updateTimer.stop();
    d->m_delayedAutoScroll.stop();
    d->m_autoScrollTimer.stop();
    d->m_delayedLayout.stop();
    d->m_fetchMoreTimer.stop();
    d->m_delayedShowToolTip.stop();
}

QWidget *GlodonAbstractItemView::initCustomComment(const QModelIndex &, GString &)
{
    return NULL;
}

void GlodonAbstractItemView::setModel(QAbstractItemModel *model)
{
    Q_D(GlodonAbstractItemView);

    if (model == d->m_model)
    {
        return;
    }

    if (d->m_model && d->m_model != QAbstractItemModelPrivate::staticEmptyModel())
    {
        disconnect(d->m_model, SIGNAL(destroyed()),
                   this, SLOT(_q_modelDestroyed()));
        disconnect(d->m_model, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)),
                   this, SLOT(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
        disconnect(d->m_model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
                   this, SLOT(_q_headerDataChanged()));
        disconnect(d->m_model, SIGNAL(rowsInserted(QModelIndex, int, int)),
                   this, SLOT(rowsInserted(QModelIndex, int, int)));
        disconnect(d->m_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)),
                   this, SLOT(rowsAboutToBeRemoved(QModelIndex, int, int)));
        disconnect(d->m_model, SIGNAL(rowsRemoved(QModelIndex, int, int)),
                   this, SLOT(_q_rowsRemoved(QModelIndex, int, int)));
        disconnect(d->m_model, SIGNAL(rowsInserted(QModelIndex, int, int)),
                   this, SLOT(_q_rowsInserted(QModelIndex, int, int)));
        disconnect(d->m_model, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)),
                   this, SLOT(_q_columnsAboutToBeRemoved(QModelIndex, int, int)));
        disconnect(d->m_model, SIGNAL(columnsRemoved(QModelIndex, int, int)),
                   this, SLOT(_q_columnsRemoved(QModelIndex, int, int)));
        disconnect(d->m_model, SIGNAL(columnsInserted(QModelIndex, int, int)),
                   this, SLOT(_q_columnsInserted(QModelIndex, int, int)));

        disconnect(d->m_model, SIGNAL(modelReset()), this, SLOT(reset()));
        disconnect(d->m_model, SIGNAL(layoutChanged()), this, SLOT(_q_layoutChanged()));
    }

    d->m_model = (model ? model : QAbstractItemModelPrivate::staticEmptyModel());

    //These asserts do basic sanity checking of the model
    Q_ASSERT_X(d->m_model->index(0, 0) == d->m_model->index(0, 0),
               "GlodonAbstractItemView::setModel",
               "A model should return the exact same index "
               "(including its internal id/pointer) when asked for it twice in a row.");
    Q_ASSERT_X(!d->m_model->index(0, 0).parent().isValid(),
               "GlodonAbstractItemView::setModel",
               "The parent of a top level index should be invalid");

    if (d->m_model != QAbstractItemModelPrivate::staticEmptyModel())
    {
        connect(d->m_model, SIGNAL(destroyed()),
                this, SLOT(_q_modelDestroyed()));
        connect(d->m_model, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)),
                this, SLOT(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
        connect(d->m_model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
                this, SLOT(_q_headerDataChanged()));
        connect(d->m_model, SIGNAL(rowsInserted(QModelIndex, int, int)),
                this, SLOT(rowsInserted(QModelIndex, int, int)));
        connect(d->m_model, SIGNAL(rowsInserted(QModelIndex, int, int)),
                this, SLOT(_q_rowsInserted(QModelIndex, int, int)));
        connect(d->m_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)),
                this, SLOT(rowsAboutToBeRemoved(QModelIndex, int, int)));
        connect(d->m_model, SIGNAL(rowsRemoved(QModelIndex, int, int)),
                this, SLOT(_q_rowsRemoved(QModelIndex, int, int)));
        connect(d->m_model, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)),
                this, SLOT(_q_columnsAboutToBeRemoved(QModelIndex, int, int)));
        connect(d->m_model, SIGNAL(columnsRemoved(QModelIndex, int, int)),
                this, SLOT(_q_columnsRemoved(QModelIndex, int, int)));
        connect(d->m_model, SIGNAL(columnsInserted(QModelIndex, int, int)),
                this, SLOT(_q_columnsInserted(QModelIndex, int, int)));

        connect(d->m_model, SIGNAL(modelReset()), this, SLOT(reset()));
        connect(d->m_model, SIGNAL(layoutChanged()), this, SLOT(_q_layoutChanged()));
    }

    QItemSelectionModel *selection_model = new QItemSelectionModel(d->m_model, this);
    connect(d->m_model, SIGNAL(destroyed()), selection_model, SLOT(deleteLater()));
    setSelectionModel(selection_model);

    reset(); // kill editors, set new root and do layout
}

QAbstractItemModel *GlodonAbstractItemView::model() const
{
    Q_D(const GlodonAbstractItemView);
    return (d->m_model == QAbstractItemModelPrivate::staticEmptyModel() ? 0 : d->m_model);
}

void GlodonAbstractItemView::setDataModel(QAbstractItemModel *dataModel)
{
    Q_D(GlodonAbstractItemView);
    d->m_dataModel = dataModel;
}

QAbstractItemModel *GlodonAbstractItemView::dataModel() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_dataModel;
}

void GlodonAbstractItemView::setSelectionModel(QItemSelectionModel *selectionModel)
{
    // ### if the given model is null, we should use the original selection model
    Q_ASSERT(selectionModel);
    Q_D(GlodonAbstractItemView);

    if (selectionModel->model() != d->m_model)
    {
        qWarning("GlodonAbstractItemView::setSelectionModel() failed: "
                 "Trying to set a selection model, which works on "
                 "a different model than the view.");
        return;
    }

    if (d->m_selectionModel)
    {
        disconnect(d->m_selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
                   this, SLOT(selectionChanged(QItemSelection, QItemSelection)));
        disconnect(d->m_selectionModel, SIGNAL(currentChanged(QModelIndex, QModelIndex)),
                   this, SLOT(currentChanged(QModelIndex, QModelIndex)));
    }

    d->m_selectionModel = selectionModel;

    if (d->m_selectionModel)
    {
        connect(d->m_selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
                this, SLOT(selectionChanged(QItemSelection, QItemSelection)));
        connect(d->m_selectionModel, SIGNAL(currentChanged(QModelIndex, QModelIndex)),
                this, SLOT(currentChanged(QModelIndex, QModelIndex)));
    }
}

QItemSelectionModel *GlodonAbstractItemView::selectionModel() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_selectionModel;
}

bool GlodonAbstractItemView::canStartDrag(QModelIndexList indexes)
{
    Q_UNUSED(indexes);
    return true;
}

void GlodonAbstractItemView::setItemDelegate(GlodonDefaultItemDelegate *pNewItemDelegate)
{
    Q_D(GlodonAbstractItemView);

    if (pNewItemDelegate == d->m_itemDelegate)
    {
        return;
    }

    GlodonDefaultItemDelegate *pOldItemDelegate = d->m_itemDelegate.data();

    if (pOldItemDelegate != NULL && d->delegateRefCount(pOldItemDelegate) == 1)
    {
        disconnect(pOldItemDelegate, &GlodonDefaultItemDelegate::closeEditor,
                   this, &GlodonAbstractItemView::closeEditor);
        disconnect(pOldItemDelegate, SIGNAL(sizeHintChanged(QModelIndex)), this, SLOT(doItemsLayout()));

        disconnect(pOldItemDelegate, SIGNAL(commitData(QWidget *, bool &)),
                   this, SLOT(commitData(QWidget *, bool &)));
        disconnect(pOldItemDelegate,
                   SIGNAL(onQueryImageAspectRatioMode(QModelIndex, GlodonDefaultItemDelegate::GAspectRatioMode &)),
                   this, SIGNAL(onQueryImageRatioMode(QModelIndex, GlodonDefaultItemDelegate::GAspectRatioMode &)));
        disconnect(pOldItemDelegate, SIGNAL(onQueryFloatOrDoubleViewFormat(QModelIndex, QString &)),
                   this, SIGNAL(onQueryFloatOrDoubleViewFormat(QModelIndex, QString &)));
        disconnect(pOldItemDelegate,
                   SIGNAL(onQueryIndexDataType(QModelIndex, GlodonDefaultItemDelegate::GDataType &)),
                   this, SIGNAL(onQueryIndexDataType(QModelIndex, GlodonDefaultItemDelegate::GDataType &)));
        disconnect(pOldItemDelegate,
                   SIGNAL(onCommitDataAndCloseEditor()), this, SLOT(commitDataAndCloseEditor()));
    }

    if (pNewItemDelegate != NULL)
    {
        if (d->delegateRefCount(pNewItemDelegate) == 0)
        {
            connect(pNewItemDelegate, &GlodonDefaultItemDelegate::closeEditor,
                    this, &GlodonAbstractItemView::closeEditor);
            qRegisterMetaType<QModelIndex>("QModelIndex");
            connect(pNewItemDelegate, SIGNAL(sizeHintChanged(QModelIndex)), this, SLOT(doItemsLayout()), Qt::QueuedConnection);

            connect(pNewItemDelegate, SIGNAL(commitData(QWidget *, bool &)),
                    this, SLOT(commitData(QWidget *, bool &)));
            connect(pNewItemDelegate,
                    SIGNAL(onQueryImageAspectRatioMode(QModelIndex, GlodonDefaultItemDelegate::GAspectRatioMode &)),
                    this, SIGNAL(onQueryImageRatioMode(QModelIndex, GlodonDefaultItemDelegate::GAspectRatioMode &)));
            connect(pNewItemDelegate, SIGNAL(onQueryFloatOrDoubleViewFormat(QModelIndex, QString &)),
                    this, SIGNAL(onQueryFloatOrDoubleViewFormat(QModelIndex, QString &)));
            connect(pNewItemDelegate,
                    SIGNAL(onQueryIndexDataType(const QModelIndex &, GlodonDefaultItemDelegate::GDataType &)),
                    this, SIGNAL(onQueryIndexDataType(const QModelIndex &, GlodonDefaultItemDelegate::GDataType &)));
            connect(pNewItemDelegate,
                    SIGNAL(onCommitDataAndCloseEditor()), this, SLOT(commitDataAndCloseEditor()));
        }

        syncStateToDelegate(pNewItemDelegate);

        pNewItemDelegate->m_pTable = this;
    }

    d->m_itemDelegate = pNewItemDelegate;
    viewport()->update();
}

GlodonDefaultItemDelegate *GlodonAbstractItemView::itemDelegate() const
{
    return d_func()->m_itemDelegate;
}

QVariant GlodonAbstractItemView::inputMethodQuery(Qt::InputMethodQuery query) const
{
    const QModelIndex c_oCurrentIndex = currentIndex();

    if (!c_oCurrentIndex.isValid() || query != Qt::ImMicroFocus)
    {
        return QAbstractScrollArea::inputMethodQuery(query);
    }

    return visualRect(c_oCurrentIndex);
}

bool GlodonAbstractItemView::inLinkCell(const QPoint& pos)
{
    QModelIndex index = indexAt(pos);
    GlodonDefaultItemDelegate::GDataType type = GlodonDefaultItemDelegate::GTypeNormal;
    emit onQueryIndexDataType(index, type);
    return (type == GlodonDefaultItemDelegate::GTypeHTML);
}

bool GlodonAbstractItemView::alwaysShowEditorPro()
{
    Q_D(GlodonAbstractItemView);
    return d->m_alwaysShowEditorPro;
}

void GlodonAbstractItemView::setAlwaysShowEditorPro(bool value)
{
    Q_D(GlodonAbstractItemView);

    if (d->m_alwaysShowEditorPro == value)
    {
        return;
    }
    else
    {
        d->m_alwaysShowEditorPro = value;

        if (value)
        {
            for (int row = 0; row < d->m_model->rowCount(); ++row)
            {
                for (int column = 0; column < d->m_model->columnCount(); ++column)
                {
                    QModelIndex index = d->m_model->index(row, column);
                    edit(index);

                    if (NULL != d->m_currentEditor)
                    {
                        setIndexWidget(index, d->m_currentEditor);
                        d->m_currentEditor = NULL;
                    }
                }
            }
        }
        else
        {
            foreach(const GEditorInfo & info, d->m_indexEditorHash)
            {
                if (info.widget)
                {
                    d->releaseEditor(info.widget.data());
                }
            }
            d->m_editorIndexHash.clear();
            d->m_indexEditorHash.clear();
            d->m_persistent.clear();
        }
    }
}

bool GlodonAbstractItemView::isLegalData()
{
    return false;
}

void GlodonAbstractItemView::setLegalData(bool value)
{
    G_UNUSED(value);
}

QModelIndex GlodonAbstractItemView::editorModelIndex()
{
    Q_D(GlodonAbstractItemView);
    return d->m_oEditorIndex;
}

bool GlodonAbstractItemView::gridTextIncludeLeading() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_bGridTextIncludeLeading;
}

void GlodonAbstractItemView::setGridTextIncludeLeading(bool value)
{
    Q_D(GlodonAbstractItemView);

    if (d->m_bGridTextIncludeLeading != value)
    {
        d->m_itemDelegate.data()->setIncludeLeading(value);
        d->m_bGridTextIncludeLeading = value;
    }
}

bool GlodonAbstractItemView::allowSelectAll() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_bAllowSelectAll;
}

bool GlodonAbstractItemView::scrollBarTracking() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_bScrollBarTracking;
}

void GlodonAbstractItemView::setScrollBarTracking(bool value)
{
    Q_D(GlodonAbstractItemView);

    if (d->m_bScrollBarTracking != value)
    {
        d->m_bScrollBarTracking = value;

        if (d->vbar)
        {
            d->vbar->setTracking(value);
        }

        if (d->hbar)
        {
            d->hbar->setTracking(value);
        }
    }
}

void GlodonAbstractItemView::setAllowSelectAll(bool value)
{
    Q_D(GlodonAbstractItemView);
    d->m_bAllowSelectAll = value;
}

QColor GlodonAbstractItemView::gridColor() const
{
    Q_D(const GlodonAbstractItemView);

    return d->m_oGridColor;
}

void GlodonAbstractItemView::setGridColor(QColor value)
{
    Q_D(GlodonAbstractItemView);

    if (value == QColor(5, 0, 0)) //对应delphi的clWindow，使用默认颜色
    {
        return;
    }

    if (d->m_oGridColor != value)
    {
        d->m_oGridColor = value;
        //设置Grid背景色
        const_cast<QPalette &>(palette()).setColor(QPalette::Window, d->m_oGridColor);
        const_cast<QPalette &>(d->viewport->palette()).setColor(QPalette::Window, d->m_oGridColor);

        d->viewport->setBackgroundRole(QPalette::Window);
        d->viewport->update();
    }
}

void GlodonAbstractItemView::setIsShowHint(bool value, bool showIndexContent)
{
    Q_D(GlodonAbstractItemView);

    if (value)
    {
        if (!(d->m_pToolTipFrame))
        {
            d->m_pToolTipFrame = new GToolTipFrame(NULL);//保证frame可以在控件顶层
        }

        connect(this, SIGNAL(entered(QModelIndex)), this, SLOT(showToolTip(QModelIndex)));
        d->m_bShowIndexContent = showIndexContent;
    }
    else
    {
        disconnect(this, SIGNAL(entered(QModelIndex)), this, SLOT(showToolTip(QModelIndex)));
    }
}

void GlodonAbstractItemView::setShowHintDelay(int time)
{
    Q_D(GlodonAbstractItemView);
    d->m_nDelayedHintTime = time;
}

int GlodonAbstractItemView::verticalScrollbarValue()
{
    Q_D(GlodonAbstractItemView);
    return d->vbar->value();
}

void GlodonAbstractItemView::setVerticalScrollBarValue(int value)
{
    Q_D(GlodonAbstractItemView);
    d->vbar->setValue(value);
}

int GlodonAbstractItemView::horizontalScrollbarValue()
{
    Q_D(GlodonAbstractItemView);
    return d->hbar->value();
}

void GlodonAbstractItemView::setHorizontalScrollbarValue(int value)
{
    Q_D(GlodonAbstractItemView);
    d->hbar->setValue(value);
}

void GlodonAbstractItemView::setIsSearchModel(bool value)
{
    Q_D(GlodonAbstractItemView);
    d->m_isSearchModel = value;
}

bool GlodonAbstractItemView::isSearchModel()
{
    Q_D(GlodonAbstractItemView);
    return d->m_isSearchModel;
}

void GlodonAbstractItemView::setAllowRangeMoving(bool value)
{
    G_UNUSED(value);
}

bool GlodonAbstractItemView::allowRangeMoving() const
{
    return false;
}

void GlodonAbstractItemView::setItemDelegateForRow(int row, GlodonDefaultItemDelegate *delegate)
{
    Q_D(GlodonAbstractItemView);

    if (GlodonDefaultItemDelegate *rowDelegate = d->m_rowDelegates.value(row, 0))
    {
        if (d->delegateRefCount(rowDelegate) == 1)
        {
            disconnect(rowDelegate, &GlodonDefaultItemDelegate::closeEditor,
                       this, &GlodonAbstractItemView::closeEditor);
            disconnect(rowDelegate,
                       SIGNAL(commitData(QWidget *, bool &)), this, SLOT(commitData(QWidget *, bool &)));

            disconnect(rowDelegate,
                       SIGNAL(onQueryImageAspectRatioMode(QModelIndex, GlodonDefaultItemDelegate::GAspectRatioMode &)),
                       this, SIGNAL(onQueryImageRatioMode(QModelIndex, GlodonDefaultItemDelegate::GAspectRatioMode &)));
            disconnect(rowDelegate, SIGNAL(onQueryFloatOrDoubleViewFormat(QModelIndex, QString &)),
                       this, SIGNAL(onQueryFloatOrDoubleViewFormat(QModelIndex, QString &)));
            disconnect(rowDelegate,
                       SIGNAL(onQueryIndexDataType(QModelIndex, GlodonDefaultItemDelegate::GDataType &)),
                       this, SIGNAL(onQueryIndexDataType(QModelIndex, GlodonDefaultItemDelegate::GDataType &)));
        }

        d->m_rowDelegates.remove(row);
    }

    if (delegate != NULL)
    {
        if (d->delegateRefCount(delegate) == 0)
        {
            connect(delegate, &GlodonDefaultItemDelegate::closeEditor,
                    this, &GlodonAbstractItemView::closeEditor);
            connect(delegate, SIGNAL(commitData(QWidget *, bool &)), this, SLOT(commitData(QWidget *, bool &)));

            connect(delegate,
                    SIGNAL(onQueryImageAspectRatioMode(QModelIndex, GlodonDefaultItemDelegate::GAspectRatioMode &)),
                    this, SIGNAL(onQueryImageRatioMode(QModelIndex, GlodonDefaultItemDelegate::GAspectRatioMode &)));
            connect(delegate, SIGNAL(onQueryFloatOrDoubleViewFormat(QModelIndex, QString &)),
                    this, SIGNAL(onQueryFloatOrDoubleViewFormat(QModelIndex, QString &)));
            connect(delegate, SIGNAL(onQueryIndexDataType(QModelIndex, GlodonDefaultItemDelegate::GDataType &)),
                    this, SIGNAL(onQueryIndexDataType(QModelIndex, GlodonDefaultItemDelegate::GDataType &)));
        }

        syncStateToDelegate(delegate);
        d->m_rowDelegates.insert(row, delegate);
    }

    viewport()->update();
}

GlodonDefaultItemDelegate *GlodonAbstractItemView::itemDelegateForRow(int row) const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_rowDelegates.value(row, 0);
}

void GlodonAbstractItemView::setItemDelegateForColumn(int column, GlodonDefaultItemDelegate *delegate)
{
    Q_D(GlodonAbstractItemView);

    if (GlodonDefaultItemDelegate *columnDelegate = d->m_columnDelegates.value(column, 0))
    {
        if (d->delegateRefCount(columnDelegate) == 1)
        {
            disconnect(columnDelegate, SIGNAL(closeEditor(QWidget *, bool &, GlodonDefaultItemDelegate::EndEditHint)),
                       this, SLOT(closeEditor(QWidget *, bool &, GlodonDefaultItemDelegate::EndEditHint)));
            disconnect(columnDelegate, SIGNAL(commitData(QWidget *, bool &)),
                       this, SLOT(commitData(QWidget *, bool &)));

            disconnect(columnDelegate,
                       SIGNAL(onQueryImageAspectRatioMode(QModelIndex, GlodonDefaultItemDelegate::GAspectRatioMode &)),
                       this, SIGNAL(onQueryImageRatioMode(QModelIndex, GlodonDefaultItemDelegate::GAspectRatioMode &)));
            disconnect(columnDelegate, SIGNAL(onQueryFloatOrDoubleViewFormat(QModelIndex, QString &)),
                       this, SIGNAL(onQueryFloatOrDoubleViewFormat(QModelIndex, QString &)));
            disconnect(columnDelegate,
                       SIGNAL(onQueryIndexDataType(QModelIndex, GlodonDefaultItemDelegate::GDataType &)),
                       this, SIGNAL(onQueryIndexDataType(QModelIndex, GlodonDefaultItemDelegate::GDataType &)));
        }

        d->m_columnDelegates.remove(column);
    }

    if (delegate != NULL)
    {
        if (d->delegateRefCount(delegate) == 0)
        {
            connect(delegate, SIGNAL(closeEditor(QWidget *, bool &, GlodonDefaultItemDelegate::EndEditHint)),
                    this, SLOT(closeEditor(QWidget *, bool &, GlodonDefaultItemDelegate::EndEditHint)));
            connect(delegate, SIGNAL(commitData(QWidget *, bool &)), this, SLOT(commitData(QWidget *, bool &)));

            connect(delegate,
                    SIGNAL(onQueryImageAspectRatioMode(QModelIndex, GlodonDefaultItemDelegate::GAspectRatioMode &)),
                    this, SIGNAL(onQueryImageRatioMode(QModelIndex, GlodonDefaultItemDelegate::GAspectRatioMode &)));
            connect(delegate, SIGNAL(onQueryFloatOrDoubleViewFormat(QModelIndex, QString &)),
                    this, SIGNAL(onQueryFloatOrDoubleViewFormat(QModelIndex, QString &)));
            connect(delegate, SIGNAL(onQueryIndexDataType(QModelIndex, GlodonDefaultItemDelegate::GDataType &)),
                    this, SIGNAL(onQueryIndexDataType(QModelIndex, GlodonDefaultItemDelegate::GDataType &)));
        }

        syncStateToDelegate(delegate);
        d->m_columnDelegates.insert(column, delegate);
    }

    viewport()->update();
}

GlodonDefaultItemDelegate *GlodonAbstractItemView::itemDelegateForColumn(int column) const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_columnDelegates.value(column, 0);
}

GlodonDefaultItemDelegate *GlodonAbstractItemView::itemDelegate(const QModelIndex &index) const
{
    Q_D(const GlodonAbstractItemView);
    return d->delegateForIndex(index);
}

void GlodonAbstractItemView::setSelectionMode(SelectionMode mode)
{
    Q_D(GlodonAbstractItemView);
    d->m_selectionMode = mode;
}

GlodonAbstractItemView::SelectionMode GlodonAbstractItemView::selectionMode() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_selectionMode;
}

void GlodonAbstractItemView::setSelectionBehavior(GlodonAbstractItemView::SelectionBehavior behavior)
{
    Q_D(GlodonAbstractItemView);
    d->m_selectionBehavior = behavior;
}

GlodonAbstractItemView::SelectionBehavior GlodonAbstractItemView::selectionBehavior() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_selectionBehavior;
}

void GlodonAbstractItemView::setCurrentIndex(const QModelIndex &index)
{
    Q_D(GlodonAbstractItemView);

    if (d->m_selectionModel && (!index.isValid() || d->isIndexEnabled(index)))
    {
        QItemSelectionModel::SelectionFlags command = selectionCommand(index, 0);
        moveCurrent(currentIndex(), index, command, mrProgram);
        d->m_bCurrentIndexSet = true;

        if ((command & QItemSelectionModel::Current) == 0)
        {
            d->m_pressedPosition = visualRect(currentIndex()).center() + d->offset();
        }
    }
}

QModelIndex GlodonAbstractItemView::currentIndex() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_selectionModel ? d->m_selectionModel->currentIndex() : QModelIndex();
}

void GlodonAbstractItemView::reset()
{
    beforeReset();
    doReset();
    afterReset();
}

void GlodonAbstractItemView::beforeReset()
{

}

void GlodonAbstractItemView::doReset()
{
    Q_D(GlodonAbstractItemView);

    d->m_delayedReset.stop(); //make sure we stop the timer

    foreach(const GEditorInfo & info, d->m_indexEditorHash)
    {
        if (info.widget)
        {
            d->releaseEditor(info.widget.data());
        }
    }

    d->m_editorIndexHash.clear();
    d->m_indexEditorHash.clear();
    d->m_persistent.clear();
    d->m_bCurrentIndexSet = false;
    d->m_oEditorIndex = QModelIndex();

    setState(NoState);
    setRootIndex(QModelIndex());

    if (d->m_selectionModel)
    {
        d->m_selectionModel->reset();
    }

    //通过插入数据的方式，结束编辑状态，应该把delegate的状态重置
    if (itemDelegate() != NULL)
    {
        itemDelegate()->setCurEditor(NULL);
    }

#ifndef QT_NO_ACCESSIBILITY
#ifdef Q_WS_X11

    if (QAccessible::isActive())
    {
        QAccessible::queryAccessibleInterface(this)->table2Interface()->modelReset();
        QAccessible::updateAccessibility(this, 0, QAccessible::TableModelChanged);
    }

#endif
#endif
}

void GlodonAbstractItemView::afterReset()
{

}

void GlodonAbstractItemView::setRootIndex(const QModelIndex &index)
{
    Q_D(GlodonAbstractItemView);

    if (index.isValid() && index.model() != d->m_model)
    {
        qWarning("GlodonAbstractItemView::setRootIndex failed : index must be from the currently set model");
        return;
    }

    d->m_root = index;
    d->doDelayedItemsLayout();
}

QModelIndex GlodonAbstractItemView::rootIndex() const
{
    return QModelIndex(d_func()->m_root);
}

void GlodonAbstractItemView::selectAll()
{
    Q_D(GlodonAbstractItemView);

    if (!d->m_bAllowSelectAll)
    {
        return;
    }

    SelectionMode mode = d->m_selectionMode;
    if (mode == MultiSelection || mode == ExtendedSelection)
    {
        d->selectAll(QItemSelectionModel::ClearAndSelect
                     | d->selectionBehaviorFlags());
    }
    else if (mode != SingleSelection)
    {
        d->selectAll(selectionCommand(d->m_model->index(0, 0, d->m_root)));
    }
}

void GlodonAbstractItemView::edit(const QModelIndex &index)
{
    Q_D(GlodonAbstractItemView);

    if (!d->isIndexValid(index))
    {
        qWarning("edit: index was invalid");
    }

    if (!edit(index, AllEditTriggers, 0))
    {
        qWarning("edit: editing failed");
    }
}

void GlodonAbstractItemView::doCommitDataAndCloseEditor(bool &canCloseEditor)
{
    Q_D(GlodonAbstractItemView);

    QModelIndex index = currentIndex();

    if (!d->isIndexValid(index) || (d->m_state != EditingState && !d->m_alwaysShowEditorPro))
        return;

    QWidget *pEditor = d->editorForIndex(index).widget.data();

    if (NULL == pEditor)
        return;

    GlodonDefaultItemDelegate * pGlodonDelegate = d->delegateForIndex(index);
    try
    {
        commitData(pEditor, canCloseEditor);

        if (canCloseEditor)
        {
            closeEditor(pEditor, canCloseEditor, GlodonDefaultItemDelegate::NoHint);
        }

        if (canCloseEditor)
        {
            d->m_state = NoState;
        }
        else
        {
            //提交非法内容，被拒绝的时候，控件中的文字内容全选
            if (NULL != pGlodonDelegate)
            {
                pGlodonDelegate->setTextAllSelected(pEditor);
            }
            d->m_state = EditingState;
        }
    }
    catch (...)
    {
        if (NULL != pGlodonDelegate)
        {
            pGlodonDelegate->setTextAllSelected(pEditor);
        }
        d->m_state = EditingState;

        throw;
    }
}

void GlodonAbstractItemView::setModelData(GlodonDefaultItemDelegate *pDelegate, QWidget * editor,
                                          const QModelIndex & index, bool & canCloseEditor)
{
    Q_D(GlodonAbstractItemView);

    try
    {
        pDelegate->setModelData(editor, d->m_model, index);
    }
    catch (...)
    {
        d->m_pCurCommittingEditor = NULL;
        setLegalData(false);
        canCloseEditor = false;
        editor->setFocus();
        throw;
    }
}

void GlodonAbstractItemView::setLegitimacyOfData(GlodonDefaultItemDelegate *pDelegate, bool &canCloseEditor)
{
    Q_D(GlodonAbstractItemView);

    canCloseEditor = pDelegate->checkSetModelData();

    if (canCloseEditor)
    {
        setLegalData(true);
        d->m_oEditorIndex = QModelIndex();
    }
    else
    {
        setLegalData(false);
    }
}

void GlodonAbstractItemView::clearSelection()
{
    Q_D(GlodonAbstractItemView);

    if (d->m_selectionModel)
    {
        d->m_selectionModel->clearSelection();
    }
}

void GlodonAbstractItemView::doItemsLayout()
{
    Q_D(GlodonAbstractItemView);
    d->interruptDelayedItemsLayout();
    updateGeometries();
    d->viewport->update();
}

void GlodonAbstractItemView::setEditTriggers(EditTriggers actions)
{
    Q_D(GlodonAbstractItemView);
    d->m_editTriggers = actions;
}

GlodonAbstractItemView::EditTriggers GlodonAbstractItemView::editTriggers() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_editTriggers;
}

void GlodonAbstractItemView::setVerticalScrollMode(ScrollMode mode)
{
    Q_D(GlodonAbstractItemView);

    if (mode == d->m_verticalScrollMode)
    {
        return;
    }

    QModelIndex topLeft = indexAt(QPoint(0, 0));
    d->m_verticalScrollMode = mode;
    updateGeometries(); // update the scroll bars
    scrollTo(topLeft, GlodonAbstractItemView::PositionAtTop);
}

GlodonAbstractItemView::ScrollMode GlodonAbstractItemView::verticalScrollMode() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_verticalScrollMode;
}

void GlodonAbstractItemView::setHorizontalScrollMode(ScrollMode mode)
{
    Q_D(GlodonAbstractItemView);
    d->m_horizontalScrollMode = mode;
    updateGeometries(); // update the scroll bars
}

GlodonAbstractItemView::ScrollMode GlodonAbstractItemView::horizontalScrollMode() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_horizontalScrollMode;
}

#ifndef QT_NO_DRAGANDDROP

void GlodonAbstractItemView::setDragDropOverwriteMode(bool overwrite)
{
    Q_D(GlodonAbstractItemView);
    d->m_bDragDropOverwrite = overwrite;
}

bool GlodonAbstractItemView::dragDropOverwriteMode() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_bDragDropOverwrite;
}

#endif

void GlodonAbstractItemView::setAutoScroll(bool enable)
{
    Q_D(GlodonAbstractItemView);
    d->m_bAutoScroll = enable;
}

bool GlodonAbstractItemView::hasAutoScroll() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_bAutoScroll;
}

void GlodonAbstractItemView::setAutoScrollMargin(int margin)
{
    Q_D(GlodonAbstractItemView);
    d->m_nAutoScrollMargin = margin;
}

int GlodonAbstractItemView::autoScrollMargin() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_nAutoScrollMargin;
}

void GlodonAbstractItemView::setToolTipFrameHide()
{
    Q_D(GlodonAbstractItemView);
    if (d->m_pToolTipFrame)
    {
        d->m_pToolTipFrame->hide();
        d->m_delayedShowToolTip.stop();
    }
}

void GlodonAbstractItemView::setTabKeyNavigation(bool enable)
{
    Q_D(GlodonAbstractItemView);
    d->m_bTabKeyNavigation = enable;
}

bool GlodonAbstractItemView::tabKeyNavigation() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_bTabKeyNavigation;
}

#ifndef QT_NO_DRAGANDDROP

void GlodonAbstractItemView::setDropIndicatorShown(bool enable)
{
    Q_D(GlodonAbstractItemView);
    d->m_bShowDropIndicator = enable;
}

bool GlodonAbstractItemView::showDropIndicator() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_bShowDropIndicator;
}

void GlodonAbstractItemView::setDragEnabled(bool enable)
{
    Q_D(GlodonAbstractItemView);
    d->m_bDragEnabled = enable;
}

bool GlodonAbstractItemView::dragEnabled() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_bDragEnabled;
}

void GlodonAbstractItemView::setDragDropMode(DragDropMode behavior)
{
    Q_D(GlodonAbstractItemView);
    d->m_dragDropMode = behavior;
    setDragEnabled(behavior == DragOnly || behavior == DragDrop || behavior == InternalMove);
    setAcceptDrops(behavior == DropOnly || behavior == DragDrop || behavior == InternalMove);
}

GlodonAbstractItemView::DragDropMode GlodonAbstractItemView::dragDropMode() const
{
    Q_D(const GlodonAbstractItemView);
    DragDropMode setBehavior = d->m_dragDropMode;

    if (!dragEnabled() && !acceptDrops())
    {
        return NoDragDrop;
    }

    if (dragEnabled() && !acceptDrops())
    {
        return DragOnly;
    }

    if (!dragEnabled() && acceptDrops())
    {
        return DropOnly;
    }

    if (dragEnabled() && acceptDrops())
    {
        if (setBehavior == InternalMove)
        {
            return setBehavior;
        }
        else
        {
            return DragDrop;
        }
    }

    return NoDragDrop;
}

void GlodonAbstractItemView::setDefaultDropAction(Qt::DropAction dropAction)
{
    Q_D(GlodonAbstractItemView);
    d->m_defaultDropAction = dropAction;
}

Qt::DropAction GlodonAbstractItemView::defaultDropAction() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_defaultDropAction;
}

#endif // QT_NO_DRAGANDDROP

void GlodonAbstractItemView::setAlternatingRowColors(bool enable)
{
    Q_D(GlodonAbstractItemView);

    if (d->m_bAlternatingColors == enable)
    {
        return;
    }

    d->m_bAlternatingColors = enable;
    if (isVisible())
    {
        d->viewport->update();
    }
}

bool GlodonAbstractItemView::alternatingRowColors() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_bAlternatingColors;
}

void GlodonAbstractItemView::setIconSize(const QSize &size)
{
    Q_D(GlodonAbstractItemView);

    if (size == d->m_oIconSize)
    {
        return;
    }

    d->m_oIconSize = size;
    d->doDelayedItemsLayout();
}

QSize GlodonAbstractItemView::iconSize() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_oIconSize;
}

void GlodonAbstractItemView::setTextElideMode(Qt::TextElideMode mode)
{
    Q_D(GlodonAbstractItemView);
    d->m_textElideMode = mode;
}

Qt::TextElideMode GlodonAbstractItemView::textElideMode() const
{
    return d_func()->m_textElideMode;
}

bool GlodonAbstractItemView::focusNextPrevChild(bool next)
{
//    Q_D(GlodonAbstractItemView);
//    if (d->m_tabKeyNavigation && isEnabled() && d->viewport->isEnabled()) {
//        QKeyEvent event(QEvent::KeyPress, next ? Qt::Key_Tab : Qt::Key_Backtab, Qt::NoModifier);
//        keyPressEvent(&event);
//        if (event.isAccepted())
//            return true;
//    }
//    return QAbstractScrollArea::focusNextPrevChild(next);
    //此处不需要QWdiget的focusNextPrevChild处理，返回false，直接调用keyPressEvent自己控制焦点方向
    Q_UNUSED(next);
    return false;
}

bool GlodonAbstractItemView::event(QEvent *event)
{
    Q_D(GlodonAbstractItemView);
    GLDEvent *gldEvent = dynamic_cast<GLDEvent *>(event);

    if (gldEvent)
    {
        switch (gldEvent->type())
        {
            case GM_FIRST:
            {
                doGMFirst(gldEvent);
                break;
            }

            case GM_QUERYFIRST:
            {
                doGMQueryFirst(gldEvent);
                break;
            }

            case GM_PREV:
            {
                doGMPrev(gldEvent);
                break;
            }

            case GM_QUERYPREV:
            {
                doGMQueryPrev(gldEvent);
                break;
            }

            case GM_NEXT:
            {
                doGMNext(gldEvent);
                break;
            }

            case GM_QUERYNEXT:
            {
                doGMQueryNext(gldEvent);
                break;
            }

            case GM_Last:
            {
                doGMLast(gldEvent);
                break;
            }

            case GM_QUERYLAST:
            {
                doGMQueryLast(gldEvent);
                break;
            }

            case GM_SETSEL:
            {
                doGMSetSel(gldEvent);
                break;
            }

            case GM_QUERYSETSEL:
            {
                doGMQuerySetSel(gldEvent);
                break;
            }

            case GM_SETCOLROW:
            {
                doGMSetColRow(gldEvent);
                break;
            }

            case GM_SetScrollBarStep:
            {
                switch ((GLDScrollBarType)(gldEvent->wParam()))
                {
                    case gsbtVertical: // Vertical
                        d->vbar->setEnabled(gldEvent->lParam());
                        break;

                    case gsbtHorizon: // Horizon:
                        d->hbar->setEnabled(gldEvent->lParam());
                        break;

                    case gsbtAll:
                        d->vbar->setEnabled(gldEvent->lParam());
                        d->hbar->setEnabled(gldEvent->lParam());
                        break;

                    default:
                        break;
                }

                return true;
            }

            default:
                break;
        }
    }
    else
    {
        switch (event->type())
        {
            case QEvent::Paint:
                //we call this here because the scrollbars' visibility might be altered
                //so this can't be done in the paintEvent method
                d->executePostedLayout(); //make sure we set the layout properly
                break;

            case QEvent::Show:
                d->executePostedLayout(); //make sure we set the layout properly

                if (d->m_shouldScrollToCurrentOnShow)
                {
                    d->m_shouldScrollToCurrentOnShow = false;
                    const QModelIndex c_oCurrentIndex = currentIndex();
                    if (c_oCurrentIndex.isValid() && (d->m_state == GlodonAbstractItemView::EditingState || d->m_bAutoScroll))
                    {
                        scrollTo(c_oCurrentIndex);
                    }
                }

                break;

            case QEvent::LocaleChange:
                viewport()->update();
                break;

            case QEvent::LayoutDirectionChange:
            case QEvent::ApplicationLayoutDirectionChange:
                updateGeometries();
                break;

            case QEvent::StyleChange:
                doItemsLayout();
                break;

            case QEvent::FocusOut:
                d->checkPersistentEditorFocus();
                break;

            case QEvent::FontChange:
                d->doDelayedItemsLayout(); // the size of the items will change
                break;
#ifdef QT_SOFTKEYS_ENABLED

            case QEvent::LanguageChange:
                d->doneSoftKey->setText(QSoftKeyManager::standardSoftKeyText(QSoftKeyManager::DoneSoftKey));
                break;
#endif

            default:
                break;
        }
    }

    return QAbstractScrollArea::event(event);
}

bool GlodonAbstractItemView::viewportEvent(QEvent *event)
{
    Q_D(GlodonAbstractItemView);

    switch (event->type())
    {
        case QEvent::HoverMove:
        case QEvent::HoverEnter:
            d->setHoverIndex(indexAt(static_cast<QHoverEvent *>(event)->pos()));
            break;

        case QEvent::HoverLeave:
            d->setHoverIndex(QModelIndex());
            break;

        case QEvent::Enter:
            d->m_viewportEnteredNeeded = true;
            break;

        case QEvent::Leave:
#ifndef QT_NO_STATUSTIP
            if (d->m_shouldClearStatusTip && d->parent)
            {
                QString empty;
                QStatusTipEvent tip(empty);
                QApplication::sendEvent(d->parent, &tip);
                d->m_shouldClearStatusTip = false;
            }

#endif
            d->m_enteredIndex = QModelIndex();
            break;
        case QEvent::ToolTip:
        case QEvent::QueryWhatsThis:
        case QEvent::WhatsThis:
        {
            QHelpEvent *he = static_cast<QHelpEvent *>(event);
            const QModelIndex c_index = indexAt(he->pos());

            QStyleOptionViewItem option = d->viewOptionsV4();
            option.rect = visualRect(c_index);
            option.state |= (c_index == currentIndex() ? QStyle::State_HasFocus : QStyle::State_None);

            bool bRetval = false;
            // ### Qt 5: make this a normal function call to a virtual function
            QMetaObject::invokeMethod(d->delegateForIndex(c_index), "helpEvent",
                                      Q_RETURN_ARG(bool, bRetval),
                                      Q_ARG(QHelpEvent *, he),
                                      Q_ARG(GlodonAbstractItemView *, this),
                                      Q_ARG(QStyleOptionViewItem, option),
                                      Q_ARG(QModelIndex, c_index));
            return bRetval;
        }

        case QEvent::FontChange:
            d->doDelayedItemsLayout(); // the size of the items will change
            break;

        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
            d->viewport->update();
            break;

        case QEvent::Wheel:
            //数据非法会出现两次弹窗
//            setFocus(Qt::MouseFocusReason);
            return QAbstractScrollArea::viewportEvent(event);

        default:
            break;
    }

    return QAbstractScrollArea::viewportEvent(event);
}

void GlodonAbstractItemView::mousePressEvent(QMouseEvent *event)
{
    Q_D(GlodonAbstractItemView);
    d->m_delayedAutoScroll.stop(); //any interaction with the view cancel the auto scrolling

    QPoint pos = event->pos();
    QPersistentModelIndex index = indexAt(pos);
    //如果点击的是格线之外的区域，不做任何操作，保持和Delphi版一致
    if (!index.isValid())
    {
        d->m_noSelectionOnMousePress = true;
        return;
    }

    // wangdd-a，后面的判断觉得不是特别正确，进入编辑状态后，TableView应该接不到鼠标消息了
    // 但是Qt里面也是这么写的，先不做修改
    if (!d->m_selectionModel || (d->m_state == EditingState && d->hasEditor(index)))
    {
        return;
    }

    dealWithSelectionAndAutoScroll(index, event);
}

void GlodonAbstractItemView::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(GlodonAbstractItemView);

    QPoint oPrePoint;
    QPoint oCurPoint = event->pos();
    QPersistentModelIndex oCurIndex = indexAt(oCurPoint);

    switch (state())
    {
        case ExpandingState:
        case CollapsingState:
        {
            return;
        }

#ifndef QT_NO_DRAGANDDROP
        case DraggingState:
        {
            if (canStartDrag(d->selectedDraggableIndexes()))
            {
                oPrePoint = d->m_pressedPosition - d->offset();

                if ((oPrePoint - oCurPoint).manhattanLength() > QApplication::startDragDistance())
                {
                    d->m_pressedIndex = QModelIndex();
                    startDrag(d->m_model->supportedDragActions());
                    setState(NoState); // the startDrag will return when the dnd operation is done
                    stopAutoScroll();
                }

                return;
            }
        }
#endif // QT_NO_DRAGANDDROP

        case EditingState:
        {
            QModelIndex buddy = d->m_model->buddy(d->m_pressedIndex);

            if (d->hasEditor(buddy))
            {
                // todo liurx huy-a 在Revision: 5161 修改 always edit 模式状态下支持 ToolTip（施工需求）
                if (d->m_alwaysShowEditorPro)
                {
                    d->checkMouseMove(oCurIndex);
                }

                return;
            }
        }
        default:
            break;
    }

    // 同上
    if (d->m_oEditorIndex.isValid())
    {
        if (d->m_alwaysShowEditorPro)
        {
            d->checkMouseMove(oCurIndex);
        }

        return;
    }

    if (d->m_selectionMode != SingleSelection)
    {
        oPrePoint = d->m_pressedPosition - d->offset();
    }
    else
    {
        oPrePoint = oCurPoint;
    }

    d->checkMouseMove(oCurIndex);

#ifndef QT_NO_DRAGANDDROP

    if (d->m_pressedIndex.isValid()
            && d->m_bDragEnabled
            && (state() != DragSelectingState)
            && (event->buttons() != Qt::NoButton)
            && !d->selectedDraggableIndexes().isEmpty())
    {
        setState(DraggingState);
        return;
    }

#endif

    setSelectionAndScrollToOnMouseMove(oCurIndex, event, oPrePoint, oCurPoint);
}

void GlodonAbstractItemView::leaveEvent(QEvent *event)
{
    G_UNUSED(event);
    Q_D(GlodonAbstractItemView);

    if (NULL != d->m_pToolTipFrame)
    {
        d->m_pToolTipFrame->hide();
    }
}

void GlodonAbstractItemView::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(GlodonAbstractItemView);

    QPoint pos = event->pos();
    QPersistentModelIndex index = indexAt(pos);

    if (state() == EditingState)
    {
        if (d->isIndexValid(index)
                && d->isIndexEnabled(index)
                && d->sendDelegateEvent(index, event))
        {
            update(index);
        }

        return;
    }

    bool bClick = (index == d->m_pressedIndex && index.isValid());
    bool bSelectedClicked = bClick && (Qt::LeftButton == (event->button() & Qt::LeftButton))
                            && d->m_pressedAlreadySelected;
    EditTrigger trigger = (bSelectedClicked ? SelectedClicked : NoEditTriggers);
    const bool c_edited = edit(index, trigger, event);

    d->m_ctrlDragSelectionFlag = QItemSelectionModel::NoUpdate;

    if (d->m_selectionModel && d->m_noSelectionOnMousePress)
    {
        d->m_noSelectionOnMousePress = false;
//        d->selectionModel->select(index, selectionCommand(index, event));
        QRect rect(d->m_pressedPosition - d->offset(), pos);
        setSelection(rect, selectionCommand(index, event));
    }

    if (!c_edited)
    {
        setState(NoState);
    }

    if (bClick)
    {
        if (Qt::LeftButton == event->button())
        {
            emit clicked(index);
        }

        if (c_edited)
        {
            return;
        }

        QStyleOptionViewItem option = d->viewOptionsV4();

        if (d->m_pressedAlreadySelected)
        {
            option.state |= QStyle::State_Selected;
        }

        if (0 != style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick, &option, this))
        {
            emit activated(index);
        }
    }
}

void GlodonAbstractItemView::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_D(GlodonAbstractItemView);

    QModelIndex index = indexAt(event->pos());

    if (!index.isValid()
            || !d->isIndexEnabled(index)
            || (d->m_pressedIndex != index))
    {
        QMouseEvent me(QEvent::MouseButtonPress,
                       event->pos(), event->button(),
                       event->buttons(), event->modifiers());
        mousePressEvent(&me);
        return;
    }

    // signal handlers may change the model
    QPersistentModelIndex persistent = index;
    emit doubleClicked(persistent);

    if ((Qt::LeftButton == (event->button() & Qt::LeftButton)) && cursor().shape() == Qt::ArrowCursor)
        if (!edit(persistent, DoubleClicked, event)
                && 0 == style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick, 0, this))
        {
            emit activated(persistent);
        }
}

#ifndef QT_NO_DRAGANDDROP

void GlodonAbstractItemView::dragEnterEvent(QDragEnterEvent *event)
{
    if (dragDropMode() == InternalMove
            && (event->source() != this || !(event->possibleActions() & Qt::MoveAction)))
    {
        return;
    }

    if (d_func()->canDecode(event))
    {
        event->accept();
        setState(DraggingState);
    }
    else
    {
        event->ignore();
    }
}

void GlodonAbstractItemView::dragMoveEvent(QDragMoveEvent *event)
{
    Q_D(GlodonAbstractItemView);

    if (dragDropMode() == InternalMove
            && (event->source() != this || !(event->possibleActions() & Qt::MoveAction)))
    {
        return;
    }

    // ignore by default
    event->ignore();

    QModelIndex dropIndex;
    int col = -1;
    int row = -1;
    if (d->dropOn(event, &row, &col, &dropIndex))
    {
        if (!d->m_model->canDropMimeData(
                    event->mimeData(),
                    dragDropMode() == InternalMove ? Qt::MoveAction : event->dropAction(), row, col, dropIndex))
        {
            return;
        }
    }

    QModelIndex index = indexAt(event->pos());
    d->m_hover = index;
    if (!d->droppingOnItself(event, index)
            && d->canDecode(event))
    {
        if (index.isValid() && d->m_bShowDropIndicator)
        {
            QRect rect = visualRect(index);
            d->m_dropIndicatorPosition = d->position(event->pos(), rect, index);
            switch (d->m_dropIndicatorPosition)
            {
            case AboveItem:
                if (d->isIndexDropEnabled(index.parent()))
                {
                    d->m_dropIndicatorRect = QRect(rect.left(), rect.top(), rect.width(), 0);
                    event->accept();
                }
                else
                {
                    d->m_dropIndicatorRect = QRect();
                }
                break;
            case BelowItem:
                if (d->isIndexDropEnabled(index.parent()))
                {
                    d->m_dropIndicatorRect = QRect(rect.left(), rect.bottom(), rect.width(), 0);
                    event->accept();
                }
                else
                {
                    d->m_dropIndicatorRect = QRect();
                }
                break;
            case OnItem:
                if (d->isIndexDropEnabled(index))
                {
                    d->m_dropIndicatorRect = rect;
                    event->accept();
                }
                else
                {
                    d->m_dropIndicatorRect = QRect();
                }
                break;
            case OnViewport:
                d->m_dropIndicatorRect = QRect();

                if (d->isIndexDropEnabled(rootIndex()))
                {
                    event->accept(); // allow dropping in empty areas
                }
                break;
            }
        }
        else
        {
            d->m_dropIndicatorRect = QRect();
            d->m_dropIndicatorPosition = OnViewport;
            if (d->isIndexDropEnabled(rootIndex()))
            {
                event->accept(); // allow dropping in empty areas
            }
        }

        d->viewport->update();
    } // can decode

    if (d->shouldAutoScroll(event->pos()))
    {
        startAutoScroll();
    }
}

bool GlodonAbstractItemViewPrivate::droppingOnItself(QDropEvent *event, const QModelIndex &index)
{
    Q_Q(GlodonAbstractItemView);
    Qt::DropAction dropAction = event->dropAction();

    if (q->dragDropMode() == GlodonAbstractItemView::InternalMove)
    {
        dropAction = Qt::MoveAction;
    }

    if (event->source() == q
            && (Qt::MoveAction == (event->possibleActions() & Qt::MoveAction))
            && dropAction == Qt::MoveAction)
    {
        QModelIndexList selectedIndexes = q->selectedIndexes();
        QModelIndex child = index;

        while (child.isValid() && child != m_root)
        {
            if (selectedIndexes.contains(child))
            {
                return true;
            }

            child = child.parent();
        }
    }

    return false;
}

void GlodonAbstractItemView::dragLeaveEvent(QDragLeaveEvent *)
{
    Q_D(GlodonAbstractItemView);
    stopAutoScroll();
    setState(NoState);
    d->m_hover = QModelIndex();
    d->viewport->update();
}

void GlodonAbstractItemView::dropEvent(QDropEvent *event)
{
    Q_D(GlodonAbstractItemView);

    if (dragDropMode() == InternalMove)
    {
        if (event->source() != this || !(event->possibleActions() & Qt::MoveAction))
        {
            return;
        }
    }

    QModelIndex index;
    int col = -1;
    int row = -1;

    if (d->dropOn(event, &row, &col, &index))
    {
        if (d->m_model->dropMimeData(event->mimeData(),
                                     dragDropMode() == InternalMove ? Qt::MoveAction : event->dropAction(),
                                     row, col, index))
        {
            if (dragDropMode() == InternalMove)
            {
                event->setDropAction(Qt::MoveAction);
            }

            event->accept();
        }
    }

    stopAutoScroll();
    setState(NoState);
    d->viewport->update();
}

bool GlodonAbstractItemViewPrivate::dropOn(QDropEvent *event, int *dropRow, int *dropCol, QModelIndex *dropIndex)
{
    Q_Q(GlodonAbstractItemView);

    if (event->isAccepted())
    {
        return false;
    }

    QModelIndex index;
    // rootIndex() (i.e. the viewport) might be a valid index
    if (viewport->rect().contains(event->pos()))
    {
        index = q->indexAt(event->pos());
        if (!index.isValid() || !q->visualRect(index).contains(event->pos()))
        {
            index = m_root;
        }
    }

    // If we are allowed to do the drop
    if (0 != (m_model->supportedDropActions() & event->dropAction()))
    {
        int row = -1;
        int col = -1;
        if (index != m_root)
        {
            m_dropIndicatorPosition = position(event->pos(), q->visualRect(index), index);

            switch (m_dropIndicatorPosition)
            {
                case GlodonAbstractItemView::AboveItem:
                    row = index.row();
                    col = index.column();
                    index = index.parent();
                    break;

                case GlodonAbstractItemView::BelowItem:
                    row = index.row() + 1;
                    col = index.column();
                    index = index.parent();
                    break;

                case GlodonAbstractItemView::OnItem:
                case GlodonAbstractItemView::OnViewport:
                    break;
            }
        }
        else
        {
            m_dropIndicatorPosition = GlodonAbstractItemView::OnViewport;
        }

        *dropIndex = index;
        *dropRow = row;
        *dropCol = col;

        if (!droppingOnItself(event, index))
        {
            return true;
        }
    }

    return false;
}

GlodonAbstractItemView::DropIndicatorPosition
GlodonAbstractItemViewPrivate::position(const QPoint &pos, const QRect &rect, const QModelIndex &index) const
{
    GlodonAbstractItemView::DropIndicatorPosition dropPosition = GlodonAbstractItemView::OnViewport;

    if (!m_bDragDropOverwrite)
    {
        const int c_margin = 2;

        if (pos.y() - rect.top() < c_margin)
        {
            dropPosition = GlodonAbstractItemView::AboveItem;
        }
        else if (rect.bottom() - pos.y() < c_margin)
        {
            dropPosition = GlodonAbstractItemView::BelowItem;
        }
        else if (rect.contains(pos, true))
        {
            dropPosition = GlodonAbstractItemView::OnItem;
        }
    }
    else
    {
        QRect touchingRect = rect;
        touchingRect.adjust(-1, -1, 1, 1);

        if (touchingRect.contains(pos, false))
        {
            dropPosition = GlodonAbstractItemView::OnItem;
        }
    }

    if (dropPosition == GlodonAbstractItemView::OnItem && (!(m_model->flags(index) & Qt::ItemIsDropEnabled)))
        dropPosition
            = pos.y() < rect.center().y() ? GlodonAbstractItemView::AboveItem : GlodonAbstractItemView::BelowItem;

    return dropPosition;
}

bool GlodonAbstractItemViewPrivate::canDecode(QDropEvent *e) const
{
    QStringList modelTypes = m_model->mimeTypes();
    const QMimeData *mime = e->mimeData();

    for (int i = 0; i < modelTypes.count(); ++i)
    {
        if (mime->hasFormat(modelTypes.at(i))
                && (Qt::IgnoreAction != (e->dropAction() & m_model->supportedDropActions())))
        {
            return true;
        }
    }

    return false;
}

void GlodonAbstractItemViewPrivate::paintDropIndicator(QPainter *painter)
{
    if (m_bShowDropIndicator && m_state == GlodonAbstractItemView::DraggingState
        #ifndef QT_NO_CURSOR
            && viewport->cursor().shape() != Qt::ForbiddenCursor
        #endif
            )
    {
        QStyleOption opt;
        opt.init(q_func());
        opt.rect = m_dropIndicatorRect;
        q_func()->style()->drawPrimitive(QStyle::PE_IndicatorItemViewItemDrop, &opt, painter, q_func());
    }
}

#endif // QT_NO_DRAGANDDROP

void GlodonAbstractItemView::focusInEvent(QFocusEvent *event)
{
    Q_D(GlodonAbstractItemView);
    QAbstractScrollArea::focusInEvent(event);

    const QItemSelectionModel *pSelectionModel = selectionModel();
    const bool c_bCurrentIndexValid = currentIndex().isValid();

    // 第一次Show之前没有设置CurrentIndex时
    if (pSelectionModel && !d->m_bCurrentIndexSet && !c_bCurrentIndexValid)
    {
        bool bAutoScroll = d->m_bAutoScroll;
        d->m_bAutoScroll = false;

        QModelIndex index = moveCursor(MoveNext, Qt::NoModifier); // first visible index
        if (index.isValid() && d->isIndexEnabled(index) && event->reason() != Qt::MouseFocusReason)
        {
            selectionModel()->setCurrentIndex(index, QItemSelectionModel::NoUpdate);
        }

        d->m_bAutoScroll = bAutoScroll;
    }

    if (pSelectionModel && c_bCurrentIndexValid)
    {
        if (currentIndex().flags() != Qt::ItemIsEditable)
        {
            setAttribute(Qt::WA_InputMethodEnabled, false);
        }
        else
        {
            setAttribute(Qt::WA_InputMethodEnabled);
        }
    }

    if (!c_bCurrentIndexValid)
    {
        setAttribute(Qt::WA_InputMethodEnabled, false);
    }

    d->viewport->update();
}

void GlodonAbstractItemView::focusOutEvent(QFocusEvent *event)
{
    Q_D(GlodonAbstractItemView);
    QAbstractScrollArea::focusOutEvent(event);
    d->viewport->update();

#ifdef QT_SOFTKEYS_ENABLED

    if (!hasEditFocus())
    {
        removeAction(d->doneSoftKey);
    }

#endif
}

void GlodonAbstractItemView::keyPressEvent(QKeyEvent *event)
{
    Q_D(GlodonAbstractItemView);
    d->m_delayedAutoScroll.stop(); //any interaction with the view cancel the auto scrolling

#ifdef QT_KEYPAD_NAVIGATION
    dealWithKeyPadNavigation(event);
#endif

#if !defined(QT_NO_CLIPBOARD) && !defined(QT_NO_SHORTCUT)
    // TODO wangdd-a这里之前是处理复制粘贴的，我们为了自己控制是否允许复制粘贴，先不在这里处理复制粘贴了
    // 后面需要看看复制粘贴是不是有必要重新实现
#endif

    d->m_moveCursorUpdatedView = false;

    QPersistentModelIndex oNewCurrent = getNewCurrentAccordingToKeyPressOperation(event);
    if (setNewCurrentSelection(event, oNewCurrent))
        return;

    dealWithEventAcception(event);
}

void GlodonAbstractItemView::resizeEvent(QResizeEvent *event)
{
    QAbstractScrollArea::resizeEvent(event);
    updateGeometries();
}

/*!
  This function is called with the given \a event when a timer event is sent
  to the widget.

  \sa QObject::timerEvent()
*/
void GlodonAbstractItemView::timerEvent(QTimerEvent *event)
{
    Q_D(GlodonAbstractItemView);

    if (event->timerId() == d->m_fetchMoreTimer.timerId())
    {
        d->fetchMore();
    }
    else if (event->timerId() == d->m_delayedReset.timerId())
    {
        reset();
    }
    else if (event->timerId() == d->m_autoScrollTimer.timerId())
    {
        doAutoScroll();
    }
    else if (event->timerId() == d->m_updateTimer.timerId())
    {
        d->updateDirtyRegion();
    }
    else if (d->m_delayedShowToolTip.timerId() == event->timerId())
    {
        showCurIndexToolTip();
        d->m_delayedShowToolTip.stop();
    }
    else if (event->timerId() == d->m_delayedLayout.timerId())
    {
        d->m_delayedLayout.stop();
        doLayoutAndScroll();
    }
    else if (event->timerId() == d->m_delayedAutoScroll.timerId())
    {
        d->m_delayedAutoScroll.stop();

        //end of the timer: if the current item is still the same as the one when the mouse press occurred
        //we only get here if there was no double click
        if (d->m_pressedIndex.isValid() && d->m_pressedIndex == currentIndex())
        {
            scrollTo(d->m_pressedIndex);
        }
    }
}

void GlodonAbstractItemView::inputMethodEvent(QInputMethodEvent *event)
{
    if (event->commitString().isEmpty() && event->preeditString().isEmpty())
    {
        event->ignore();
        return;
    }

    if (!edit(currentIndex(), AnyKeyPressed, event))
    {
        if (!event->commitString().isEmpty())
        {
            keyboardSearch(event->commitString());
        }

        event->ignore();
    }
}

#ifndef QT_NO_DRAGANDDROP

GlodonAbstractItemView::DropIndicatorPosition GlodonAbstractItemView::dropIndicatorPosition() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_dropIndicatorPosition;
}

#endif

void  GlodonAbstractItemView::doGMFirst(GLDEvent *event)
{
    moveCursor(MoveHome, Qt::ControlModifier);
    G_UNUSED(event);
}

void  GlodonAbstractItemView::doGMQueryFirst(GLDEvent *event)
{
    event->setResult(currentIndex().row() != 0);
    G_UNUSED(event);
}

void  GlodonAbstractItemView::doGMPrev(GLDEvent *event)
{
    moveCursor(MoveUp, Qt::NoModifier);
    G_UNUSED(event);
}

void  GlodonAbstractItemView::doGMQueryPrev(GLDEvent *event)
{
    doGMQueryFirst(event);
    G_UNUSED(event);
}

void  GlodonAbstractItemView::doGMNext(GLDEvent *event)
{
    moveCursor(MoveDown, Qt::NoModifier);
    G_UNUSED(event);
}

void  GlodonAbstractItemView::doGMQueryNext(GLDEvent *event)
{
    doGMQueryLast(event);
    G_UNUSED(event);
}

void  GlodonAbstractItemView::doGMLast(GLDEvent *event)
{
    moveCursor(MoveEnd, Qt::ControlModifier);
    G_UNUSED(event);
}

void  GlodonAbstractItemView::doGMQueryLast(GLDEvent *event)
{
    Q_D(GlodonAbstractItemView);
    event->setResult(currentIndex().row() != d->m_model->rowCount(d->m_root) - 1);
    G_UNUSED(event);
}

void  GlodonAbstractItemView::doGMSetSel(GLDEvent *event)
{
    selectAll();
    G_UNUSED(event);
}

void  GlodonAbstractItemView::doGMQuerySetSel(GLDEvent *event)
{
    Q_D(GlodonAbstractItemView);
    event->setResult(d->m_bAllowSelectAll);
    G_UNUSED(event);
}

void  GlodonAbstractItemView::doGMSetColRow(GLDEvent *event)
{
    Q_D(GlodonAbstractItemView);
    int col = event->lParam();
    int row = event->wParam();
    QModelIndex index = d->m_model->index(row, col, d->m_root);
    setCurrentIndex(index);
    G_UNUSED(event);
}

// 控制Editor的通用Actions，例如：对Edior的复制，粘贴，剪切，删除等操作的Action
void GlodonAbstractItemView::controlEditorNormalActions(const QModelIndex &index)
{
    Q_D(GlodonAbstractItemView);

    GlodonDefaultItemDelegate *pGlodonDelegate = d->delegateForIndex(index);
    if (pGlodonDelegate == NULL)
    {
        return;
    }

    bool bReadOnly = false;
    GEditStyle edtStyle = pGlodonDelegate->editStyle(pGlodonDelegate->dataIndex(index), bReadOnly);
    QWidget *pEditor = pGlodonDelegate->curEditor();
    if (NULL != pEditor
            && !(edtStyle == esVectorGraph
                 || edtStyle == esBool
                 || edtStyle == esLineWidthList
                 || edtStyle == esFontList
                 || edtStyle == esColorList
                 || edtStyle == esImage
                 || edtStyle == esNone))
    {
        setEditorActSignals(pEditor);
        resetActAvailable(pEditor);
    }
}

void GlodonAbstractItemView::setEditorActSignals(QWidget *editor)
{
    if (connectEditorSignal(editor, "copyAvailable(bool)",
                            this, "doEditorCopyAvailable(bool)"))
    {
        doNothingMacro();
    }
    else
    {
        if (connectEditorSignal(editor, "cursorPositionChanged()",
                                this, "doCursorPositionChanged()"))
        {
            doNothingMacro();
        }
        else if (connectEditorSignal(editor, "cursorPositionChanged(int, int)",
                                     this, "doCursorPositionChanged(int, int)"))
        {
            doNothingMacro();
        }

        if (connectEditorSignal(editor, "selectionChanged()",
                                this, "doSelectionChanged()"))
        {
            doNothingMacro();
        }
    }

    if (connectEditorSignal(this, "onEditorCopy()",
                            editor, "copy()"))
    {
        doNothingMacro();
    }

    if (connectEditorSignal(this, "onEditorCut()",
                            editor, "cut()"))
    {
        doNothingMacro();
    }

    if (connectEditorSignal(this, "onEditorPaste()",
                            editor, "paste()"))
    {
        doNothingMacro();
    }

    if (connectEditorSignal(this, "onEditorDelete()",
                            editor, "deleteSelectedText()"))
    {
        doNothingMacro();
    }
}

bool GlodonAbstractItemView::connectEditorSignal(QWidget *sender, const char *signalName,
        QWidget *receiver, const char *slotName)
{
    int nSignalIndex = sender->metaObject()->indexOfSignal(signalName);
    int nSlotIndex = receiver->metaObject()->indexOfSlot(slotName);

    if (nSignalIndex != -1 && nSlotIndex != -1)
    {
        this->metaObject()->connect(sender, nSignalIndex, receiver, nSlotIndex);
        return true;
    }

    return false;
}

bool GlodonAbstractItemView::editorHasSelText(QWidget *editor)
{
    int nPropertyIndex = editor->metaObject()->indexOfProperty("hasSelectedText");

    if (nPropertyIndex != -1)
    {
        QMetaProperty meta = editor->metaObject()->property(nPropertyIndex);
        return meta.read(editor).toBool();
    }

    return false;
}

bool GlodonAbstractItemView::editorReadOnly(QWidget *editor)
{
    int nReadOnlyIndex = editor->metaObject()->indexOfProperty("readOnly");

    if (nReadOnlyIndex != -1)
    {
        QMetaProperty meta = editor->metaObject()->property(nReadOnlyIndex);
        return meta.read(editor).toBool();
    }

    return false;
}

void GlodonAbstractItemView::resetActAvailable(QWidget *editor)
{
    if (editor == NULL)
    {
        return;
    }

    bool bHasSelText = editorHasSelText(editor);
    emit onEditorCanCopy(bHasSelText);

    bool bIsROnly = editorReadOnly(editor);
    emit onEditorCanDelete(!bIsROnly);
    emit onEditorCanCut(bHasSelText && !bIsROnly);

    bool bHasClip = qApp->clipboard()->text().isEmpty();
    emit onEditorCanPaste(!bIsROnly && !bHasClip);
}

void GlodonAbstractItemView::disableEditorActions()
{
    emit onEditorCanCopy(false);
    emit onEditorCanDelete(false);
    emit onEditorCanCut(false);
    emit onEditorCanPaste(false);
}

bool GlodonAbstractItemView::isComment(const QPersistentModelIndex &index)
{
    GString commentText = model()->data(index, gidrCommentRole).toString();
    return !commentText.isEmpty();
}

/*!
    This convenience function returns a list of all selected and
    non-hidden item indexes in the view. The list contains no
    duplicates, and is not sorted.

    \sa QItemSelectionModel::selectedIndexes()
*/
QModelIndexList GlodonAbstractItemView::selectedIndexes() const
{
    Q_D(const GlodonAbstractItemView);
    QModelIndexList indexes;

    if (d->m_selectionModel)
    {
        indexes = d->m_selectionModel->selectedIndexes();
        QList<QModelIndex>::iterator it = indexes.begin();

        while (it != indexes.end())
        {
            if (isIndexHidden(*it))
            {
                it = indexes.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    return indexes;
}

bool GlodonAbstractItemView::edit(const QModelIndex &index, EditTrigger trigger, QEvent *event)
{
    Q_D(GlodonAbstractItemView);

    if (!d->isIndexValid(index))
    {
        return false;
    }

    QWidget *pIndexWidget = (d->m_persistent.isEmpty() ? static_cast<QWidget *>(0) : d->editorForIndex(index).widget.data());

    if (pIndexWidget != NULL)
    {
        if (pIndexWidget->focusPolicy() == Qt::NoFocus)
        {
            return false;
        }

        pIndexWidget->setFocus();

        return true;
    }

    if (trigger == DoubleClicked)
    {
        d->m_delayedAutoScroll.stop();
    }

    if (d->sendDelegateEvent(index, event))
    {
        update(index);
        return true;
    }

    // save the previous trigger before updating
    EditTriggers lastTrigger = d->m_lastTrigger;
    d->m_lastTrigger = trigger;

    if (!d->shouldEdit(trigger, d->m_model->buddy(index)))
    {
        return false;
    }

    // we will receive a mouseButtonReleaseEvent after a
    // mouseDoubleClickEvent, so we need to check the previous trigger
    if (lastTrigger == DoubleClicked && trigger == SelectedClicked)
    {
        return false;
    }

    // we may get a double click event later
    if (trigger == SelectedClicked)
    {
        edit(index);
        return true;
    }
    else
    {
        if (d->openEditor(index, d->shouldForwardEvent(trigger, event) ? event : 0))
        {
            emit isInEditing(true);
            controlEditorNormalActions(index);
            return true;
        }
    }

    return false;
}

void GlodonAbstractItemView::updateEditorData()
{
    Q_D(GlodonAbstractItemView);
    d->updateEditorData(QModelIndex(), QModelIndex());
}

/*!
    \internal
    Updates the geometry of the open editor widgets in the view.
*/
void GlodonAbstractItemView::updateEditorGeometries()
{
    Q_D(GlodonAbstractItemView);

    if (d->m_editorIndexHash.isEmpty())
    {
        return;
    }

    QStyleOptionViewItem option = d->viewOptionsV4();
    GEditorIndexHash::iterator it = d->m_editorIndexHash.begin();
    QWidgetList oEditorToReleaseList;
    QWidgetList oEditorToHideList;

    while (it != d->m_editorIndexHash.end())
    {
        QModelIndex oIndex = it.value();
        QWidget *pEditor = it.key();
        if (oIndex.isValid() && pEditor)
        {
            option.rect = visualRect(oIndex);
            if (option.rect.isValid())
            {
                pEditor->show();
                GlodonDefaultItemDelegate *pIndexDelegate = d->delegateForIndex(oIndex);
                if (pIndexDelegate)
                {
                    pIndexDelegate->updateEditorGeometry(pEditor, option, oIndex);
                }
            }
            else
            {
                oEditorToHideList << pEditor;
            }
            ++it;
        }
        else
        {
            d->m_indexEditorHash.remove(it.value());
            it = d->m_editorIndexHash.erase(it);
            oEditorToReleaseList << pEditor;
        }
    }

    //we hide and release the editor outside of the loop because it might change the focus and try
    //to change the editors hashes.
    for (int i = 0; i < oEditorToHideList.count(); ++i)
    {
        oEditorToHideList.at(i)->hide();
    }

    for (int i = 0; i < oEditorToReleaseList.count(); ++i)
    {
        d->releaseEditor(oEditorToReleaseList.at(i));
    }
}

void GlodonAbstractItemView::updateGeometries()
{
    updateEditorGeometries();
    d_func()->m_fetchMoreTimer.start(0, this); //fetch more later
}

void GlodonAbstractItemView::verticalScrollbarValueChanged(int value)
{
    Q_D(GlodonAbstractItemView);

    if (verticalScrollBar()->maximum() == value && d->m_model->canFetchMore(d->m_root))
    {
        d->m_model->fetchMore(d->m_root);
    }

    QPoint posInVp = viewport()->mapFromGlobal(QCursor::pos());

    if (viewport()->rect().contains(posInVp))
    {
        d->checkMouseMove(posInVp);
    }
}

void GlodonAbstractItemView::syncStateToDelegate(GlodonDefaultItemDelegate *delegate)
{
    Q_D(GlodonAbstractItemView);
    delegate->setIncludeLeading(d->m_bGridTextIncludeLeading);
}

void GlodonAbstractItemView::installEventFilterWidthEdit(QWidget *widget, GlodonDefaultItemDelegate *delegate)
{
    if (QAbstractScrollArea *pScrollArea = dynamic_cast<QAbstractScrollArea *>(widget))
    {
        pScrollArea->viewport()->installEventFilter(delegate);
    }
    else if (GLDWindowComboBox *pComboBox = dynamic_cast<GLDWindowComboBox *>(widget))
    {
        if (pComboBox && pComboBox->getLineEdit())
        {
            pComboBox->getLineEdit()->installEventFilter(delegate);
        }
    }
    else if (GLDWindowComboBoxEx *pComboBox = dynamic_cast<GLDWindowComboBoxEx *>(widget))
    {
        if (pComboBox && pComboBox->getLineEdit())
        {
            pComboBox->getLineEdit()->viewport()->installEventFilter(delegate);
        }
    }
    else if (GLDCustomComboBox *pComboBox = dynamic_cast<GLDCustomComboBox *>(widget))
    {
        if (pComboBox && (pComboBox->lineEdit() != NULL))
        {
            pComboBox->lineEdit()->installEventFilter(delegate);
        }
    }
}

void GlodonAbstractItemView::dealWithSelectionAndAutoScroll(QModelIndex index, QMouseEvent *event)
{
    Q_D(GlodonAbstractItemView);

    QPoint pos = event->pos();

    // 以下都在处理SelectionModel，并将可选中的Index滚出来
    d->m_pressedAlreadySelected = d->m_selectionModel->isSelected(index);
    d->m_pressedIndex = index;
    d->m_pressedModifiers = event->modifiers();

    QItemSelectionModel::SelectionFlags command = selectionCommand(index, event);
    if (command == QItemSelectionModel::NoUpdate)
    {
        d->m_noSelectionOnMousePress = true;
    }
    else
    {
        d->m_noSelectionOnMousePress = false;
    }

    QPoint offset = d->offset();

    // 如果index即将被设置为Current，需要更新pressedPosition
    if ((command & QItemSelectionModel::Current) == 0)
    {
        d->m_pressedPosition = pos + offset;
    }
    // 下面这个分支不知道啥时候会进来，看样子是下面的意思：
    // 如果index不会即将被设置为Current
    // 但是由于滚动之类的，导致通过m_pressedPosition获取的Index无效了，则需要重新设置
    else if (!indexAt(d->m_pressedPosition - offset).isValid())
    {
        d->m_pressedPosition = visualRect(currentIndex()).center() + offset;
    }

    if (d->isIndexEnabled(index)
            && (cursor().shape() == Qt::ArrowCursor
                || cursor().shape() == Qt::PointingHandCursor))
    {
        if (command.testFlag(QItemSelectionModel::Toggle))
        {
            command &= ~QItemSelectionModel::Toggle;
            d->m_ctrlDragSelectionFlag = d->m_selectionModel->isSelected(index) ? QItemSelectionModel::Deselect : QItemSelectionModel::Select;
            command |= d->m_ctrlDragSelectionFlag;

            if ((d->m_selectionMode != SingleSelection) || (d->m_selectionMode != NoSelection))
            {
                d->m_bIsInMultiSelect = true;
            }
        }
        else
        {
            d->m_bIsInMultiSelect = false;
        }

        // we disable scrollTo for mouse press so the item doesn't change position
        // when the user is interacting with it (ie. clicking on it)
        bool bAutoScroll = d->m_bAutoScroll;
        d->m_bAutoScroll = false;
        // 给外部判断时机，决定是否可以改变焦点格子
        bool bShouldSelect = moveCurrent(currentIndex(), index, command, mrMouse);
        d->m_bAutoScroll = bAutoScroll;

        if (bShouldSelect)
        {
            QRect rect(d->m_pressedPosition - offset, pos);
            setSelection(rect, command);
        }

        // signal handlers may change the model
        emit pressed(index);

        if (d->m_bAutoScroll)
        {
            //we delay the autoscrolling to filter out double click event
            //100 is to be sure that there won't be a double-click misinterpreted as a 2 single clicks
            d->m_delayedAutoScroll.start(QApplication::doubleClickInterval() + 100, this);
        }
    }
    else
    {
        // Forces a finalize() even if mouse is pressed, but not on a item
        d->m_selectionModel->select(QModelIndex(), QItemSelectionModel::Select);
    }
}

#ifdef QT_KEYPAD_NAVIGATION
void GlodonAbstractItemView::dealWithKeyPadNavigation(QKeyEvent *event)
{
    Q_D(GlodonAbstractItemView);
    switch (event->key())
    {
    case Qt::Key_Select:
        if (QApplication::keypadNavigationEnabled())
        {
            if (!hasEditFocus())
            {
                setEditFocus(true);
                return;
            }
        }
        break;
    case Qt::Key_Back:
        if (QApplication::keypadNavigationEnabled() && hasEditFocus())
        {
            setEditFocus(false);
        }
        else
        {
            event->ignore();
        }
        return;
    case Qt::Key_Down:
    case Qt::Key_Up:
        // Let's ignore vertical navigation events, only if there is no other widget
        // what can take the focus in vertical direction. This means widget can handle navigation events
        // even the widget don't have edit focus, and there is no other widget in requested direction.
        if (QApplication::keypadNavigationEnabled() && !hasEditFocus()
                && QWidgetPrivate::canKeypadNavigate(Qt::Vertical))
        {
            event->ignore();
            return;
        }
        break;
    case Qt::Key_Left:
    case Qt::Key_Right:
        // Similar logic as in up and down events
        if (QApplication::keypadNavigationEnabled() && !hasEditFocus()
                && (QWidgetPrivate::canKeypadNavigate(Qt::Horizontal) || QWidgetPrivate::inTabWidget(this)))
        {
            event->ignore();
            return;
        }
        break;
    default:
        if (QApplication::keypadNavigationEnabled() && !hasEditFocus())
        {
            event->ignore();
            return;
        }
    }
}
#endif

QPersistentModelIndex GlodonAbstractItemView::getNewCurrentAccordingToKeyPressOperation(QKeyEvent *event)
{
    Q_D(GlodonAbstractItemView);

    QPersistentModelIndex oNewCurrent;
    switch (event->key())
    {
    case Qt::Key_Down:
    {
        oNewCurrent = moveCursor(MoveDown, event->modifiers());
        break;
    }
    case Qt::Key_Up:
    {
        oNewCurrent = moveCursor(MoveUp, event->modifiers());
        break;
    }
    case Qt::Key_Left:
    {
        oNewCurrent = moveCursor(MoveLeft, event->modifiers());
        break;
    }
    case Qt::Key_Right:
    {
        oNewCurrent = moveCursor(MoveRight, event->modifiers());
        break;
    }
    case Qt::Key_Home:
    {
        oNewCurrent = moveCursor(MoveHome, event->modifiers());
        break;
    }
    case Qt::Key_End:
    {
        oNewCurrent = moveCursor(MoveEnd, event->modifiers());
        break;
    }
    case Qt::Key_PageUp:
    {
        oNewCurrent = moveCursor(MovePageUp, event->modifiers());
        scrollToNewCurrent(event, oNewCurrent);
        break;
    }
    case Qt::Key_PageDown:
    {
        oNewCurrent = moveCursor(MovePageDown, event->modifiers());
        scrollToNewCurrent(event, oNewCurrent);
        break;
    }
    case Qt::Key_Tab:
    {
        if (d->m_bTabKeyNavigation)
        {
            oNewCurrent = moveCursor(MoveNext, event->modifiers());
        }
        break;
    }
    case Qt::Key_Backtab:
    {
        if (d->m_bTabKeyNavigation)
        {
            oNewCurrent = moveCursor(MovePrevious, event->modifiers());
        }
        break;
    }
    }
    return oNewCurrent;
}

bool GlodonAbstractItemView::setNewCurrentSelection(QKeyEvent *event, QPersistentModelIndex oNewCurrent)
{
    Q_D(GlodonAbstractItemView);

    QPersistentModelIndex oOldCurrent = currentIndex();

    if (!oNewCurrent.isValid() || oNewCurrent == oOldCurrent || !d->isIndexEnabled(oNewCurrent))
        return false;

    // TODO liurx 以下代码比较诡异，需要详细测试决定是否需要
//    // 如果编辑方式是Ellipsis的时候，focusWidget永远都是QlineEdit,indexWidget(oldCurrent)是GLDLineButtonEdit
//    // 在此只应该判断newCurrent与oldCurrent是否是一个
//    if (!hasFocus())
//    {
//        commitData(indexWidget(oOldCurrent));

//        if (isLegalData())
//        {
//            setFocus();
//        }
//        else
//        {
//            return;
//        }
//    }

    QItemSelectionModel::SelectionFlags command = selectionCommand(oNewCurrent, event);

    if (command == QItemSelectionModel::NoUpdate
            && 0 == style()->styleHint(QStyle::SH_ItemView_MovementWithoutUpdatingSelection, 0, this))
        return false;

    // note that we don't check if the new current index is enabled because moveCursor() makes sure it is
    if (command & QItemSelectionModel::Current)
    {
        bool bShouldSelect = moveCurrent(currentIndex(), oNewCurrent, command, mrKey);

        if (!indexAt(d->m_pressedPosition - d->offset()).isValid())
        {
            d->m_pressedPosition = visualRect(oOldCurrent).center() + d->offset();
        }

        if (bShouldSelect)
        {
            QRect rect(d->m_pressedPosition - d->offset(), visualRect(oNewCurrent).center());
            setSelection(rect, command);
        }
    }
    else
    {
        //当按着Control键时，如有选择区域的改变，则也会重新设置选择区域
        if (event->modifiers() & Qt::ControlModifier)
        {
            command |= QItemSelectionModel::ClearAndSelect;
        }

        bool bShouldSelect = moveCurrent(currentIndex(), oNewCurrent, command, mrKey);
        d->m_pressedPosition = visualRect(oNewCurrent).center() + d->offset();

        if (oNewCurrent.isValid() && bShouldSelect)
        {
            // We copy the same behaviour as for mousePressEvent().
            QRect rect(d->m_pressedPosition - d->offset(), QSize(1, 1));
            setSelection(rect, command);
        }
    }

    event->accept();
    return true;
}

void GlodonAbstractItemView::scrollToNewCurrent(QKeyEvent *event, QPersistentModelIndex &oNewCurrent)
{
    if (Qt::ControlModifier == (event->modifiers() & Qt::ControlModifier))
    {
        scrollTo(oNewCurrent, EnsureVisible);
    }
    else
    {
        scrollTo(oNewCurrent, PositionAtBottom);
    }
}

void GlodonAbstractItemView::dealWithEventAcception(QKeyEvent *event)
{
    Q_D(GlodonAbstractItemView);

    switch (event->key())
    {
    // ignored keys
    case Qt::Key_Down:
    case Qt::Key_Up:
#ifdef QT_KEYPAD_NAVIGATION
        if (QApplication::keypadNavigationEnabled() && QWidgetPrivate::canKeypadNavigate(Qt::Vertical))
        {
            event->accept(); // don't change focus
            break;
        }

#endif
    case Qt::Key_Left:
    case Qt::Key_Right:
#ifdef QT_KEYPAD_NAVIGATION
        if (QApplication::navigationMode() == Qt::NavigationModeKeypadDirectional
                && (QWidgetPrivate::canKeypadNavigate(Qt::Horizontal)
                    || (QWidgetPrivate::inTabWidget(this) && d->model->columnCount(d->root) > 1)))
        {
            event->accept(); // don't change focus
            break;
        }
#endif // QT_KEYPAD_NAVIGATION
    case Qt::Key_Home:
    case Qt::Key_End:
    case Qt::Key_PageUp:
    case Qt::Key_PageDown:
    case Qt::Key_Escape:
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        event->ignore();
        break;
    case Qt::Key_Space:
    case Qt::Key_Select:
        if (!edit(currentIndex(), AnyKeyPressed, event) && d->m_selectionModel)
        {
            d->m_selectionModel->select(currentIndex(), selectionCommand(currentIndex(), event));
        }
#ifdef QT_KEYPAD_NAVIGATION
        if (event->key() == Qt::Key_Select)
        {
            // Also do Key_Enter action.
            if (currentIndex().isValid())
            {
                if (state() != EditingState)
                {
                    emit activated(currentIndex());
                }
            }
            else
            {
                event->ignore();
            }
        }
#endif
        break;
#ifdef Q_WS_MAC
    case Qt::Key_Enter:
    case Qt::Key_Return:
        // Propagate the enter if you couldn't edit the item and there are no
        // current editors (if there are editors, the event was most likely propagated from it).
        if (!edit(currentIndex(), EditKeyPressed, event) && d->editorIndexHash.isEmpty())
        {
            event->ignore();
        }
        break;
#else
    case Qt::Key_F2:
        if (edit(currentIndex(), EditKeyPressed, event))
        {
            QWidget *editor = dynamic_cast<QWidget *>(d->editorForIndex(currentIndex()).widget.data());
            if (editor && editor->hasFocus())
            {
                QCoreApplication::sendEvent(editor, event);
            }
        }
        else
        {
            event->ignore();
        }
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        // ### we can't open the editor on enter, becuse
        // some widgets will forward the enter event back
        // to the viewport, starting an endless loop
        if (state() != EditingState || hasFocus())
        {
            if (currentIndex().isValid())
            {
                emit activated(currentIndex());
            }
            event->ignore();
        }
        break;
#endif
    case Qt::Key_A:
        if (event->modifiers() & Qt::ControlModifier)
        {
            selectAll();
            break;
        }
    default:
    {
#ifdef Q_WS_MAC
        if (event->key() == Qt::Key_O && event->modifiers() & Qt::ControlModifier && currentIndex().isValid())
        {
            emit activated(currentIndex());
            break;
        }
#endif
        bool bModified = event->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier);
        if (!event->text().isEmpty() && !bModified && !edit(currentIndex(), AnyKeyPressed, event) && d->m_isSearchModel)
        {
            keyboardSearch(event->text());
            event->accept();
        }
        else
        {
            event->ignore();
        }
        break;
    }
    }

    if (d->m_moveCursorUpdatedView)
    {
        event->accept();
    }
}

void GlodonAbstractItemView::setSelectionAndScrollToOnMouseMove(
        const QModelIndex &oCurIndex, QMouseEvent *event, const QPoint &oPrePoint, const QPoint &oCurPoint)
{
    Q_D(GlodonAbstractItemView);

    if (event->buttons() & Qt::LeftButton && d->selectionAllowed(oCurIndex) && d->m_selectionModel)
    {
        setState(DragSelectingState);
        QItemSelectionModel::SelectionFlags command = selectionCommand(oCurIndex, event);

        if (d->m_ctrlDragSelectionFlag != QItemSelectionModel::NoUpdate
                && command.testFlag(QItemSelectionModel::Toggle))
        {
            command &= ~QItemSelectionModel::Toggle;
            command |= d->m_ctrlDragSelectionFlag;
        }

        // Do the normalize ourselves, since QRect::normalized() is flawed
        QRect selectionRect = QRect(oPrePoint, oCurPoint);
        setSelection(selectionRect, command);

        // set at the end because it might scroll the view
        if (oCurIndex.isValid()
                && (oCurIndex != d->m_selectionModel->currentIndex())
                && d->isIndexEnabled(oCurIndex))
        {
            //不改变当前选择区域的焦点，且能使滚动条在适当的时机滚动
            if (!d->m_autoScrollTimer.isActive())
            {
                if (d->m_bAutoScroll)
                {
                    // 最后一列和一行是需要滚出来的，固定行列是需要滚动的
                    scrollTo(oCurIndex);
                }

                update(oCurIndex);

                // 乐观预计，可能支持懒加载。。。。
                if (oCurIndex.row() == (d->m_model->rowCount(d->m_root) - 1))
                {
                    d->fetchMore();
                }
            }
        }
    }
}

void GlodonAbstractItemView::showCurIndexToolTip()
{
    Q_D(GlodonAbstractItemView);

    QPoint oCurLocalPos = d->viewport->mapFromGlobal(QCursor::pos());
    //QPoint localPt = QCursor::pos();
    QPersistentModelIndex oCurIndex = indexAt(oCurLocalPos);

    // TODO wangdd-a，批注框和ToolTip是互斥的
    // 修改批注框的时候，仔细看一下，基类不应该关心子类的功能，看能不能移到子类
    if (isComment(oCurIndex) || !d->m_pToolTipFrame)
    {
        return;
    }

    oCurLocalPos = d->viewport->mapToGlobal(oCurLocalPos);
    //frame本来应该在自己的位置显示，通过外部来调节，但由于目前其他代码可能用到，没办法更改了。
    //现在frame在移动的时候，x增加35，y增加45，所以这里通过外部重新计算frame的合适位置
    int nCurPosY = oCurLocalPos.y() - 45;
    QWidget *pCurWidget = QApplication::widgetAt(QCursor::pos());

    //当刚刚获得焦点的时候，可能获得不到widget.
    if (pCurWidget == NULL)
    {
        return;
    }

    Qt::CursorShape cursorShpae = pCurWidget->cursor().shape();

    // TODO wangdd-a，不知道这些数字是啥。。。
    if (cursorShpae ==  Qt::IBeamCursor)
    {
        nCurPosY += 10;
    }
    else
    {
        nCurPosY += 20;
    }

    oCurLocalPos.setX(oCurLocalPos.x() - 35);
    oCurLocalPos.setY(nCurPosY);
    d->m_pToolTipFrame->move(oCurLocalPos);
    QCoreApplication::processEvents();

    QVariant hintContent = oCurIndex.data(Qt::ToolTipRole);
    d->m_pToolTipFrame->setHintText(hintContent.isNull() && d->m_bShowIndexContent ?
                                        oCurIndex.data().toString() : hintContent.toString());
}

void GlodonAbstractItemView::doLayoutAndScroll()
{
    Q_D(GlodonAbstractItemView);

    if (!isVisible())
    {
        return;
    }

    d->interruptDelayedItemsLayout();
    doItemsLayout();
    const QModelIndex c_current = currentIndex();

    if (c_current.isValid() && d->m_state == GlodonAbstractItemView::EditingState)
    {
        scrollTo(c_current);
    }
}

void GlodonAbstractItemView::scrollVerticalScrollBar(int nVerticalValue, QScrollBar *verticalScroll)
{
    Q_D(GlodonAbstractItemView);

    int nMargin = d->m_nAutoScrollMargin;
    QRect oClipRect = static_cast<GlodonAbstractItemView *>(d->viewport)->d_func()->clipRect();
    QPoint oCurGlobalPos = d->viewport->mapFromGlobal(QCursor::pos());

    if (oCurGlobalPos.y() - oClipRect.top() < nMargin)
    {
        verticalScroll->setValue(nVerticalValue - d->m_autoScrollCount);
    }
    else if (oClipRect.bottom() - oCurGlobalPos.y() < nMargin)
    {
        verticalScroll->setValue(nVerticalValue + d->m_autoScrollCount);
    }
}

void GlodonAbstractItemView::scrollHorizontalScrollBar(int nHorizontalValue, QScrollBar *horizontalScroll)
{
    Q_D(GlodonAbstractItemView);

    int nMargin = d->m_nAutoScrollMargin;
    QRect oClipRect = static_cast<GlodonAbstractItemView *>(d->viewport)->d_func()->clipRect();
    QPoint oCurGlobalPos = d->viewport->mapFromGlobal(QCursor::pos());

    if (oCurGlobalPos.x() - oClipRect.left() < nMargin)
    {
        horizontalScroll->setValue(nHorizontalValue - d->m_autoScrollCount);
    }
    else if (oClipRect.right() - oCurGlobalPos.x() < nMargin)
    {
        horizontalScroll->setValue(nHorizontalValue + d->m_autoScrollCount);
    }
}

void GlodonAbstractItemView::removeNonPersistentEditor(QWidget *editor)
{
    Q_D(GlodonAbstractItemView);

    bool bPersistent = d->m_persistent.contains(editor);
    if (bPersistent)
    {
        return;
    }

    setState(NoState);
    QModelIndex oEditorIndex = d->indexForEditor(editor);
    editor->removeEventFilter(d->delegateForIndex(oEditorIndex));
    d->removeEditor(editor);
    d->m_oEditorIndex = QModelIndex();
}

void GlodonAbstractItemView::setEditorFocus(QWidget *editor)
{
    Q_D(GlodonAbstractItemView);

    bool bHadFocus = editor->hasFocus();

    if (bHadFocus)
    {
        setFocus();    // this will send a focusLost event to the editor
    }
    else
    {
        d->checkPersistentEditorFocus();
    }

    QPointer<QWidget> ed = editor;
    QApplication::sendPostedEvents(editor, 0);
    editor = ed;

    bool bPersistent = d->m_persistent.contains(editor);

    if (!bPersistent && editor)
    {
        d->releaseEditor(editor);
    }
}

void GlodonAbstractItemView::dealWithEndEditHint(GlodonDefaultItemDelegate::EndEditHint hint)
{
    Q_D(GlodonAbstractItemView);

    QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::ClearAndSelect
            | d->selectionBehaviorFlags();

    switch (hint)
    {
    case GlodonDefaultItemDelegate::EditNextItem:
    {
        QModelIndex index = moveCursor(MoveNext, Qt::NoModifier);

        if (index.isValid())
        {
            QPersistentModelIndex persistent(index);
            moveCurrent(currentIndex(), persistent, flags, mrProgram);
        }

        break;
    }

    case GlodonDefaultItemDelegate::EditPreviousItem:
    {
        QModelIndex index = moveCursor(MovePrevious, Qt::NoModifier);

        if (index.isValid())
        {
            QPersistentModelIndex persistent(index);
            moveCurrent(currentIndex(), persistent, flags, mrProgram);
        }

        break;
    }

    case GlodonDefaultItemDelegate::SubmitModelCache:
    {
        d->m_model->submit();
        break;
    }

    case GlodonDefaultItemDelegate::RevertModelCache:
    {
        d->m_model->revert();
        break;
    }

    default:
        break;
    }
}

void GlodonAbstractItemView::resetEditorFocus()
{
    Q_D(GlodonAbstractItemView);

    QWidget *widget = d->delegateForIndex(d->m_oEditorIndex)->curEditor();

    if (widget && widget->isVisible())
    {
        widget->setFocus();
    }
}

void GlodonAbstractItemView::_q_columnsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    d_func()->_q_columnsAboutToBeRemoved(parent, start, end);
}

void GlodonAbstractItemView::_q_columnsRemoved(const QModelIndex &index, int start, int end)
{
    d_func()->_q_columnsRemoved(index, start, end);
}

void GlodonAbstractItemView::_q_columnsInserted(const QModelIndex &index, int start, int end)
{
    d_func()->_q_columnsInserted(index, start, end);
}

void GlodonAbstractItemView::_q_rowsInserted(const QModelIndex &index, int start, int end)
{
    d_func()->_q_rowsInserted(index, start, end);
}

void GlodonAbstractItemView::_q_rowsRemoved(const QModelIndex &index, int start, int end)
{
    d_func()->_q_rowsRemoved(index, start, end);
}

void GlodonAbstractItemView::_q_modelDestroyed()
{
    d_func()->_q_modelDestroyed();
}

void GlodonAbstractItemView::_q_layoutChanged()
{
    d_func()->_q_layoutChanged();
}

void GlodonAbstractItemView::_q_headerDataChanged()
{
    d_func()->_q_headerDataChanged();
}

void GlodonAbstractItemView::horizontalScrollbarValueChanged(int value)
{
    Q_D(GlodonAbstractItemView);

    if (horizontalScrollBar()->maximum() == value && d->m_model->canFetchMore(d->m_root))
    {
        d->m_model->fetchMore(d->m_root);
    }

    QPoint posInVp = viewport()->mapFromGlobal(QCursor::pos());

    if (viewport()->rect().contains(posInVp))
    {
        d->checkMouseMove(posInVp);
    }
}

void GlodonAbstractItemView::verticalScrollbarAction(int)
{
    //do nothing
}

void GlodonAbstractItemView::horizontalScrollbarAction(int)
{
    //do nothing
}

void GlodonAbstractItemView::closeEditor(QWidget *editor, bool &canCloseEditor, GlodonDefaultItemDelegate::EndEditHint hint)
{
    Q_UNUSED(canCloseEditor)

    Q_D(GlodonAbstractItemView);

    // TODO wangdd-a，觉得可以去掉
    // 使所有Editor的操作都变成disable
    disableEditorActions();

    // Close the editor
    if (editor)
    {
        QModelIndex oEditorIndex = d->indexForEditor(editor);

        if (!oEditorIndex.isValid())
        {
            return; // the editor was not registered
        }

        removeNonPersistentEditor(editor);
        setEditorFocus(editor);
        emit isInEditing(false);
    }

    // The EndEditHint part
    dealWithEndEditHint(hint);
}

void GlodonAbstractItemView::commitData(QWidget *editor, bool &canCloseEditor)
{
    Q_D(GlodonAbstractItemView);

    if (!editor || !d->m_itemDelegate || d->m_pCurCommittingEditor)
    {
        return;
    }

    QModelIndex pEditorIndex = d->indexForEditor(editor);

    if(!pEditorIndex.isValid())
    {
        return;
    }

    GlodonDefaultItemDelegate *pDelegate = d->delegateForIndex(pEditorIndex);
    editor->removeEventFilter(pDelegate);

    try
    {
        setModelData(pDelegate, editor, pEditorIndex, canCloseEditor);
        setLegitimacyOfData(pDelegate, canCloseEditor);
    }
    catch(...)
    {
        editor->installEventFilter(pDelegate);
        d->m_pCurCommittingEditor = NULL;
        throw;
    }

    editor->installEventFilter(pDelegate);
    d->m_pCurCommittingEditor = NULL;
}

void GlodonAbstractItemView::editorDestroyed(QObject *editor)
{
    Q_D(GlodonAbstractItemView);
    QWidget *widgt = dynamic_cast<QWidget *>(editor);
    d->removeEditor(widgt);
    d->m_persistent.remove(widgt);

    if (state() == EditingState)
    {
        setState(NoState);
    }
}

void GlodonAbstractItemView::doSelectionChanged()
{
    QWidget *editor = (QWidget *)this->sender();
    resetActAvailable(editor);
}

void GlodonAbstractItemView::doEditorCopyAvailable(bool canCopy)
{
    QWidget *editor = (QWidget *)this->sender();

    emit onEditorCanCopy(canCopy);

    bool breadOnly = editorReadOnly(editor);
    emit onEditorCanCut(canCopy && !breadOnly);
    emit onEditorCanDelete(canCopy && !breadOnly);

    bool bhasClip = qApp->clipboard()->text().isEmpty();
    emit onEditorCanPaste(!breadOnly && !bhasClip);
}

void GlodonAbstractItemView::doCursorPositionChanged()
{
    QWidget *editor = (QWidget *)this->sender();
    resetActAvailable(editor);
}

void GlodonAbstractItemView::doCursorPositionChanged(int, int)
{
    QWidget *editor = (QWidget *)this->sender();
    resetActAvailable(editor);
}

void GlodonAbstractItemView::setHorizontalStepsPerItem(int steps)
{
    Q_UNUSED(steps)
    // do nothing
}

int GlodonAbstractItemView::horizontalStepsPerItem() const
{
    return 1;
}

void GlodonAbstractItemView::setVerticalStepsPerItem(int steps)
{
    Q_UNUSED(steps)
    // do nothing
}

int GlodonAbstractItemView::verticalStepsPerItem() const
{
    return 1;
}

bool GlodonAbstractItemView::moveCurrent(const QModelIndex &oldIndex, const QModelIndex &newIndex,
        QItemSelectionModel::SelectionFlags command,
        MoveReason moveReason)
{
    bool bCanMove = true;
    beforeMoveCurrent(oldIndex, newIndex, command, moveReason, bCanMove);

    if (!bCanMove)
    {
        return bCanMove;
    }

    doMoveCurrent(oldIndex, newIndex, command, moveReason);
    afterMoveCurrent(oldIndex, newIndex, command, moveReason);
    return bCanMove;
}

void GlodonAbstractItemView::beforeMoveCurrent(const QModelIndex &oldIndex, const QModelIndex &newIndex,
        QItemSelectionModel::SelectionFlags command,
        MoveReason moveReason, bool &canMove)
{
    emit onbeforeMoveCurrent(oldIndex, newIndex, command, moveReason, canMove);
}

void GlodonAbstractItemView::doMoveCurrent(const QModelIndex &oldIndex, const QModelIndex &newIndex,
        QItemSelectionModel::SelectionFlags command, MoveReason moveReason)
{
    Q_D(GlodonAbstractItemView);
    d->m_selectionModel->setCurrentIndex(newIndex, command);

    G_UNUSED(oldIndex);
    G_UNUSED(moveReason);
}

void GlodonAbstractItemView::afterMoveCurrent(const QModelIndex &oldIndex, const QModelIndex &newIndex,
        QItemSelectionModel::SelectionFlags command, MoveReason moveReason)
{
    emit onAfterMoveCurrent(oldIndex, newIndex, command, moveReason);
}

void GlodonAbstractItemView::keyboardSearch(const QString &search)
{
    Q_D(GlodonAbstractItemView);

    if (0 == d->m_model->rowCount(d->m_root) || 0 == d->m_model->columnCount(d->m_root))
    {
        return;
    }

    QModelIndex oStartIndex = currentIndex().isValid() ? currentIndex()
                        : d->m_model->index(0, 0, d->m_root);
    bool bSkipRow = false;
    bool bKeyboardTimeWasValid = d->m_keyboardInputTime.isValid();
    gint64 nKeyboardInputTimeElapsed = d->m_keyboardInputTime.restart();
    if (search.isEmpty() || !bKeyboardTimeWasValid
            || nKeyboardInputTimeElapsed > QApplication::keyboardInputInterval())
    {
        d->m_keyboardInput = search;
        bSkipRow = currentIndex().isValid(); //if it is not valid we should really start at QModelIndex(0,0)
    }
    else
    {
        d->m_keyboardInput += search;
    }

    // special case for searches with same key like 'aaaaa'
    bool bsameKey = false;
    if (d->m_keyboardInput.length() > 1)
    {
        int nCount = d->m_keyboardInput.count(d->m_keyboardInput.at(d->m_keyboardInput.length() - 1));
        bsameKey = (nCount == d->m_keyboardInput.length());
        if (bsameKey)
        {
            bSkipRow = true;
        }
    }

    // skip if we are searching for the same key or a new search started
    if (bSkipRow)
    {
        QModelIndex parent = oStartIndex.parent();
        int newRow = (oStartIndex.row() < d->m_model->rowCount(parent) - 1) ? oStartIndex.row() + 1 : 0;
        oStartIndex = d->m_model->index(newRow, oStartIndex.column(), parent);
    }

    // search from start with wraparound
    const QString c_strSearchString = bsameKey ? QString(d->m_keyboardInput.at(0)) : d->m_keyboardInput;
    QModelIndex oCurrentIndex = oStartIndex;
    QModelIndexList oMatchIndexList;
    QModelIndex oFirstMatchIndex;
    QModelIndex oStartMatchIndex;
    QModelIndexList oPreviousIndexList;

    do
    {
        oMatchIndexList = d->m_model->match(oCurrentIndex, Qt::DisplayRole, c_strSearchString);

        if (oMatchIndexList == oPreviousIndexList)
        {
            break;
        }

        oFirstMatchIndex = oMatchIndexList.value(0);
        oPreviousIndexList = oMatchIndexList;
        if (oFirstMatchIndex.isValid())
        {
            if (d->isIndexEnabled(oFirstMatchIndex))
            {
                setCurrentIndex(oFirstMatchIndex);
                break;
            }

            int row = oFirstMatchIndex.row() + 1;
            if (row >= d->m_model->rowCount(oFirstMatchIndex.parent()))
            {
                row = 0;
            }

            oCurrentIndex = oFirstMatchIndex.sibling(row, oFirstMatchIndex.column());
            //avoid infinite loop if all the matching items are disabled.
            if (!oStartMatchIndex.isValid())
            {
                oStartMatchIndex = oFirstMatchIndex;
            }
            else if (oStartMatchIndex == oFirstMatchIndex)
            {
                break;
            }
        }
    }
    while (oCurrentIndex != oStartIndex && oFirstMatchIndex.isValid());
}

QSize GlodonAbstractItemView::sizeHintForIndex(const QModelIndex &index) const
{
    Q_D(const GlodonAbstractItemView);

    if (!d->isIndexValid(index) || !d->m_itemDelegate)
    {
        return QSize();
    }

    return d->delegateForIndex(index)->sizeHint(d->viewOptionsV4(), index);
}

int GlodonAbstractItemView::sizeHintForRow(int row) const
{
    Q_D(const GlodonAbstractItemView);

    if (row < 0 || row >= d->m_model->rowCount(d->m_root))
    {
        return -1;
    }

    ensurePolished();

    int nHeight = 0;

    QModelIndex index;

    int nColCount = d->m_model->columnCount(d->m_root);

    for (int i = 0; i < nColCount; ++i)
    {
        index = d->m_model->index(row, i, d->m_root);

        if (QWidget *editor = d->editorForIndex(index).widget.data())
        {
            nHeight = qMax(nHeight, editor->height());
        }

        int nHint = d->delegateForIndex(index)->sizeHint(d->viewOptionsV4(), index).height();
        nHeight = qMax(nHeight, nHint);
    }

    return nHeight;
}

int GlodonAbstractItemView::sizeHintForColumn(int column) const
{
    Q_D(const GlodonAbstractItemView);

    if (column < 0 || column >= d->m_model->columnCount(d->m_root))
    {
        return -1;
    }

    ensurePolished();

    QStyleOptionViewItem option = d->viewOptionsV4();
    int nWidth = 0;
    int nRows = d->m_model->rowCount(d->m_root);
    QModelIndex index;

    for (int i = 0; i < nRows; ++i)
    {
        index = d->m_model->index(i, column, d->m_root);

        if (QWidget *editor = d->editorForIndex(index).widget.data())
        {
            nWidth = qMax(nWidth, editor->sizeHint().width());
        }

        int nHint = d->delegateForIndex(index)->sizeHint(option, index).width();
        nWidth = qMax(nWidth, nHint);
    }

    return nWidth;
}

void GlodonAbstractItemView::openPersistentEditor(const QModelIndex &index)
{
    Q_D(GlodonAbstractItemView);

    QStyleOptionViewItem options = d->viewOptionsV4();
    options.rect = visualRect(index);
    options.state |= (index == currentIndex() ? QStyle::State_HasFocus : QStyle::State_None);

    QWidget *editor = d->editor(index, options);

    if (editor)
    {
        editor->show();
        d->m_persistent.insert(editor);
    }
}

void GlodonAbstractItemView::closePersistentEditor(const QModelIndex &index)
{
    Q_D(GlodonAbstractItemView);

    if (QWidget *editor = d->editorForIndex(index).widget.data())
    {
        if (index == selectionModel()->currentIndex())
        {
            bool bCanCloseEditor = true;
            closeEditor(editor, bCanCloseEditor, GlodonDefaultItemDelegate::RevertModelCache);
        }

        d->m_persistent.remove(editor);
        d->removeEditor(editor);
        d->releaseEditor(editor);
    }
}

void GlodonAbstractItemView::setIndexWidget(const QModelIndex &index, QWidget *widget)
{
    Q_D(GlodonAbstractItemView);

    if (!d->isIndexValid(index))
    {
        return;
    }

    if (QWidget *oldWidget = indexWidget(index))
    {
        d->m_persistent.remove(oldWidget);
        d->removeEditor(oldWidget);
        oldWidget->deleteLater();
    }

    if (widget)
    {
        widget->setParent(viewport());
        d->m_persistent.insert(widget);
        d->addEditor(index, widget, true);
        widget->show();
        dataChanged(index, index); // update the geometry

        if (!d->m_delayedPendingLayout)
        {
            widget->setGeometry(visualRect(index));
        }
    }
}

QWidget *GlodonAbstractItemView::indexWidget(const QModelIndex &index) const
{
    Q_D(const GlodonAbstractItemView);

    if (d->isIndexValid(index))
    {
        if (QWidget *editor = d->editorForIndex(index).widget.data())
        {
            return editor;
        }
    }

    return 0;
}

void GlodonAbstractItemView::scrollToTop()
{
    verticalScrollBar()->setValue(verticalScrollBar()->minimum());
}

void GlodonAbstractItemView::scrollToBottom()
{
    Q_D(GlodonAbstractItemView);

    if (d->m_delayedPendingLayout)
    {
        d->executePostedLayout();
        updateGeometries();
    }

    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void GlodonAbstractItemView::update(const QModelIndex &index)
{
    Q_D(GlodonAbstractItemView);

    if (index.isValid())
    {
        const QRect c_rect = visualRect(index);

        //this test is important for peformance reason
        //For example in dataChanged we simply update all the cells without checking
        //it can be a major bottleneck to update rects that aren't even part of the viewport
        if (d->viewport->rect().intersects(c_rect))
        {
            d->viewport->update(c_rect);
        }
    }
}

bool GlodonAbstractItemView::commitDataAndCloseEditor()
{
    bool bCanCloseEditor = true;
    doCommitDataAndCloseEditor(bCanCloseEditor);

    if (!bCanCloseEditor)
    {
        resetEditorFocus();
    }

    return bCanCloseEditor;
}

void GlodonAbstractItemView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
        const QVector<int> &roles)
{
    // Single item changed
    G_UNUSED(roles);
    Q_D(GlodonAbstractItemView);

    if (topLeft == bottomRight && topLeft.isValid())
    {
        d->updateNoStaticEditorData(topLeft);

        if (isVisible() && !d->m_delayedPendingLayout)
        {
            // otherwise the items will be update later anyway
            update(topLeft);
        }

        return;
    }

    d->updateEditorData(topLeft, bottomRight);

    if (!isVisible() || d->m_delayedPendingLayout)
    {
        return;    // no need to update
    }

    d->viewport->update();
}

void GlodonAbstractItemView::rowsInserted(const QModelIndex &, int, int)
{
    if (!isVisible())
    {
        d_func()->m_fetchMoreTimer.start(0, this);    //fetch more later
    }
    else
    {
        updateEditorGeometries();
    }
}

void GlodonAbstractItemView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    Q_D(GlodonAbstractItemView);

    setState(CollapsingState);

    // Ensure one selected item in single selection mode.
    QModelIndex current = currentIndex();

    if (d->m_selectionMode == SingleSelection
            && current.isValid()
            && current.row() >= start
            && current.row() <= end
            && current.parent() == parent)
    {
        int nTotalToRemove = end - start + 1;

        if (d->m_model->rowCount(parent) <= nTotalToRemove)
        {
            // no more children
            QModelIndex index = parent;

            while (index != d->m_root && !d->isIndexEnabled(index))
            {
                index = index.parent();
            }

            if (index != d->m_root)
            {
                setCurrentIndex(index);
            }
        }
        else
        {
            int row = end + 1;
            QModelIndex next;

            do
            {
                // find the next visible and enabled item
                next = d->m_model->index(row++, current.column(), current.parent());
            }
            while (next.isValid() && (isIndexHidden(next) || !d->isIndexEnabled(next)));

            if (row > d->m_model->rowCount(parent))
            {
                row = start - 1;

                do
                {
                    // find the previous visible and enabled item
                    next = d->m_model->index(row--, current.column(), current.parent());
                }
                while (next.isValid() && (isIndexHidden(next) || !d->isIndexEnabled(next)));
            }

            setCurrentIndex(next);
        }
    }

    // Remove all affected editors; this is more efficient than waiting
    // for updateGeometries() to clean out editors for invalid indexes
    GEditorIndexHash::iterator iter = d->m_editorIndexHash.begin();

    while (iter != d->m_editorIndexHash.end())
    {
        const QModelIndex c_index = iter.value();

        if (c_index.row() >= start && c_index.row() <= end && d->m_model->parent(c_index) == parent)
        {
            QWidget *editor = iter.key();
            GEditorInfo info = d->m_indexEditorHash.take(c_index);
            iter = d->m_editorIndexHash.erase(iter);

            if (info.widget)
            {
                d->releaseEditor(editor);
            }
        }
        else
        {
            ++iter;
        }
    }
}

void GlodonAbstractItemViewPrivate::_q_rowsRemoved(const QModelIndex &index, int start, int end)
{
    Q_UNUSED(index)
    Q_UNUSED(start)
    Q_UNUSED(end)

    Q_Q(GlodonAbstractItemView);

    if (q->isVisible())
    {
        q->updateEditorGeometries();
    }

    q->setState(GlodonAbstractItemView::NoState);
#ifndef QT_NO_ACCESSIBILITY
#ifdef Q_WS_X11

    if (QAccessible::isActive())
    {
        QAccessible::queryAccessibleInterface(q)->table2Interface()->rowsRemoved(index, start, end);
        QAccessible::updateAccessibility(q, 0, QAccessible::TableModelChanged);
    }

#endif
#endif
}

void GlodonAbstractItemViewPrivate::_q_columnsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    Q_Q(GlodonAbstractItemView);

    q->setState(GlodonAbstractItemView::CollapsingState);

    // Ensure one selected item in single selection mode.
    QModelIndex current = q->currentIndex();

    if (current.isValid()
            && m_selectionMode == GlodonAbstractItemView::SingleSelection
            && current.column() >= start
            && current.column() <= end)
    {
        int nTotalToRemove = end - start + 1;

        if (m_model->columnCount(parent) < nTotalToRemove)
        {
            // no more columns
            QModelIndex index = parent;

            while (index.isValid() && !isIndexEnabled(index))
            {
                index = index.parent();
            }

            if (index.isValid())
            {
                q->setCurrentIndex(index);
            }
        }
        else
        {
            int nColumn = end;
            QModelIndex next;

            do
            {
                // find the next visible and enabled item
                next = m_model->index(current.row(), nColumn++, current.parent());
            }
            while (next.isValid() && (q->isIndexHidden(next) || !isIndexEnabled(next)));

            q->setCurrentIndex(next);
        }
    }

    // Remove all affected editors; this is more efficient than waiting
    // for updateGeometries() to clean out editors for invalid indexes
    GEditorIndexHash::iterator it = m_editorIndexHash.begin();

    while (it != m_editorIndexHash.end())
    {
        QModelIndex index = it.value();

        if (index.column() <= start && index.column() >= end && m_model->parent(index) == parent)
        {
            QWidget *editor = it.key();
            GEditorInfo info = m_indexEditorHash.take(it.value());
            it = m_editorIndexHash.erase(it);

            if (info.widget)
            {
                releaseEditor(editor);
            }
        }
        else
        {
            ++it;
        }
    }

}

void GlodonAbstractItemViewPrivate::_q_columnsRemoved(const QModelIndex &index, int start, int end)
{
    Q_UNUSED(index)
    Q_UNUSED(start)
    Q_UNUSED(end)

    Q_Q(GlodonAbstractItemView);

    if (q->isVisible())
    {
        q->updateEditorGeometries();
    }

    q->setState(GlodonAbstractItemView::NoState);
#ifndef QT_NO_ACCESSIBILITY
#ifdef Q_WS_X11

    if (QAccessible::isActive())
    {
        QAccessible::queryAccessibleInterface(q)->table2Interface()->columnsRemoved(index, start, end);
        QAccessible::updateAccessibility(q, 0, QAccessible::TableModelChanged);
    }

#endif
#endif
}

void GlodonAbstractItemViewPrivate::_q_rowsInserted(const QModelIndex &index, int start, int end)
{
    Q_UNUSED(index)
    Q_UNUSED(start)
    Q_UNUSED(end)

#ifndef QT_NO_ACCESSIBILITY
#ifdef Q_WS_X11
    Q_Q(GlodonAbstractItemView);

    if (QAccessible::isActive())
    {
        QAccessible::queryAccessibleInterface(q)->table2Interface()->rowsInserted(index, start, end);
        QAccessible::updateAccessibility(q, 0, QAccessible::TableModelChanged);
    }

#endif
#endif
}

void GlodonAbstractItemViewPrivate::_q_columnsInserted(const QModelIndex &index, int start, int end)
{
    Q_UNUSED(index)
    Q_UNUSED(start)
    Q_UNUSED(end)

    Q_Q(GlodonAbstractItemView);

    if (q->isVisible())
    {
        q->updateEditorGeometries();
    }

#ifndef QT_NO_ACCESSIBILITY
#ifdef Q_WS_X11

    if (QAccessible::isActive())
    {
        QAccessible::queryAccessibleInterface(q)->table2Interface()->columnsInserted(index, start, end);
        QAccessible::updateAccessibility(q, 0, QAccessible::TableModelChanged);
    }

#endif
#endif
}

void GlodonAbstractItemViewPrivate::_q_modelDestroyed()
{
    m_model = QAbstractItemModelPrivate::staticEmptyModel();
    doDelayedReset();
}

void GlodonAbstractItemViewPrivate::_q_layoutChanged()
{
    doDelayedItemsLayout();
#ifndef QT_NO_ACCESSIBILITY
#ifdef Q_WS_X11
    Q_Q(GlodonAbstractItemView);

    if (QAccessible::isActive())
    {
        QAccessible::queryAccessibleInterface(q)->table2Interface()->modelReset();
        QAccessible::updateAccessibility(q, 0, QAccessible::TableModelChanged);
    }

#endif
#endif
}

void GlodonAbstractItemView::selectionChanged(const QItemSelection &selected,
        const QItemSelection &deselected)
{
    Q_D(GlodonAbstractItemView);

    if (isVisible() && updatesEnabled())
    {
        d->viewport->update(visualRegionForSelection(deselected) | visualRegionForSelection(selected));
    }
}

void GlodonAbstractItemView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_D(GlodonAbstractItemView);
    Q_ASSERT(d->m_model);

    if (previous.isValid())
    {
        QModelIndex oBuddyIndex = d->m_model->buddy(previous);
        QWidget *pEditor = d->editorForIndex(oBuddyIndex).widget.data();

        if (pEditor && !d->m_persistent.contains(pEditor))
        {
            bool bCanCloseEditor = true;
            commitData(pEditor, bCanCloseEditor);

            if (bCanCloseEditor)
            {
                if (current.row() == previous.row())
                {
                    closeEditor(pEditor, bCanCloseEditor, GlodonDefaultItemDelegate::NoHint);
                }
                else
                {
                    closeEditor(pEditor, bCanCloseEditor, GlodonDefaultItemDelegate::SubmitModelCache);
                }
            }
        }

        if (isVisible())
        {
            update(previous);
        }
    }

    if (current.isValid() && !d->m_autoScrollTimer.isActive())
    {
        if (isVisible())
        {
            if (d->m_bAutoScroll)
            {
                scrollTo(current);
            }

            update(current);
            edit(current, CurrentChanged, 0);

            if (current.row() == (d->m_model->rowCount(d->m_root) - 1))
            {
                d->fetchMore();
            }
        }
        else
        {
            d->m_shouldScrollToCurrentOnShow = d->m_bAutoScroll;
        }
    }
}

#ifndef QT_NO_DRAGANDDROP

void GlodonAbstractItemView::startDrag(Qt::DropActions supportedActions)
{
    Q_D(GlodonAbstractItemView);
    QModelIndexList indexes = d->selectedDraggableIndexes();

    if (indexes.count() <= 0)
    {
        return;
    }

    QMimeData *data = d->m_model->mimeData(indexes);

    if (!data)
    {
        return;
    }

    QRect rect;
    QPixmap pixmap = d->renderToPixmap(indexes, &rect);
    rect.adjust(horizontalOffset(), verticalOffset(), 0, 0);
    QDrag *drag = new QDrag(this);
    drag->setPixmap(pixmap);
    drag->setMimeData(data);
    drag->setHotSpot(d->m_pressedPosition - rect.topLeft());
    Qt::DropAction defaultDropAction = Qt::IgnoreAction;

    if (d->m_defaultDropAction != Qt::IgnoreAction
            && (supportedActions & d->m_defaultDropAction))
    {
        defaultDropAction = d->m_defaultDropAction;
    }
    else if (supportedActions & Qt::CopyAction
             && dragDropMode() != GlodonAbstractItemView::InternalMove)
    {
        defaultDropAction = Qt::CopyAction;
    }

    if (drag->exec(supportedActions, defaultDropAction) == Qt::MoveAction)
    {
        d->clearOrRemove();
    }
}

#endif // QT_NO_DRAGANDDROP

QStyleOptionViewItem GlodonAbstractItemView::viewOptions() const
{
    Q_D(const GlodonAbstractItemView);

    QStyleOptionViewItem option;
    option.init(this);
    option.state &= ~QStyle::State_MouseOver;
    option.font = font();

#ifndef Q_WS_MAC

    // On mac the focus appearance follows window activation
    // not widget activation
    if (!hasFocus())
    {
        option.state &= ~QStyle::State_Active;
    }

#endif

    option.state &= ~QStyle::State_HasFocus;

    if (d->m_oIconSize.isValid())
    {
        option.decorationSize = d->m_oIconSize;
    }
    else
    {
        int nPm = style()->pixelMetric(QStyle::PM_SmallIconSize, 0, this);
        option.decorationSize = QSize(nPm, nPm);
    }

    option.decorationPosition = QStyleOptionViewItem::Left;
    option.decorationAlignment = Qt::AlignCenter;
    option.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    option.textElideMode = d->m_textElideMode;
    option.rect = QRect();
    option.showDecorationSelected = (0 != style()->styleHint(QStyle::SH_ItemView_ShowDecorationSelected, 0, this));
    return option;
}

QStyleOptionViewItem GlodonAbstractItemViewPrivate::viewOptionsV4() const
{
    Q_Q(const GlodonAbstractItemView);

    QStyleOptionViewItem option = q->viewOptions();

    if (m_bWrapItemText)
    {
        option.features = QStyleOptionViewItem::WrapText;
    }

    option.locale = q->locale();
    option.locale.setNumberOptions(QLocale::OmitGroupSeparator);
    option.widget = q;

    return option;
}

void GlodonAbstractItemViewPrivate::doDelayedReset()
{
    //we delay the reset of the timer because some views (QTableView)
    //with headers can't handle the fact that the model has been destroyed
    //all _q_modelDestroyed slots must have been called
    if (!m_delayedReset.isActive())
    {
        m_delayedReset.start(0, q_func());
    }
}

bool GlodonAbstractItemViewPrivate::mouseMoveEventFlag(
        Qt::KeyboardModifiers &modifiers, const QEvent *event, QItemSelectionModel::SelectionFlags &selectionFlags) const
{
    modifiers = static_cast<const QMouseEvent *>(event)->modifiers();

    if (Qt::ControlModifier == (modifiers & Qt::ControlModifier))
    {
        selectionFlags = QItemSelectionModel::ToggleCurrent | selectionBehaviorFlags();
        return true;
    }
    return false;
}

bool GlodonAbstractItemViewPrivate::mouseButtonPressEventFlag(
        Qt::KeyboardModifiers &modifiers, const QModelIndex &index, const QEvent *event, QItemSelectionModel::SelectionFlags &selectionFlags) const
{
    modifiers = static_cast<const QMouseEvent *>(event)->modifiers();
    const Qt::MouseButton c_button = static_cast<const QMouseEvent *>(event)->button();
    const bool c_rightButtonPressed = (Qt::RightButton == (c_button & Qt::RightButton));
    const bool c_shiftKeyPressed = (Qt::ShiftModifier == (modifiers & Qt::ShiftModifier));
    const bool c_controlKeyPressed = (Qt::ControlModifier == (modifiers & Qt::ControlModifier));
    const bool c_indexIsSelected = m_selectionModel->isSelected(index);

    if ((c_shiftKeyPressed || c_controlKeyPressed) && c_rightButtonPressed)
    {
        selectionFlags = QItemSelectionModel::NoUpdate;
        return true;
    }

    if (!c_shiftKeyPressed && !c_controlKeyPressed && c_indexIsSelected)
    {
        selectionFlags = QItemSelectionModel::NoUpdate;
        return true;
    }

    if (!index.isValid() && !c_rightButtonPressed && !c_shiftKeyPressed && !c_controlKeyPressed)
    {
        selectionFlags = QItemSelectionModel::Clear;
        return true;
    }

    if (!index.isValid())
    {
        selectionFlags = QItemSelectionModel::NoUpdate;
        return true;
    }

    return false;
}

QItemSelectionModel::SelectionFlags GlodonAbstractItemViewPrivate::mouseButtonReleaseEventFlag(
        Qt::KeyboardModifiers &modifiers, const QModelIndex &index, const QEvent *event) const
{
    if (isNewSelect(modifiers, index, event))
    {
        return QItemSelectionModel::ClearAndSelect | selectionBehaviorFlags();
    }

    return QItemSelectionModel::NoUpdate;
}

bool GlodonAbstractItemViewPrivate::keyPressEventFlag(
        Qt::KeyboardModifiers &modifiers, const QEvent *event, QItemSelectionModel::SelectionFlags &selectionFlags) const
{
    modifiers = static_cast<const QKeyEvent *>(event)->modifiers();

    switch (static_cast<const QKeyEvent *>(event)->key())
    {
    case Qt::Key_Backtab:
        modifiers = modifiers & ~Qt::ShiftModifier; // special case for backtab

    case Qt::Key_Down:
    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Home:
    case Qt::Key_End:
    case Qt::Key_PageUp:
    case Qt::Key_PageDown:
    case Qt::Key_Tab:
        if ((modifiers & Qt::ControlModifier)
#ifdef QT_KEYPAD_NAVIGATION
                // Preserve historical tab order navigation behavior
                || QApplication::navigationMode() == Qt::NavigationModeKeypadTabOrder
#endif
                )
        {
            selectionFlags = QItemSelectionModel::NoUpdate;
            return true;
        }

        break;

    case Qt::Key_Select:
    {
        selectionFlags = QItemSelectionModel::Toggle | selectionBehaviorFlags();
        return true;
    }

    case Qt::Key_Space:// Toggle on Ctrl-Qt::Key_Space, Select on Space
    {
        if (modifiers & Qt::ControlModifier)
        {
            selectionFlags = QItemSelectionModel::Toggle | selectionBehaviorFlags();
            return true;
        }
        selectionFlags = QItemSelectionModel::Select | selectionBehaviorFlags();
        return true;
    }

    default:
        break;
    }

    return false;
}

bool GlodonAbstractItemViewPrivate::isNewSelect(Qt::KeyboardModifiers &modifiers, const QModelIndex &index, const QEvent *event) const
{
    modifiers = static_cast<const QMouseEvent *>(event)->modifiers();
    const Qt::MouseButton c_button = static_cast<const QMouseEvent *>(event)->button();
    const bool c_rightButtonPressed = (Qt::RightButton == (c_button & Qt::RightButton));
    const bool c_shiftKeyPressed = (Qt::ShiftModifier == (modifiers & Qt::ShiftModifier));
    const bool c_controlKeyPressed = (Qt::ControlModifier == (modifiers & Qt::ControlModifier));

    return !c_shiftKeyPressed
            && !c_controlKeyPressed
            && (!c_rightButtonPressed || !index.isValid())
            &&(!index.isValid() || (index == m_pressedIndex && m_selectionModel->isSelected(index)))
            && m_state != GlodonAbstractItemView::DragSelectingState ;
}

void GlodonAbstractItemViewPrivate::updateNoStaticEditorData(const QModelIndex &editorIndex)
{
    const GEditorInfo &editorInfo = editorForIndex(editorIndex);

    //we don't update the edit data if it is static
    if (!editorInfo.isStatic && editorInfo.widget)
    {
        GlodonDefaultItemDelegate *delegate = delegateForIndex(editorIndex);

        if (delegate)
        {
            delegate->setEditorData(editorInfo.widget.data(), editorIndex);
        }
    }
}

GlodonAbstractItemView::State GlodonAbstractItemView::state() const
{
    Q_D(const GlodonAbstractItemView);
    return d->m_state;
}

void GlodonAbstractItemView::setState(State state)
{
    Q_D(GlodonAbstractItemView);
    d->m_state = state;
}

void GlodonAbstractItemView::showToolTip(const QModelIndex &index)
{
    Q_D(GlodonAbstractItemView);

    if (d->m_pToolTipFrame)
    {
        d->m_pToolTipFrame->hide();
    }

    if (!index.isValid())
    {
        return;
    }

    if (d->m_pToolTipFrame)
    {
        d->m_delayedShowToolTip.stop();
        d->m_delayedShowToolTip.start(d->m_nDelayedHintTime, this);
    }
}

void GlodonAbstractItemView::editorCopy()
{
    Q_D(GlodonAbstractItemView);

    emit onEditorCopy();
    resetActAvailable(d->delegateForIndex(currentIndex())->curEditor());
}

void GlodonAbstractItemView::editorCut()
{
    emit onEditorCut();
}

void GlodonAbstractItemView::editorPaste()
{
    emit onEditorPaste();
}

void GlodonAbstractItemView::editorDelete()
{
    emit onEditorDelete();
}

QStyleOptionViewItem GlodonAbstractItemView::StyleOptionViewItem()
{
    Q_D(GlodonAbstractItemView);
    return d->viewOptionsV4();
}

void GlodonAbstractItemView::scheduleDelayedItemsLayout()
{
    Q_D(GlodonAbstractItemView);
    d->doDelayedItemsLayout();
}

void GlodonAbstractItemView::executeDelayedItemsLayout()
{
    Q_D(GlodonAbstractItemView);
    d->executePostedLayout();
}

void GlodonAbstractItemView::setDirtyRegion(const QRegion &region)
{
    Q_D(GlodonAbstractItemView);
    d->setDirtyRegion(region);
}

void GlodonAbstractItemView::scrollDirtyRegion(int dx, int dy)
{
    Q_D(GlodonAbstractItemView);
    d->scrollDirtyRegion(dx, dy);
}

void GlodonAbstractItemView::startAutoScroll()
{
    d_func()->startAutoScroll();
}

void GlodonAbstractItemView::stopAutoScroll()
{
    d_func()->stopAutoScroll();
}

void GlodonAbstractItemView::doAutoScroll()
{
    // find how much we should scroll with
    Q_D(GlodonAbstractItemView);

    QScrollBar *pVerticalScroll = verticalScrollBar();
    QScrollBar *pHorizontalScroll = horizontalScrollBar();

    GlodonHeaderView *pHeaderView = qobject_cast<GlodonHeaderView *>(this);
    if (pHeaderView)
    {
        QAbstractScrollArea *pParent = qobject_cast<QAbstractScrollArea *>(parentWidget());
        if (pHeaderView->orientation() == Qt::Horizontal)
        {
            if (!pHeaderView->horizontalScrollBar()
                    || !pHeaderView->horizontalScrollBar()->isVisible())
            {
                pHorizontalScroll = pParent->horizontalScrollBar();
            }
        }
        else
        {
            if (!pHeaderView->verticalScrollBar()
                    || !pHeaderView->verticalScrollBar()->isVisible())
            {
                pHorizontalScroll = pParent->horizontalScrollBar();
            }
        }
    }

    int nVerticalStep = pVerticalScroll->pageStep();
    int nHorizontalStep = pHorizontalScroll->pageStep();
    if (d->m_autoScrollCount < qMax(nVerticalStep, nHorizontalStep))
    {
        ++d->m_autoScrollCount;
    }

    int nVerticalValue = pVerticalScroll->value();
    int nHorizontalValue = pHorizontalScroll->value();

    // do the scrolling if we are in the scroll margins
    scrollVerticalScrollBar(nVerticalValue, pVerticalScroll);
    scrollHorizontalScrollBar(nHorizontalValue, pHorizontalScroll);

    // if nothing changed, stop scrolling
    bool bVerticalUnchanged = (nVerticalValue == pVerticalScroll->value());
    bool bHorizontalUnchanged = (nHorizontalValue == pHorizontalScroll->value());

    if (bVerticalUnchanged && bHorizontalUnchanged)
    {
        stopAutoScroll();
    }
    else
    {
#ifndef QT_NO_DRAGANDDROP
        d->m_dropIndicatorRect = QRect();
        d->m_dropIndicatorPosition = GlodonAbstractItemView::OnViewport;
#endif
        d->viewport->update();
    }
}

QItemSelectionModel::SelectionFlags GlodonAbstractItemView::selectionCommand(const QModelIndex &index,
        const QEvent *event) const
{
    Q_D(const GlodonAbstractItemView);

    switch (d->m_selectionMode)
    {
        case NoSelection: // Never update selection model
            return QItemSelectionModel::NoUpdate;

        case SingleSelection: // ClearAndSelect on valid index otherwise NoUpdate
            if (event && event->type() == QEvent::MouseButtonRelease)
            {
                return QItemSelectionModel::NoUpdate;
            }

            return QItemSelectionModel::ClearAndSelect | d->selectionBehaviorFlags();

        case MultiSelection:
            return d->multiSelectionCommand(index, event);

        case ExtendedSelection:
            return d->extendedSelectionCommand(index, event);

        case ContiguousSelection:
            return d->contiguousSelectionCommand(index, event);
    }

    return QItemSelectionModel::NoUpdate;
}

QItemSelectionModel::SelectionFlags GlodonAbstractItemViewPrivate::multiSelectionCommand(
    const QModelIndex &index, const QEvent *event) const
{
    Q_UNUSED(index)

    if (event)
    {
        switch (event->type())
        {
            case QEvent::KeyPress:
                if (static_cast<const QKeyEvent *>(event)->key() == Qt::Key_Space
                        || static_cast<const QKeyEvent *>(event)->key() == Qt::Key_Select)
                {
                    return QItemSelectionModel::Toggle | selectionBehaviorFlags();
                }

                break;

            case QEvent::MouseButtonPress:
                if (static_cast<const QMouseEvent *>(event)->button() == Qt::LeftButton)
                {
                    return QItemSelectionModel::Toggle | selectionBehaviorFlags();    // toggle
                }

                break;

            case QEvent::MouseButtonRelease:
                if (static_cast<const QMouseEvent *>(event)->button() == Qt::LeftButton)
                {
                    return QItemSelectionModel::NoUpdate | selectionBehaviorFlags();    // finalize
                }

                break;

            case QEvent::MouseMove:
                if (Qt::LeftButton == (static_cast<const QMouseEvent *>(event)->buttons() & Qt::LeftButton))
                {
                    return QItemSelectionModel::ToggleCurrent | selectionBehaviorFlags();    // toggle drag select
                }

            default:
                break;
        }

        return QItemSelectionModel::NoUpdate;
    }

    return QItemSelectionModel::Toggle | selectionBehaviorFlags();
}

QItemSelectionModel::SelectionFlags GlodonAbstractItemViewPrivate::extendedSelectionCommand(
    const QModelIndex &index, const QEvent *event) const
{
    Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
    QItemSelectionModel::SelectionFlags selectionFlags;

    if (event)
    {
        switch (event->type())
        {
        case QEvent::MouseMove:
        {
            // Toggle on MouseMove
            if (mouseMoveEventFlag(modifiers, event, selectionFlags))
            {
                return selectionFlags;
            }
            break;
        }
        case QEvent::MouseButtonPress:
        {
            if (mouseButtonPressEventFlag(modifiers, index, event, selectionFlags))
            {
                return selectionFlags;
            }
            break;
        }
        case QEvent::MouseButtonRelease:
        {
            // ClearAndSelect on MouseButtonRelease if MouseButtonPress on selected item or empty area
            return mouseButtonReleaseEventFlag(modifiers, index, event);
        }
        case QEvent::KeyPress:
        {
            // NoUpdate on Key movement and Ctrl
            if (keyPressEventFlag(modifiers, event, selectionFlags))
            {
                return selectionFlags;
            }
            break;
        }
        default:
            break;
        }
    }

    if (Qt::ShiftModifier == (modifiers & Qt::ShiftModifier))
    {
        return QItemSelectionModel::SelectCurrent | selectionBehaviorFlags();
    }

    if (Qt::ControlModifier == (modifiers & Qt::ControlModifier))
    {
        if (event)
        {
            return QItemSelectionModel::Toggle | selectionBehaviorFlags();
        }
        return QItemSelectionModel::ClearAndSelect | selectionBehaviorFlags();
    }

    if (m_state == GlodonAbstractItemView::DragSelectingState)
    {
        //when drag-selecting we need to clear any previous selection and select the current one
        return QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent | selectionBehaviorFlags();
    }

    return QItemSelectionModel::ClearAndSelect | selectionBehaviorFlags();
}

QItemSelectionModel::SelectionFlags
GlodonAbstractItemViewPrivate::contiguousSelectionCommand(const QModelIndex &index,
        const QEvent *event) const
{
    QItemSelectionModel::SelectionFlags flags = extendedSelectionCommand(index, event);
    const int c_Mask = QItemSelectionModel::Clear | QItemSelectionModel::Select
                       | QItemSelectionModel::Deselect | QItemSelectionModel::Toggle
                       | QItemSelectionModel::Current;

    switch (flags & c_Mask)
    {
        case QItemSelectionModel::Clear:
        case QItemSelectionModel::ClearAndSelect:
        case QItemSelectionModel::SelectCurrent:
            return flags;

        case QItemSelectionModel::NoUpdate:
            if (event
                    && (event->type() == QEvent::MouseButtonPress
                        || event->type() == QEvent::MouseButtonRelease))
            {
                return flags;
            }

            return QItemSelectionModel::ClearAndSelect | selectionBehaviorFlags();

        default:
            return QItemSelectionModel::SelectCurrent | selectionBehaviorFlags();
    }
}

void GlodonAbstractItemViewPrivate::fetchMore()
{
    m_fetchMoreTimer.stop();

    if (!m_model->canFetchMore(m_root))
    {
        return;
    }

    int nlast = m_model->rowCount(m_root) - 1;

    if (nlast < 0)
    {
        m_model->fetchMore(m_root);
        return;
    }

    QModelIndex index = m_model->index(nlast, 0, m_root);
    QRect rect = q_func()->visualRect(index);

    if (viewport->rect().intersects(rect))
    {
        m_model->fetchMore(m_root);
    }
}

bool GlodonAbstractItemViewPrivate::shouldEdit(
        GlodonAbstractItemView::EditTrigger trigger, const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return false;
    }

    Qt::ItemFlags flags = m_model->flags(index);

    if (((flags & Qt::ItemIsEditable) == 0) || ((flags & Qt::ItemIsEnabled) == 0))
    {
        return false;
    }

    if (m_state == GlodonAbstractItemView::EditingState && !m_alwaysShowEditorPro)
    {
        return false;
    }

    if (hasEditor(index))
    {
        return false;
    }

    if (trigger == GlodonAbstractItemView::AllEditTriggers) // force editing
    {
        return true;
    }

    if ((trigger & m_editTriggers) == GlodonAbstractItemView::SelectedClicked
            && !m_selectionModel->isSelected(index))
    {
        return false;
    }

    return trigger & m_editTriggers;
}

bool GlodonAbstractItemViewPrivate::shouldForwardEvent(
        GlodonAbstractItemView::EditTrigger trigger, const QEvent *event) const
{
    if (!event || (trigger & m_editTriggers) != GlodonAbstractItemView::AnyKeyPressed)
    {
        return false;
    }

    switch (event->type())
    {
        case QEvent::KeyPress:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
            return true;

        default:
            break;
    };

    return false;
}

bool GlodonAbstractItemViewPrivate::shouldAutoScroll(const QPoint &pos) const
{
    if (!m_bAutoScroll)
    {
        return false;
    }

    QRect area = static_cast<GlodonAbstractItemView *>(viewport)->d_func()->clipRect();
    // access QWidget private by bending C++ rules
    return (pos.y() - area.top() < m_nAutoScrollMargin)
           || (area.bottom() - pos.y() < m_nAutoScrollMargin)
           || (pos.x() - area.left() < m_nAutoScrollMargin)
           || (area.right() - pos.x() < m_nAutoScrollMargin);
}

void GlodonAbstractItemViewPrivate::doDelayedItemsLayout(int delay)
{
    if (!m_delayedPendingLayout)
    {
        m_delayedPendingLayout = true;
        m_delayedLayout.start(delay, q_func());
    }
}

void GlodonAbstractItemViewPrivate::doWheel()
{
}

void GlodonAbstractItemViewPrivate::interruptDelayedItemsLayout() const
{
    m_delayedLayout.stop();
    m_delayedPendingLayout = false;
}

QWidget *GlodonAbstractItemViewPrivate::editor(const QModelIndex &index,
        const QStyleOptionViewItem &options)
{
    Q_Q(GlodonAbstractItemView);
    QWidget *widgt = editorForIndex(index).widget.data();

    if (!widgt)
    {
        GlodonDefaultItemDelegate *delegate = delegateForIndex(index);

        if (!delegate)
        {
            return 0;
        }

        widgt = delegate->createEditor(viewport, options, index);

        if (widgt)
        {
            delegate->setCurEditor(widgt);

            m_state = GlodonAbstractItemView::EditingState;
            widgt->installEventFilter(delegate);
            q->installEventFilterWidthEdit(widgt, delegate);
            QObject::connect(widgt, SIGNAL(destroyed(QObject *)), q, SLOT(editorDestroyed(QObject *)));
            //options.showDecorationSelected = true;
            delegate->updateEditorGeometry(widgt, options, index);
            delegate->setEditorData(widgt, index);

            if (!m_alwaysShowEditorPro)
            {
                addEditor(index, widgt, false);
            }

            if (widgt->parent() == viewport)
            {
                QWidget::setTabOrder(q, widgt);
            }

            // Special cases for some editors containing QLineEdit
            QWidget *focusWidget = widgt;

            while (QWidget *fp = focusWidget->focusProxy())
            {
                focusWidget = fp;
            }

            m_currentEditor = widgt;
        }
    }

    return widgt;
}

void GlodonAbstractItemViewPrivate::updateEditorData(const QModelIndex &tl, const QModelIndex &br)
{
    // we are counting on having relatively few editors
    const bool c_checkIndexes = tl.isValid() && br.isValid();
    const QModelIndex c_parent = tl.parent();
    GIndexEditorHash::const_iterator it = m_indexEditorHash.constBegin();

    for (; it != m_indexEditorHash.constEnd(); ++it)
    {
        QWidget *editor = it.value().widget.data();
        const QModelIndex c_index = it.key();

        if (it.value().isStatic || !editor || !c_index.isValid()
                || (c_checkIndexes
                    && (c_index.row() < tl.row() || c_index.row() > br.row()
                        || c_index.column() < tl.column() || c_index.column() > br.column()
                        || c_index.parent() != c_parent)))
        {
            continue;
        }

        GlodonDefaultItemDelegate *delegate = delegateForIndex(c_index);

        if (delegate)
        {
            delegate->setEditorData(editor, c_index);
        }
    }
}

void GlodonAbstractItemViewPrivate::clearOrRemove()
{
#ifndef QT_NO_DRAGANDDROP
    const QItemSelection c_selection = m_selectionModel->selection();
    QList<QItemSelectionRange>::const_iterator it = c_selection.constBegin();

    if (!m_bDragDropOverwrite)
    {
        for (; it != c_selection.constEnd(); ++it)
        {
            QModelIndex parent = (*it).parent();

            if ((*it).left() != 0)
            {
                continue;
            }

            if ((*it).right() != (m_model->columnCount(parent) - 1))
            {
                continue;
            }

            int ncount = (*it).bottom() - (*it).top() + 1;
            m_model->removeRows((*it).top(), ncount, parent);
        }
    }
    else
    {
        // we can't remove the rows so reset the items (i.e. the view is like a table)
        QModelIndexList list = c_selection.indexes();

        for (int i = 0; i < list.size(); ++i)
        {
            QModelIndex index = list.at(i);
            QMap<int, QVariant> roles = m_model->itemData(index);

            for (QMap<int, QVariant>::Iterator it = roles.begin(); it != roles.end(); ++it)
            {
                it.value() = QVariant();
            }

            m_model->setItemData(index, roles);
        }
    }

#endif
}

void GlodonAbstractItemViewPrivate::checkPersistentEditorFocus()
{
    Q_Q(GlodonAbstractItemView);

    if (QWidget *widget = QApplication::focusWidget())
    {
        if (m_persistent.contains(widget))
        {
            //a persistent editor has gained the focus
            QModelIndex index = indexForEditor(widget);

            if (m_selectionModel->currentIndex() != index)
            {
                q->setCurrentIndex(index);
            }
        }
    }
}

const GEditorInfo &GlodonAbstractItemViewPrivate::editorForIndex(const QModelIndex &index) const
{
    static GEditorInfo s_nullInfo;

    GIndexEditorHash::const_iterator it = m_indexEditorHash.find(index);

    if (it == m_indexEditorHash.end())
    {
        return s_nullInfo;
    }

    return it.value();
}

QModelIndex GlodonAbstractItemViewPrivate::indexForEditor(QWidget *editor) const
{
    GEditorIndexHash::const_iterator it = m_editorIndexHash.find(editor);

    if (it == m_editorIndexHash.end())
    {
        return QModelIndex();
    }

    return it.value();
}

void GlodonAbstractItemViewPrivate::removeEditor(QWidget *editor)
{
    GEditorIndexHash::iterator it = m_editorIndexHash.find(editor);

    if (it != m_editorIndexHash.end())
    {
        m_indexEditorHash.remove(it.value());
        m_editorIndexHash.erase(it);
    }
}

int GlodonAbstractItemViewPrivate::delegateRefCount(const GlodonDefaultItemDelegate *delegate) const
{
    int nRef = 0;

    if (m_itemDelegate == delegate)
    {
        ++nRef;
    }

    typedef QMap<int, QPointer<GlodonDefaultItemDelegate> > DelegateMap;

    for (int nMaps = 0; nMaps < 2; ++nMaps)
    {
        const DelegateMap *delegates = (0 != nMaps) ? &m_columnDelegates : &m_rowDelegates;

        DelegateMap::const_iterator it = delegates->begin();
        DelegateMap::const_iterator end = delegates->end();

        for (; it != end; ++it)
        {
            if (it.value() == delegate)
            {
                ++nRef;

                // optimization, we are only interested in the ref count values 0, 1 or >=2
                if (nRef >= 2)
                {
                    return nRef;
                }
            }
        }
    }

    return nRef;
}

bool GlodonAbstractItemViewPrivate::isPersistent(const QModelIndex &index) const
{
    GAbstractItemModel *temp = static_cast<GAbstractItemModel *>(m_model);
    QAbstractItemModelPrivate *pPrivate = static_cast<QAbstractItemModelPrivate *>(temp->d_ptr.data());
    return pPrivate->persistent.indexes.contains(index);
}

void GlodonAbstractItemViewPrivate::addEditor(const QModelIndex &index, QWidget *editor, bool isStatic)
{
    m_editorIndexHash.insert(editor, index);
    m_indexEditorHash.insert(index, GEditorInfo(editor, isStatic));
}

bool GlodonAbstractItemViewPrivate::sendDelegateEvent(const QModelIndex &index, QEvent *event) const
{
    Q_Q(const GlodonAbstractItemView);

    QModelIndex buddy = m_model->buddy(index);

    QStyleOptionViewItem options = viewOptionsV4();
    options.rect = q->visualRect(buddy);
    options.state |= (buddy == q->currentIndex() ? QStyle::State_HasFocus : QStyle::State_None);
    GlodonDefaultItemDelegate *delegate = delegateForIndex(index);

    return (event && delegate && delegate->editorEvent(event, m_model, options, buddy));
}

bool GlodonAbstractItemViewPrivate::openEditor(const QModelIndex &index, QEvent *event)
{
    Q_Q(GlodonAbstractItemView);

    QModelIndex buddy = m_model->buddy(index);
    m_oEditorIndex = buddy;
    QStyleOptionViewItem options = viewOptionsV4();
    options.rect = q->visualRect(buddy);
    options.state |= (buddy == q->currentIndex() ? QStyle::State_HasFocus : QStyle::State_None);

    QWidget *pCurEditor = editor(buddy, options);
    if (!pCurEditor)
    {
        m_oEditorIndex = QModelIndex();
        return false;
    }

    q->setState(GlodonAbstractItemView::EditingState);
    pCurEditor->show();

    // TODO wangdd-a，后面考虑重新实现，并且移到子类
    if (!q->alwaysShowEditorPro())
    {
        pCurEditor->setFocus();
    }

    if (event)
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if (event->type() == QEvent::KeyPress) // todo qt5 && inputContext())
#else
        if (event->type() == QEvent::KeyPress && inputContext())
#endif
            QApplication::sendEvent(pCurEditor->focusProxy() ? pCurEditor->focusProxy() : pCurEditor, event);
    }

    return true;
}

GItemViewPaintPairs GlodonAbstractItemViewPrivate::draggablePaintPairs(const QModelIndexList &indexes, QRect *r) const
{
    Q_ASSERT(r);
    Q_Q(const GlodonAbstractItemView);
    QRect &rect = *r;
    const QRect c_viewportRect = viewport->rect();
    GItemViewPaintPairs ret;

    for (int i = 0; i < indexes.count(); ++i)
    {
        const QModelIndex &index = indexes.at(i);
        const QRect c_current = q->visualRect(index);

        if (c_current.intersects(c_viewportRect))
        {
            ret += qMakePair(c_current, index);
            rect |= c_current;
        }
    }

    rect &= c_viewportRect;
    return ret;
}

void GlodonAbstractItemViewPrivate::releaseEditor(QWidget *editor) const
{
    if (editor)
    {
        QObject::disconnect(editor, SIGNAL(destroyed(QObject *)),
                            q_func(), SLOT(editorDestroyed(QObject *)));
        editor->removeEventFilter(m_itemDelegate);
        editor->hide();
        editor->deleteLater();
    }
}

QPixmap GlodonAbstractItemViewPrivate::renderToPixmap(const QModelIndexList &indexes, QRect *r) const
{
    Q_ASSERT(r);
    GItemViewPaintPairs paintPairs = draggablePaintPairs(indexes, r);

    if (paintPairs.isEmpty())
    {
        return QPixmap();
    }

    QPixmap pixmap(r->size());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);

    QStyleOptionViewItem option = viewOptionsV4();
    option.state |= QStyle::State_Selected;

    for (int j = 0; j < paintPairs.count(); ++j)
    {
        option.rect = paintPairs.at(j).first.translated(-r->topLeft());
        const QModelIndex &current = paintPairs.at(j).second;
        adjustViewOptionsForIndex(&option, current);
        delegateForIndex(current)->paint(&painter, option, current);
    }

    return pixmap;
}

void GlodonAbstractItemViewPrivate::selectAll(QItemSelectionModel::SelectionFlags command)
{
    if (!m_selectionModel)
    {
        return;
    }

    QItemSelection selection;
    QModelIndex oTopLeftIndex = m_model->index(0, 0, m_root);
    QModelIndex oBottomRightIndex =
            m_model->index(m_model->rowCount(m_root) - 1, m_model->columnCount(m_root) - 1, m_root);

    selection.append(QItemSelectionRange(oTopLeftIndex, oBottomRightIndex));
    m_selectionModel->select(selection, command);
}

QModelIndexList GlodonAbstractItemViewPrivate::selectedDraggableIndexes() const
{
    Q_Q(const GlodonAbstractItemView);
    QModelIndexList indexes = q->selectedIndexes();

    for (int i = indexes.count() - 1; i >= 0; --i)
    {
        if (!isIndexDragEnabled(indexes.at(i)))
        {
            indexes.removeAt(i);
        }
    }

    return indexes;
}
