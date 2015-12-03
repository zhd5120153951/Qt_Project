#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFocusEvent>
#include <QPalette>
#include <QCursor>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QPropertyAnimation>
#include "GLDFilterWidget.h"

GLDHintLineEdit::GLDHintLineEdit(QWidget *parent)
    : QLineEdit(parent), m_eDefaultFocusPolicy(focusPolicy()), m_bRefuseFocus(false)
{
}

bool GLDHintLineEdit::refuseFocus() const
{
    return m_bRefuseFocus;
}

void GLDHintLineEdit::setRefuseFocus(bool v)
{
    if (v == m_bRefuseFocus)
    {
        return;
    }

    m_bRefuseFocus = v;
    setFocusPolicy(m_bRefuseFocus ? Qt::NoFocus : m_eDefaultFocusPolicy);
}

void GLDHintLineEdit::mousePressEvent(QMouseEvent *event)
{
    // Explicitly focus on click.

    if (m_bRefuseFocus && !hasFocus())
    {
        setFocus(Qt::OtherFocusReason);
    }
    QLineEdit::mousePressEvent(event);
}

void GLDHintLineEdit::focusInEvent(QFocusEvent *e)
{
    if (m_bRefuseFocus)
    {
        // Refuse the focus if the mouse it outside. In addition to the mouse
        // press logic, this prevents a re-focussing which occurs once
        // we actually had focus
        const Qt::FocusReason eReason = e->reason();
        if ((eReason == Qt::ActiveWindowFocusReason) || (eReason == Qt::PopupFocusReason))
        {
            const QPoint oMousePos = mapFromGlobal(QCursor::pos());
            const bool bRefuse = !geometry().contains(oMousePos);
            if (bRefuse)
            {
                e->ignore();
                return;
            }
        }
    }

    QLineEdit::focusInEvent(e);
}


GLDIconButton::GLDIconButton(QWidget *parent)
    : QToolButton(parent), m_fFader(1.0f)
{
    setCursor(Qt::ArrowCursor);
}

float GLDIconButton::fader()
{
    return m_fFader;
}

void GLDIconButton::setFader(float value)
{
    m_fFader = value;
    update();
}

void GLDIconButton::animateShow(bool visible)
{
    if (visible)
    {
        QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "fader");
        pAnimation->setDuration(160);
        pAnimation->setEndValue(1.0);
        pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    else
    {
        QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "fader");
        pAnimation->setDuration(160);
        pAnimation->setEndValue(0.0);
        pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void GLDIconButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter oPainter(this);
    // Note isDown should really use the active state but in most styles
    // this has no proper feedback
    QIcon::Mode eState = QIcon::Disabled;
    if (isEnabled())
    {
        eState = isDown() ? QIcon::Selected : QIcon::Normal;
    }
    QPixmap oIconpixmap = icon().pixmap(
                QSize(ICONBUTTON_SIZE, ICONBUTTON_SIZE), eState, QIcon::Off);
    QRect oPixmapRect = QRect(0, 0, oIconpixmap.width(), oIconpixmap.height());
    oPixmapRect.moveCenter(rect().center());
    oPainter.setOpacity(m_fFader);
    oPainter.drawPixmap(oPixmapRect, oIconpixmap);
}


GLDFilterWidget::GLDFilterWidget(
        QWidget *parent, GLDFilterWidget::GLDLayoutMode mode)
    : QWidget(parent),
      m_pLedtEditor(new GLDHintLineEdit(this)),
      m_pBtnIcon(new GLDIconButton(m_pLedtEditor)),
      m_nButtonWidth(0),
      m_autoResize(false)
{
    QIcon value(":/icons/FilterWidget_ClearText.png");
    init(mode, value);
}

GLDFilterWidget::GLDFilterWidget(QIcon icon, QWidget *parent, GLDFilterWidget::GLDLayoutMode mode)
      : QWidget(parent),
      m_pLedtEditor(new GLDHintLineEdit(this)),
      m_pBtnIcon(new GLDIconButton(m_pLedtEditor)),
      m_nButtonWidth(0),
      m_autoResize(false)
{
    init(mode, icon);
}

GString GLDFilterWidget::text() const
{
    return m_pLedtEditor->text();
}

void GLDFilterWidget::setText(const GString str)
{
    m_pLedtEditor->setText(str);
}

void GLDFilterWidget::resizeEvent(QResizeEvent *event)
{
    if (m_autoResize)
    {
        m_pLedtEditor->resize(event->size());
        m_pLedtEditor->move(0,0);
    }

    QRect oContentRect = m_pLedtEditor->rect();

    if (layoutDirection() == Qt::LeftToRight)
    {
        const int nIconoffset = m_pLedtEditor->textMargins().right() + 4;
        m_pBtnIcon->setGeometry(oContentRect.adjusted(m_pLedtEditor->width() - nIconoffset, 0, 0, 0));
    }
    else
    {
        const int oIconoffset = m_pLedtEditor->textMargins().left() + 4;
        m_pBtnIcon->setGeometry(oContentRect.adjusted(0, 0, -m_pLedtEditor->width() + oIconoffset, 0));
    }
}

bool GLDFilterWidget::refuseFocus() const
{
    return m_pLedtEditor->refuseFocus();
}

void GLDFilterWidget::setRefuseFocus(bool v)
{
    m_pLedtEditor->setRefuseFocus(v);
}

void GLDFilterWidget::setAutoResize(bool autoResize)
{
    m_autoResize = autoResize;
}

void GLDFilterWidget::setInputPlaceholderText(const GString &text)
{
    m_inputPlaceholderText = text;
    m_pLedtEditor->setPlaceholderText(tr(m_inputPlaceholderText.toStdString().c_str()));
}

void GLDFilterWidget::reset()
{
    if (!m_pLedtEditor->text().isEmpty())
    {
        // Editor has lost focus once this is pressed
        m_pLedtEditor->clear();
        emit filterChanged(GString());
    }
}

void GLDFilterWidget::checkButton(const GString &text)
{
    if (m_sOldText.isEmpty() || text.isEmpty())
    {
        m_pBtnIcon->animateShow(!m_pLedtEditor->text().isEmpty());
    }
    m_sOldText = text;
}

void GLDFilterWidget::init(GLDLayoutMode mode, QIcon value)
{
    m_pLedtEditor->setPlaceholderText(tr(m_inputPlaceholderText.toStdString().c_str()));

    // Let the style determine minimum height for our widget
    QSize oSize(ICONBUTTON_SIZE + 6, ICONBUTTON_SIZE + 2);

    // Note KDE does not reserve space for the highlight color
    if (style()->inherits("OxygenStyle"))
    {
        oSize = oSize.expandedTo(QSize(24, 0));
    }

    // Make room for clear icon
    QMargins oMargins = m_pLedtEditor->textMargins();
    if (layoutDirection() == Qt::LeftToRight)
    {
        oMargins.setRight(oSize.width());
    }
    else
    {
        oMargins.setLeft(oSize.width());
    }

    m_pLedtEditor->setTextMargins(oMargins);

    QHBoxLayout *pHblFilter = new QHBoxLayout(this);
    pHblFilter->setMargin(0);
    pHblFilter->setSpacing(0);
    if (mode == lmAlignRight)
    {
        pHblFilter->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    }

    pHblFilter->addWidget(m_pLedtEditor);

    m_pBtnIcon->setIcon(value);
    // Clear text=Çå¿Õ
    m_pBtnIcon->setToolTip(tr("Clear text"));
    connect(m_pBtnIcon, &GLDIconButton::clicked, this, &GLDFilterWidget::reset);
    connect(m_pLedtEditor, &GLDHintLineEdit::textChanged, this, &GLDFilterWidget::checkButton);
    connect(m_pLedtEditor, &GLDHintLineEdit::textEdited, this, &GLDFilterWidget::filterChanged);
}
