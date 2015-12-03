#include "GLDWindowComboBoxEx.h"

#include <QStyleFactory>
#ifndef QT_NO_EFFECTS
#include <private/qeffects_p.h>
#endif
#include <QStyle>
#include <QTimer>
#include <QMenu>
#include "private/qabstractspinbox_p.h"

#include "GLDWidget_Global.h"
#include "GLDShadow.h"
#include "GLDTextEdit.h"
#include "GLDFileUtils.h"
#include "GLDScrollStyle.h"

const GString c_sWindowComboBoxQssFile = ":/qsses/GLDWindowComboBoxEx.qss";

GLDWindowComboBoxEx::GLDWindowComboBoxEx(QWidget *parent)
    : GLDWAbstractSpinBox(parent), m_subIconRect(QRect())
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
    m_hasBorder = true;
    m_isKeepFouseInLineEdit = false;

#ifdef QT_KEYPAD_NAVIGATION
    focusOnButton = false;
#endif
    init();

    connect(getLineEdit(), SIGNAL(cursorPositionChanged()),
            this, SLOT(doCursorPositionChanged()));
    connect(getLineEdit(), SIGNAL(selectionChanged()),
            this, SLOT(doSelectionChanged()));
}

GLDWindowComboBoxEx::~GLDWindowComboBoxEx()
{
    freeAndNil(m_pComboxPopup);
}

QWidget *GLDWindowComboBoxEx::popupWidget() const
{
    if (!m_framePopuped || m_pComboxPopup == NULL)
    {
        return NULL;
    }
    return m_pComboxPopup->popupWidget();
}

void GLDWindowComboBoxEx::setPopupWidget(QWidget *popup)
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

bool GLDWindowComboBoxEx::framePopup() const
{
    return m_framePopuped;
}

void GLDWindowComboBoxEx::setFramePopup(bool enable)
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

void GLDWindowComboBoxEx::setPopupScrollTime(int time)
{
    if (m_pComboxPopup != NULL)
    {
        m_pComboxPopup->setScrollTime(time);
    }
}

int GLDWindowComboBoxEx::popupScrollTime()
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

void GLDWindowComboBoxEx::setResizeEnable(bool value)
{
    if (m_pComboxPopup != NULL)
    {
        m_pComboxPopup->setSizeGripEnabled(value);
    }
}

void GLDWindowComboBoxEx::setPopupSize(QSize value)
{
    if (m_pComboxPopup != NULL)
    {
        m_pComboxPopup->setPopupSize(value);
    }
}

void GLDWindowComboBoxEx::hidePopup()
{
    if (m_pComboxPopup->isVisible())
    {
        m_pComboxPopup->hide();
    }
}

void GLDWindowComboBoxEx::setButtonTypes(GLDWindowComboBoxEx::ButtonTypes type)
{
    if (type == None)
    {
        setButtonSymbols(QAbstractSpinBox::NoButtons);
    }

    m_buttonType = type;

    setEditProperty(false);
    this->setStyleSheet(loadQssFile(c_sWindowComboBoxQssFile));
}

GLDWindowComboBoxEx::ButtonTypes GLDWindowComboBoxEx::buttonType()
{
    return m_buttonType;
}

bool GLDWindowComboBoxEx::hasSelectedText()
{
    return lineEdit()->hasSelectedText();
}

void GLDWindowComboBoxEx::setComboBoxPopupOffset(const int xOffset, const int yOffset)
{
    if (comboBoxPopup())
    {
        comboBoxPopup()->setOffset(xOffset, yOffset);
    }
}

void GLDWindowComboBoxEx::showPopup()
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

void GLDWindowComboBoxEx::closePopup()
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

bool GLDWindowComboBoxEx::isEditable() const
{
    return !lineEdit()->isReadOnly();
}

void GLDWindowComboBoxEx::setEditable(bool editable)
{
    lineEdit()->setReadOnly(!editable);
}

QString GLDWindowComboBoxEx::editText() const
{
    QString strPlainText = lineEdit()->toPlainText();
#ifdef WIN32
    strPlainText.replace("\n", "\r\n");
#endif
    return strPlainText;
}

void GLDWindowComboBoxEx::setEditText(const QString &text)
{
    lineEdit()->setPlainText(text);
}

void GLDWindowComboBoxEx::init()
{
#ifdef QT_KEYPAD_NAVIGATION

    if (QApplication::keypadNavigationEnabled())
    {
        setFramePopup(true);
    }

#endif
    setInputMethodHints(Qt::ImhPreferNumbers);
    setFramePopup(true);

    setStyle(new GLDScrollStyle(this));
    setHasBorder(true);

    setObjectName("GLDWindowComboBoxEx");
    setEditProperty(false);
    this->setStyleSheet(loadQssFile(c_sWindowComboBoxQssFile));

    // 移除阴影
    GLDShadow *pShadow = new GLDShadow(this);
    pShadow->removeShadow();
}

QStyle::SubControl GLDWindowComboBoxEx::newHoverControl(const QPoint &pos)
{
    QStyleOptionComboBox optCombo;
    optCombo.init(this);
    optCombo.editable = true;
    optCombo.subControls = QStyle::SC_All;
    return style()->hitTestComplexControl(QStyle::CC_ComboBox, &optCombo, pos, this);
}

void GLDWindowComboBoxEx::updateEditFieldGeometry()
{
    QStyleOptionComboBox optCombo;
    optCombo.init(this);
    optCombo.editable = true;
    optCombo.subControls = QStyle::SC_ComboBoxFrame | QStyle::SC_ComboBoxEditField;

    if (m_buttonType != None)
    {
        optCombo.subControls |= QStyle::SC_ComboBoxArrow;
        // Qss需要
        QRect editRect = rect().adjusted(0, 0, 0, 0);
        // 由于在setGeometry的时候，会再次计算并设置MinimumSize和MaximumSize
        // 会出现区域改变的问题，因此在这里先设置好MinimumSize和MaximumSize
        lineEdit()->setMinimumSize(editRect.width(), editRect.height());
        lineEdit()->setMaximumSize(editRect.width(), editRect.height());
        lineEdit()->setGeometry(editRect);
    }
    else
    {
        QRect editRect = rect().adjusted(0, 0, 0, -1);
        lineEdit()->setMinimumSize(editRect.width(), editRect.height());
        lineEdit()->setMaximumSize(editRect.width(), editRect.height());
    }
}

void GLDWindowComboBoxEx::doPopupHide()
{
    updateArrow(QStyle::State_None);    
    emit onHide();
    // 添加状态，判断是它没有点击下拉框，将背景图片切换为向上的箭头
    setEditProperty(false);
    this->setStyleSheet(loadQssFile(c_sWindowComboBoxQssFile));
    // 加载Qss后，会出现Edit错位问题，因此重新计算位置
    updateEditFieldGeometry();
}

void GLDWindowComboBoxEx::doSelectionChanged()
{
    emit selectionChanged();
}

void GLDWindowComboBoxEx::doCursorPositionChanged()
{
    emit cursorPositionChanged();
}

void GLDWindowComboBoxEx::updateArrow(QStyle::StateFlag state)
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

void GLDWindowComboBoxEx::syncPopupWidget()
{
    if (m_pComboxPopup != NULL)
    {
        const bool c_sb = m_pComboxPopup->blockSignals(true);
        m_pComboxPopup->blockSignals(c_sb);
        m_pComboxPopup->setAnimationPopup(m_animationPopup);
    }
}

void GLDWindowComboBoxEx::initComboBoxPopup(QWidget *cw)
{
    if (m_pComboxPopup == NULL)
    {
        m_pComboxPopup = new GLDComboBoxPopupEx(this, cw);
        m_pComboxPopup->setObjectName(QLatin1String("GLDComboBoxPopupEx"));

        QObject::connect(m_pComboxPopup, SIGNAL(onHide()), this, SLOT(doPopupHide()));
        QObject::connect(m_pComboxPopup, SIGNAL(shouldShowPopup(bool &)), this, SIGNAL(shouldShowPopup(bool &)));

        m_pComboxPopup->installEventFilter(this);
    }
    else if (cw != NULL)
    {
        m_pComboxPopup->setpopupWidget(cw);
    }

    syncPopupWidget();
}

void GLDWindowComboBoxEx::keyPressEvent(QKeyEvent *event)
{
    bool bhandled = false;

    emit keyPressType(Qt::Key(event->key()), bhandled);

    if (bhandled)
    {
        return;
    }

    bool bSelect = true;

    QWidget *popWidgetChild = NULL;

    if (m_isKeepFouseInLineEdit)
    {
        for (int i = 0; i < popupWidget()->children().count(); ++i)
        {
            if ((popWidgetChild = qobject_cast<QWidget*>(popupWidget()->children().at(i))))
            {
                break;
            }
        }
    }
    switch (event->key())
    {
        if(m_isKeepFouseInLineEdit)
        {
            case Qt::Key_Up:
            case Qt::Key_Down:
            {
                if(popWidgetChild)
                {
                    popWidgetChild->setFocus();
                }
           }
           break;
        }
        case Qt::Key_Enter:
        case Qt::Key_Return:
//        d->interpret(AlwaysEmit);
            event->ignore();
            emit editingFinished();
            break;

        default:
             if (m_isKeepFouseInLineEdit)
             {
                 getLineEdit()->setFocus();
                 break;
             }
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

void GLDWindowComboBoxEx::focusInEvent(QFocusEvent *event)
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

void GLDWindowComboBoxEx::mousePressEvent(QMouseEvent *event)
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
                // 添加状态，判断是否点击了，重新载入QSS
                setEditProperty(true);
                this->setStyleSheet(loadQssFile(c_sWindowComboBoxQssFile));
                // 加载Qss后，会出现Edit错位问题，因此重新计算位置
                updateEditFieldGeometry();
                showPopup();
            }
        }
    }
    else
    {
        GLDWAbstractSpinBox::mousePressEvent(event);
    }
}

void GLDWindowComboBoxEx::paintEvent(QPaintEvent *event)
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
    m_subIconRect = subRect;
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
        if (m_isKeepFouseInLineEdit)
        {
           getLineEdit()->setFocus();
        }
    }
}

void GLDWindowComboBoxEx::contextMenuEvent(QContextMenuEvent *event)
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
                         ? event->globalPos() : mapToGlobal(QPoint(event->pos().x(), 0)) + QPoint(width() / 2, 
                                                                                                  height() / 2);
    menu->exec(c_pos);
    delete static_cast<QMenu *>(menu);
    event->accept();
#endif // QT_NO_CONTEXTMENU
}

void GLDWindowComboBoxEx::initStyleOption(QStyleOptionSpinBox *option) const
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

bool GLDWindowComboBoxEx::eventFilter(QObject *object, QEvent *event)
{
    if (!m_hidePopupOnEdit)
    {
        GLDPlainTextEdit *pPlainTxtEdit = lineEdit();
        GLDComboBoxPopupEx *pComboxPopup = dynamic_cast<GLDComboBoxPopupEx *>(object);

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
                    const QMouseEvent *pPopupMouseEvt = static_cast<QMouseEvent*>(event);
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

void GLDWindowComboBoxEx::resizeEvent(QResizeEvent *event)
{
    updateEditFieldGeometry();
    Q_UNUSED(event);
}

void GLDWindowComboBoxEx::moveEvent(QMoveEvent *)
{
    if (m_pComboxPopup != NULL)
    {
        m_pComboxPopup->hide();
    }
}

void GLDWindowComboBoxEx::setPopupFixedWidth(int width)
{
    m_pComboxPopup->setFixedWidth(width);
}

void GLDWindowComboBoxEx::cut()
{
    lineEdit()->cut();
}

void GLDWindowComboBoxEx::copy()
{
    lineEdit()->copy();
}

void GLDWindowComboBoxEx::paste()
{
    lineEdit()->paste();
}

void GLDComboBoxPopupEx::setpopupWidget(QWidget *cw)
{
    Q_ASSERT(cw);
    QVBoxLayout *widgetLayout = dynamic_cast<QVBoxLayout *>(layout());

    if (!widgetLayout)
    {        
        widgetLayout = new QVBoxLayout(this);
        widgetLayout->setMargin(0);
        widgetLayout->setSpacing(0);
        // 下面的拖拽的位置所占区域
        widgetLayout->setContentsMargins(0, 0, 0, 18);
        widgetLayout->setDirection(QBoxLayout::LeftToRight);
    }

    delete m_popupWidget.data();
    m_popupWidget = QPointer<QWidget>(cw);
    widgetLayout->addWidget(cw);
    cw->setFocus();
    setSizeGripEnabled(true);
}

void GLDComboBoxPopupEx::setSizeGripEnabled(bool enabled)
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
            layout()->setContentsMargins(0, 0, 0, 18);
        }
        else
        {
            delete m_resizer;
            m_resizer = 0;
            layout()->setContentsMargins(0, 0, 0, 0);
        }
    }
}

void GLDComboBoxPopupEx::setPopupSize(QSize value)
{
    if (value == m_popupSize)
    {
        return;
    }

    m_popupSize = value;
}

QWidget *GLDComboBoxPopupEx::verifyPopupInstance()
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

void GLDComboBoxPopupEx::setEdgeCursor()
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

void GLDComboBoxPopupEx::alignResizer(const QRect &rect)
{
    if (m_isAtBottom)
    {
        if (isRightToLeft())
        {
            m_resizer->move(rect.bottomLeft() - m_resizer->rect().bottomLeft());
        }
        else
        {
            m_resizer->move(rect.bottomRight() - m_resizer->rect().bottomRight());
        }
    }
    else
    {
        if (isRightToLeft())
        {
            m_resizer->move(rect.topLeft() - m_resizer->rect().topLeft());
        }
        else
        {
            m_resizer->move(rect.topRight() - m_resizer->rect().topRight());
        }
    }
}

void GLDComboBoxPopupEx::initPopupSize()
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

GLDComboBoxPopupEx::GLDComboBoxPopupEx(QWidget *parent, QWidget *cw)
    : QDialog(parent, Qt::Popup), m_resizer(NULL), m_sizeGripEnabled(false),
      m_cursorChanged(false), m_canDrag(false), m_isAtBottom(true), m_scrollTime(150), m_animationPopup(true),
      m_xOffset(0), m_yOffset(0)
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

void GLDComboBoxPopupEx::hideEvent(QHideEvent *)
{
    emit onHide();
}

void GLDComboBoxPopupEx::mousePressEvent(QMouseEvent *e)
{
    m_canDrag = false;
    GLDWindowComboBoxEx *combox = dynamic_cast<GLDWindowComboBoxEx *>(parentWidget());

    if (!combox)
    {
        QDialog::mousePressEvent(e);
        return;
    }

    else
    {
        QStyleOptionComboBox opt;
        opt.init(combox);
        QRect arrowRect = combox->style()->subControlRect(QStyle::CC_ComboBox, &opt,
                          QStyle::SC_ComboBoxArrow, combox);
        arrowRect.moveTo(combox->mapToGlobal(arrowRect.topLeft()));

        if (arrowRect.contains(e->globalPos()))
        {
            setAttribute(Qt::WA_NoMouseReplay);

            bool bValue = true;
            emit shouldShowPopup(bValue);
        }

        if (rect().contains(e->pos()) && m_cursorChanged)
        {
            m_canDrag = true;
            m_curPos = e->pos();
        }
    }

    if (!combox->m_hidePopupOnEdit)
    {
        e->ignore();
        if ((windowType() == Qt::Popup))
        {
            e->accept();

            // 关闭所有不在当前Widget里面的PopupWidget
            QWidget* pPopupWidget;
            while ((pPopupWidget = QApplication::activePopupWidget()) && pPopupWidget != this)
            {
                pPopupWidget->close();
                if (QApplication::activePopupWidget() == pPopupWidget) // widget does not want to disappear
                {
                    pPopupWidget->hide(); // hide at least
                }
            }

            QRect rectPopUp(rect().left(), rect().top(), rect().right(), rect().bottom());
            QRect rectLineEdit = combox->lineEdit()->rect();
            rectLineEdit.adjust(0, -rectLineEdit.bottom() , -combox->subIconRect().width(), -rectLineEdit.bottom());
            if (!rectPopUp.contains(e->pos()))
            {
                if (!rectLineEdit.contains(e->pos()))
                {
                    close();
                }
                else
                {
                    combox->lineEdit()->setFocus();
                }
            }
        }
    }
    else
    {
        QWidget::mousePressEvent(e);
    }
}

void GLDComboBoxPopupEx::mouseReleaseEvent(QMouseEvent *)
{
    if (m_canDrag)
    {
        m_canDrag = false;
    }

    emit onHide();
}

void GLDComboBoxPopupEx::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    if (m_resizer && m_sizeGripEnabled)
    {
        alignResizer(rect());
    }
}

void GLDComboBoxPopupEx::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    QTimer::singleShot(10, this, SLOT(setEditorFocus()));
}

void GLDComboBoxPopupEx::setEditorFocus()
{
    GLDWindowComboBoxEx *box = dynamic_cast<GLDWindowComboBoxEx *>(parentWidget());

    if (box)
    {
        if (box->m_editorNeedFocus && !box->lineEdit()->hasFocus())
        {
            box->lineEdit()->setFocus();
            box->m_editorNeedFocus = false;
        }
    }
}

void GLDComboBoxPopupEx::show()
{
    positionFramePopup();

    if (m_animationPopup && QApplication::isEffectEnabled(Qt::UI_AnimateCombo)
            /*&& !style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, this)*/ 
            && !window()->testAttribute(Qt::WA_DontShowOnScreen))
    {
        qScrollEffect(this, true ? QEffects::DownScroll : QEffects::UpScroll, m_scrollTime);
    }

    QWidget::show();
}

int GLDComboBoxPopupEx::exec()
{
    positionFramePopup();
    return QDialog::exec();
}

bool GLDComboBoxPopupEx::event(QEvent *e)
{
    return QWidget::event(e);
}

bool GLDComboBoxPopupEx::eventFilter(QObject *object, QEvent *event)
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

void GLDComboBoxPopupEx::positionFramePopup()
{
    QWidget *pParent = static_cast<QWidget *>(parent());
    setAtBottom(true);
    QPoint pos = (pParent->layoutDirection() == Qt::RightToLeft) ? pParent->rect().bottomRight()
                 : pParent->rect().bottomLeft();
    QPoint pos2 = (pParent->layoutDirection() == Qt::RightToLeft) ? pParent->rect().topRight()
                  : pParent->rect().topLeft();
    pos = pParent->mapToGlobal(pos);
    pos2 = pParent->mapToGlobal(pos2);
    QSize size = sizeHint();
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
    pos.setX(pos.x() + m_xOffset);
    pos.setY(pos.y() + m_yOffset);
    move(pos);
}

QSize GLDComboBoxPopupEx::sizeHint() const
{
    return m_popupSize;
}

void GLDComboBoxPopupEx::setOffset(int xOffset, int yOffset)
{
    m_xOffset = xOffset;
    m_yOffset = yOffset;
}

void GLDWindowComboBoxEx::deleteSelectedText()
{
    if (hasSelectedText())
    {
        lineEdit()->deleteSelectedText();
    }
}

void GLDWindowComboBoxEx::setEditProperty(bool bShow)
{
    QString sProperty = QString("");
    if (m_buttonType == ComboBoxArrow)
    {
        sProperty = sProperty + QString("ComboBoxArrow");
    }
    else if (m_buttonType == Ellipsis)
    {
        sProperty = sProperty + QString("Ellipsis");
    }
    if (bShow)
    {
        sProperty = sProperty + QString("-show");
    }
    else
    {
        sProperty = sProperty + QString("-hide");
    }

    getLineEdit()->setProperty(
                "GLDPlainTextEditState", sProperty);
}

bool GLDWindowComboBoxEx::isHidePopupOnEdit()
{
    return m_hidePopupOnEdit;
}

void GLDWindowComboBoxEx::setIsHidePopupOnEdit(bool bIsHidePopupOnEdit)
{
    m_hidePopupOnEdit = bIsHidePopupOnEdit;
}

void GLDWindowComboBoxEx::setFouseKeepInLineEdit(bool isKeepFouseInLineEdit)
{
    getLineEdit()->setFouseIsKeepInLineEdit(true);
    m_isKeepFouseInLineEdit = isKeepFouseInLineEdit;
}

void GLDWindowComboBoxEx::setHasBorder(bool hasBorder)
{
    if (m_hasBorder != hasBorder)
    {
        m_hasBorder = hasBorder;
    }

    if (!m_hasBorder)
    {
        getLineEdit()->setProperty("borderStyle", "noBorder");
        this->setStyleSheet(loadQssFile(c_sWindowComboBoxQssFile));
    }
}
