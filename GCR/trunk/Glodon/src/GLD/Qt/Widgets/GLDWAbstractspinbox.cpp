#include <qplatformdefs.h>
#include <GLDWAbstractspinbox_p.h>
#include <private/qdatetime_p.h>
#include <private/qlineedit_p.h>
#include "GLDWAbstractspinbox.h"
#include "GLDTextEdit.h"

#ifndef QT_NO_SPINBOX

#include <qapplication.h>
#include <qstylehints.h>
#include <qclipboard.h>
#include <qdatetime.h>
#include <qdatetimeedit.h>
#include <qevent.h>
#include <qmenu.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qstylepainter.h>
#ifdef GLDWAbstractSpinBox_QSBDEBUG
#include <qdebug.h>
#endif
#ifndef QT_NO_ACCESSIBILITY
# include <qaccessible.h>
#endif


//#define GLDWAbstractSpinBox_QSBDEBUG
#ifdef GLDWAbstractSpinBox_QSBDEBUG
#  define QASBDEBUG qDebug
#else
#  define QASBDEBUG if (false) qDebug
#endif


#if (QT_VERSION > QT_VERSION_CHECK(5, 1, 1))
#define QDATETIMEEDIT_TIME_MIN QTime(0, 0, 0, 0)
#define QDATETIMEEDIT_TIME_MAX QTime(23, 59, 59, 999)
#define QDATETIMEEDIT_DATE_MIN QDate(100, 1, 1)
#define QDATETIMEEDIT_COMPAT_DATE_MIN QDate(1752, 9, 14)
#define QDATETIMEEDIT_DATE_MAX QDate(7999, 12, 31)
#define QDATETIMEEDIT_DATETIME_MIN QDateTime(QDATETIMEEDIT_DATE_MIN, QDATETIMEEDIT_TIME_MIN)
#define QDATETIMEEDIT_COMPAT_DATETIME_MIN QDateTime(QDATETIMEEDIT_COMPAT_DATE_MIN, QDATETIMEEDIT_TIME_MIN)
#define QDATETIMEEDIT_DATETIME_MAX QDateTime(QDATETIMEEDIT_DATE_MAX, QDATETIMEEDIT_TIME_MAX)
#define QDATETIMEEDIT_DATE_INITIAL QDate(2000, 1, 1)
#endif

const int c_FixedPlaintEditText = 25;

GLDWAbstractSpinBox::GLDWAbstractSpinBox(QWidget *parent)
    : QWidget(*new GLDWAbstractSpinBoxPrivate, parent, 0)
{
    Q_D(GLDWAbstractSpinBox);
    d->init();
    setObjectName("GLDWAbstractSpinBox");
}

/*!
    \internal
*/
GLDWAbstractSpinBox::GLDWAbstractSpinBox(GLDWAbstractSpinBoxPrivate &dd, QWidget *parent)
    : QWidget(dd, parent, 0)
{
    Q_D(GLDWAbstractSpinBox);
    d->init();
}

/*!
    Called when the GLDWAbstractSpinBox is destroyed.
*/

GLDWAbstractSpinBox::~GLDWAbstractSpinBox()
{
}

/*!
    \enum GLDWAbstractSpinBox::ButtonSymbols

    This enum type describes the symbols that can be displayed on the buttons
    in a spin box.

    \inlineimage qspinbox-updown.png
    \inlineimage qspinbox-plusminus.png

    \value UpDownArrows Little arrows in the classic style.
    \value PlusMinus \b{+} and \b{-} symbols.
    \value NoButtons Don't display buttons.

    \sa GLDWAbstractSpinBox::buttonSymbols
*/

/*!
    \property GLDWAbstractSpinBox::buttonSymbols

    \brief the current button symbol mode

    The possible values can be either \c UpDownArrows or \c PlusMinus.
    The default is \c UpDownArrows.

    Note that some styles might render PlusMinus and UpDownArrows
    identically.

    \sa ButtonSymbols
*/

QAbstractSpinBox::ButtonSymbols GLDWAbstractSpinBox::buttonSymbols() const
{
    Q_D(const GLDWAbstractSpinBox);
    return d->buttonSymbols;
}

void GLDWAbstractSpinBox::setButtonSymbols(QAbstractSpinBox::ButtonSymbols buttonSymbols)
{
    Q_D(GLDWAbstractSpinBox);

    if (d->buttonSymbols != buttonSymbols)
    {
        d->buttonSymbols = buttonSymbols;
        d->updateEditFieldGeometry();
        update();
    }
}

/*!
    \property GLDWAbstractSpinBox::text

    \brief the spin box's text, including any prefix and suffix

    There is no default text.
*/

QString GLDWAbstractSpinBox::text() const
{
    return lineEdit()->toPlainText();
}

/*!
    \property GLDWAbstractSpinBox::specialValueText
    \brief the special-value text

    If set, the spin box will display this text instead of a numeric
    value whenever the current value is equal to minimum(). Typical use
    is to indicate that this choice has a special (default) meaning.

    For example, if your spin box allows the user to choose a scale factor
    (or zoom level) for displaying an image, and your application is able
    to automatically choose one that will enable the image to fit completely
    within the display window, you can set up the spin box like this:

    \snippet widgets/spinboxes/window.cpp 3

    The user will then be able to choose a scale from 1% to 1000%
    or select "Auto" to leave it up to the application to choose. Your code
    must then interpret the spin box value of 0 as a request from the user
    to scale the image to fit inside the window.

    All values are displayed with the prefix and suffix (if set), \e
    except for the special value, which only shows the special value
    text. This special text is passed in the QSpinBox::valueChanged()
    signal that passes a QString.

    To turn off the special-value text display, call this function
    with an empty string. The default is no special-value text, i.e.
    the numeric value is shown as usual.

    If no special-value text is set, specialValueText() returns an
    empty string.
*/

QString GLDWAbstractSpinBox::specialValueText() const
{
    Q_D(const GLDWAbstractSpinBox);
    return d->specialValueText;
}

void GLDWAbstractSpinBox::setSpecialValueText(const QString &specialValueText)
{
    Q_D(GLDWAbstractSpinBox);

    d->specialValueText = specialValueText;
    d->cachedSizeHint = QSize(); // minimumSizeHint doesn't care about specialValueText
    d->clearCache();
    d->updateEdit();
}

/*!
    \property GLDWAbstractSpinBox::wrapping

    \brief whether the spin box is circular.

    If wrapping is true stepping up from maximum() value will take you
    to the minimum() value and vica versa. Wrapping only make sense if
    you have minimum() and maximum() values set.

    \snippet code/src_gui_widgets_GLDWAbstractSpinBox.cpp 0

    \sa QSpinBox::minimum(), QSpinBox::maximum()
*/

bool GLDWAbstractSpinBox::wrapping() const
{
    Q_D(const GLDWAbstractSpinBox);
    return 0 != d->wrapping;
}

void GLDWAbstractSpinBox::setWrapping(bool wrapping)
{
    Q_D(GLDWAbstractSpinBox);
    d->wrapping = wrapping;
}

/*!
    \property GLDWAbstractSpinBox::readOnly
    \brief whether the spin box is read only.

    In read-only mode, the user can still copy the text to the
    clipboard, or drag and drop the text;
    but cannot edit it.

    The QLineEdit in the GLDWAbstractSpinBox does not show a cursor in
    read-only mode.

    \sa QLineEdit::readOnly
*/

bool GLDWAbstractSpinBox::isReadOnly() const
{
    Q_D(const GLDWAbstractSpinBox);
    return 0 != d->readOnly;
}

void GLDWAbstractSpinBox::setReadOnly(bool enable)
{
    Q_D(GLDWAbstractSpinBox);
    d->readOnly = enable;
    d->edit->setReadOnly(enable);
    update();
}

/*!
    \property GLDWAbstractSpinBox::keyboardTracking
    \brief whether keyboard tracking is enabled for the spinbox.
    \since 4.3

    If keyboard tracking is enabled (the default), the spinbox
    emits the valueChanged() signal while the new value is being
    entered from the keyboard.

    E.g. when the user enters the value 600 by typing 6, 0, and 0,
    the spinbox emits 3 signals with the values 6, 60, and 600
    respectively.

    If keyboard tracking is disabled, the spinbox doesn't emit the
    valueChanged() signal while typing. It emits the signal later,
    when the return key is pressed, when keyboard focus is lost, or
    when other spinbox functionality is used, e.g. pressing an arrow
    key.
*/

bool GLDWAbstractSpinBox::keyboardTracking() const
{
    Q_D(const GLDWAbstractSpinBox);
    return 0 != d->keyboardTracking;
}

void GLDWAbstractSpinBox::setKeyboardTracking(bool enable)
{
    Q_D(GLDWAbstractSpinBox);
    d->keyboardTracking = enable;
}

/*!
    \property GLDWAbstractSpinBox::frame
    \brief whether the spin box draws itself with a frame

    If enabled (the default) the spin box draws itself inside a frame,
    otherwise the spin box draws itself without any frame.
*/

bool GLDWAbstractSpinBox::hasFrame() const
{
    Q_D(const GLDWAbstractSpinBox);
    return 0 != d->frame;
}

void GLDWAbstractSpinBox::setFrame(bool enable)
{
    Q_D(GLDWAbstractSpinBox);
    d->frame = enable;
    update();
    d->updateEditFieldGeometry();
}

/*!
    \property GLDWAbstractSpinBox::accelerated
    \brief whether the spin box will accelerate the frequency of the steps when
    pressing the step Up/Down buttons.
    \since 4.2

    If enabled the spin box will increase/decrease the value faster
    the longer you hold the button down.
*/

void GLDWAbstractSpinBox::setAccelerated(bool accelerate)
{
    Q_D(GLDWAbstractSpinBox);
    d->accelerate = accelerate;

}

bool GLDWAbstractSpinBox::isAccelerated() const
{
    Q_D(const GLDWAbstractSpinBox);
    return 0 != d->accelerate;
}

/*!
    \enum GLDWAbstractSpinBox::CorrectionMode

    This enum type describes the mode the spinbox will use to correct
    an \l{QValidator::}{Intermediate} value if editing finishes.

    \value CorrectToPreviousValue The spinbox will revert to the last
                                  valid value.

    \value CorrectToNearestValue The spinbox will revert to the nearest
                                 valid value.

    \sa correctionMode
*/

/*!
    \property GLDWAbstractSpinBox::correctionMode
    \brief the mode to correct an \l{QValidator::}{Intermediate}
           value if editing finishes
    \since 4.2

    The default mode is GLDWAbstractSpinBox::CorrectToPreviousValue.

    \sa acceptableInput, validate(), fixup()
*/
void GLDWAbstractSpinBox::setCorrectionMode(QAbstractSpinBox::CorrectionMode correctionMode)
{
    Q_D(GLDWAbstractSpinBox);
    d->correctionMode = correctionMode;

}

QAbstractSpinBox::CorrectionMode GLDWAbstractSpinBox::correctionMode() const
{
    Q_D(const GLDWAbstractSpinBox);
    return d->correctionMode;
}

/*!
  \property GLDWAbstractSpinBox::acceptableInput
  \brief whether the input satisfies the current validation
  \since 4.2

  \sa validate(), fixup(), correctionMode
*/

bool GLDWAbstractSpinBox::hasAcceptableInput() const
{
//    Q_D(const GLDWAbstractSpinBox);
    return true;
//    return d->edit->hasAcceptableInput();
}

/*!
    \property GLDWAbstractSpinBox::alignment
    \brief the alignment of the spin box

    Possible Values are Qt::AlignLeft, Qt::AlignRight, and Qt::AlignHCenter.

    By default, the alignment is Qt::AlignLeft

    Attempting to set the alignment to an illegal flag combination
    does nothing.

    \sa Qt::Alignment
*/

Qt::Alignment GLDWAbstractSpinBox::alignment() const
{
//    Q_D(const GLDWAbstractSpinBox);
    return Qt::AlignLeft;
//    return (Qt::Alignment)d->edit->alignment();
}

void GLDWAbstractSpinBox::setAlignment(Qt::Alignment flag)
{
//    Q_D(GLDWAbstractSpinBox);
    Q_UNUSED(flag)
//    d->edit->setAlignment(flag);
}

/*!
    Selects all the text in the spinbox except the prefix and suffix.
*/

void GLDWAbstractSpinBox::selectAll()
{
    Q_D(GLDWAbstractSpinBox);

    d->edit->selectAll();
}

/*!
    Clears the lineedit of all text but prefix and suffix.
*/

void GLDWAbstractSpinBox::clear()
{
    Q_D(GLDWAbstractSpinBox);

    d->edit->setPlainText(d->prefix + d->suffix);
//    d->edit->setCursorPosition(d->prefix.size());
    d->cleared = true;
}

GLDPlainTextEdit *GLDWAbstractSpinBox::lineEdit() const
{
    Q_D(const GLDWAbstractSpinBox);

    return d->edit;
}

void GLDWAbstractSpinBox::setLineEdit(GLDPlainTextEdit *lineEdit)
{
    Q_D(GLDWAbstractSpinBox);

    if (!lineEdit)
    {
        Q_ASSERT(lineEdit);
        return;
    }

    delete d->edit;
    d->edit = lineEdit;
//    if (!d->edit->validator())
//        d->edit->setValidator(d->validator);

    if (d->edit->parent() != this)
    {
        d->edit->setParent(this);
    }

//    d->edit->setFrame(false);
    d->edit->setFocusProxy(this);
    d->edit->setAcceptDrops(false);

    if (d->type != QVariant::Invalid)
    {
        connect(d->edit, SIGNAL(textChanged(QString)),
                this, SLOT(_q_editorTextChanged(QString)));
        connect(d->edit, SIGNAL(cursorPositionChanged(int, int)),
                this, SLOT(_q_editorCursorPositionChanged(int, int)));
    }

    d->updateEditFieldGeometry();
    d->edit->setContextMenuPolicy(Qt::NoContextMenu);

    if (isVisible())
    {
        d->edit->show();
    }

    if (isVisible())
    {
        d->updateEdit();
    }
}

/*!
    This function interprets the text of the spin box. If the value
    has changed since last interpretation it will emit signals.
*/

void GLDWAbstractSpinBox::interpretText()
{
    Q_D(GLDWAbstractSpinBox);
    d->interpret(EmitIfChanged);
}

/*
    Reimplemented in 4.6, so be careful.
 */
/*!
    \reimp
*/
QVariant GLDWAbstractSpinBox::inputMethodQuery(Qt::InputMethodQuery query) const
{
    Q_D(const GLDWAbstractSpinBox);
    return d->edit->inputMethodQuery(query);
}

/*!
    \reimp
*/

bool GLDWAbstractSpinBox::event(QEvent *event)
{
    Q_D(GLDWAbstractSpinBox);

    switch (event->type())
    {
        case QEvent::FontChange:
        case QEvent::StyleChange:
            d->cachedSizeHint = d->cachedMinimumSizeHint = QSize();
            break;

        case QEvent::ApplicationLayoutDirectionChange:
        case QEvent::LayoutDirectionChange:
            d->updateEditFieldGeometry();
            break;

        case QEvent::HoverEnter:
        case QEvent::HoverLeave:
        case QEvent::HoverMove:
            if (const QHoverEvent *he = static_cast<const QHoverEvent *>(event))
            {
                d->updateHoverControl(he->pos());
            }

            break;

        case QEvent::ShortcutOverride:
            if (d->edit->event(event))
            {
                return true;
            }

            break;
#ifdef QT_KEYPAD_NAVIGATION

        case QEvent::EnterEditFocus:
        case QEvent::LeaveEditFocus:
            if (QApplication::keypadNavigationEnabled())
            {
                const bool b = d->edit->event(event);
                d->edit->setSelection(d->edit->displayText().size() - d->suffix.size(), 0);

                if (event->type() == QEvent::LeaveEditFocus)
                {
                    emit editingFinished();
                }

                if (b)
                {
                    return true;
                }
            }

            break;
#endif

        case QEvent::InputMethod:
            return d->edit->event(event);

        default:
            break;
    }

    return QWidget::event(event);
}

/*!
    \reimp
*/

void GLDWAbstractSpinBox::showEvent(QShowEvent *)
{
    Q_D(GLDWAbstractSpinBox);
    d->reset();

    if (0 != d->ignoreUpdateEdit)
    {
        d->ignoreUpdateEdit = false;
    }
    else
    {
        d->updateEdit();
    }
}

/*!
    \reimp
*/

void GLDWAbstractSpinBox::changeEvent(QEvent *event)
{
    Q_D(GLDWAbstractSpinBox);

    switch (event->type())
    {
        case QEvent::StyleChange:
            d->spinClickTimerInterval = style()->styleHint(QStyle::SH_SpinBox_ClickAutoRepeatRate, 0, this);
            d->spinClickThresholdTimerInterval
                    = style()->styleHint(QStyle::SH_SpinBox_ClickAutoRepeatThreshold, 0, this);
            d->reset();
            d->updateEditFieldGeometry();
            break;

        case QEvent::EnabledChange:
            if (!isEnabled())
            {
                d->reset();
            }

            break;

        case QEvent::ActivationChange:
            if (!isActiveWindow())
            {
                d->reset();

                if (0 != d->pendingEmit) // pendingEmit can be true even if it hasn't changed.
                {
                    d->interpret(EmitIfChanged);    // E.g. 10 to 10.0
                }
            }

            break;

        default:
            break;
    }

    QWidget::changeEvent(event);
}

/*!
    \reimp
*/

void GLDWAbstractSpinBox::resizeEvent(QResizeEvent *event)
{
    Q_D(GLDWAbstractSpinBox);
    QWidget::resizeEvent(event);

    d->updateEditFieldGeometry();
    update();
}

/*!
    \reimp
*/

QSize GLDWAbstractSpinBox::sizeHint() const
{
    Q_D(const GLDWAbstractSpinBox);

    if (d->cachedSizeHint.isEmpty())
    {
        ensurePolished();

        const QFontMetrics c_fm(fontMetrics());
        int nH = d->edit->sizeHint().height();
        int nW = 0;
        QString strs;
        QString strFixedContent =  d->prefix + d->suffix + QLatin1Char(' ');
        strs = d->textFromValue(d->minimum) + strFixedContent;
        nW = qMax(nW, c_fm.width(strs));
        strs = d->textFromValue(d->maximum) + strFixedContent;
        nW = qMax(nW, c_fm.width(strs));

        if (d->specialValueText.size() > 0)
        {
            strs = d->specialValueText;
            nW = qMax(nW, c_fm.width(strs));
        }

        nW += 2; // cursor blinking space

        QStyleOptionSpinBox opt;
        initStyleOption(&opt);
        QSize hint(nW, nH);
        d->cachedSizeHint = style()->sizeFromContents(QStyle::CT_SpinBox, &opt, hint, this)
                            .expandedTo(QApplication::globalStrut());
    }

    d->cachedSizeHint.setHeight(c_FixedPlaintEditText);
    return d->cachedSizeHint;
}

/*!
    \reimp
*/

QSize GLDWAbstractSpinBox::minimumSizeHint() const
{
    Q_D(const GLDWAbstractSpinBox);

    if (d->cachedMinimumSizeHint.isEmpty())
    {
        //Use the prefix and range to calculate the minimumSizeHint
        ensurePolished();

        const QFontMetrics c_fm(fontMetrics());
        int nH = d->edit->minimumSizeHint().height();
        int nW = 0;

        QString strs;
        QString strFixedContent =  d->prefix + QLatin1Char(' ');
        strs = d->textFromValue(d->minimum) + strFixedContent;
        nW = qMax(nW, c_fm.width(strs));
        strs = d->textFromValue(d->maximum) + strFixedContent;
        nW = qMax(nW, c_fm.width(strs));

        if (d->specialValueText.size() > 0)
        {
            strs = d->specialValueText;
            nW = qMax(nW, c_fm.width(strs));
        }

        nW += 2; // cursor blinking space

        QStyleOptionSpinBox opt;
        initStyleOption(&opt);
        QSize hint(nW, nH);

        d->cachedMinimumSizeHint = style()->sizeFromContents(QStyle::CT_SpinBox, &opt, hint, this)
                                   .expandedTo(QApplication::globalStrut());
    }

    d->cachedMinimumSizeHint.setHeight(c_FixedPlaintEditText);
    return d->cachedMinimumSizeHint;
}

/*!
    \reimp
*/

void GLDWAbstractSpinBox::paintEvent(QPaintEvent *)
{
    QStyleOptionSpinBox opt;
    initStyleOption(&opt);
    QStylePainter painter(this);
    painter.drawComplexControl(QStyle::CC_SpinBox, opt);
}

/*!
    \reimp

    This function handles keyboard input.

    The following keys are handled specifically:
    \table
    \row \li Enter/Return
         \li This will reinterpret the text and emit a signal even if the value has not changed
         since last time a signal was emitted.
    \row \li Up
         \li This will invoke stepBy(1)
    \row \li Down
         \li This will invoke stepBy(-1)
    \row \li Page up
         \li This will invoke stepBy(10)
    \row \li Page down
         \li This will invoke stepBy(-10)
    \endtable
*/


void GLDWAbstractSpinBox::keyPressEvent(QKeyEvent *event)
{
    Q_D(GLDWAbstractSpinBox);

//    if (!event->toPlainText().isEmpty() && d->edit->cursorPosition() < d->prefix.size())
//        d->edit->setCursorPosition(d->prefix.size());

    int nsteps = 1;
    bool bisPgUpOrDown = false;

    switch (event->key())
    {
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            nsteps *= 10;
            bisPgUpOrDown = true;

        case Qt::Key_Up:
        case Qt::Key_Down:
        {
#ifdef QT_KEYPAD_NAVIGATION

            if (QApplication::keypadNavigationEnabled())
            {
                // Reserve up/down for nav - use left/right for edit.
                if (!hasEditFocus() && (event->key() == Qt::Key_Up
                                        || event->key() == Qt::Key_Down))
                {
                    event->ignore();
                    return;
                }
            }

#endif
//        event->accept();
//        const bool up = (event->key() == Qt::Key_PageUp || event->key() == Qt::Key_Up);
////        if (!(stepEnabled() & (up ? StepUpEnabled : StepDownEnabled)))
////            return;
//        if (!up)
//            steps *= -1;
//        if (style()->styleHint(QStyle::SH_SpinBox_AnimateButton, 0, this)) {
//            d->buttonState = (Keyboard | (up ? Up : Down));
//        }
////        if (d->spinClickTimerId == -1)
////            stepBy(steps);
//        if(event->isAutoRepeat() && !isPgUpOrDown) {
//            if(d->spinClickThresholdTimerId == -1 && d->spinClickTimerId == -1) {
//                d->updateState(up, true);
//            }
//        }
//#ifndef QT_NO_ACCESSIBILITY
//        QAccessibleValueChangeEvent event(this, d->value);
//        QAccessible::updateAccessibility(&event);
//#endif
//        return;
            break;
        }

#ifdef QT_KEYPAD_NAVIGATION

        case Qt::Key_Left:
        case Qt::Key_Right:
            if (QApplication::keypadNavigationEnabled() && !hasEditFocus())
            {
                event->ignore();
                return;
            }

            break;

        case Qt::Key_Back:
            if (QApplication::keypadNavigationEnabled() && !hasEditFocus())
            {
                event->ignore();
                return;
            }

            break;
#endif

        case Qt::Key_Enter:
        case Qt::Key_Return:
            // todo
//        d->edit->d_func()->control->clearUndo();
//        d->interpret(d->keyboardTracking ? AlwaysEmit : EmitIfChanged);
//        selectAll();
//        event->ignore();
//        emit editingFinished();
//        emit d->edit->returnPressed();
            break;

#ifdef QT_KEYPAD_NAVIGATION

        case Qt::Key_Select:
            if (QApplication::keypadNavigationEnabled())
            {
                // Toggles between left/right moving cursor and inc/dec.
                setEditFocus(!hasEditFocus());
            }

            return;
#endif

        case Qt::Key_U:
            if (Qt::ControlModifier == (event->modifiers() & Qt::ControlModifier)
                    && QGuiApplication::platformName() == QLatin1String("xcb"))   // only X11
            {
                event->accept();

                if (!isReadOnly())
                {
                    clear();
                }

                return;
            }

            break;

        case Qt::Key_End:
        case Qt::Key_Home:
//        if (event->modifiers() & Qt::ShiftModifier) {
//            int currentPos = d->edit->cursorPosition();
//            const QString text = d->edit->displayText();
//            if (event->key() == Qt::Key_End) {
//                if ((currentPos == 0 && !d->prefix.isEmpty()) || text.size() - d->suffix.size() <= currentPos) {
//                    break; // let lineedit handle this
//                } else {
//                    d->edit->setSelection(currentPos, text.size() - d->suffix.size() - currentPos);
//                }
//            } else {
//                if ((currentPos == text.size() && !d->suffix.isEmpty()) || currentPos <= d->prefix.size()) {
//                    break; // let lineedit handle this
//                } else {
//                    d->edit->setSelection(currentPos, d->prefix.size() - currentPos);
//                }
//            }
//            event->accept();
//            return;
//        }
            break;

        default:
#ifndef QT_NO_SHORTCUT
            if (event == QKeySequence::SelectAll)
            {
                selectAll();
                event->accept();
                return;
            }

#endif
            break;
    }

    d->edit->event(event);

    if (!isVisible())
    {
        d->ignoreUpdateEdit = true;
    }
}

/*!
    \reimp
*/

void GLDWAbstractSpinBox::keyReleaseEvent(QKeyEvent *event)
{
    Q_D(GLDWAbstractSpinBox);

    if (d->buttonState & Keyboard && !event->isAutoRepeat())
    {
        d->reset();
    }
    else
    {
        d->edit->event(event);
    }
}

/*!
    \reimp
*/

#ifndef QT_NO_WHEELEVENT
void GLDWAbstractSpinBox::wheelEvent(QWheelEvent *event)
{
    const int c_steps = (event->delta() > 0 ? 1 : -1);
    Q_UNUSED(c_steps) //ÏÂ¶ÏµãÓÃ?
//    if (stepEnabled() & (steps > 0 ? StepUpEnabled : StepDownEnabled))
//        stepBy(event->modifiers() & Qt::ControlModifier ? steps * 10 : steps);
    event->accept();
}

#endif

/*!
    \reimp
*/

void GLDWAbstractSpinBox::focusInEvent(QFocusEvent *event)
{
    Q_D(GLDWAbstractSpinBox);

    d->edit->event(event);

    if (event->reason() == Qt::TabFocusReason || event->reason() == Qt::BacktabFocusReason)
    {
        selectAll();
    }

    QWidget::focusInEvent(event);
}

/*!
    \reimp
*/

void GLDWAbstractSpinBox::focusOutEvent(QFocusEvent *event)
{
    Q_D(GLDWAbstractSpinBox);

    if (0 != d->pendingEmit)
    {
        d->interpret(EmitIfChanged);
    }

    d->reset();
    d->edit->event(event);
    d->updateEdit();
    QWidget::focusOutEvent(event);

#ifdef QT_KEYPAD_NAVIGATION

    // editingFinished() is already emitted on LeaveEditFocus
    if (!QApplication::keypadNavigationEnabled())
#endif
        emit editingFinished();
}

/*!
    \reimp
*/

void GLDWAbstractSpinBox::closeEvent(QCloseEvent *event)
{
    Q_D(GLDWAbstractSpinBox);

    d->reset();

    if (0 != d->pendingEmit)
    {
        d->interpret(EmitIfChanged);
    }

    QWidget::closeEvent(event);
}

/*!
    \reimp
*/

void GLDWAbstractSpinBox::hideEvent(QHideEvent *event)
{
    Q_D(GLDWAbstractSpinBox);
    d->reset();

    if (0 != d->pendingEmit)
    {
        d->interpret(EmitIfChanged);
    }

    QWidget::hideEvent(event);
}

/*!
    \reimp
*/

void GLDWAbstractSpinBox::timerEvent(QTimerEvent *event)
{
    Q_D(GLDWAbstractSpinBox);

    bool bdoStep = false;

    if (event->timerId() == d->spinClickThresholdTimerId)
    {
        killTimer(d->spinClickThresholdTimerId);
        d->spinClickThresholdTimerId = -1;
        d->effectiveSpinRepeatRate = d->buttonState & Keyboard
                                     ? qApp->styleHints()->keyboardAutoRepeatRate()
                                     : d->spinClickTimerInterval;
        d->spinClickTimerId = startTimer(d->effectiveSpinRepeatRate);
        bdoStep = true;
    }
    else if (event->timerId() == d->spinClickTimerId)
    {
        if (0 != d->accelerate)
        {
            d->acceleration = d->acceleration + (int)(d->effectiveSpinRepeatRate * 0.05);

            if (d->effectiveSpinRepeatRate - d->acceleration >= 10)
            {
                killTimer(d->spinClickTimerId);
                d->spinClickTimerId = startTimer(d->effectiveSpinRepeatRate - d->acceleration);
            }
        }

        bdoStep = true;
    }

    if (bdoStep)
    {
//        const StepEnabled st = stepEnabled();
//        if (d->buttonState & Up) {
//            if (!(st & StepUpEnabled)) {
//                d->reset();
//            } else {
//                stepBy(1);
//            }
//        } else if (d->buttonState & Down) {
//            if (!(st & StepDownEnabled)) {
//                d->reset();
//            } else {
//                stepBy(-1);
//            }
//        }
        return;
    }

    QWidget::timerEvent(event);
    return;
}

/*!
    \reimp
*/

void GLDWAbstractSpinBox::contextMenuEvent(QContextMenuEvent *event)
{
#ifdef QT_NO_CONTEXTMENU
    Q_UNUSED(event);
#else
    Q_D(GLDWAbstractSpinBox);

    QPointer<QMenu> menu = d->edit->createStandardContextMenu();

    if (!menu)
    {
        return;
    }

    d->reset();

    QAction *selAll = new QAction(tr("&Select All"), menu);
    Q_UNUSED(selAll)
//    menu->insertAction(d->edit->d_func()->selectAllAction,
//                      selAll);
//    menu->removeAction(d->edit->d_func()->selectAllAction);
    menu->addSeparator();
//    const uint se = stepEnabled();
    QAction *up = menu->addAction(tr("&Step up"));
    Q_UNUSED(up)
//    up->setEnabled(se & StepUpEnabled);
    QAction *down = menu->addAction(tr("Step &down"));
    Q_UNUSED(down)
//    down->setEnabled(se & StepDownEnabled);
    menu->addSeparator();

//    const QPointer<GLDWAbstractSpinBox> c_that = this;
    const QPoint c_pos = (event->reason() == QContextMenuEvent::Mouse)
            ? event->globalPos() : mapToGlobal(QPoint(event->pos().x(), 0)) + QPoint(width() / 2, height() / 2);
    const QAction *action = menu->exec(c_pos);
    Q_UNUSED(action)
    delete static_cast<QMenu *>(menu);
//    if (that && action) {
//        if (action == up) {
//            stepBy(1);
//        } else if (action == down) {
//            stepBy(-1);
//        } else if (action == selAll) {
//            selectAll();
//        }
//    }
    event->accept();
#endif // QT_NO_CONTEXTMENU
}

/*!
    \reimp
*/

void GLDWAbstractSpinBox::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(GLDWAbstractSpinBox);

    d->updateHoverControl(event->pos());

    // If we have a timer ID, update the state
    if (d->spinClickTimerId != -1 && d->buttonSymbols != QAbstractSpinBox::NoButtons)
    {
//        const StepEnabled se = stepEnabled();
//        if ((se & StepUpEnabled) && d->hoverControl == QStyle::SC_SpinBoxUp)
//            d->updateState(true);
//        else if ((se & StepDownEnabled) && d->hoverControl == QStyle::SC_SpinBoxDown)
//            d->updateState(false);
//        else
//            d->reset();
//        event->accept();
    }
}

/*!
    \reimp
*/

void GLDWAbstractSpinBox::mousePressEvent(QMouseEvent *event)
{
    Q_D(GLDWAbstractSpinBox);

    if (event->button() != Qt::LeftButton || d->buttonState != None)
    {
        return;
    }

    d->updateHoverControl(event->pos());
    event->accept();

//    const StepEnabled se = (d->buttonSymbols == QAbstractSpinBox::NoButtons) ? StepEnabled(StepNone) : stepEnabled();
//    if ((se & StepUpEnabled) && d->hoverControl == QStyle::SC_SpinBoxUp) {
//        d->updateState(true);
//    } else if ((se & StepDownEnabled) && d->hoverControl == QStyle::SC_SpinBoxDown) {
//        d->updateState(false);
//    } else {
//        event->ignore();
//    }
}

/*!
    \reimp
*/
void GLDWAbstractSpinBox::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(GLDWAbstractSpinBox);

    if ((d->buttonState & Mouse) != 0)
    {
        d->reset();
    }

    event->accept();
}

// --- GLDWAbstractSpinBoxPrivate ---

/*!
    \internal
    Constructs a GLDWAbstractSpinBoxPrivate object
*/

GLDWAbstractSpinBoxPrivate::GLDWAbstractSpinBoxPrivate()
    : edit(0), type(QVariant::Invalid), spinClickTimerId(-1),
      spinClickTimerInterval(100), spinClickThresholdTimerId(-1), spinClickThresholdTimerInterval(-1),
      effectiveSpinRepeatRate(1), buttonState(None), cachedText(QLatin1String("\x01")),
      cachedState(QValidator::Invalid), pendingEmit(false), readOnly(false), wrapping(false),
      ignoreCursorPositionChanged(false), frame(true), accelerate(false), keyboardTracking(true),
      cleared(false), ignoreUpdateEdit(false), correctionMode(QAbstractSpinBox::CorrectToPreviousValue),
      acceleration(0), hoverControl(QStyle::SC_None), buttonSymbols(QAbstractSpinBox::UpDownArrows)
{
}

/*
   \internal
   Called when the GLDWAbstractSpinBoxPrivate is destroyed
*/
GLDWAbstractSpinBoxPrivate::~GLDWAbstractSpinBoxPrivate()
{
}

/*!
    \internal
    Updates the old and new hover control. Does nothing if the hover
    control has not changed.
*/
bool GLDWAbstractSpinBoxPrivate::updateHoverControl(const QPoint &pos)
{
    Q_Q(GLDWAbstractSpinBox);
    QRect lastHoverRect = hoverRect;
    QStyle::SubControl lastHoverControl = hoverControl;
    bool bdoesHover = q->testAttribute(Qt::WA_Hover);

    if (lastHoverControl != newHoverControl(pos) && bdoesHover)
    {
        q->update(lastHoverRect);
        q->update(hoverRect);
        return true;
    }

    return !bdoesHover;
}

/*!
    \internal
    Returns the hover control at \a pos.
    This will update the hoverRect and hoverControl.
*/
QStyle::SubControl GLDWAbstractSpinBoxPrivate::newHoverControl(const QPoint &pos)
{
    Q_Q(GLDWAbstractSpinBox);

    QStyleOptionSpinBox opt;
    q->initStyleOption(&opt);
    opt.subControls = QStyle::SC_All;
    hoverControl = q->style()->hitTestComplexControl(QStyle::CC_SpinBox, &opt, pos, q);
    hoverRect = q->style()->subControlRect(QStyle::CC_SpinBox, &opt, hoverControl, q);
    return hoverControl;
}

/*!
    \internal
    Strips any prefix/suffix from \a text.
*/

QString GLDWAbstractSpinBoxPrivate::stripped(const QString &t, int *pos) const
{
    QString text = t;

    if (specialValueText.size() == 0 || text != specialValueText)
    {
        int nfrom = 0;
        int nsize = text.size();
        bool bchanged = false;

        if (prefix.size() > 0 && text.startsWith(prefix))
        {
            nfrom += prefix.size();
            nsize -= nfrom;
            bchanged = true;
        }

        if (suffix.size() > 0 && text.endsWith(suffix))
        {
            nsize -= suffix.size();
            bchanged = true;
        }

        if (bchanged)
        {
            text = text.mid(nfrom, nsize);
        }
    }

    const int c_size = text.size();
    text = text.trimmed();

    if (pos)
    {
        (*pos) -= (c_size - text.size());
    }

    return text;

}

void GLDWAbstractSpinBoxPrivate::updateEditFieldGeometry()
{
    Q_Q(GLDWAbstractSpinBox);
    QStyleOptionSpinBox opt;
    q->initStyleOption(&opt);
    opt.subControls = QStyle::SC_SpinBoxEditField;
    edit->setGeometry(q->style()->subControlRect(QStyle::CC_SpinBox, &opt,
                      QStyle::SC_SpinBoxEditField, q));
}

/*!
    \internal
    Returns true if a specialValueText has been set and the current value is minimum.
*/

bool GLDWAbstractSpinBoxPrivate::specialValue() const
{
    return (value == minimum && !specialValueText.isEmpty());
}

/*!
    \internal Virtual function that emits signals when the value
    changes. Reimplemented in the different subclasses.
*/

void GLDWAbstractSpinBoxPrivate::emitSignals(EmitPolicy, const QVariant &)
{
}

/*!
    \internal

    Slot connected to the line edit's textChanged(const QString &)
    signal.
*/

void GLDWAbstractSpinBoxPrivate::_q_editorTextChanged(const QString &t)
{
//    Q_Q(GLDWAbstractSpinBox);

    if (0 != keyboardTracking)
    {
        QString tmp = t;
//        int pos = edit->cursorPosition();
//        QValidator::State state = q->validate(tmp, pos);
//        if (state == QValidator::Acceptable)
        {
            const QVariant var = valueFromText(tmp);
            setValue(var, EmitIfChanged, tmp != t);
            pendingEmit = false;
        }
//        else
//        {
//            pendingEmit = true;
//        }
    }
    else
    {
        pendingEmit = true;
    }
}

/*!
    \internal

    Virtual slot connected to the line edit's
    cursorPositionChanged(int, int) signal. Will move the cursor to a
    valid position if the new one is invalid. E.g. inside the prefix.
    Reimplemented in Q[Date|Time|DateTime]EditPrivate to account for
    the different sections etc.
*/

void GLDWAbstractSpinBoxPrivate::_q_editorCursorPositionChanged(int oldpos, int newpos)
{
    Q_UNUSED(oldpos)
    Q_UNUSED(newpos)
//    if (!edit->hasSelectedText() && !ignoreCursorPositionChanged && !specialValue()) {
//        ignoreCursorPositionChanged = true;

//        bool allowSelection = true;
//        int pos = -1;
//        if (newpos < prefix.size() && newpos != 0) {
//            if (oldpos == 0) {
//                allowSelection = false;
//                pos = prefix.size();
//            } else {
//                pos = oldpos;
//            }
//        } else if (newpos > edit->toPlainText().size() - suffix.size()
//                   && newpos != edit->toPlainText().size()) {
//            if (oldpos == edit->toPlainText().size()) {
//                pos = edit->toPlainText().size() - suffix.size();
//                allowSelection = false;
//            } else {
//                pos = edit->toPlainText().size();
//            }
//        }
//        if (pos != -1) {
//            const int selSize = edit->selectionStart() >= 0 && allowSelection
//                                  ? (edit->selectedText().size()
//                                     * (newpos < pos ? -1 : 1)) - newpos + pos
//                                  : 0;

//            const bool wasBlocked = edit->blockSignals(true);
//            if (selSize != 0) {
//                edit->setSelection(pos - selSize, selSize);
//            } else {
//                edit->setCursorPosition(pos);
//            }
//            edit->blockSignals(wasBlocked);
//        }
//        ignoreCursorPositionChanged = false;
//    }
}

/*!
    \internal

    Initialises the GLDWAbstractSpinBoxPrivate object.
*/

void GLDWAbstractSpinBoxPrivate::init()
{
    Q_Q(GLDWAbstractSpinBox);

    GLDPlainTextEdit *edit = new GLDPlainTextEdit(q);
    edit->setContentsMargins(0, 0, 0, 0);
    edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setFixedHeight(q->height());
    q->setLineEdit(edit);
    edit->setObjectName(QLatin1String("GLDPlainTextEdit"));
//    validator = new GLDWSpinBoxValidator(q, this);
//    edit->setValidator(validator);

    QStyleOptionSpinBox opt;
    q->initStyleOption(&opt);
    spinClickTimerInterval = q->style()->styleHint(QStyle::SH_SpinBox_ClickAutoRepeatRate, &opt, q);
    spinClickThresholdTimerInterval = q->style()->styleHint(QStyle::SH_SpinBox_ClickAutoRepeatThreshold, &opt, q);
    q->setFocusPolicy(Qt::WheelFocus);
    q->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::SpinBox));
    q->setAttribute(Qt::WA_InputMethodEnabled);

    q->setAttribute(Qt::WA_MacShowFocusRect);
}

/*!
    \internal

    Resets the state of the spinbox. E.g. the state is set to
    (Keyboard|Up) if Key up is currently pressed.
*/

void GLDWAbstractSpinBoxPrivate::reset()
{
    Q_Q(GLDWAbstractSpinBox);

    buttonState = None;

    if (q)
    {
        if (spinClickTimerId != -1)
        {
            q->killTimer(spinClickTimerId);
        }

        if (spinClickThresholdTimerId != -1)
        {
            q->killTimer(spinClickThresholdTimerId);
        }

        spinClickTimerId = spinClickThresholdTimerId = -1;
        acceleration = 0;
        q->update();
    }
}

/*!
    \internal

    Updates the state of the spinbox.
*/

void GLDWAbstractSpinBoxPrivate::updateState(bool up, bool fromKeyboard /* = false */)
{
//    Q_Q(GLDWAbstractSpinBox);
    Q_UNUSED(fromKeyboard)

    if ((up && (buttonState & Up)) || (!up && (buttonState & Down)))
    {
        return;
    }

    reset();
//    if (q && (q->stepEnabled() & (up ? GLDWAbstractSpinBox::StepUpEnabled
//                                  : GLDWAbstractSpinBox::StepDownEnabled))) {
//        spinClickThresholdTimerId = q->startTimer(spinClickThresholdTimerInterval);
//        buttonState = (up ? Up : Down) | (fromKeyboard ? Keyboard : Mouse);
//        q->stepBy(up ? 1 : -1);
//#ifndef QT_NO_ACCESSIBILITY
//        QAccessibleValueChangeEvent event(q, value);
//        QAccessible::updateAccessibility(&event);
//#endif
//    }
}

/*!
    Initialize \a option with the values from this QSpinBox. This method
    is useful for subclasses when they need a QStyleOptionSpinBox, but don't want
    to fill in all the information themselves.

    \sa QStyleOption::initFrom()
*/

void GLDWAbstractSpinBox::initStyleOption(QStyleOptionSpinBox *option) const
{
    if (!option)
    {
        return;
    }

    Q_D(const GLDWAbstractSpinBox);
    option->initFrom(this);
    option->activeSubControls = QStyle::SC_None;
    option->buttonSymbols = d->buttonSymbols;
    option->subControls = QStyle::SC_SpinBoxFrame | QStyle::SC_SpinBoxEditField;

    if (d->buttonSymbols != QAbstractSpinBox::NoButtons)
    {
        option->subControls |= QStyle::SC_SpinBoxUp | QStyle::SC_SpinBoxDown;

        if (d->buttonState & Up)
        {
            option->activeSubControls = QStyle::SC_SpinBoxUp;
        }
        else if (d->buttonState & Down)
        {
            option->activeSubControls = QStyle::SC_SpinBoxDown;
        }
    }

    if (d->buttonState > 0)
    {
        option->state |= QStyle::State_Sunken;
    }
    else
    {
        option->activeSubControls = d->hoverControl;
    }

//    option->stepEnabled = style()->styleHint(QStyle::SH_SpinControls_DisableOnBounds)
//                      ? stepEnabled()
//                      : (QAbstractSpinBox::StepDownEnabled | QAbstractSpinBox::StepUpEnabled);

    option->frame = (0 != d->frame);
}

/*!
    \internal

    Bounds \a val to be within minimum and maximum. Also tries to be
    clever about setting it at min and max depending on what it was
    and what direction it was changed etc.
*/

QVariant GLDWAbstractSpinBoxPrivate::bound(const QVariant &val, const QVariant &old, int steps) const
{
    QVariant var = val;

    if (0 == wrapping || steps == 0 || old.isNull())
    {
        if (variantCompare(var, minimum) < 0)
        {
            var = (0 != wrapping) ? maximum : minimum;
        }

        if (variantCompare(var, maximum) > 0)
        {
            var = (0 != wrapping) ? minimum : maximum;
        }
    }
    else
    {
        const bool c_wasMin = old == minimum;
        const bool c_wasMax = old == maximum;
        const int c_oldcmp = variantCompare(var, old);
        const int c_maxcmp = variantCompare(var, maximum);
        const int c_mincmp = variantCompare(var, minimum);
        const bool c_wrapped = (c_oldcmp > 0 && steps < 0) || (c_oldcmp < 0 && steps > 0);

        if (c_maxcmp > 0)
        {
            var = ((c_wasMax && !c_wrapped && steps > 0) || (steps < 0 && !c_wasMin && c_wrapped))
                ? minimum : maximum;
        }
        else if (c_wrapped && (c_maxcmp > 0 || c_mincmp < 0))
        {
            var = ((c_wasMax && steps > 0) || (!c_wasMin && steps < 0)) ? minimum : maximum;
        }
        else if (c_mincmp < 0)
        {
            var = (!c_wasMax && !c_wasMin ? minimum : maximum);
        }
    }

    return var;
}

/*!
    \internal

    Sets the value of the spin box to \a val. Depending on the value
    of \a ep it will also emit signals.
*/

void GLDWAbstractSpinBoxPrivate::setValue(const QVariant &val, EmitPolicy ep,
        bool doUpdate)
{
    Q_Q(GLDWAbstractSpinBox);
    const QVariant c_old = value;
    value = bound(val);
    pendingEmit = false;
    cleared = false;

    if (doUpdate)
    {
        updateEdit();
    }

    q->update();

    if (ep == AlwaysEmit || (ep == EmitIfChanged && c_old != value))
    {
        emitSignals(ep, c_old);
    }
}

/*!
    \internal

    Updates the line edit to reflect the current value of the spin box.
*/

void GLDWAbstractSpinBoxPrivate::updateEdit()
{
    Q_Q(GLDWAbstractSpinBox);

    if (type == QVariant::Invalid)
    {
        return;
    }

    const QString c_newText = specialValue() ? specialValueText : prefix + textFromValue(value) + suffix;

    if (c_newText == edit->toPlainText() || 0 != cleared)
    {
        return;
    }

    const bool c_empty = edit->toPlainText().isEmpty();
    Q_UNUSED(c_empty)
//    int cursor = edit->cursorPosition();
//    int selsize = edit->selectedText().size();
    const bool c_sb = edit->blockSignals(true);
    edit->setPlainText(c_newText);

//    if (!specialValue()) {
//        cursor = qBound(prefix.size(), cursor, edit->displayText().size() - suffix.size());

//        if (selsize > 0) {
//            edit->setSelection(cursor, selsize);
//        } else {
//            edit->setCursorPosition(empty ? prefix.size() : cursor);
//        }
//    }
    edit->blockSignals(c_sb);
    q->update();
}

/*!
    \internal

    Convenience function to set min/max values.
*/

void GLDWAbstractSpinBoxPrivate::setRange(const QVariant &min, const QVariant &max)
{
    Q_Q(GLDWAbstractSpinBox);

    clearCache();
    minimum = min;
    maximum = (variantCompare(min, max) < 0 ? max : min);
    cachedSizeHint = QSize();
    cachedMinimumSizeHint = QSize(); // minimumSizeHint cares about min/max

    reset();

    if (!(bound(value) == value))
    {
        setValue(bound(value), EmitIfChanged);
    }
    else if (value == minimum && !specialValueText.isEmpty())
    {
        updateEdit();
    }

    q->updateGeometry();
}

/*!
    \internal

    Convenience function to get a variant of the right type.
*/

QVariant GLDWAbstractSpinBoxPrivate::getZeroVariant() const
{
    QVariant ret;

    switch (type)
    {
        case QVariant::Int:
            ret = QVariant((int)0);
            break;

        case QVariant::Double:
            ret = QVariant((double)0.0);
            break;

        default:
            break;
    }

    return ret;
}

/*!
    \internal

    Virtual method called that calls the public textFromValue()
    functions in the subclasses. Needed to change signature from
    QVariant to int/double/QDateTime etc. Used when needing to display
    a value textually.

    This method is reimeplemented in the various subclasses.
*/

QString GLDWAbstractSpinBoxPrivate::textFromValue(const QVariant &) const
{
    return QString();
}

/*!
    \internal

    Virtual method called that calls the public valueFromText()
    functions in the subclasses. Needed to change signature from
    QVariant to int/double/QDateTime etc. Used when needing to
    interpret a string as another type.

    This method is reimeplemented in the various subclasses.
*/

QVariant GLDWAbstractSpinBoxPrivate::valueFromText(const QString &) const
{
    return QVariant();
}

/*!
    \internal

    Interprets text and emits signals. Called when the spinbox needs
    to interpret the text on the lineedit.
*/

void GLDWAbstractSpinBoxPrivate::interpret(EmitPolicy ep)
{
//    Q_Q(GLDWAbstractSpinBox);
    if (type == QVariant::Invalid || 0 != cleared)
    {
        return;
    }

    QVariant var = getZeroVariant();
    bool bdoInterpret = true;
    QString tmp = edit->toPlainText();
//    int pos = edit->cursorPosition();
//    const int oldpos = pos;

//    if (q->validate(tmp, pos) != QValidator::Acceptable) {
//        const QString copy = tmp;
//        q->fixup(tmp);
//        QASBDEBUG() << "GLDWAbstractSpinBoxPrivate::interpret() text '"
//                    << edit->toPlainText()
//                    << "' >> '" << copy << '\''
//                    << "' >> '" << tmp << '\'';

//        doInterpret = tmp != copy && (q->validate(tmp, pos) == QValidator::Acceptable);
//        if (!doInterpret) {
//            v = (correctionMode == QAbstractSpinBox::CorrectToNearestValue
//                 ? variantBound(minimum, v, maximum) : value);
//        }
//    }
    if (bdoInterpret)
    {
        var = valueFromText(tmp);
    }

    clearCache();
    setValue(var, ep, true);
//    if (oldpos != pos)
//        edit->setCursorPosition(pos);
}

void GLDWAbstractSpinBoxPrivate::clearCache() const
{
    cachedText.clear();
    cachedValue.clear();
    cachedState = QValidator::Acceptable;
}

// --- GLDWSpinBoxValidator ---

/*!
    \internal
    Constructs a GLDWSpinBoxValidator object
*/

//GLDWSpinBoxValidator::GLDWSpinBoxValidator(GLDWAbstractSpinBox *qp, GLDWAbstractSpinBoxPrivate *dp)
//    : QValidator(qp), qptr(qp), dptr(dp)
//{
//    setObjectName(QLatin1String("toPlainText_validator"));
//}

// --- global ---

/*!
    \internal
    Adds two variants together and returns the result.
*/

QVariant operator+(const QVariant &arg1, const QVariant &arg2)
{
    QVariant ret;

    if (arg1.type() != arg2.type())
        qWarning("GLDWAbstractSpinBox: Internal error: Different types (%s vs %s) (%s:%d)",
                 arg1.typeName(), arg2.typeName(), __FILE__, __LINE__);

    switch (arg1.type())
    {
        case QVariant::Int:
        {
            const int c_int1 = arg1.toInt();
            const int c_int2 = arg2.toInt();

            if (c_int1 > 0 && (c_int2 >= INT_MAX - c_int1))
            {
                // The increment overflows
                ret = QVariant(INT_MAX);
            }
            else if (c_int1 < 0 && (c_int2 <= INT_MIN - c_int1))
            {
                // The increment underflows
                ret = QVariant(INT_MIN);
            }
            else
            {
                ret = QVariant(c_int1 + c_int2);
            }

            break;
        }

        case QVariant::Double:
            ret = QVariant(arg1.toDouble() + arg2.toDouble());
            break;

        case QVariant::DateTime:
        {
            QDateTime a2 = arg2.toDateTime();
            QDateTime a1 = arg1.toDateTime().addDays(QDATETIMEEDIT_DATETIME_MIN.daysTo(a2));
            a1.setTime(a1.time().addMSecs(QTime().msecsTo(a2.time())));
            ret = QVariant(a1);
        }

        default:
            break;
    }

    return ret;
}

/*!
    \internal
    Subtracts two variants and returns the result.
*/

QVariant operator-(const QVariant &arg1, const QVariant &arg2)
{
    QVariant ret;

    if (arg1.type() != arg2.type())
        qWarning("GLDWAbstractSpinBox: Internal error: Different types (%s vs %s) (%s:%d)",
                 arg1.typeName(), arg2.typeName(), __FILE__, __LINE__);

    switch (arg1.type())
    {
        case QVariant::Int:
            ret = QVariant(arg1.toInt() - arg2.toInt());
            break;

        case QVariant::Double:
            ret = QVariant(arg1.toDouble() - arg2.toDouble());
            break;

        case QVariant::DateTime:
        {
            QDateTime a1 = arg1.toDateTime();
            QDateTime a2 = arg2.toDateTime();
            int ndays = a2.daysTo(a1);
            int nsecs = a2.secsTo(a1);
            int nmsecs = qMax(0, a1.time().msec() - a2.time().msec());

            if (ndays < 0 || nsecs < 0 || nmsecs < 0)
            {
                ret = arg1;
            }
            else
            {
                QDateTime dt = a2.addDays(ndays).addSecs(nsecs);

                if (nmsecs > 0)
                {
                    dt.setTime(dt.time().addMSecs(nmsecs));
                }

                ret = QVariant(dt);
            }
        }

        default:
            break;
    }

    return ret;
}

/*!
    \internal
    Multiplies \a arg1 by \a multiplier and returns the result.
*/

QVariant operator*(const QVariant &arg1, double multiplier)
{
    QVariant ret;

    switch (arg1.type())
    {
        case QVariant::Int:
            ret = static_cast<int>(qBound<double>(INT_MIN, arg1.toInt() * multiplier, INT_MAX));
            break;

        case QVariant::Double:
            ret = QVariant(arg1.toDouble() * multiplier);
            break;

        case QVariant::DateTime:
        {
            double days = QDATETIMEEDIT_DATE_MIN.daysTo(arg1.toDateTime().date()) * multiplier;
            int ndaysInt = (int)days;
            days -= ndaysInt;
            long lmsecs = (long)((QDATETIMEEDIT_TIME_MIN.msecsTo(arg1.toDateTime().time()) * multiplier)
                                + (days * (24 * 3600 * 1000)));
            ret = QDateTime(QDate().addDays(int(days)), QTime().addMSecs(lmsecs));
            break;
        }

        default:
            ret = arg1;
            break;
    }

    return ret;
}

double operator/(const QVariant &arg1, const QVariant &arg2)
{
    double da1 = 0;
    double da2 = 0;

    switch (arg1.type())
    {
        case QVariant::Int:
            da1 = (double)arg1.toInt();
            da2 = (double)arg2.toInt();
            break;

        case QVariant::Double:
            da1 = arg1.toDouble();
            da2 = arg2.toDouble();
            break;

        case QVariant::DateTime:
            da1 = QDATETIMEEDIT_DATE_MIN.daysTo(arg1.toDate());
            da2 = QDATETIMEEDIT_DATE_MIN.daysTo(arg2.toDate());
            da1 += (double)QDATETIMEEDIT_TIME_MIN.msecsTo(arg1.toDateTime().time()) / (long)(3600 * 24 * 1000);
            da2 += (double)QDATETIMEEDIT_TIME_MIN.msecsTo(arg2.toDateTime().time()) / (long)(3600 * 24 * 1000);

        default:
            break;
    }

    return (da1 != 0 && da2 != 0) ? (da1 / da2) : 0.0;
}

int GLDWAbstractSpinBoxPrivate::variantCompare(const QVariant &arg1, const QVariant &arg2)
{
    switch (arg2.type())
    {
        case QVariant::Date:
            Q_ASSERT_X(arg1.type() == QVariant::Date, "GLDWAbstractSpinBoxPrivate::variantCompare",
                       qPrintable(QString::fromLatin1("Internal error 1 (%1)").
                                  arg(QString::fromLatin1(arg1.typeName()))));

            if (arg1.toDate() == arg2.toDate())
            {
                return 0;
            }
            else if (arg1.toDate() < arg2.toDate())
            {
                return -1;
            }
            else
            {
                return 1;
            }

        case QVariant::Time:
            Q_ASSERT_X(arg1.type() == QVariant::Time, "GLDWAbstractSpinBoxPrivate::variantCompare",
                       qPrintable(QString::fromLatin1("Internal error 2 (%1)").
                                  arg(QString::fromLatin1(arg1.typeName()))));

            if (arg1.toTime() == arg2.toTime())
            {
                return 0;
            }
            else if (arg1.toTime() < arg2.toTime())
            {
                return -1;
            }
            else
            {
                return 1;
            }


        case QVariant::DateTime:
            if (arg1.toDateTime() == arg2.toDateTime())
            {
                return 0;
            }
            else if (arg1.toDateTime() < arg2.toDateTime())
            {
                return -1;
            }
            else
            {
                return 1;
            }

        case QVariant::Int:
            if (arg1.toInt() == arg2.toInt())
            {
                return 0;
            }
            else if (arg1.toInt() < arg2.toInt())
            {
                return -1;
            }
            else
            {
                return 1;
            }

        case QVariant::Double:
            if (arg1.toDouble() == arg2.toDouble())
            {
                return 0;
            }
            else if (arg1.toDouble() < arg2.toDouble())
            {
                return -1;
            }
            else
            {
                return 1;
            }

        case QVariant::Invalid:
            if (arg2.type() == QVariant::Invalid)
            {
                return 0;
            }

        default:
            Q_ASSERT_X(0, "GLDWAbstractSpinBoxPrivate::variantCompare",
                       qPrintable(QString::fromLatin1("Internal error 3 (%1 %2)").
                                  arg(QString::fromLatin1(arg1.typeName())).
                                  arg(QString::fromLatin1(arg2.typeName()))));
    }

    return -2;
}

QVariant GLDWAbstractSpinBoxPrivate::variantBound(const QVariant &min,
        const QVariant &value,
        const QVariant &max)
{
    Q_ASSERT(variantCompare(min, max) <= 0);

    if (variantCompare(min, value) < 0)
    {
        const int c_compMax = variantCompare(value, max);
        return (c_compMax < 0 ? value : max);
    }
    else
    {
        return min;
    }
}

#endif // QT_NO_SPINBOX
