#include "GLDDateTimeEditEx.h"

#include <QFile>
#include <QDate>
#include <QPainter>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>

#include "GLDWidget_Global.h"
#include "GLDShadow.h"
#include "GLDStrUtils.h"
#include "GLDFileUtils.h"

const QString c_sDateTimeQssFile               = ":/qsses/GLDDateTimeEditEx.qss";

const char *c_objectName               = "GLDDateTimeEditEx";
const char *c_hasPopup                 = "GLDHasPopup";
const char *c_hasBorderStyle           = "GLDBorderStyle";
const char *c_noBorder                 = "noBorder";
const char *c_upDownBtnBorder          = "UpDownBtnBorder";

const char *c_hint                     = QT_TRANSLATE_NOOP("GLDDateTimeEditEx", "HINT"); // 提示
const char *c_dateLessThanSysDate      = QT_TRANSLATE_NOOP("GLDDateTimeEditEx", "The selecting dateTime is before the minimum date"); // 您所选择的日期小于最小日期
const char *c_dateLargerThanSysDate    = QT_TRANSLATE_NOOP("GLDDateTimeEditEx", "The selecting dateTime is after the maximum date"); // 您所选择的日期大于最大日期
const char *c_excludeLetter            = QT_TRANSLATE_NOOP("GLDDateTimeEditEx", "The datetime can not include letters"); // 日期不能包含字母
const char *c_errorDate                = QT_TRANSLATE_NOOP("GLDDateTimeEditEx", "You have input a wrong datetime, please input it again"); // 您的日期输入方式有误，请重新输入

GLDDateTimeEditEx::GLDDateTimeEditEx(const QDateTime &dt, QWidget *parent) :
    GLDDateTimeEdit(dt, parent),
    m_isCalendarPopup(false),
    m_calendarWidget(NULL),
    m_hasBorder(true),
    m_curKeyIsEnter(false),
    m_buttonType(None),
    m_justKeyPressed(false),
    m_curPos(0),
    m_keyPressing(false),
    m_xOffset(0),
    m_yOffset(0)
{
    if (!dt.isValid())
    {
        m_dateTime = QDateTime::currentDateTime();
    }
    else if(dt < minimumDateTime())
    {
        emit inputInvalid(true);
        dateLessThanSysDateMessageBox();
        m_dateTime = QDateTime::currentDateTime();
    }
    else if(dt > maximumDateTime())
    {
        emit inputInvalid(true);
        dateLargerThanSysDateMessageBox();
        m_dateTime = QDateTime::currentDateTime();
    }
    else
    {
        m_dateTime = dt;
    }
    setDateTime(m_dateTime);
    setObjectName(c_objectName);
    setProperty(c_hasPopup, false);
    this->setStyleSheet(loadQssFile(qssFilePath(c_sDateTimeQssFile)));
    GLDShadow *pShadow = new GLDShadow(this);
    pShadow->removeShadow();
}

GLDDateTimeEditEx::~GLDDateTimeEditEx()
{

}

QString GLDDateTimeEditEx::plainText()
{
    return parseText(lineEdit()->text());
}

bool GLDDateTimeEditEx::calendarPopup() const
{
    return m_isCalendarPopup;
}

void GLDDateTimeEditEx::setCalendarPopup(bool enable)
{
    m_isCalendarPopup = enable;
    setProperty(c_hasPopup, enable);
    if (enable)
    {
        m_buttonType = ComboBoxArrow;
        initCalendarPopup();
        m_calendarWidget->setFocus();
    }
    this->setStyleSheet(loadQssFile(qssFilePath(c_sDateTimeQssFile)));
}

void GLDDateTimeEditEx::setHasBorder(const bool bValue)
{
    m_hasBorder = bValue;
    if (!bValue)
    {
        setProperty(c_hasBorderStyle, c_noBorder);
    }
    this->setStyleSheet(loadQssFile(qssFilePath(c_sDateTimeQssFile)));
}

const QString GLDDateTimeEditEx::qssFilePath(const QString &path)
{
    return path;
}

void GLDDateTimeEditEx::excludeLetterMessageBox()
{
    QMessageBox::information(this, tr(c_hint), tr(c_excludeLetter));
}

void GLDDateTimeEditEx::errorDateMessageBox()
{
    QMessageBox::information(this, tr(c_hint), tr(c_errorDate));
}

void GLDDateTimeEditEx::dateLessThanSysDateMessageBox()
{
    QMessageBox::information(this, tr(c_hint), tr(c_dateLessThanSysDate));
}

void GLDDateTimeEditEx::dateLargerThanSysDateMessageBox()
{
    QMessageBox::information(this, tr(c_hint), tr(c_dateLargerThanSysDate));
}

void GLDDateTimeEditEx::updateValue()
{
    m_curKeyIsEnter = true;
    if (lineEdit()->text().trimmed().isEmpty())
    {
        return;
    }
    QString strDateTimeValue = parseText(lineEdit()->text());

    //QDateTime中的合法性验证利用了lineEdit的字符串长度,所以需要将字符串设置入控件中
    GString strOriginValue = lineEdit()->text();

    lineEdit()->setText(strDateTimeValue);

    if (!strDateTimeValue.isEmpty())
    {
        if (0 != strOriginValue.compare(dateTime().toString(displayFormat()))
                &&0 == strDateTimeValue.compare(dateTime().toString(displayFormat())))
        {
            QDateTime dateTime = QDateTime::fromString(strDateTimeValue, displayFormat());
            selectDateTime(dateTime);
            return;
        }
        if (0 != strDateTimeValue.compare(dateTime().toString(displayFormat())))
        {
            for (int i = 0; i < strDateTimeValue.length(); ++i)
            {
                if (strDateTimeValue.at(i).isLetter())
                {
                    emit inputInvalid(true);
                    excludeLetterMessageBox();
                    selectDateTime(QDateTime::currentDateTime());
                    return;
                }
                QValidator::State state;
                state = QDateTimeEdit::validate(strDateTimeValue, i);

                if (QValidator::Acceptable != state)
                {
                    emit inputInvalid(true);
                    errorDateMessageBox();
                    selectDateTime(QDateTime::currentDateTime());
                    return;
                }
            }

            QDateTime dateTime = QDateTime::fromString(strDateTimeValue, displayFormat());
            selectDateTime(dateTime);
            return;
        }
    }
}

QString GLDDateTimeEditEx::displayFormat() const
{
    return GLDDateTimeEdit::displayFormat();
}

void GLDDateTimeEditEx::setDisplayFormat(const QString &format)
{
    GLDDateTimeEdit::setDisplayFormat(format);
}

void GLDDateTimeEditEx::setCalendarPopupOffset(int xOffset, int yOffset)
{
    m_xOffset = xOffset;
    m_yOffset = yOffset;
}

void GLDDateTimeEditEx::keyPressEvent(QKeyEvent *event)
{
    m_curKeyIsEnter = false;
    switch (event->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        updateValue();
        break;
    default:
        // 编辑框在全删除完日期的第一次输入，会在keyPressEvent时回调dateTimeFromText和textFromDateTime函数，
        // 在focusOutEvent将值设到lineEdit上,而之后输入日期的时，会在keyPressEvent时回调dateTimeFromText，
        // keyPressEvent执行完毕再去回调textFromDateTime函数
        m_justKeyPressed = false;
        m_keyPressing = true;
        QAbstractSpinBox::keyPressEvent(event);
        m_curStr = lineEdit()->text();
        m_curPos = lineEdit()->cursorPosition();
        m_justKeyPressed = true;
        m_keyPressing = false;
        break;
    }
}

void GLDDateTimeEditEx::mousePressEvent(QMouseEvent *event)
{
    if (!calendarPopup())
    {
        GLDDateTimeEdit::mousePressEvent(event);
        return;
    }

    if (ComboBoxArrow == m_buttonType
            && NULL != style()
            && NULL != style()->proxy()
            )
    {
        QStyleOptionComboBox optCombo;
        optCombo.init(this);
        QRect subRect = style()->proxy()->subControlRect(QStyle::CC_ComboBox, &optCombo,
                        QStyle::SC_ComboBoxArrow, this);
        event->accept();
        positionCalendarPopup();
        if (subRect.contains(event->pos()))
        {
            bool bValue = true;
            emit onshowPopup(bValue);
            if (bValue)
            {
                showPopUp();
            }
        }
    }
    else
    {
        GLDDateTimeEdit::mousePressEvent(event);
    }
}

void GLDDateTimeEditEx::focusOutEvent(QFocusEvent *event)
{
    QAbstractSpinBox::focusOutEvent(event);
    if (m_justKeyPressed)
    {
        lineEdit()->setText(m_curStr);
        lineEdit()->setCursorPosition(m_curPos);
    }
    if (!m_curKeyIsEnter)
    {
        updateValue();
    }
}

void GLDDateTimeEditEx::paintEvent(QPaintEvent *event)
{
    GLDDateTimeEdit::paintEvent(event);
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

    // 鼠标位于箭头所在区域高亮，其他地方不高亮
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

    if (ComboBoxArrow == m_buttonType && NULL != style())
    {
        style()->drawComplexControl(QStyle::CC_ComboBox, &optCombo, &tmpPt, this);
    }
}

void GLDDateTimeEditEx::enterEvent(QEvent *event)
{
    // 处理无弹出框的日历控件进入上下微调按钮的特效
    if (None == m_buttonType)
    {
        setProperty(c_upDownBtnBorder, true);
    }
    this->setStyleSheet(loadQssFile(qssFilePath(c_sDateTimeQssFile)));
    G_UNUSED(event);
}

void GLDDateTimeEditEx::leaveEvent(QEvent *event)
{
    // 处理无弹出框的日历控件进入上下微调按钮的特效
    if (None == m_buttonType)
    {
        setProperty(c_upDownBtnBorder, false);
    }
    this->setStyleSheet(loadQssFile(qssFilePath(c_sDateTimeQssFile)));
    G_UNUSED(event);
}

QDateTime GLDDateTimeEditEx::dateTimeFromText(const QString &text) const
{
    // 不带日历弹出框的时间控件
    if (None == m_buttonType)
    {
        return QDateTimeEdit::dateTimeFromText(text);
    }

    // 带日历弹出框的时间控件
    m_lastStrForDatetime = text;
    if (!text.isNull() && text.length() > 0)
    {
        return QDateTime::fromString(text, displayFormat());
    }
    return QDateTime::currentDateTime();
}

QString GLDDateTimeEditEx::textFromDateTime(const QDateTime &dt) const
{
    // 不带日历弹出框的时间控件
    if (None == m_buttonType)
    {
        return locale().toString(dt, displayFormat());
    }

    // 带日历弹出框的时间控件
    // 编辑框删除后第二次之后的输入由此进入
    if (m_justKeyPressed)
    {
        return m_curStr;
    }
    // 编辑框删除后第一次输入由此进入
    else if (m_keyPressing)
    {
        return m_lastStrForDatetime;
    }
    return locale().toString(dt, displayFormat());
}

QValidator::State GLDDateTimeEditEx::validate(QString &input, int &pos) const
{
    // 不带日历弹出框的时间控件
    if (None == m_buttonType)
    {
        return QDateTimeEdit::validate(input, pos);
    }

    // 带日历弹出框的时间控件
    // 当前的key是QKey_Enter和QKey_Return时，执行校验
    if (m_curKeyIsEnter)
    {
        return QDateTimeEdit::validate(input, pos);
    }
    QValidator::State state = QValidator::Acceptable;
    return state;
}

void GLDDateTimeEditEx::initCalendarPopup()
{
    if (NULL == m_calendarWidget)
    {
        m_calendarWidget = new GLDCalendarWidget(m_dateTime, this);
        m_calendarWidget->setWindowFlags(Qt::Popup);
    }
    connect(m_calendarWidget, &GLDCalendarWidget::clicked,
            this, &GLDDateTimeEditEx::selectDate);
    connect(m_calendarWidget, &GLDCalendarWidget::activated,
            this, &GLDDateTimeEditEx::selectDate);
    connect(m_calendarWidget, &GLDCalendarWidget::passDateToEdit,
            this, &GLDDateTimeEditEx::setDate);
    // 日历控件发出信号，只要编辑框的日期时间变化，就发出信号，弹出框GLDCalendarWidget去响应这种变化
    connect(this, &GLDDateTimeEditEx::dateTimeChanged,
            m_calendarWidget, &GLDCalendarWidget::dateTimeChanged);
    this->setStyleSheet(loadQssFile(qssFilePath(c_sDateTimeQssFile)));
}

void GLDDateTimeEditEx::selectDate(const QDate &date)
{
    if (!date.isValid())
    {
        return;
    }
    QDateTime dateTime(date, QTime::currentTime());
    selectDateTime(dateTime);
}

void GLDDateTimeEditEx::selectDateTime(const QDateTime &dateTime)
{
    // 此处设置两变量为false在于让textFromDateTime由默认的分支执行
    m_justKeyPressed  = false;
    m_keyPressing = false;
    if (!dateTime.isValid())
    {
        return;
    }
    setDateTime(dateTime);
    hidePopUp();
}

 // 定位弹出框的位置
void GLDDateTimeEditEx::positionCalendarPopup()
{
    QPoint pos = (layoutDirection() == Qt::RightToLeft) ? rect().bottomRight() :
                                                          rect().bottomLeft();
    QPoint pos2 = (layoutDirection() == Qt::RightToLeft) ? rect().topRight() :
                                                           rect().topLeft();

    pos = mapToGlobal(pos);
    pos2 = mapToGlobal(pos2);

    QSize size = m_calendarWidget->sizeHint();
    QRect screen = QApplication::desktop()->availableGeometry(pos);

    //handle popup falling off screen
    if (Qt::RightToLeft == layoutDirection())
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
    }
    pos.setX(pos.x() + m_xOffset);
    pos.setY(pos.y() + m_yOffset);
    m_calendarWidget->move(pos);
}

void GLDDateTimeEditEx::showPopUp()
{
    if (NULL != m_calendarWidget && m_calendarWidget->isVisible())
    {
        return;
    }
    m_calendarWidget->show();
}

void GLDDateTimeEditEx::hidePopUp()
{
    if (NULL != m_calendarWidget && m_calendarWidget->isVisible())
    {
        m_calendarWidget->hide();
    }
}

QStyle::SubControl GLDDateTimeEditEx::newHoverControl(const QPoint &pos)
{
    QStyleOptionComboBox optCombo;
    optCombo.init(this);
    optCombo.editable = true;
    optCombo.subControls = QStyle::SC_ComboBoxArrow;
    return style()->hitTestComplexControl(QStyle::CC_ComboBox, &optCombo, pos, this);
}

QString GLDDateTimeEditEx::parseText(const QString &input)
{
    if (input.trimmed().isEmpty())
    {
        return input;
    }

    QString text = formatDate(input.trimmed());
    QString format = displayFormat();
    // 如果是系统默认格式， 不做解析，直接返回
    if (format.endsWith("yyyy/M/d H:mm"))
    {
        return input;
    }

    // 对比用户设置格式与用户当前输入的每一位，发现格式匹配不上，在单位数前补0
    if (!format.isEmpty() && format.length() > 0)
    {
        int delta = 0;
        // 进行到最后一位，用户输入的日期较用户设置格式位数少，需要做跳出操作
        for (int i = 0; i < format.length(); ++i)
        {
            if (i == text.length())
            {
                text.insert(i - 1, '0');
                break;
            }

            // 发现用户设置格式当前位为字母，用户输入日期当前位不为数字，对用户输入日期当前位的前一位补0
            if (format.at(i).isLetter() && !text.at(i).isDigit())
            {
                text.insert(i - 1, '0');
                ++delta;
            }
        }
    }

    return text;
}

QString GLDDateTimeEditEx::formatDate(const QString &input)
{
    QString text = input;

    if (!displayFormat().contains("yy") && !displayFormat().contains("MM")
            && !displayFormat().contains("dd"))
    {
        return text;
    }

    const GString c_sSpliterString = "-";
    text.replace(".", "-");
    text.replace("/", "-");
    text.replace("y", "-");
    text.replace("m", "-");
    text.replace("d", "");
    // "年"
    text.replace(tr("GLD_Year"), "-");
    // "月"
    text.replace(tr("GLD_Month"), "-");
    // "日"
    text.replace(tr("GLD_Day"), "");

    int nSplitSymbolNumber = 0;
    for (int i = 0; i < text.length(); ++i)
    {
        if(sameText(text.at(i), c_sSpliterString))
        {
            nSplitSymbolNumber++;
        }
    }

    if (nSplitSymbolNumber == 1)
    {
        QString sCurrentYear = QString::number(QDateTime::currentDateTime().date().year());
        text  = sCurrentYear + "-" + text;
    }

    if (nSplitSymbolNumber == 0)
    {
        QString sCurrentYear = QString::number(QDateTime::currentDateTime().date().year());
        QString sCurrentMonth = QString::number(QDateTime::currentDateTime().date().month());
        text  = sCurrentYear + "-" + sCurrentMonth + "-" + text;
    }
    return text;
}
