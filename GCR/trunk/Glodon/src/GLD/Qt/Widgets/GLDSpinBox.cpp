#include "GLDWidget_Global.h"
#include "GLDSpinBox.h"
#include "GLDFileUtils.h"

#include <QKeyEvent>
#include <QLineEdit>
#include <QString>
#include <QStyle>
#include <QStyleOption>

const GString c_sSpinBoxQssFile = ":/qsses/GLDSpinBoxEx.qss";
const GString c_sDoubleSpinBoxQssFile = ":/qsses/GLDSpinBoxEx.qss";

G_GLODON_BEGIN_NAMESPACE

GLDSpinBox::GLDSpinBox(QWidget *parent) :
    QSpinBox(parent), m_wheelEnable(true), m_upAndDownEnable(true)
{
    setMaximum(MaxInt);
    setMinimum(MinInt);
    setButtonSymbols(NoButtons);

    connect(lineEdit(), SIGNAL(cursorPositionChanged(int, int)),
            this, SLOT(doCursorPositionChanged(int, int)));
    connect(lineEdit(), SIGNAL(selectionChanged()),
            this, SLOT(doSelectionChanged()));
}

bool GLDSpinBox::hasSelectedText()
{
    return lineEdit()->hasSelectedText();
}

void GLDSpinBox::keyPressEvent(QKeyEvent *event)
{
    if (!m_upAndDownEnable)
    {
        switch (event->key())
        {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            return event->accept();
        default:
            break;
        }
    }
    QSpinBox::keyPressEvent(event);
}

void GLDSpinBox::mouseReleaseEvent(QMouseEvent *event)
{
    QSpinBox::mouseReleaseEvent(event);
}

#ifndef QT_NO_WHEELEVENT
void GLDSpinBox::wheelEvent(QWheelEvent *event)
{
    if (!m_wheelEnable)
    {
        return event->accept();
    }
    else
        QSpinBox::wheelEvent(event);
}
#endif

void GLDSpinBox::doSelectionChanged()
{
    emit selectionChanged();
}

void GLDSpinBox::doCursorPositionChanged(int, int)
{
    emit cursorPositionChanged();
}


GLDDoubleSpinBox::GLDDoubleSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent), m_decimalsEnable(true), m_wheelEnable(true), m_upAndDownEnable(true), m_orgDecimals(2),
    m_validator(NULL)
{
    setMinimum(MINDOUBLE);
    setMaximum(MAXDOUBLE);
    setButtonSymbols(NoButtons);
    setDecimalsEnable(false);

    connect(lineEdit(), SIGNAL(cursorPositionChanged(int, int)),
            this, SLOT(doCursorPositionChanged(int, int)));
    connect(lineEdit(), SIGNAL(selectionChanged()),
            this, SLOT(doSelectionChanged()));
}

GLDDoubleSpinBox::~GLDDoubleSpinBox()
{
    if (m_validator)
        delete m_validator;//freeAndNil(m_validator);
}

QValidator::State GLDDoubleSpinBox::validate(QString &input, int &pos) const
{
    if (m_decimalsEnable && !input.contains(QLatin1Char('e')))
        return QDoubleSpinBox::validate(input, pos);
    else
    {
        bool bOk = false;
        input.toDouble(&bOk);
        if (bOk)
            return QValidator::Acceptable;
        else    //也许是输入e的特殊情形
        {
            if (!m_validator)
            {
                m_validator = new QDoubleValidator(minimum(), maximum(), decimals(), NULL);
                m_validator->setNotation(QDoubleValidator::ScientificNotation);
            }
            m_validator->setDecimals(decimals());
            QValidator::State tmp = m_validator->validate(input, pos);
            return  tmp;
        }
    }
}

double GLDDoubleSpinBox::valueFromText(const QString &text) const
{
    if (m_decimalsEnable)
        return QDoubleSpinBox::valueFromText(text);
    else
        return text.toDouble();
}

QString GLDDoubleSpinBox::textFromValue(double val) const
{
    if (m_decimalsEnable)
        return QDoubleSpinBox::textFromValue(val);
    else
    {
        QString tmp =  QString::number(val, 'g', normalDecimals);
        return tmp;
    }
}

void GLDDoubleSpinBox::setDecimalsEnable(bool value)
{
    if (value == m_decimalsEnable)
        return;
    m_decimalsEnable = value;
    if (m_decimalsEnable)
        setDecimals(m_orgDecimals);
    else
    {
        m_orgDecimals = decimals();
        setDecimals(normalDecimals);
    }
}

bool GLDDoubleSpinBox::hasSelectedText()
{
    return lineEdit()->hasSelectedText();
}

void GLDDoubleSpinBox::cut()
{
    lineEdit()->cut();
}

void GLDDoubleSpinBox::keyPressEvent(QKeyEvent *event)
{
    if (!m_upAndDownEnable)
    {
        switch (event->key())
        {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            return;
        default:
            break;
        }
    }
    QDoubleSpinBox::keyPressEvent(event);
}

#ifndef QT_NO_WHEELEVENT
void GLDDoubleSpinBox::wheelEvent(QWheelEvent *event)
{
    if (!m_wheelEnable)
    {
        return event->accept();
    }
    else
        QDoubleSpinBox::wheelEvent(event);
}
#endif

void GLDDoubleSpinBox::setDecimals(int prec)
{
    QDoubleSpinBox::setDecimals(prec);
}

void GLDDoubleSpinBox::doSelectionChanged()
{
    emit selectionChanged();
}

void GLDDoubleSpinBox::doCursorPositionChanged(int, int)
{
    emit cursorPositionChanged();
}

void GLDSpinBox::paste()
{
    lineEdit()->paste();
}

void GLDSpinBox::deleteSelectedText()
{
    if (hasSelectedText())
    {
        lineEdit()->del();
    }
}

void GLDSpinBox::copy()
{
    lineEdit()->copy();
}

void GLDSpinBox::cut()
{
    lineEdit()->cut();
}

void GLDDoubleSpinBox::paste()
{
    lineEdit()->paste();
}

void GLDDoubleSpinBox::deleteSelectedText()
{
    if (hasSelectedText())
    {
        lineEdit()->del();
    }
}

void GLDDoubleSpinBox::copy()
{
    lineEdit()->copy();
}

GLDSpinBoxEx::GLDSpinBoxEx(QWidget *parent) :
    GLDSpinBox(parent)
{
    setObjectName("GLDSpinBoxEx");
    setButtonSymbols(UpDownArrows);
    this->setStyleSheet(loadQssFile(c_sSpinBoxQssFile));
}

void GLDSpinBoxEx::setHasBorder(bool bHasBorder)
{
    if (bHasBorder)
    {
        setProperty("border", true);
    }
    else
    {
        setProperty("border", false);
    }
}

void GLDSpinBoxEx::enterEvent(QEvent *event)
{
    setProperty("UpDownBtnBorder", true);
    this->setStyleSheet(loadQssFile(c_sSpinBoxQssFile));
    G_UNUSED(event);
}

void GLDSpinBoxEx::leaveEvent(QEvent *event)
{
    setProperty("UpDownBtnBorder", false);
    this->setStyleSheet(loadQssFile(c_sSpinBoxQssFile));
    G_UNUSED(event);
}

void GLDSpinBoxEx::resizeEvent(QResizeEvent *event)
{
    updateEditFieldGeometry();
    G_UNUSED(event);
}

void GLDSpinBoxEx::updateEditFieldGeometry()
{
    QStyleOptionSpinBox optSpinBox;
    optSpinBox.init(this);
    optSpinBox.subControls = QStyle::SC_SpinBoxFrame | QStyle::SC_SpinBoxEditField;

    QRect rect = style()->subControlRect(QStyle::CC_SpinBox, &optSpinBox, QStyle::SC_SpinBoxEditField, this);
    if (buttonSymbols() == QAbstractSpinBox::NoButtons)
    {
        rect = rect.adjusted(0, 0, 22, 0);
    }
    else{
        rect = rect.adjusted(0, 0, 6, 0);
    }

    lineEdit()->setMinimumSize(rect.width(), rect.height());
    lineEdit()->setMaximumSize(rect.width(), rect.height());
    lineEdit()->setGeometry(rect);
}

GLDDoubleSpinBoxEx::GLDDoubleSpinBoxEx(QWidget *parent) :
    GLDDoubleSpinBox(parent)
{
    setObjectName("GLDDoubleSpinBoxEx");
    setButtonSymbols(UpDownArrows);
    this->setStyleSheet(loadQssFile(c_sDoubleSpinBoxQssFile));
}

void GLDDoubleSpinBoxEx::setHasBorder(bool bHasBorder)
{
    if (bHasBorder)
    {
        setProperty("border", true);
    }
    else
    {
        setProperty("border", false);
    }
}

void GLDDoubleSpinBoxEx::enterEvent(QEvent *event)
{
    setProperty("UpDownBtnBorder", true);
    this->setStyleSheet(loadQssFile(c_sDoubleSpinBoxQssFile));
    G_UNUSED(event);
}

void GLDDoubleSpinBoxEx::leaveEvent(QEvent *event)
{
    setProperty("UpDownBtnBorder", false);
    this->setStyleSheet(loadQssFile(c_sDoubleSpinBoxQssFile));
    G_UNUSED(event);
}

void GLDDoubleSpinBoxEx::resizeEvent(QResizeEvent *event)
{
    updateEditFieldGeometry();
    G_UNUSED(event);
}

void GLDDoubleSpinBoxEx::updateEditFieldGeometry()
{
    QStyleOptionSpinBox optSpinBox;
    optSpinBox.init(this);
    optSpinBox.subControls = QStyle::SC_SpinBoxFrame | QStyle::SC_SpinBoxEditField;

    QRect rect = style()->subControlRect(QStyle::CC_SpinBox, &optSpinBox, QStyle::SC_SpinBoxEditField, this);
    if (buttonSymbols() == QAbstractSpinBox::NoButtons)
    {
        rect = rect.adjusted(0, 0, 22, 0);
    }
    else{
        rect = rect.adjusted(0, 0, 6, 0);
    }

    lineEdit()->setMinimumSize(rect.width(), rect.height());
    lineEdit()->setMaximumSize(rect.width(), rect.height());
    lineEdit()->setGeometry(rect);
}

G_GLODON_END_NAMESPACE
