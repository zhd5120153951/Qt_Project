#include "GLDWindowComboBox.h"
#include "GLDGlobal.h"
#include "GLDTextEdit.h"
#include "private/qabstractspinbox_p.h"

#include <QStyleFactory>
#ifndef QT_NO_EFFECTS
#include <private/qeffects_p.h>
#endif
#include <QStyle>
#include <QTimer>
#include <QMenu>

GLDWindowComboBox::GLDWindowComboBox(QWidget *parent)
    : GLDWAbstractSpinBox(parent)
{
    setWrapping(true);
    m_hasHadFocus = false;
    m_framePopuped = false;
    m_hidePopupOnEdit = true;
    m_showPopupOnEnter = false;
    m_hideEditOnExit = false;
    m_editorNeedFocus = false;
    m_arrowState = QStyle::State_None;
    m_pComboxPopup = NULL;
    m_buttonType = ComboBoxArrow;
    m_animationPopup = true;

#ifdef QT_KEYPAD_NAVIGATION
    focusOnButton = false;
#endif
    init();
    setObjectName("GLDWindowComboBox");

    connect(getLineEdit(), SIGNAL(cursorPositionChanged()),
            this, SLOT(doCursorPositionChanged()));
    connect(getLineEdit(), SIGNAL(selectionChanged()),
            this, SLOT(doSelectionChanged()));
}

GLDWindowComboBox::~GLDWindowComboBox()
{
    freeAndNil(m_pComboxPopup);
}

QWidget *GLDWindowComboBox::popupWidget() const
{
    if (!m_framePopuped || m_pComboxPopup == NULL)
    {
        return NULL;
    }


    return m_pComboxPopup->popupWidget();
}

void GLDWindowComboBox::setPopupWidget(QWidget *popup)
{
    if (!popup)
    {
        qWarning("GWindowComboBox::setPopupWidget: Cannot set a null popup widget");
        return;
    }

    if (!m_framePopuped)
    {
        qWarning("GWindowComboBox::setPopupWidget: comboBoxPopup is set to false");
        return;
    }

    initComboBoxPopup(popup);
}

bool GLDWindowComboBox::framePopup() const
{
    return m_framePopuped;
}

void GLDWindowComboBox::setFramePopup(bool enable)
{
    if (enable == m_framePopuped)
    {
        return;
    }

    setAttribute(Qt::WA_MacShowFocusRect, !enable);
    m_framePopuped = enable;
#ifdef QT_KEYPAD_NAVIGATION

    if (!enable)
    {
        focusOnButton = false;
    }

#endif
    updateEditFieldGeometry();
    update();
}

void GLDWindowComboBox::setPopupScrollTime(int time)
{
    if (m_pComboxPopup != NULL)
    {
        m_pComboxPopup->setScrollTime(time);
    }
}

int GLDWindowComboBox::popupScrollTime()
{
    if (m_pComboxPopup != NULL)
    {
        return m_pComboxPopup->scrollTime();
    }
    else
    {
        return -1;
    }
}

void GLDWindowComboBox::setResizeEnable(bool value)
{
    if (m_pComboxPopup != NULL)
    {
        m_pComboxPopup->setSizeGripEnabled(value);
    }
}

void GLDWindowComboBox::setPopupSize(QSize value)
{
    if (m_pComboxPopup != NULL)
    {
        m_pComboxPopup->setPopupSize(value);
    }
}

void GLDWindowComboBox::setPopupHeight(int value)
{
    if (m_pComboxPopup != NULL)
    {
        m_pComboxPopup->setPopupHeight(value);
    }
}

void GLDWindowComboBox::hidePopup()
{
    if (m_pComboxPopup->isVisible())
    {
        m_pComboxPopup->hide();
    }
}

void GLDWindowComboBox::setButtonTypes(GLDWindowComboBox::ButtonTypes type)
{
    if (type == None)
    {
        setButtonSymbols(QAbstractSpinBox::NoButtons);
    }

    m_buttonType = type;
}

GLDWindowComboBox::ButtonTypes GLDWindowComboBox::buttonType()
{
    return m_buttonType;
}

bool GLDWindowComboBox::hasSelectedText()
{
    return lineEdit()->hasSelectedText();
}

void GLDWindowComboBox::showPopup()
{
    if (m_pComboxPopup != NULL && m_pComboxPopup->isVisible())
    {
        return;
    }

    updateArrow(QStyle::State_Sunken);
    initComboBoxPopup();

    QStyleOptionSpinBox opt;
    initStyleOption(&opt);

    //Show the popup
    m_pComboxPopup->show();
}

void GLDWindowComboBox::closePopup()
{
    if (m_pComboxPopup == NULL)
    {
        return;
    }

    if (!m_pComboxPopup->isVisible())
    {
        return;
    }

    m_pComboxPopup->close();
    emit onManualColsePopup();
}

bool GLDWindowComboBox::isEditable() const
{
    return !lineEdit()->isReadOnly();
}

void GLDWindowComboBox::setEditable(bool editable)
{
    lineEdit()->setReadOnly(!editable);
}

QString GLDWindowComboBox::editText() const
{
    return lineEdit()->toPlainText();
}

void GLDWindowComboBox::setEditText(const QString &text)
{
    lineEdit()->setPlainText(text);
}

void GLDWindowComboBox::init()
{
#ifdef QT_KEYPAD_NAVIGATION

    if (QApplication::keypadNavigationEnabled())
    {
        setFramePopup(true);
    }

#endif
    setInputMethodHints(Qt::ImhPreferNumbers);
    setFramePopup(true);
}

QStyle::SubControl GLDWindowComboBox::newHoverControl(const QPoint &pos)
{
    QStyleOptionComboBox optCombo;
    optCombo.init(this);
    optCombo.editable = true;
    optCombo.subControls = QStyle::SC_All;
    return style()->hitTestComplexControl(QStyle::CC_ComboBox, &optCombo, pos, this);
}

void GLDWindowComboBox::updateEditFieldGeometry()
{
    QStyleOptionComboBox optCombo;
    optCombo.init(this);
    optCombo.editable = true;
    optCombo.subControls = QStyle::SC_ComboBoxFrame | QStyle::SC_ComboBoxEditField;

    if (m_buttonType != None)
    {
        optCombo.subControls |= QStyle::SC_ComboBoxArrow;
        // 原来获取rect的方式有问题，因此改为整个减去右侧的按钮的方式获取
        QRect comboBoxArrowRect = style()->subControlRect(QStyle::CC_ComboBox, &optCombo,
                                  QStyle::SC_ComboBoxArrow, this);
        QRect editRect = rect().adjusted(0, 0, -comboBoxArrowRect.width(), 0);
        // 由于在setGeometry的时候，会再次计算并设置MinimumSize和MaximumSize
        // 会出现区域改变的问题，因此在这里先设置好MinimumSize和MaximumSize
        lineEdit()->setMinimumSize(editRect.width(), editRect.height());
        lineEdit()->setMaximumSize(editRect.width(), editRect.height());
        lineEdit()->setGeometry(editRect);
    }
    else
    {
        QRect editRect = rect().adjusted(1, 0, 0, -1);
        lineEdit()->setMinimumSize(editRect.width(), editRect.height());
        lineEdit()->setMaximumSize(editRect.width(), editRect.height());
    }
}

void GLDWindowComboBox::doPopupHide()
{
    updateArrow(QStyle::State_None);
    emit onHide();
}

void GLDWindowComboBox::doSelectionChanged()
{
    emit selectionChanged();
}

void GLDWindowComboBox::doCursorPositionChanged()
{
    emit cursorPositionChanged();
}

void GLDWindowComboBox::updateArrow(QStyle::StateFlag state)
{
    if (m_arrowState == state)
    {
        return;
    }

    m_arrowState = state;
//    if (arrowState != QStyle::State_None)
//        buttonState |= Mouse;
//    else
//    {
//        buttonState = 0;
//        hoverControl = QStyle::SC_ComboBoxFrame;
//    }
    update();
}

void GLDWindowComboBox::syncPopupWidget()
{
    if (m_pComboxPopup != NULL)
    {
        const bool c_sb = m_pComboxPopup->blockSignals(true);
        m_pComboxPopup->blockSignals(c_sb);
        m_pComboxPopup->setAnimationPopup(m_animationPopup);
    }
}

void GLDWindowComboBox::initComboBoxPopup(QWidget *cw)
{
    if (m_pComboxPopup == NULL)
    {
        m_pComboxPopup = new GLDComboBoxPopup(this, cw);
        m_pComboxPopup->setObjectName(QLatin1String("GLDComboBoxPopup"));

        QObject::connect(m_pComboxPopup, SIGNAL(onHide()),
                         this, SLOT(doPopupHide()));
        QObject::connect(m_pComboxPopup, SIGNAL(shouldShowPopup(bool &)),
                         this, SIGNAL(shouldShowPopup(bool &)));

        m_pComboxPopup->installEventFilter(this);
    }
    else if (cw != NULL)
    {
        m_pComboxPopup->setpopupWidget(cw);
    }

    syncPopupWidget();
}

void GLDWindowComboBox::keyPressEvent(QKeyEvent *event)
{
    bool bhandled = false;

    emit keyPressType(Qt::Key(event->key()), bhandled);

    if (bhandled)
    {
        return;
    }

    bool bSelect = true;

    switch (event->key())
    {
        case Qt::Key_Enter:
        case Qt::Key_Return:
//        d->interpret(AlwaysEmit);
            event->ignore();
            emit editingFinished();
            break;

        default:
#ifdef QT_KEYPAD_NAVIGATION
            if (QApplication::keypadNavigationEnabled() && !hasEditFocus()
                    && !event->text().isEmpty() && event->text().at(0).isLetterOrNumber())
            {
                setEditFocus(true);

                //hide cursor
                lineEdit()->d_func()->setCursorVisible(false);
                lineEdit()->d_func()->control->setCursorBlinkPeriod(0);
            }

#endif

        case Qt::Key_Left:
        case Qt::Key_Right:
            if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)
            {
                if (
#ifdef QT_KEYPAD_NAVIGATION
                    QApplication::keypadNavigationEnabled() && !hasEditFocus()
                    || !QApplication::keypadNavigationEnabled() &&
#endif
                    !(event->modifiers() & Qt::ControlModifier))
                {
                    bSelect = false;
                    break;
                }

#ifdef Q_WS_MAC
                else
#ifdef QT_KEYPAD_NAVIGATION
                    if (!QApplication::keypadNavigationEnabled())
#endif
                    {
                        select = (event->modifiers() & Qt::ShiftModifier);
                        break;
                    }

#endif
            }

            // else fall through
    }

    GLDWAbstractSpinBox::keyPressEvent(event);
    G_UNUSED(bSelect)
}

void GLDWindowComboBox::focusInEvent(QFocusEvent *event)
{
    GLDWAbstractSpinBox::focusInEvent(event);
    const bool c_oldHasHadFocus = m_hasHadFocus;
    m_hasHadFocus = true;

    switch (event->reason())
    {
        case Qt::MouseFocusReason:
        case Qt::PopupFocusReason:
            return;

        case Qt::ActiveWindowFocusReason:
            if (c_oldHasHadFocus)
            {
                return;
            }

        case Qt::ShortcutFocusReason:
        case Qt::TabFocusReason:
        default:
            break;
    }

    if (m_showPopupOnEnter && !m_pComboxPopup->isVisible())
    {
        showPopup();
        m_editorNeedFocus = true;
    }
}

void GLDWindowComboBox::mousePressEvent(QMouseEvent *event)
{
    if (!framePopup())
    {
        GLDWAbstractSpinBox::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton && m_buttonType != None)
    {
        QStyleOptionComboBox optCombo;
        optCombo.init(this);

        QRect subRect = style()->proxy()->subControlRect(QStyle::CC_ComboBox, &optCombo,
                        QStyle::SC_ComboBoxArrow, this);

        if (subRect.contains(event->pos()))
        {
            event->accept();
            //if (isReadOnly()) return;
            bool bValue = true;
            emit shouldShowPopup(bValue);

            if (bValue)
            {
                showPopup();
            }
        }
    }
    else
    {
        GLDWAbstractSpinBox::mousePressEvent(event);
    }
}

void GLDWindowComboBox::paintEvent(QPaintEvent *event)
{
    if (!m_framePopuped)
    {
        GLDWAbstractSpinBox::paintEvent(event);
        return;
    }

    QStyleOptionSpinBox opt;
    initStyleOption(&opt);

    QStyleOptionComboBox optCombo;

    optCombo.init(this);
    optCombo.editable = true;
    optCombo.frame = opt.frame;
    optCombo.subControls = opt.subControls;
    optCombo.activeSubControls = opt.activeSubControls;
    optCombo.state = opt.state;

    if (lineEdit()->isReadOnly())
    {
        optCombo.state &= ~QStyle::State_Enabled;
    }

    //鼠标位于箭头所在区域高亮，其他地方不高亮
    QPoint curPos = mapFromGlobal(QCursor::pos());
    QRect subRect = style()->proxy()->subControlRect(QStyle::CC_ComboBox, &optCombo, QStyle::SC_ComboBoxArrow, this);

    if (subRect.contains(curPos))
    {
        optCombo.activeSubControls |= newHoverControl(curPos);
    }
    else
    {
        optCombo.activeSubControls &= newHoverControl(curPos);
    }

    QPainter tmpPt(this);

    if (m_buttonType == ComboBoxArrow)
    {
        style()->drawComplexControl(QStyle::CC_ComboBox, &optCombo, &tmpPt, this);
    }
    else if (m_buttonType == Ellipsis)
    {
        QStyleOptionButton button;
        button.rect = subRect;
        style()->drawControl(QStyle::CE_PushButton, &button, &tmpPt);

        QPen pen = tmpPt.pen();
        pen.setColor(Qt::black);
        tmpPt.setPen(pen);

        tmpPt.drawRect(subRect.left() + 2, subRect.height() / 2, 1, 1);
        tmpPt.drawRect(subRect.center().x(), subRect.height() / 2, 1, 1);
        tmpPt.drawRect(subRect.right() - 2, subRect.height() / 2, 1, 1);

        tmpPt.fillRect(lineEdit()->width(), lineEdit()->y(),
                       this->width() - lineEdit()->width() - subRect.width(), subRect.height() - 3, Qt::white);
    }
}

void GLDWindowComboBox::contextMenuEvent(QContextMenuEvent *event)
{
#ifdef QT_NO_CONTEXTMENU
    Q_UNUSED(event);
#else
    QPointer<QMenu> menu = lineEdit()->createStandardContextMenu();

    if (!menu)
    {
        return;
    }

    const QPoint c_pos = (event->reason() == QContextMenuEvent::Mouse)
                         ? event->globalPos() : mapToGlobal(QPoint(event->pos().x(), 0)) + QPoint(width() / 2, height() / 2);
    menu->exec(c_pos);
    delete static_cast<QMenu *>(menu);
    event->accept();
#endif // QT_NO_CONTEXTMENU
}

void GLDWindowComboBox::initStyleOption(QStyleOptionSpinBox *option) const
{
    if (!option)
    {
        return;
    }

    GLDWAbstractSpinBox::initStyleOption(option);

    if (m_framePopuped)
    {
        option->subControls = QStyle::SC_ComboBoxFrame | QStyle::SC_ComboBoxEditField
                              | QStyle::SC_ComboBoxArrow;

        if (m_arrowState == QStyle::State_Sunken)
        {
            option->state |= QStyle::State_Sunken;
        }
        else
        {
            option->state &= ~QStyle::State_Sunken;
        }
    }
}

bool GLDWindowComboBox::eventFilter(QObject *object, QEvent *event)
{
    if (!m_hidePopupOnEdit)
    {
        GLDPlainTextEdit *pPlainTxtEdit = lineEdit();
        GLDComboBoxPopup *pComboxPopup = dynamic_cast<GLDComboBoxPopup *>(object);

        if (pComboxPopup != NULL)
        {
            switch (event->type())
            {
                case QEvent::KeyPress:
                    if (pPlainTxtEdit->hasFocus() || pPlainTxtEdit->viewport()->hasFocus())
                    {
                        keyPressEvent((QKeyEvent *)event);
                        return true;
                    }

                    break;

                case QEvent::MouseButtonPress:
                {
                    const QMouseEvent *pPopupMouseEvt = static_cast<QMouseEvent *>(event);
                    QPoint pt = pPlainTxtEdit->mapFromGlobal(pPopupMouseEvt->pos());

                    if (pPlainTxtEdit->rect().contains(pt))
                    {
                        if (!pPlainTxtEdit->hasFocus())
                        {
                            pPlainTxtEdit->setFocus();
                            pPlainTxtEdit->cursor().setPos(pPopupMouseEvt->pos());
                        }

                        QScopedPointer<QMouseEvent> mouseEvt(new QMouseEvent(
                                pPopupMouseEvt->type(),
                                pt,
                                pPopupMouseEvt->button(),
                                pPopupMouseEvt->buttons(),
                                pPopupMouseEvt->modifiers()
                                                             ));

                        QApplication::sendEvent(pPlainTxtEdit->viewport(), mouseEvt.data());

                        event->accept();

                        return true;
                    }
                    else if (!m_pComboxPopup->rect().contains(m_pComboxPopup->mapFromGlobal(QCursor::pos())))
                    {
                        if (QWidget *pParent = static_cast<QWidget *>(parent()))
                        {
                            if (!pParent->rect().contains(m_pComboxPopup->mapFromGlobal(QCursor::pos())))
                            {
                                if (m_hideEditOnExit)
                                {
                                    emit onManualColsePopup();
                                }
                            }
                        }
                    }

                    break;
                }

                case QEvent::MouseButtonRelease:
                {
                    QPoint pt = pPlainTxtEdit->mapFromGlobal(QCursor::pos());

                    if (pPlainTxtEdit->rect().contains(pt))
                    {
                        QApplication::sendEvent(pPlainTxtEdit, (QMouseEvent *)event);
                        event->accept();
                        return true;
                    }

                    break;
                }

                case QEvent::MouseMove:
                {
                    QPoint pt = pPlainTxtEdit->mapFromGlobal(QCursor::pos());

                    if (pPlainTxtEdit->rect().contains(pt))
                    {
                        QApplication::sendEvent(pPlainTxtEdit, (QMouseEvent *)event);
                        event->accept();
                        return true;
                    }

                    break;
                }

                default:
                    break;
            }
        }
    }

    return GLDWAbstractSpinBox::eventFilter(object, event);
}

void GLDWindowComboBox::resizeEvent(QResizeEvent *event)
{
    updateEditFieldGeometry();
    Q_UNUSED(event);
}

void GLDWindowComboBox::moveEvent(QMoveEvent *)
{
    if (m_pComboxPopup != NULL)
    {
        m_pComboxPopup->hide();
    }
}

void GLDWindowComboBox::setPopupFixedWidth(int width)
{
    m_pComboxPopup->setFixedWidth(width);
}

void GLDWindowComboBox::cut()
{
    lineEdit()->cut();
}

void GLDWindowComboBox::copy()
{
    lineEdit()->copy();
}

void GLDWindowComboBox::paste()
{
    lineEdit()->paste();
}

void GLDComboBoxPopup::setpopupWidget(QWidget *cw)
{
    Q_ASSERT(cw);
    QVBoxLayout *widgetLayout = dynamic_cast<QVBoxLayout *>(layout());

    if (!widgetLayout)
    {
        widgetLayout = new QVBoxLayout(this);
        widgetLayout->setMargin(0);
        widgetLayout->setSpacing(0);
        widgetLayout->setContentsMargins(0, 0, 0, 0);
        widgetLayout->setDirection(QBoxLayout::LeftToRight);
    }

    delete m_popupWidget.data();
    m_popupWidget = QPointer<QWidget>(cw);

    widgetLayout->addWidget(cw);
    cw->setFocus();
    setSizeGripEnabled(true);
}

void GLDComboBoxPopup::setSizeGripEnabled(bool enabled)
{
    if (m_sizeGripEnabled == enabled)
    {
        return;
    }

    m_sizeGripEnabled = enabled;

    if (!enabled != !m_resizer)
    {
        if (enabled)
        {
            m_resizer = new QSizeGrip(this);
            m_resizer->resize(m_resizer->sizeHint());
            alignResizer(rect());
            m_resizer->raise();
            m_resizer->show();
        }
        else
        {
            delete m_resizer;
            m_resizer = 0;
        }
    }
}

void GLDComboBoxPopup::setPopupSize(QSize value)
{
    if (value == m_popupSize)
    {
        return;
    }

    m_popupSize = value;
}

void GLDComboBoxPopup::setPopupHeight(int value)
{
    if (value == m_popupSize.height())
    {
        return;
    }

    m_popupSize.setHeight(value);
}

int GLDComboBoxPopup::popuHeight()
{
    return m_popupSize.height();
}

QWidget *GLDComboBoxPopup::verifyPopupInstance()
{
    if (m_popupWidget.isNull())
    {
        return NULL;
    }
    else
    {
        return m_popupWidget.data();
    }
}

void GLDComboBoxPopup::setEdgeCursor()
{
    const int c_Hight = 3;
    const int c_Width = 3;

    int nlx = rect().left();
    int nrx = rect().right();
    int nly = rect().top();
    int nry = rect().bottom();

    const QSize c_defSize(c_Width, c_Hight);

    QRect bottomRight(QPoint(nrx - c_Width, nry - c_Hight), c_defSize);
    QRect bottomLeft(QPoint(nlx, nry - c_Hight), c_defSize);
    QRect topleft(QPoint(nlx, nly), c_defSize);
    QRect topright(QPoint(nrx - c_Width, nly), c_defSize);

    QRect top(nlx + c_Width, nly, rect().width() - 2 * c_Width, c_Hight);
    QRect bottom(nlx + c_Width, nry - c_Hight, rect().width() - 2 * c_Width, c_Hight);
    QRect left(nlx, nly + c_Hight, c_Width, rect().height() - 2 * c_Hight);
    QRect right(nrx - c_Width, nly + c_Hight, c_Width, rect().height() - 2 * c_Hight);
    QPoint pos = mapFromGlobal(cursor().pos());

    m_cursorChanged = true;

    if (bottomRight.contains(pos) || topleft.contains(pos))
    {
        setCursor(Qt::SizeFDiagCursor);
    }
    else if (bottomLeft.contains(pos) || topright.contains(pos))
    {
        setCursor(Qt::SizeBDiagCursor);
    }
    else if (top.contains(pos) || bottom.contains(pos))
    {
        setCursor(Qt::SizeVerCursor);
    }
    else if (left.contains(pos) || right.contains(pos))
    {
        setCursor(Qt::SizeHorCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        m_cursorChanged = false;
    }
}

void GLDComboBoxPopup::alignResizer(const QRect &rect)
{
    QPoint adjustEdge(0, 0);

    if (!QStyleFactory::keys().contains(qApp->style()->objectName(), Qt::CaseInsensitive))
    {
        adjustEdge = QPoint(2, 0);
    }

    if (m_isAtBottom)
    {
        if (isRightToLeft())
        {
            m_resizer->move(rect.bottomLeft() - m_resizer->rect().bottomLeft() - adjustEdge);
        }
        else
        {
            m_resizer->move(rect.bottomRight() - m_resizer->rect().bottomRight() - adjustEdge);
        }
    }
    else
    {
        if (isRightToLeft())
        {
            m_resizer->move(rect.topLeft() - m_resizer->rect().topLeft() - adjustEdge);
        }
        else
        {
            m_resizer->move(rect.topRight() - m_resizer->rect().topRight() - adjustEdge);
        }
    }
}

void GLDComboBoxPopup::initPopupSize()
{
    if (parent())
    {
        QSize size = parentWidget()->size();

        if (size.height() < 30)
        {
            size.setHeight(size.height() > 30 ? size.height() : 30);
        }

        m_popupSize = size;
    }
    else
    {
        m_popupSize = QWidget::sizeHint();
    }
}

GLDComboBoxPopup::GLDComboBoxPopup(QWidget *parent, QWidget *cw)
    : QDialog(parent, Qt::Popup),
      m_resizer(NULL),
      m_sizeGripEnabled(false),
      m_cursorChanged(false),
      m_canDrag(false),
      m_isAtBottom(true),
      m_scrollTime(150),
      m_animationPopup(true)
{
    setAttribute(Qt::WA_WindowPropagation);

    if (!cw)
    {
        verifyPopupInstance();
    }
    else
    {
        setpopupWidget(cw);
    }

    setFocusPolicy(Qt::ClickFocus);
    initPopupSize();
}

void GLDComboBoxPopup::hideEvent(QHideEvent *)
{
    emit onHide();
}

void GLDComboBoxPopup::mousePressEvent(QMouseEvent *e)
{
    m_canDrag = false;
    GLDWindowComboBox *combox = dynamic_cast<GLDWindowComboBox *>(parentWidget());

    if (combox)
    {
        QStyleOptionComboBox opt;
        opt.init(combox);
        QRect arrowRect = combox->style()->subControlRect(QStyle::CC_ComboBox, &opt,
                          QStyle::SC_ComboBoxArrow, combox);
        arrowRect.moveTo(combox->mapToGlobal(arrowRect.topLeft()));

        if (arrowRect.contains(e->globalPos()))
        {
            setAttribute(Qt::WA_NoMouseReplay);

            bool value = true;
            emit shouldShowPopup(value);
        }

        if (rect().contains(e->pos()) && m_cursorChanged)
        {
            m_canDrag = true;
            m_curPos = e->pos();
        }
    }

    QWidget::mousePressEvent(e);
}

void GLDComboBoxPopup::mouseReleaseEvent(QMouseEvent *)
{
    if (m_canDrag)
    {
        m_canDrag = false;
    }

    emit onHide();
}

void GLDComboBoxPopup::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    if (m_resizer && m_sizeGripEnabled)
    {
        alignResizer(rect());
    }
}

void GLDComboBoxPopup::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    QTimer::singleShot(10, this, SLOT(setEditorFocus()));
}

void GLDComboBoxPopup::setEditorFocus()
{
    GLDWindowComboBox *box = dynamic_cast<GLDWindowComboBox *>(parentWidget());

    if (box)
    {
        if (box->m_editorNeedFocus && !box->lineEdit()->hasFocus())
        {
            box->lineEdit()->setFocus();
            box->m_editorNeedFocus = false;
        }
    }
}

void GLDComboBoxPopup::show()
{
    positionFramePopup();

    if (m_animationPopup && QApplication::isEffectEnabled(Qt::UI_AnimateCombo)
            /*&& !style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, this)*/ &&
            !window()->testAttribute(Qt::WA_DontShowOnScreen))
    {
        qScrollEffect(this, true ? QEffects::DownScroll : QEffects::UpScroll, m_scrollTime);
    }

    QWidget::show();
}

int GLDComboBoxPopup::exec()
{
    positionFramePopup();
    return QDialog::exec();
}

bool GLDComboBoxPopup::event(QEvent *e)
{
    return QWidget::event(e);
}

bool GLDComboBoxPopup::eventFilter(QObject *object, QEvent *event)
{
    QWidget *widget = dynamic_cast<QWidget *>(object);

    if (widget == NULL)
    {
        return false;
    }

    if (event->type() == QEvent::MouseMove)
    {
        setEdgeCursor();
        return true;
    }
    else if (event->type() == QEvent::Leave)
    {
        if (underMouse())
        {
            unsetCursor();
            return true;
        }
    }

    return QWidget::eventFilter(object, event);
}

void GLDComboBoxPopup::positionFramePopup()
{
    QWidget *pParent = static_cast<QWidget *>(parent());
    setAtBottom(true);

    QPoint pos = (pParent->layoutDirection() == Qt::RightToLeft) ? pParent->rect().bottomRight()
                 : pParent->rect().bottomLeft();
    QPoint pos2 = (pParent->layoutDirection() == Qt::RightToLeft) ? pParent->rect().topRight()
                  : pParent->rect().topLeft();
    pos = pParent->mapToGlobal(pos);
    pos2 = pParent->mapToGlobal(pos2);

    QSize size(this->sizeHint().width(), this->sizeHint().height());
    QRect screen = QApplication::desktop()->availableGeometry(pos);

    //handle popup falling "off screen"
    if (layoutDirection() == Qt::RightToLeft)
    {
        pos.setX(pos.x() - size.width());
        pos2.setX(pos2.x() - size.width());

        if (pos.x() < screen.left())
        {
            pos.setX(qMax(pos.x(), screen.left()));
        }
        else if (pos.x() + size.width() > screen.right())
        {
            pos.setX(qMax(pos.x() - size.width(), screen.right() - size.width()));
        }
    }
    else
    {
        if (pos.x() + size.width() > screen.right())
        {
            pos.setX(screen.right() - size.width());
        }

        pos.setX(qMax(pos.x(), screen.left()));
    }

    if (pos.y() + size.height() > screen.bottom())
    {
        pos.setY(pos2.y() - size.height());
        setAtBottom(false);
    }
    else if (pos.y() < screen.top())
    {
        pos.setY(screen.top());
    }

    if (pos.y() < screen.top())
    {
        pos.setY(screen.top());
    }

    if (pos.y() + size.height() > screen.bottom())
    {
        pos.setY(screen.bottom() - size.height());
        setAtBottom(false);
    }

    move(pos);
}

QSize GLDComboBoxPopup::sizeHint() const
{
    return m_popupSize;
}

void GLDWindowComboBox::deleteSelectedText()
{
    if (hasSelectedText())
    {
        lineEdit()->deleteSelectedText();
    }
}
