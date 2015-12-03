#include "GLDPopupWindow.h"

#include "qcombobox.h"

#include <qstylepainter.h>
#include <qlineedit.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qlistview.h>
#include <qtableview.h>
#include <qitemdelegate.h>
#include <qmap.h>
#include <qmenu.h>
#include <qevent.h>
#include <qlayout.h>
#include <qscrollbar.h>
#include <qtreeview.h>
#include <qheaderview.h>
#include <qmath.h>
#ifndef QT_NO_IM
#include "qinputcontext.h"
#endif
#include <private/qapplication_p.h>
#include <private/qcombobox_p.h>
#include <private/qabstractitemmodel_p.h>
#include <private/qabstractscrollarea_p.h>
#include <private/qsoftkeymanager_p.h>
#include <qdebug.h>

#ifndef QT_NO_EFFECTS
# include <private/qeffects_p.h>
#endif

#ifndef QT_NO_ACCESSIBILITY
#include "qaccessible.h"
#endif

GLDPopupWindowPrivate::GLDPopupWindowPrivate()
    : QWidgetPrivate(),
      lineEdit(0),
      container(0),
      sizeAdjustPolicy(GLDPopupWindow::AdjustToContentsOnFirstShow),
      minimumContentsLength(0),
      shownOnce(false),
      duplicatesEnabled(false),
      frame(true),
      arrowState(QStyle::State_None),
      hoverControl(QStyle::SC_None),
      indexBeforeChange(-1)
{
}

void GLDPopupWindowPrivate::updateArrow(QStyle::StateFlag state)
{
    Q_Q(GLDPopupWindow);
    if (arrowState == state)
        return;
    arrowState = state;
    QStyleOptionComboBox opt;
    q->initStyleOption(&opt);
    q->update(q->style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, q));
}

QRect GLDPopupWindowPrivate::popupGeometry(int screen) const
{
    return QApplication::desktop()->screenGeometry(screen);
}

bool GLDPopupWindowPrivate::updateHoverControl(const QPoint &pos)
{

    Q_Q(GLDPopupWindow);
    QRect lastHoverRect = hoverRect;
    QStyle::SubControl lastHoverControl = hoverControl;
    bool doesHover = q->testAttribute(Qt::WA_Hover);
    if (lastHoverControl != newHoverControl(pos) && doesHover) {
        q->update(lastHoverRect);
        q->update(hoverRect);
        return true;
    }
    return !doesHover;
}

QStyle::SubControl GLDPopupWindowPrivate::newHoverControl(const QPoint &pos)
{
    Q_Q(GLDPopupWindow);
    QStyleOptionComboBox opt;
    q->initStyleOption(&opt);
    opt.subControls = QStyle::SC_All;
    hoverControl = q->style()->hitTestComplexControl(QStyle::CC_ComboBox, &opt, pos, q);
    hoverRect = (hoverControl != QStyle::SC_None)
                   ? q->style()->subControlRect(QStyle::CC_ComboBox, &opt, hoverControl, q)
                   : QRect();
    return hoverControl;
}

int GLDPopupWindowPrivate::computeWidthHint() const
{
    Q_Q(const GLDPopupWindow);

    int width = 0;

    QStyleOptionComboBox opt;
    q->initStyleOption(&opt);
    QSize tmp(width, 0);
    tmp = q->style()->sizeFromContents(QStyle::CT_ComboBox, &opt, tmp, q);
    return tmp.width();
}

QSize GLDPopupWindowPrivate::recomputeSizeHint(QSize &sh) const
{
    Q_Q(const GLDPopupWindow);
    if (!sh.isValid())
    {
        const QFontMetrics &fm = q->fontMetrics();

        // text width
        if (&sh == &sizeHint || minimumContentsLength == 0)
        {
            switch (sizeAdjustPolicy)
            {
            case GLDPopupWindow::AdjustToContents:
            case GLDPopupWindow::AdjustToContentsOnFirstShow:
                sh.rwidth() = 7 * fm.width(QLatin1Char('x'));
                break;
            default:
                ;
            }
        }

        if (minimumContentsLength > 0)
            sh.setWidth(qMax(sh.width(), minimumContentsLength * fm.width(QLatin1Char('X'))));

        // height
        sh.setHeight(qMax(qCeil(QFontMetricsF(fm).height()), 14) + 2);

        // add style and strut values
        QStyleOptionComboBox opt;
        q->initStyleOption(&opt);
        sh = q->style()->sizeFromContents(QStyle::CT_ComboBox, &opt, sh, q);
    }
    return sh.expandedTo(QApplication::globalStrut());
}

void GLDPopupWindowPrivate::adjustComboBoxSize()
{
    viewContainer()->adjustSizeTimer.start(20, container);
}

void GLDPopupWindowPrivate::updateLayoutDirection()
{
    Q_Q(const GLDPopupWindow);
    QStyleOptionComboBox opt;
    q->initStyleOption(&opt);
    Qt::LayoutDirection dir = Qt::LayoutDirection(
        q->style()->styleHint(QStyle::SH_ComboBox_LayoutDirection, &opt, q));
    if (lineEdit)
        lineEdit->setLayoutDirection(dir);
    if (container)
        container->setLayoutDirection(dir);
}


void GPopupWindowPrivateContainer::timerEvent(QTimerEvent *timerEvent)
{
    if (timerEvent->timerId() == adjustSizeTimer.timerId()) {
        adjustSizeTimer.stop();
        if (combo->sizeAdjustPolicy() == GLDPopupWindow::AdjustToContents) {
            combo->updateGeometry();
            combo->adjustSize();
            combo->update();
        }
    }
}

void GPopupWindowPrivateContainer::resizeEvent(QResizeEvent *e)
{
    QStyleOptionComboBox opt = comboStyleOption();
    if (combo->style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, combo)) {
        QStyleOption myOpt;
        myOpt.initFrom(this);
        QStyleHintReturnMask mask;
        if (combo->style()->styleHint(QStyle::SH_Menu_Mask, &myOpt, this, &mask)) {
            setMask(mask.region);
        }
    } else {
        clearMask();
    }
    QFrame::resizeEvent(e);
}

GPopupWindowPrivateContainer::GPopupWindowPrivateContainer(GLDPopupWindow *parent)
    : QFrame(parent, Qt::Popup), combo(parent)
{
    Q_ASSERT(parent);

    setAttribute(Qt::WA_WindowPropagation);
    setAttribute(Qt::WA_X11NetWmWindowTypeCombo);

    // setup container
    blockMouseReleaseTimer.setSingleShot(true);

    // we need a vertical layout
    QBoxLayout *layout =  new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->setSpacing(0);
    layout->setMargin(0);

    // add scroller arrows if style needs them
    QStyleOptionComboBox opt = comboStyleOption();
    const bool usePopup = combo->style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, combo);
    if (!usePopup)
    {
        setLineWidth(1);
    }

    setFrameStyle(combo->style()->styleHint(QStyle::SH_ComboBox_PopupFrameStyle, &opt, combo));

    // Some styles (Mac) have a margin at the top and bottom of the popup.
    layout->insertSpacing(0, 0);
    layout->addSpacing(0);
    updateTopBottomMargin();
}

int GPopupWindowPrivateContainer::spacing() const
{
    return 0;
}

void GPopupWindowPrivateContainer::updateTopBottomMargin()
{
    if (!layout() || layout()->count() < 1)
        return;

    QBoxLayout *boxLayout = qobject_cast<QBoxLayout *>(layout());
    if (!boxLayout)
        return;

    const QStyleOptionComboBox opt = comboStyleOption();
    const bool usePopup = combo->style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, combo);
    const int margin = usePopup ? combo->style()->pixelMetric(QStyle::PM_MenuVMargin, &opt, combo) : 0;

    QSpacerItem *topSpacer = boxLayout->itemAt(0)->spacerItem();
    if (topSpacer)
        topSpacer->changeSize(0, margin, QSizePolicy::Minimum, QSizePolicy::Fixed);

    QSpacerItem *bottomSpacer = boxLayout->itemAt(boxLayout->count() - 1)->spacerItem();
    if (bottomSpacer && bottomSpacer != topSpacer)
        bottomSpacer->changeSize(0, margin, QSizePolicy::Minimum, QSizePolicy::Fixed);

    boxLayout->invalidate();
}

void GPopupWindowPrivateContainer::setWidget(QWidget *newWidget)
{
    if (popupWidget != newWidget)
    {
        popupWidget = newWidget;
        popupWidget->setParent(this);
        setFixedSize(popupWidget->size());
        popupWidget->setGeometry(0, 0, popupWidget->width(), popupWidget->height());
        popupWidget->setVisible(true);
    }
}

QWidget *GPopupWindowPrivateContainer::widget()
{
    return popupWidget;
}

void GPopupWindowPrivateContainer::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::StyleChange) {
        QStyleOptionComboBox opt = comboStyleOption();
        setFrameStyle(combo->style()->styleHint(QStyle::SH_ComboBox_PopupFrameStyle, &opt, combo));
    }
    QWidget::changeEvent(e);
}


bool GPopupWindowPrivateContainer::eventFilter(QObject *o, QEvent *e)
{
    switch (e->type()) {
    case QEvent::ShortcutOverride:
        switch (static_cast<QKeyEvent*>(e)->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            return true;
        case Qt::Key_Down:
            if (!(static_cast<QKeyEvent*>(e)->modifiers() & Qt::AltModifier))
                break;
            // fall through
        case Qt::Key_F4:
        case Qt::Key_Escape:
            combo->hidePopup();
            return true;
        default:
            break;
        }
    break;
    case QEvent::MouseMove:
        if (isVisible()) {
            QMouseEvent *m = static_cast<QMouseEvent *>(e);
            QWidget *widget = static_cast<QWidget *>(o);
            QPoint vector = widget->mapToGlobal(m->pos()) - initialClickPosition;
            if (vector.manhattanLength() > 9 && blockMouseReleaseTimer.isActive())
                blockMouseReleaseTimer.stop();
        }
        break;
    case QEvent::MouseButtonRelease: {
        if (isVisible())
        {
            combo->hidePopup();
            return true;
        }
        break;
    }
    default:
        break;
    }
    return QFrame::eventFilter(o, e);
}

void GPopupWindowPrivateContainer::showEvent(QShowEvent *)
{
    combo->update();
}

void GPopupWindowPrivateContainer::hideEvent(QHideEvent *)
{
    emit resetButton();
    combo->update();
#ifndef QT_NO_GRAPHICSVIEW
    // QGraphicsScenePrivate::removePopup closes the combo box popup, it hides it non-explicitly.
    // Hiding/showing the GLDPopupWindow after this will unexpectedly show the popup as well.
    // Re-hiding the popup container makes sure it is explicitly hidden.
    if (QGraphicsProxyWidget *proxy = graphicsProxyWidget())
        proxy->hide();
#endif
}

void GPopupWindowPrivateContainer::mousePressEvent(QMouseEvent *e)
{
    QStyleOptionComboBox opt = comboStyleOption();
    opt.subControls = QStyle::SC_All;
    opt.activeSubControls = QStyle::SC_ComboBoxArrow;
    QStyle::SubControl sc = combo->style()->hitTestComplexControl(QStyle::CC_ComboBox, &opt,
                                                           combo->mapFromGlobal(e->globalPos()),
                                                           combo);
    if ((combo->isEditable() && sc == QStyle::SC_ComboBoxArrow)
        || (!combo->isEditable() && sc != QStyle::SC_None))
        setAttribute(Qt::WA_NoMouseReplay);
}

void GPopupWindowPrivateContainer::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    if (!blockMouseReleaseTimer.isActive())
    {
        combo->hidePopup();
        emit resetButton();
    }
}

QStyleOptionComboBox GPopupWindowPrivateContainer::comboStyleOption() const
{
    // ### This should use GLDPopupWindow's initStyleOption(), but it's protected
    // perhaps, we could cheat by having the QCombo private instead?
    QStyleOptionComboBox opt;
    opt.initFrom(combo);
    opt.subControls = QStyle::SC_All;
    opt.activeSubControls = QStyle::SC_None;
    opt.editable = combo->isEditable();
    return opt;
}

GLDPopupWindow::GLDPopupWindow(QWidget *parent)
    : QWidget(*new GLDPopupWindowPrivate(), parent, 0)
{
    Q_D(GLDPopupWindow);
    d->init();
}

GLDPopupWindow::GLDPopupWindow(GLDPopupWindowPrivate &dd, QWidget *parent)
    : QWidget(dd, parent, 0)
{
    Q_D(GLDPopupWindow);
    d->init();
}

void GLDPopupWindowPrivate::init()
{
    Q_Q(GLDPopupWindow);
    q->setFocusPolicy(Qt::WheelFocus);
    q->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed,
                                 QSizePolicy::ComboBox));
    setLayoutItemMargins(QStyle::SE_ComboBoxLayoutItem);
    if (!q->isEditable())
        q->setAttribute(Qt::WA_InputMethodEnabled, false);
    else
        q->setAttribute(Qt::WA_InputMethodEnabled);
}

GPopupWindowPrivateContainer* GLDPopupWindowPrivate::viewContainer()
{
    if (container)
        return container;

    Q_Q(GLDPopupWindow);

    container = new GPopupWindowPrivateContainer(q);
    updateLayoutDirection();
    updateViewContainerPaletteAndOpacity();
    QObject::connect(container, SIGNAL(resetButton()), q, SLOT(_q_resetButton()));
    return container;
}


void GLDPopupWindowPrivate::_q_resetButton()
{
    updateArrow(QStyle::State_None);
}

void GLDPopupWindowPrivate::updateViewContainerPaletteAndOpacity()
{
    if (!container)
        return;
    Q_Q(GLDPopupWindow);
    QStyleOptionComboBox opt;
    q->initStyleOption(&opt);
#ifndef QT_NO_MENU
    if (q->style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, q)) {
        QMenu menu;
        menu.ensurePolished();
        container->setPalette(menu.palette());
        container->setWindowOpacity(menu.windowOpacity());
    } else
#endif
    {
        container->setPalette(q->palette());
        container->setWindowOpacity(1.0);
    }
    if (lineEdit)
        lineEdit->setPalette(q->palette());
}

void GLDPopupWindow::initStyleOption(QStyleOptionComboBox *option) const
{
    if (!option)
        return;

    Q_D(const GLDPopupWindow);
    option->initFrom(this);
    option->editable = isEditable();
    option->frame = d->frame;
    if (hasFocus() && !option->editable)
        option->state |= QStyle::State_Selected;
    option->subControls = QStyle::SC_All;
    if (d->arrowState == QStyle::State_Sunken) {
        option->activeSubControls = QStyle::SC_ComboBoxArrow;
        option->state |= d->arrowState;
    }
    else
    {
        option->activeSubControls = d->hoverControl;
    }
    if (d->container && d->container->isVisible())
        option->state |= QStyle::State_On;
}

void GLDPopupWindowPrivate::updateLineEditGeometry()
{
    if (!lineEdit)
        return;

    Q_Q(GLDPopupWindow);
    QStyleOptionComboBox opt;
    q->initStyleOption(&opt);
    QRect editRect = q->style()->subControlRect(QStyle::CC_ComboBox, &opt,
                                                QStyle::SC_ComboBoxEditField, q);
    lineEdit->setGeometry(editRect);
}

Qt::MatchFlags GLDPopupWindowPrivate::matchFlags() const
{
    // Base how duplicates are determined on the autocompletion case sensitivity
    Qt::MatchFlags flags = Qt::MatchFixedString;
#ifndef QT_NO_COMPLETER
    if (!lineEdit->completer() || lineEdit->completer()->caseSensitivity() == Qt::CaseSensitive)
#endif
        flags |= Qt::MatchCaseSensitive;
    return flags;
}

void GLDPopupWindowPrivate::_q_returnPressed()
{
    if (lineEdit && !lineEdit->text().isEmpty()) {
        lineEdit->deselect();
        lineEdit->end(false);

        // check for duplicates (if not enabled) and quit
        if (!duplicatesEnabled)
        {
            return;
        }
    }
}

GLDPopupWindow::~GLDPopupWindow()
{
}

bool GLDPopupWindow::duplicatesEnabled() const
{
    Q_D(const GLDPopupWindow);
    return d->duplicatesEnabled;
}

void GLDPopupWindow::setDuplicatesEnabled(bool enable)
{
    Q_D(GLDPopupWindow);
    d->duplicatesEnabled = enable;
}

GLDPopupWindow::SizeAdjustPolicy GLDPopupWindow::sizeAdjustPolicy() const
{
    Q_D(const GLDPopupWindow);
    return d->sizeAdjustPolicy;
}

void GLDPopupWindow::setSizeAdjustPolicy(GLDPopupWindow::SizeAdjustPolicy policy)
{
    Q_D(GLDPopupWindow);
    if (policy == d->sizeAdjustPolicy)
        return;

    d->sizeAdjustPolicy = policy;
    d->sizeHint = QSize();
    d->adjustComboBoxSize();
    updateGeometry();
}

int GLDPopupWindow::minimumContentsLength() const
{
    Q_D(const GLDPopupWindow);
    return d->minimumContentsLength;
}

void GLDPopupWindow::setMinimumContentsLength(int characters)
{
    Q_D(GLDPopupWindow);
    if (characters == d->minimumContentsLength || characters < 0)
        return;

    d->minimumContentsLength = characters;

    if (d->sizeAdjustPolicy == AdjustToContents
            || d->sizeAdjustPolicy == AdjustToMinimumContentsLengthWithIcon)
    {
        d->sizeHint = QSize();
        d->adjustComboBoxSize();
        updateGeometry();
    }
}

bool GLDPopupWindow::isEditable() const
{
    Q_D(const GLDPopupWindow);
    return d->lineEdit != 0;
}

void GLDPopupWindow::setEditable(bool editable)
{
    Q_D(GLDPopupWindow);
    if (isEditable() == editable)
        return;

    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    if (editable)
    {
        QLineEdit *le = new QLineEdit(this);
        setLineEdit(le);
    }
    else
    {
        setAttribute(Qt::WA_InputMethodEnabled, false);
        d->lineEdit->hide();
        d->lineEdit->deleteLater();
        d->lineEdit = 0;
    }

    d->viewContainer()->updateTopBottomMargin();
    if (!testAttribute(Qt::WA_Resized))
        adjustSize();
}

void GLDPopupWindow::setLineEdit(QLineEdit *edit)
{
    Q_D(GLDPopupWindow);
    if (!edit) {
        qWarning("GLDPopupWindow::setLineEdit: cannot set a 0 line edit");
        return;
    }

    if (edit == d->lineEdit)
        return;

    edit->setText(currentText());
    delete d->lineEdit;

    d->lineEdit = edit;
    if (d->lineEdit->parent() != this)
        d->lineEdit->setParent(this);
    connect(d->lineEdit, SIGNAL(returnPressed()), this, SLOT(_q_returnPressed()));
    connect(d->lineEdit, SIGNAL(editingFinished()), this, SLOT(_q_editingFinished()));
    connect(d->lineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(editTextChanged(QString)));
    d->lineEdit->setFrame(false);
    d->lineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    d->lineEdit->setFocusProxy(this);
    d->lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
    setAttribute(Qt::WA_InputMethodEnabled);
    d->updateLayoutDirection();
    d->updateLineEditGeometry();
    if (isVisible())
        d->lineEdit->show();

    update();
}

/*!
    Returns the line edit used to edit items in the combobox, or 0 if there
    is no line edit.

    Only editable combo boxes have a line edit.
*/
QLineEdit *GLDPopupWindow::lineEdit() const
{
    Q_D(const GLDPopupWindow);
    return d->lineEdit;
}

#ifndef QT_NO_VALIDATOR
/*!
    \fn void GLDPopupWindow::setValidator(const QValidator *validator)

    Sets the \a validator to use instead of the current validator.
*/

void GLDPopupWindow::setValidator(const QValidator *v)
{
    Q_D(GLDPopupWindow);
    if (d->lineEdit)
        d->lineEdit->setValidator(v);
}

const QValidator *GLDPopupWindow::validator() const
{
    Q_D(const GLDPopupWindow);
    return d->lineEdit ? d->lineEdit->validator() : 0;
}
#endif // QT_NO_VALIDATOR

QString GLDPopupWindow::currentText() const
{
    Q_D(const GLDPopupWindow);
    if (d->lineEdit)
        return d->lineEdit->text();
    else
        return QString();
}

QSize GLDPopupWindow::minimumSizeHint() const
{
    Q_D(const GLDPopupWindow);
    return d->recomputeSizeHint(d->minimumSizeHint);
}

QSize GLDPopupWindow::sizeHint() const
{
    Q_D(const GLDPopupWindow);
    return d->recomputeSizeHint(d->sizeHint);
}

void GLDPopupWindow::showPopup()
{
    Q_D(GLDPopupWindow);
    if (d->container->widget() == 0)
    {
        return;
    }

    QStyle * const style = this->style();

    // set current item and select it

    GPopupWindowPrivateContainer* container = d->viewContainer();
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    QRect listRect(style->subControlRect(QStyle::CC_ComboBox, &opt,
                                         QStyle::SC_ComboBoxListBoxPopup, this));

    QRect vcRect(0, 0, container->width(), container->height());

    QRect screen = d->popupGeometry(QApplication::desktop()->screenNumber(this));

    QPoint below = mapToGlobal(listRect.bottomLeft());
    int belowHeight = screen.bottom() - below.y();
    QPoint above = mapToGlobal(listRect.topLeft());
    int aboveHeight = above.y() - screen.y();
    bool boundToScreen = !window()->testAttribute(Qt::WA_DontShowOnScreen);

    const bool usePopup = style->styleHint(QStyle::SH_ComboBox_Popup, &opt, this);
    {
        // add the spacing for the grid on the top and the bottom;
        int heightMargin = 2*container->spacing();

        // add the frame of the container
        int marginTop, marginBottom = 0;
        container->getContentsMargins(0, &marginTop, 0, &marginBottom);
        heightMargin += marginTop + marginBottom;

        //add the frame of the view
        heightMargin += marginTop + marginBottom;

        listRect.setHeight(listRect.height() + heightMargin);
    }

    // Add space for margin at top and bottom if the style wants it.
    if (usePopup)
        listRect.setHeight(listRect.height() + style->pixelMetric(QStyle::PM_MenuVMargin, &opt, this) * 2);

    // Make sure the popup is wide enough to display its contents.
    if (usePopup) {
        const int diff = d->computeWidthHint() - width();
        if (diff > 0)
            listRect.setWidth(listRect.width() + diff);
    }

    //we need to activate the layout to make sure the min/maximum size are set when the widget was not yet show
    container->layout()->activate();
    //takes account of the minimum/maximum size of the container

    // make sure the widget fits on screen
    if (boundToScreen)
    {
        if (vcRect.width() > screen.width())
        {
            vcRect.setWidth(screen.width());
        }

        if (mapToGlobal(vcRect.bottomRight()).x() > screen.right())
        {
            below.setX(screen.x() + screen.width() - vcRect.width());
            above.setX(screen.x() + screen.width() - vcRect.width());
        }

        if (mapToGlobal(vcRect.topLeft()).x() < screen.x())
        {
            below.setX(screen.x());
            above.setX(screen.x());
        }
    }

    if (usePopup)
    {
        // Position horizontally.
        listRect.moveLeft(above.x());

        const int height = !boundToScreen ? listRect.height() : qMin(listRect.height(), screen.height());
        listRect.setHeight(height);

        if (boundToScreen)
        {
            if (listRect.top() < screen.top())
                listRect.moveTop(screen.top());
            if (listRect.bottom() > screen.bottom())
                listRect.moveBottom(screen.bottom());
        }
    }
    else if (!boundToScreen || vcRect.height() <= belowHeight)
    {
        vcRect.moveTopLeft(below);
    }
    else if (vcRect.height() <= aboveHeight)
    {
        vcRect.moveBottomLeft(above);
    }
    else if (belowHeight >= aboveHeight)
    {
        vcRect.setHeight(belowHeight);
        vcRect.moveTopLeft(below);
    }
    else
    {
        vcRect.setHeight(aboveHeight);
        vcRect.moveBottomLeft(above);
    }

#ifndef QT_NO_IM
    if (QInputContext *qic = inputContext())
        qic->reset();
#endif

    container->setGeometry(vcRect);

#ifndef Q_WS_MAC
    const bool updatesEnabled = container->updatesEnabled();
#endif

#if defined(Q_WS_WIN) && !defined(QT_NO_EFFECTS)
    bool scrollDown = (vcRect.topLeft() == below);
    if (QApplication::isEffectEnabled(Qt::UI_AnimateCombo)
        && !style->styleHint(QStyle::SH_ComboBox_Popup, &opt, this) && !window()->testAttribute(Qt::WA_DontShowOnScreen))
        qScrollEffect(container, scrollDown ? QEffects::DownScroll : QEffects::UpScroll, 150);
#endif

#ifndef Q_WS_MAC
    container->setUpdatesEnabled(false);
#endif

    container->raise();
    container->show();

#ifndef Q_WS_MAC
    container->setUpdatesEnabled(updatesEnabled);
#endif

    container->update();
}

void GLDPopupWindow::hidePopup()
{
    Q_D(GLDPopupWindow);
    if (d->container && d->container->isVisible())
    {
#if !defined(QT_NO_EFFECTS)
        d->container->blockSignals(true);
        bool needFade = style()->styleHint(QStyle::SH_Menu_FadeOutOnHide);
        d->container->blockSignals(false);

        if (!needFade)
#endif
            d->container->hide();
    }
    d->_q_resetButton();
}

void GLDPopupWindow::clear()
{
#ifndef QT_NO_ACCESSIBILITY
        QAccessible::updateAccessibility(this, 0, QAccessible::NameChanged);
#endif
}

void GLDPopupWindow::clearEditText()
{
    Q_D(GLDPopupWindow);
    if (d->lineEdit)
        d->lineEdit->clear();
#ifndef QT_NO_ACCESSIBILITY
        QAccessible::updateAccessibility(this, 0, QAccessible::NameChanged);
#endif
}

void GLDPopupWindow::setEditText(const QString &text)
{
    Q_D(GLDPopupWindow);
    if (d->lineEdit)
        d->lineEdit->setText(text);
#ifndef QT_NO_ACCESSIBILITY
        QAccessible::updateAccessibility(this, 0, QAccessible::NameChanged);
#endif
}

void GLDPopupWindow::focusInEvent(QFocusEvent *e)
{
    Q_D(GLDPopupWindow);
    update();
    if (d->lineEdit)
    {
        d->lineEdit->event(e);
    }
}

void GLDPopupWindow::focusOutEvent(QFocusEvent *e)
{
    Q_D(GLDPopupWindow);
    update();
    if (d->lineEdit)
        d->lineEdit->event(e);
}

void GLDPopupWindow::changeEvent(QEvent *e)
{
    Q_D(GLDPopupWindow);
    switch (e->type()) {
    case QEvent::StyleChange:
#ifdef Q_WS_MAC
    case QEvent::MacSizeChange:
#endif
        d->sizeHint = QSize(); // invalidate size hint
        d->minimumSizeHint = QSize();
        d->updateLayoutDirection();
        if (d->lineEdit)
            d->updateLineEditGeometry();
        d->setLayoutItemMargins(QStyle::SE_ComboBoxLayoutItem);
        break;
    case QEvent::EnabledChange:
        if (!isEnabled())
            hidePopup();
        break;
    case QEvent::PaletteChange: {
        d->updateViewContainerPaletteAndOpacity();
        break;
    }
    case QEvent::FontChange:
        d->sizeHint = QSize(); // invalidate size hint
        if (d->lineEdit)
            d->updateLineEditGeometry();
        break;
    default:
        break;
    }
    QWidget::changeEvent(e);
}

void GLDPopupWindow::resizeEvent(QResizeEvent *)
{
    Q_D(GLDPopupWindow);
    d->updateLineEditGeometry();
}

void GLDPopupWindow::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    // draw the combobox frame, focusrect and selected etc.
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    // draw the icon and text
    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

void GLDPopupWindow::showEvent(QShowEvent *e)
{
    Q_D(GLDPopupWindow);
    if (!d->shownOnce && d->sizeAdjustPolicy == GLDPopupWindow::AdjustToContentsOnFirstShow) {
        d->sizeHint = QSize();
        updateGeometry();
    }
    d->shownOnce = true;
    QWidget::showEvent(e);
}

void GLDPopupWindow::hideEvent(QHideEvent *)
{
    hidePopup();
}

bool GLDPopupWindow::event(QEvent *event)
{
    Q_D(GLDPopupWindow);
    switch(event->type())
    {
    case QEvent::LayoutDirectionChange:
    case QEvent::ApplicationLayoutDirectionChange:
        d->updateLayoutDirection();
        d->updateLineEditGeometry();
        break;
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::HoverMove:
    if (const QHoverEvent *he = static_cast<const QHoverEvent *>(event))
        d->updateHoverControl(he->pos());
        break;
    case QEvent::ShortcutOverride:
        if (d->lineEdit)
            return d->lineEdit->event(event);
        break;
    default:
        break;
    }
    return QWidget::event(event);
}

void GLDPopupWindow::mousePressEvent(QMouseEvent *e)
{
    Q_D(GLDPopupWindow);
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    QStyle::SubControl sc = style()->hitTestComplexControl(QStyle::CC_ComboBox, &opt, e->pos(),
                                                           this);
    if (e->button() == Qt::LeftButton && (sc == QStyle::SC_ComboBoxArrow || !isEditable())
        && !d->viewContainer()->isVisible()) {
        if (sc == QStyle::SC_ComboBoxArrow)
        {
            d->updateArrow(QStyle::State_Sunken);
        }
            d->viewContainer()->blockMouseReleaseTimer.start(QApplication::doubleClickInterval());
            d->viewContainer()->initialClickPosition = mapToGlobal(e->pos());

        showPopup();
    }
    else
    {
        QWidget::mousePressEvent(e);
    }
}

void GLDPopupWindow::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(GLDPopupWindow);
    Q_UNUSED(e);
    d->updateArrow(QStyle::State_None);
}

void GLDPopupWindow::keyPressEvent(QKeyEvent *e)
{
    Q_D(GLDPopupWindow);

    switch (e->key()) {
    case Qt::Key_Up:
        if (e->modifiers() & Qt::ControlModifier)
            break; // pass to line edit for auto completion
    case Qt::Key_Down:
        if (e->modifiers() & Qt::AltModifier) {
            showPopup();
            return;
        } else if (e->modifiers() & Qt::ControlModifier)
            break; // pass to line edit for auto completion
        // fall through
    case Qt::Key_F4:
        if (!e->modifiers()) {
            showPopup();
            return;
        }
        break;
    case Qt::Key_Space:
        if (!d->lineEdit) {
            showPopup();
            return;
        }
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Escape:
        if (!d->lineEdit)
            e->ignore();
        break;
    default:
        if (!d->lineEdit)
        {
                e->ignore();
        }
    }
    if (d->lineEdit)
    {
        d->lineEdit->event(e);
    }
}

void GLDPopupWindow::keyReleaseEvent(QKeyEvent *e)
{
    Q_D(GLDPopupWindow);
    if (d->lineEdit)
        d->lineEdit->event(e);
}

#ifndef QT_NO_WHEELEVENT
void GLDPopupWindow::wheelEvent(QWheelEvent *e)
{
    Q_D(GLDPopupWindow);
    if (!d->viewContainer()->isVisible())
    {
        e->accept();
    }
}
#endif

void GLDPopupWindow::contextMenuEvent(QContextMenuEvent *e)
{
    Q_D(GLDPopupWindow);
    if (d->lineEdit) {
        Qt::ContextMenuPolicy p = d->lineEdit->contextMenuPolicy();
        d->lineEdit->setContextMenuPolicy(Qt::DefaultContextMenu);
        d->lineEdit->event(e);
        d->lineEdit->setContextMenuPolicy(p);
    }
}

void GLDPopupWindow::inputMethodEvent(QInputMethodEvent *e)
{
    Q_D(GLDPopupWindow);
    if (d->lineEdit)
    {
        d->lineEdit->event(e);
    }
    else
    {
        if (e->commitString().isEmpty())
            e->ignore();
    }
}

QVariant GLDPopupWindow::inputMethodQuery(Qt::InputMethodQuery query) const
{
    Q_D(const GLDPopupWindow);
    if (d->lineEdit)
        return d->lineEdit->inputMethodQuery(query);
    return QWidget::inputMethodQuery(query);
}

bool GLDPopupWindow::hasFrame() const
{
    Q_D(const GLDPopupWindow);
    return d->frame;
}

void GLDPopupWindow::setPopupWidget(QWidget *newWidget)
{
    Q_D(GLDPopupWindow);
    d->viewContainer()->setWidget(newWidget);
}

QWidget *GLDPopupWindow::PopupWidget()
{
    Q_D(GLDPopupWindow);
    return d->viewContainer()->widget();
}

void GLDPopupWindow::setFrame(bool enable)
{
    Q_D(GLDPopupWindow);
    d->frame = enable;
    update();
    updateGeometry();
}
