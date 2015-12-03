#include "GLDCalendarWidget.h"
#include "GLDStrings.h"

#include <qapplication.h>
#include <qbasictimer.h>
#include <QListView>

#ifndef QT_NO_CALENDARWIDGET

QT_BEGIN_NAMESPACE

// 日历变量
const int c_MaxYear                           = 7999;
const int c_MaxMonth                          = 12;
const int c_MaxDay                            = 31;
const int c_MinYear                           = 1752;
const int c_daysPerMonth                      = 31;
const int c_monthsPerYear                     = 12;
const int c_daysPerWeek                       = 7;

// 布局变量
const int c_navBarWidth                       = 251;
const int c_navBarHeight                      = 28;
const int c_monthComboBoxWidth                = 48;
const int c_monthComboBoxHeight               = 20;
const int c_monthComboBoxViewHeight           = 226;
const int c_yearEditWidth                     = 48;
const int c_yearEditHeight                    = 20;
const int c_yearEditHeightErr                 = 4;
const int c_yearTextWidthErr                  = 2;
const int c_yearTextHeightErr                 = -4;
const int c_monthComboBoxWidthErr             = 2;
const int c_monthComboBoxHeightErr            = 4;
const int c_calendarWidgetWidth               = 251;
const int c_calendarWidgetHeight              = 266;
const int c_calendarBodyWidth                 = 245;
const int c_calendarBodyHeight                = 233;
const int c_calendarBodyLayoutMargin          = 8;
const int c_calendarHeadWidth                 = 226;
const int c_calendarHeadHeight                = 28;
const int c_calendarHearBtnWidth              = 33;
const int c_calendarHearBtnHeight             = 30;

QString GLDCalendarDateSectionValidator::highlightString(const QString &str, int pos) const
{
    if (pos == 0)
    {
        return QLatin1String("<b>") + str + QLatin1String("</b>");
    }
    int startPos = str.length() - pos;
    return str.mid(0, startPos) + QLatin1String("<b>") + str.mid(startPos, pos) + QLatin1String("</b>");
}

GLDCalendarDayValidator::GLDCalendarDayValidator():
    GLDCalendarDateSectionValidator(),
    m_pos(0),
    m_day(1),
    m_oldDay(1)
{

}

GLDCalendarDateSectionValidator::Section GLDCalendarDayValidator::handleKey(int key)
{
    if (key == Qt::Key_Right || key == Qt::Key_Left)
    {
        m_pos = 0;
        return GLDCalendarDateSectionValidator::ThisSection;
    }
    else if (key == Qt::Key_Up)
    {
        m_pos = 0;
        ++m_day;
        if (m_day > c_daysPerMonth)
        {
            m_day = 1;
        }
        return GLDCalendarDateSectionValidator::ThisSection;
    }
    else if (key == Qt::Key_Down)
    {
        m_pos = 0;
        --m_day;
        if (m_day < 1)
        {
            m_day = c_daysPerMonth;
        }
        return GLDCalendarDateSectionValidator::ThisSection;
    }
    else if (key == Qt::Key_Back || key == Qt::Key_Backspace)
    {
        --m_pos;
        if (m_pos < 0)
        {
            m_pos = 1;
        }

        if (m_pos == 0)
        {
            m_day = m_oldDay;
        }
        else
        {
            m_day = m_day / 10;
            //m_day = m_oldDay / 10 * 10 + m_day / 10;
        }

        if (m_pos == 0)
        {
            return GLDCalendarDateSectionValidator::PrevSection;
        }
        return GLDCalendarDateSectionValidator::ThisSection;
    }
    if (key < Qt::Key_0 || key > Qt::Key_9)
    {
        return GLDCalendarDateSectionValidator::ThisSection;
    }
    int pressedKey = key - Qt::Key_0;
    if (m_pos == 0)
    {
        m_day = pressedKey;
    }
    else
    {
        m_day = m_day % 10 * 10 + pressedKey;
    }
    if (m_day > c_daysPerMonth)
    {
        m_day = c_daysPerMonth;
    }
    ++m_pos;
    if (m_pos > 1)
    {
        m_pos = 0;
        return GLDCalendarDateSectionValidator::NextSection;
    }
    return GLDCalendarDateSectionValidator::ThisSection;
}

QDate GLDCalendarDayValidator::applyToDate(const QDate &date) const
{
    int day = m_day;
    if (day < 1)
    {
        day = 1;
    }
    else if (day > c_daysPerMonth)
    {
        day = c_daysPerMonth;
    }
    if (day > date.daysInMonth())
    {
        day = date.daysInMonth();
    }
    return QDate(date.year(), date.month(), day);
}

void GLDCalendarDayValidator::setDate(const QDate &date)
{
    m_day = m_oldDay = date.day();
    m_pos = 0;
}

QString GLDCalendarDayValidator::text() const
{
    QString str;
    if (m_day / 10 == 0)
    {
        str += QLatin1Char('0');
    }
    str += QString::number(m_day);
    return highlightString(str, m_pos);
}

QString GLDCalendarDayValidator::text(const QDate &date, int repeat) const
{
    if (repeat <= 1)
    {
        return QString::number(date.day());
    }
    else if (repeat == 2)
    {
        QString str;
        if (date.day() / 10 == 0)
        {
            str += QLatin1Char('0');
        }
        return str + QString::number(date.day());
    }
    else if (repeat == 3)
    {
        return m_locale.dayName(date.dayOfWeek(), QLocale::ShortFormat);
    }
    else if (repeat >= 4)
    {
        return m_locale.dayName(date.dayOfWeek(), QLocale::LongFormat);
    }
    return QString();
}

GLDCalendarMonthValidator::GLDCalendarMonthValidator() :
      GLDCalendarDateSectionValidator(),
      m_pos(0),
      m_month(1),
      m_oldMonth(1)
{

}

GLDCalendarDateSectionValidator::Section GLDCalendarMonthValidator::handleKey(int key)
{
    if (key == Qt::Key_Right || key == Qt::Key_Left)
    {
        m_pos = 0;
        return GLDCalendarDateSectionValidator::ThisSection;
    } else if (key == Qt::Key_Up)
    {
        m_pos = 0;
        ++m_month;
        if (m_month > c_monthsPerYear)
        {
            m_month = 1;
        }
        return GLDCalendarDateSectionValidator::ThisSection;
    }
    else if (key == Qt::Key_Down)
    {
        m_pos = 0;
        --m_month;
        if (m_month < 1)
        {
            m_month = c_monthsPerYear;
        }
        return GLDCalendarDateSectionValidator::ThisSection;
    }
    else if (key == Qt::Key_Back || key == Qt::Key_Backspace)
    {
        --m_pos;
        if (m_pos < 0)
        {
            m_pos = 1;
        }

        if (m_pos == 0)
        {
            m_month = m_oldMonth;
        }
        else
        {
            m_month = m_month / 10;
            //m_month = m_oldMonth / 10 * 10 + m_month / 10;
        }

        if (m_pos == 0)
        {
            return GLDCalendarDateSectionValidator::PrevSection;
        }
        return GLDCalendarDateSectionValidator::ThisSection;
    }
    if (key < Qt::Key_0 || key > Qt::Key_9)
    {
        return GLDCalendarDateSectionValidator::ThisSection;
    }
    int pressedKey = key - Qt::Key_0;
    if (m_pos == 0)
    {
        m_month = pressedKey;
    }
    else
    {
        m_month = m_month % 10 * 10 + pressedKey;
    }
    if (m_month > c_monthsPerYear)
    {
        m_month = c_monthsPerYear;
    }
    ++m_pos;
    if (m_pos > 1)
    {
        m_pos = 0;
        return GLDCalendarDateSectionValidator::NextSection;
    }
    return GLDCalendarDateSectionValidator::ThisSection;
}

QDate GLDCalendarMonthValidator::applyToDate(const QDate &date) const
{
    int month = m_month;
    if (month < 1)
    {
        month = 1;
    }
    else if (month > c_monthsPerYear)
    {
        month = c_monthsPerYear;
    }
    QDate newDate(date.year(), m_month, 1);
    int day = date.day();
    if (day > newDate.daysInMonth())
    {
        day = newDate.daysInMonth();
    }
    return QDate(date.year(), month, day);
}

void GLDCalendarMonthValidator::setDate(const QDate &date)
{
    m_month = m_oldMonth = date.month();
    m_pos = 0;
}

QString GLDCalendarMonthValidator::text() const
{
    QString str;
    if (m_month / 10 == 0)
    {
        str += QLatin1Char('0');
    }
    str += QString::number(m_month);
    return highlightString(str, m_pos);
}

QString GLDCalendarMonthValidator::text(const QDate &date, int repeat) const
{
    if (repeat <= 1)
    {
        return QString::number(date.month());
    }
    else if (repeat == 2)
    {
        QString str;
        if (date.month() / 10 == 0)
        {
            str += QLatin1Char('0');
        }
        return str + QString::number(date.month());
    }
    else if (repeat == 3)
    {
        return m_locale.standaloneMonthName(date.month(), QLocale::ShortFormat);
    }
    else /*if (repeat >= 4)*/
    {
        return m_locale.standaloneMonthName(date.month(), QLocale::LongFormat);
    }
}

GLDCalendarYearValidator::GLDCalendarYearValidator() :
    GLDCalendarDateSectionValidator(),
    m_pos(0),
    m_year(2000),
    m_oldYear(2000)
{

}

int GLDCalendarYearValidator::pow10(int n)
{
    int power = 1;
    for (int i = 0; i < n; i++)
    {
        power *= 10;
    }
    return power;
}

GLDCalendarDateSectionValidator::Section GLDCalendarYearValidator::handleKey(int key)
{
    if (key == Qt::Key_Right || key == Qt::Key_Left)
    {
        m_pos = 0;
        return GLDCalendarDateSectionValidator::ThisSection;
    }
    else if (key == Qt::Key_Up)
    {
        m_pos = 0;
        ++m_year;
        return GLDCalendarDateSectionValidator::ThisSection;
    }
    else if (key == Qt::Key_Down)
    {
        m_pos = 0;
        --m_year;
        return GLDCalendarDateSectionValidator::ThisSection;
    }
    else if (key == Qt::Key_Back || key == Qt::Key_Backspace)
    {
        --m_pos;
        if (m_pos < 0)
        {
            m_pos = 3;
        }

        int pow = pow10(m_pos);
        m_year = m_oldYear / pow * pow + m_year % (pow * 10) / 10;

        if (m_pos == 0)
        {
            return GLDCalendarDateSectionValidator::PrevSection;
        }
        return GLDCalendarDateSectionValidator::ThisSection;
    }
    if (key < Qt::Key_0 || key > Qt::Key_9)
    {
        return GLDCalendarDateSectionValidator::ThisSection;
    }
    int pressedKey = key - Qt::Key_0;
    int pow = pow10(m_pos);
    m_year = m_year / (pow * 10) * (pow * 10) + m_year % pow * 10 + pressedKey;
    ++m_pos;
    if (m_pos > 3)
    {
        m_pos = 0;
        return GLDCalendarDateSectionValidator::NextSection;
    }
    return GLDCalendarDateSectionValidator::ThisSection;
}

QDate GLDCalendarYearValidator::applyToDate(const QDate &date) const
{
    int year = m_year;
    if (year < 1)
    {
        year = 1;
    }
    QDate newDate(year, date.month(), 1);
    int day = date.day();
    if (day > newDate.daysInMonth())
    {
        day = newDate.daysInMonth();
    }
    return QDate(year, date.month(), day);
}

void GLDCalendarYearValidator::setDate(const QDate &date)
{
    m_year = m_oldYear = date.year();
    m_pos = 0;
}

QString GLDCalendarYearValidator::text() const
{
    QString str;
    int pow = 10;
    for (int i = 0; i < 3; i++)
    {
        if (m_year / pow == 0)
        {
            str += QLatin1Char('0');
        }
        pow *= 10;
    }
    str += QString::number(m_year);
    return highlightString(str, m_pos);
}

QString GLDCalendarYearValidator::text(const QDate &date, int repeat) const
{
    if (repeat < 4)
    {
        QString str;
        int year = date.year() % 100;
        if (year / 10 == 0)
        {
            str = QLatin1Char('0');
        }
        return str + QString::number(year);
    }
    return QString::number(date.year());
}

GLDCalendarDateValidator::GLDCalendarDateValidator() :
    m_currentToken(0),
    m_lastSectionMove(GLDCalendarDateSectionValidator::ThisSection)
{
    m_initialDate = m_currentDate = QDate::currentDate();
    m_yearValidator = new GLDCalendarYearValidator();
    m_monthValidator = new GLDCalendarMonthValidator();
    m_dayValidator = new GLDCalendarDayValidator();
}

void GLDCalendarDateValidator::setLocale(const QLocale &locale)
{
    m_yearValidator->m_locale = locale;
    m_monthValidator->m_locale = locale;
    m_dayValidator->m_locale = locale;
}

GLDCalendarDateValidator::~GLDCalendarDateValidator()
{
    delete m_yearValidator;
    delete m_monthValidator;
    delete m_dayValidator;
    clear();
}

// from qdatetime.cpp
int GLDCalendarDateValidator::countRepeat(const QString &str, int index) const
{
    Q_ASSERT(index >= 0 && index < str.size());
    int count = 1;
    const QChar ch = str.at(index);
    while (index + count < str.size() && str.at(index + count) == ch)
    {
        ++count;
    }
    return count;
}

void GLDCalendarDateValidator::setInitialDate(const QDate &date)
{
    m_yearValidator->setDate(date);
    m_monthValidator->setDate(date);
    m_dayValidator->setDate(date);
    m_initialDate = date;
    m_currentDate = date;
    m_lastSectionMove = GLDCalendarDateSectionValidator::ThisSection;
}

QString GLDCalendarDateValidator::currentText() const
{
    QString str;
    QStringListIterator itSep(m_separators);
    QListIterator<SectionToken *> itTok(m_tokens);
    while (itSep.hasNext())
    {
        str += itSep.next();
        if (itTok.hasNext())
        {
            SectionToken *token = itTok.next();
            GLDCalendarDateSectionValidator *validator = token->validator;
            if (m_currentToken == token)
            {
                str += validator->text();
            }
            else
            {
                str += validator->text(m_currentDate, token->repeat);
            }
        }
    }
    return str;
}

void GLDCalendarDateValidator::clear()
{
    QListIterator<SectionToken *> it(m_tokens);
    while (it.hasNext())
    {
        delete it.next();
    }

    m_tokens.clear();
    m_separators.clear();

    m_currentToken = 0;
}

void GLDCalendarDateValidator::setFormat(const QString &format)
{
    clear();

    int pos = 0;
    const QLatin1Char quote('\'');
    bool quoting = false;
    QString separator;
    while (pos < format.size())
    {
        QString mid = format.mid(pos);
        int offset = 1;

        if (mid.startsWith(quote))
        {
            quoting = !quoting;
        }
        else
        {
            const QChar nextChar = format.at(pos);
            if (quoting)
            {
                separator += nextChar;
            }
            else
            {
                SectionToken *token = 0;
                if (nextChar == QLatin1Char('d'))
                {
                    offset = qMin(4, countRepeat(format, pos));
                    token = new SectionToken(m_dayValidator, offset);
                }
                else if (nextChar == QLatin1Char('M'))
                {
                    offset = qMin(4, countRepeat(format, pos));
                    token = new SectionToken(m_monthValidator, offset);
                }
                else if (nextChar == QLatin1Char('y'))
                {
                    offset = qMin(4, countRepeat(format, pos));
                    token = new SectionToken(m_yearValidator, offset);
                }
                else
                {
                    separator += nextChar;
                }
                if (token)
                {
                    m_tokens.append(token);
                    m_separators.append(separator);
                    separator = QString();
                    if (!m_currentToken)
                    {
                        m_currentToken = token;
                    }

                }
            }
        }
        pos += offset;
    }
    m_separators += separator;
}

void GLDCalendarDateValidator::applyToDate()
{
    m_currentDate = m_yearValidator->applyToDate(m_currentDate);
    m_currentDate = m_monthValidator->applyToDate(m_currentDate);
    m_currentDate = m_dayValidator->applyToDate(m_currentDate);
}

void GLDCalendarDateValidator::toNextToken()
{
    const int idx = m_tokens.indexOf(m_currentToken);
    if (idx == -1)
    {
        return;
    }
    if (idx + 1 >= m_tokens.count())
    {
        m_currentToken = m_tokens.first();
    }
    else
    {
        m_currentToken = m_tokens.at(idx + 1);
    }
}

void GLDCalendarDateValidator::toPreviousToken()
{
    const int idx = m_tokens.indexOf(m_currentToken);
    if (idx == -1)
    {
        return;
    }

    if (idx - 1 < 0)
    {
        m_currentToken = m_tokens.last();
    }
    else
    {
        m_currentToken = m_tokens.at(idx - 1);
    }
}

void GLDCalendarDateValidator::handleKeyEvent(QKeyEvent *keyEvent)
{
    if (!m_currentToken)
    {
        return;
    }

    int key = keyEvent->key();
    if (m_lastSectionMove == GLDCalendarDateSectionValidator::NextSection)
    {
        if (key == Qt::Key_Back || key == Qt::Key_Backspace)
        {
            toPreviousToken();
        }
    }

    if (key == Qt::Key_Right)
    {
        toNextToken();
    }
    else if (key == Qt::Key_Left)
    {
        toPreviousToken();
    }

    m_lastSectionMove = m_currentToken->validator->handleKey(key);

    applyToDate();
    if (m_lastSectionMove == GLDCalendarDateSectionValidator::NextSection)
    {
        toNextToken();
    }
    else if (m_lastSectionMove == GLDCalendarDateSectionValidator::PrevSection)
    {
        toPreviousToken();
    }
}

QWidget *GLDCalendarTextNavigator::widget() const
{
    return m_widget;
}

void GLDCalendarTextNavigator::setWidget(QWidget *widget)
{
    m_widget = widget;
}

QDate GLDCalendarTextNavigator::date() const
{
    return m_date;
}

void GLDCalendarTextNavigator::setDate(const QDate &date)
{
    m_date = date;
}

void GLDCalendarTextNavigator::updateDateLabel()
{
    if (!m_widget)
    {
        return;
    }

    m_acceptTimer.start(m_editDelay, this);

    m_dateText->setText(m_dateValidator->currentText());

    QSize s = m_dateFrame->sizeHint();
    QRect r = m_widget->geometry(); // later, just the table section
    QRect newRect((r.width() - s.width()) / 2, (r.height() - s.height()) / 2, s.width(), s.height());
    m_dateFrame->setGeometry(newRect);
    // need to set palette after geometry update as phonestyle sets transparency
    // effect in move event.
    QPalette p = m_dateFrame->palette();
    p.setBrush(QPalette::Window, m_dateFrame->window()->palette().brush(QPalette::Window));
    m_dateFrame->setPalette(p);

    m_dateFrame->raise();
    m_dateFrame->show();
}

void GLDCalendarTextNavigator::applyDate()
{
    QDate date = m_dateValidator->currentDate();
    if (m_date == date)
    {
        return;
    }

    m_date = date;
    emit dateChanged(date);
}

void GLDCalendarTextNavigator::createDateLabel()
{
    if (m_dateFrame)
    {
        return;
    }
    m_dateFrame = new QFrame(m_widget);
    QVBoxLayout *vl = new QVBoxLayout;
    m_dateText = new QLabel;
    vl->addWidget(m_dateText);
    m_dateFrame->setLayout(vl);
    m_dateFrame->setFrameShadow(QFrame::Plain);
    m_dateFrame->setFrameShape(QFrame::Box);
    m_dateValidator = new GLDCalendarDateValidator();
    m_dateValidator->setLocale(m_widget->locale());
    m_dateValidator->setFormat(m_widget->locale().dateFormat(QLocale::ShortFormat));
    m_dateValidator->setInitialDate(m_date);

    m_dateFrame->setAutoFillBackground(true);
    m_dateFrame->setBackgroundRole(QPalette::Window);
}

void GLDCalendarTextNavigator::removeDateLabel()
{
    if (!m_dateFrame)
    {
        return;
    }

    m_acceptTimer.stop();
    m_dateFrame->hide();
    m_dateFrame->deleteLater();
    delete m_dateValidator;
    m_dateFrame = 0;
    m_dateText = 0;
    m_dateValidator = 0;
}

bool GLDCalendarTextNavigator::eventFilter(QObject *o, QEvent *e)
{
    if (m_widget)
    {
        if (e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease)
        {
            QKeyEvent* ke = (QKeyEvent*)e;
            if ((ke->text().length() > 0 && ke->text()[0].isPrint()) || m_dateFrame)
            {
                if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Select)
                {
                    applyDate();
                    emit editingFinished();
                    removeDateLabel();
                }
                else if (ke->key() == Qt::Key_Escape)
                {
                    removeDateLabel();
                }
                else if (e->type() == QEvent::KeyPress)
                {
                    createDateLabel();
                    m_dateValidator->handleKeyEvent(ke);
                    updateDateLabel();
                }
                ke->accept();
                return true;
            }
            // If we are navigating let the user finish his date in old locate.
            // If we change our mind and want it to update immediately simply uncomment below
            /*
        } else if (e->type() == QEvent::LocaleChange) {
            if (m_dateValidator) {
                m_dateValidator->setLocale(m_widget->locale());
                m_dateValidator->setFormat(m_widget->locale().dateFormat(QLocale::ShortFormat));
                updateDateLabel();
            }
            */
        }
    }
    return QObject::eventFilter(o,e);
}

void GLDCalendarTextNavigator::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == m_acceptTimer.timerId())
    {
        applyDate();
        removeDateLabel();
    }
}

int GLDCalendarTextNavigator::dateEditAcceptDelay() const
{
    return m_editDelay;
}

void GLDCalendarTextNavigator::setDateEditAcceptDelay(int delay)
{
    m_editDelay = delay;
}

GLDCalendarModel::GLDCalendarModel(const QDate &date, QObject *parent) : QAbstractTableModel(parent)
{
    m_date = date;
    m_minimumDate = QDate::fromJulianDay(1); // 当前日历能表示的最小日期
    m_maximumDate = QDate(c_MaxYear, c_MaxMonth, c_MaxDay); // 当前日历能表示的最大日期
    m_shownYear = m_date.year();
    m_shownMonth = m_date.month();
    m_firstDay = QLocale().firstDayOfWeek();
    m_horizontalHeaderFormat = GLDCalendarWidget::ShortDayNames;
    m_weekNumbersShown = true;
    m_firstColumn = 1;
    m_firstRow = 1;
    m_view = 0;
}

Qt::DayOfWeek GLDCalendarModel::dayOfWeekForColumn(int column) const
{
    int col = column - m_firstColumn;
    if (col < 0 || col > c_daysPerWeek - 1)
    {
        return Qt::Sunday;
    }
    int day = m_firstDay + col;
    if (day > c_daysPerWeek)
    {
        day -= c_daysPerWeek;
    }
    return Qt::DayOfWeek(day);
}

int GLDCalendarModel::columnForDayOfWeek(Qt::DayOfWeek day) const
{
    if (day < 1 || day > c_daysPerWeek)
    {
        return -1;
    }
    int column = (int)day - (int)m_firstDay;
    if (column < 0)
    {
        column += c_daysPerWeek;
    }
    return column + m_firstColumn;
}

/*
This simple algorithm tries to generate a valid date from the month shown.
Some months don't contain a first day (e.g. Jan of -4713 year,
so QDate (-4713, 1, 1) would be invalid). In that case we try to generate
another valid date for that month. Later, returned date's day is the number of cells
calendar widget will reserve for days before referenceDate. (E.g. if returned date's
day is 16, that day will be placed in 3rd or 4th row, not in the 1st or 2nd row).
Depending on referenceData we can change behaviour of Oct 1582. If referenceDate is 1st
of Oct we render 1 Oct in 1st or 2nd row. If referenceDate is 17 of Oct we show always 16
dates before 17 of Oct, and since this month contains the hole 5-14 Oct, the first of Oct
will be rendered in 2nd or 3rd row, showing more dates from previous month.
*/
QDate GLDCalendarModel::referenceDate() const
{
    int refDay = 1;
    while (refDay <= c_daysPerMonth)
    {
        QDate refDate(m_shownYear, m_shownMonth, refDay);
        if (refDate.isValid())
        {
            return refDate;
        }
        refDay += 1;
    }
    return QDate();
}

int GLDCalendarModel::columnForFirstOfMonth(const QDate &date) const
{
    return (columnForDayOfWeek(static_cast<Qt::DayOfWeek>(date.dayOfWeek())) - (date.day() % c_daysPerWeek) + (c_daysPerWeek + 1)) % c_daysPerWeek;
}

QDate GLDCalendarModel::dateForCell(int row, int column) const
{
    if (row < m_firstRow || row > m_firstRow + RowCount - 1 ||
                column < m_firstColumn || column > m_firstColumn + ColumnCount - 1)
    {
        return QDate();
    }
    const QDate refDate = referenceDate();
    if (!refDate.isValid())
    {
        return QDate();
    }

    const int columnForFirstOfShownMonth = columnForFirstOfMonth(refDate);
    if (columnForFirstOfShownMonth - m_firstColumn < MinimumDayOffset)
    {
        row -= 1;
    }

    const int requestedDay = c_daysPerWeek * (row - m_firstRow) + column - columnForFirstOfShownMonth - refDate.day() + 1;
    return refDate.addDays(requestedDay);
}

void GLDCalendarModel::cellForDate(const QDate &date, int *row, int *column) const
{
    if (!row && !column)
    {
        return;
    }

    if (row)
    {
        *row = -1;
    }
    if (column)
    {
        *column = -1;
    }

    const QDate refDate = referenceDate();
    if (!refDate.isValid())
    {
        return;
    }

    const int columnForFirstOfShownMonth = columnForFirstOfMonth(refDate);
    const int requestedPosition = refDate.daysTo(date) - m_firstColumn + columnForFirstOfShownMonth + refDate.day() - 1;

    int c = requestedPosition % c_daysPerWeek;
    int r = requestedPosition / c_daysPerWeek;
    if (c < 0)
    {
        c += c_daysPerWeek;
        r -= 1;
    }

    if (columnForFirstOfShownMonth - m_firstColumn < MinimumDayOffset)
    {
        r += 1;
    }

    if (r < 0 || r > RowCount - 1 || c < 0 || c > ColumnCount - 1)
    {
        return;
    }

    if (row)
    {
        *row = r + m_firstRow;
    }
    if (column)
    {
        *column = c + m_firstColumn;
    }
}

QString GLDCalendarModel::dayName(Qt::DayOfWeek day) const
{
    switch (m_horizontalHeaderFormat)
    {
    case GLDCalendarWidget::SingleLetterDayNames:
    {
        QString standaloneDayName = m_view->locale().standaloneDayName(day, QLocale::NarrowFormat);
        if (standaloneDayName == m_view->locale().dayName(day, QLocale::NarrowFormat))
        {
            return standaloneDayName.at(1);
        }
        return standaloneDayName;
    }
    case GLDCalendarWidget::ShortDayNames:
        return m_view->locale().dayName(day, QLocale::ShortFormat);
    case GLDCalendarWidget::LongDayNames:
        return m_view->locale().dayName(day, QLocale::LongFormat);
    default:
        break;
    }
    return QString();
}

QTextCharFormat GLDCalendarModel::formatForCell(int row, int col) const
{
    QPalette pal;
    QPalette::ColorGroup cg = QPalette::Active;
    if (m_view)
    {
        pal = m_view->palette();
        if (!m_view->isEnabled())
        {
            cg = QPalette::Disabled;
        }
        else if (!m_view->isActiveWindow())
        {
            cg = QPalette::Inactive;
        }
    }

    QTextCharFormat format;
    format.setFont(m_view->font());
    bool header = (m_weekNumbersShown && col == HeaderColumn)
                  || (m_horizontalHeaderFormat != GLDCalendarWidget::NoHorizontalHeader && row == HeaderRow);
    format.setBackground(pal.brush(cg, header ? QPalette::AlternateBase : QPalette::Base));
    format.setForeground(pal.brush(cg, QPalette::Text));
    if (header)
    {
        format.merge(m_headerFormat);
    }

    if (col >= m_firstColumn && col < m_firstColumn + ColumnCount)
    {
        Qt::DayOfWeek dayOfWeek = dayOfWeekForColumn(col);
        if (m_dayFormats.contains(dayOfWeek))
        {
            format.merge(m_dayFormats.value(dayOfWeek));
        }
    }

    if (!header)
    {
        QDate date = dateForCell(row, col);
        format.merge(m_dateFormats.value(date));
        if(date < m_minimumDate || date > m_maximumDate)
        {
            format.setBackground(pal.brush(cg, QPalette::Window));
        }
        if (m_shownMonth != date.month())
        {
            format.setForeground(pal.brush(QPalette::Disabled, QPalette::Text));
        }
    }
    return format;
}

QVariant GLDCalendarModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole)
    {
        return (int) Qt::AlignCenter;
    }

    int row = index.row();
    int column = index.column();

    if(role == Qt::DisplayRole)
    {
        if (m_weekNumbersShown && column == HeaderColumn
            && row >= m_firstRow && row < m_firstRow + RowCount)
        {
            QDate date = dateForCell(row, columnForDayOfWeek(Qt::Monday));
            if (date.isValid())
            {
                return date.weekNumber();
            }
        }
        if (m_horizontalHeaderFormat != GLDCalendarWidget::NoHorizontalHeader && row == HeaderRow
            && column >= m_firstColumn && column < m_firstColumn + ColumnCount)
        {
            return dayName(dayOfWeekForColumn(column));
        }
        QDate date = dateForCell(row, column);
        if (date.isValid())
        {
            return date.day();
        }
        return QString();
    }

    QTextCharFormat fmt = formatForCell(row, column);
    if (role == Qt::BackgroundColorRole)
    {
        return fmt.background().color();
    }
    if (role == Qt::TextColorRole)
    {
        return fmt.foreground().color();
    }
    if (role == Qt::FontRole)
    {
        return fmt.font();
    }
    if (role == Qt::ToolTipRole)
    {
        return fmt.toolTip();
    }
    return QVariant();
}

Qt::ItemFlags GLDCalendarModel::flags(const QModelIndex &index) const
{
    QDate date = dateForCell(index.row(), index.column());
    if (!date.isValid())
    {
        return QAbstractTableModel::flags(index);
    }
    if (date < m_minimumDate)
    {
        return 0;
    }
    if (date > m_maximumDate)
    {
        return 0;
    }
    return QAbstractTableModel::flags(index);
}

void GLDCalendarModel::setDate(const QDate &d)
{
    m_date = d;
    if (m_date < m_minimumDate)
    {
        m_date = m_minimumDate;
    }
    else if (m_date > m_maximumDate)
    {
        m_date = m_maximumDate;
    }
}

void GLDCalendarModel::showMonth(int year, int month)
{
    if (m_shownYear == year && m_shownMonth == month)
    {
        return;
    }

    m_shownYear = year;
    m_shownMonth = month;

    internalUpdate();
}

void GLDCalendarModel::setMinimumDate(const QDate &d)
{
    if (!d.isValid() || d == m_minimumDate)
    {
        return;
    }

    m_minimumDate = d;
    if (m_maximumDate < m_minimumDate)
    {
        m_maximumDate = m_minimumDate;
    }
    if (m_date < m_minimumDate)
    {
        m_date = m_minimumDate;
    }
    internalUpdate();
}

void GLDCalendarModel::setMaximumDate(const QDate &d)
{
    if (!d.isValid() || d == m_maximumDate)
    {
        return;
    }

    m_maximumDate = d;
    if (m_minimumDate > m_maximumDate)
    {
        m_minimumDate = m_maximumDate;
    }
    if (m_date > m_maximumDate)
    {
        m_date = m_maximumDate;
    }
    internalUpdate();
}

void GLDCalendarModel::setRange(const QDate &min, const QDate &max)
{
    m_minimumDate = min;
    m_maximumDate = max;
    if (m_minimumDate > m_maximumDate)
    {
        qSwap(m_minimumDate, m_maximumDate);
    }
    if (m_date < m_minimumDate)
    {
        m_date = m_minimumDate;
    }
    if (m_date > m_maximumDate)
    {
        m_date = m_maximumDate;
    }
    internalUpdate();
}

void GLDCalendarModel::internalUpdate()
{
    QModelIndex begin = index(0, 0);
    QModelIndex end = index(m_firstRow + RowCount - 1, m_firstColumn + ColumnCount - 1);
    emit dataChanged(begin, end);
    emit headerDataChanged(Qt::Vertical, 0, m_firstRow + RowCount - 1);
    emit headerDataChanged(Qt::Horizontal, 0, m_firstColumn + ColumnCount - 1);
}

void GLDCalendarModel::setHorizontalHeaderFormat(GLDCalendarWidget::HorizontalHeaderFormat format)
{
    if (m_horizontalHeaderFormat == format)
    {
        return;
    }

    int oldFormat = m_horizontalHeaderFormat;
    m_horizontalHeaderFormat = format;
    if (oldFormat == GLDCalendarWidget::NoHorizontalHeader)
    {
        m_firstRow = 1;
        insertRow(0);
    }
    else if (m_horizontalHeaderFormat == GLDCalendarWidget::NoHorizontalHeader)
    {
        m_firstRow = 0;
        removeRow(0);
    }
    internalUpdate();
}

void GLDCalendarModel::setFirstColumnDay(Qt::DayOfWeek dayOfWeek)
{
    if (m_firstDay == dayOfWeek)
    {
        return;
    }

    m_firstDay = dayOfWeek;
    internalUpdate();
}

Qt::DayOfWeek GLDCalendarModel::firstColumnDay() const
{
    return m_firstDay;
}

bool GLDCalendarModel::weekNumbersShown() const
{
    return m_weekNumbersShown;
}

void GLDCalendarModel::setWeekNumbersShown(bool show)
{
    if (m_weekNumbersShown == show)
    {
        return;
    }

    m_weekNumbersShown = show;
    if (show)
    {
        m_firstColumn = 1;
        insertColumn(0);
    }
    else
    {
        m_firstColumn = 0;
        removeColumn(0);
    }
    internalUpdate();
}

GLDCalendarView::GLDCalendarView(QWidget *parent)
    : QTableView(parent),
    m_readOnly(false),
    m_validDateClicked(false)
{
    setTabKeyNavigation(false);
    setShowGrid(false);
    verticalHeader()->setVisible(false);
    horizontalHeader()->setVisible(false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

QModelIndex GLDCalendarView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    GLDCalendarModel *calendarModel = qobject_cast<GLDCalendarModel *>(model());
    if (!calendarModel)
    {
        return QTableView::moveCursor(cursorAction, modifiers);
    }

    if (m_readOnly)
    {
        return currentIndex();
    }

    QModelIndex index = currentIndex();
    QDate currentDate = static_cast<GLDCalendarModel*>(model())->dateForCell(index.row(), index.column());
    switch (cursorAction)
    {
    case QAbstractItemView::MoveUp:
        currentDate = currentDate.addDays(- c_daysPerWeek);
        break;
    case QAbstractItemView::MoveDown:
        currentDate = currentDate.addDays(c_daysPerWeek);
        break;
    case QAbstractItemView::MoveLeft:
        currentDate = currentDate.addDays(isRightToLeft() ? 1 : -1);
        break;
    case QAbstractItemView::MoveRight:
        currentDate = currentDate.addDays(isRightToLeft() ? -1 : 1);
        break;
    case QAbstractItemView::MoveHome:
        currentDate = QDate(currentDate.year(), currentDate.month(), 1);
        break;
    case QAbstractItemView::MoveEnd:
        currentDate = QDate(currentDate.year(), currentDate.month(), currentDate.daysInMonth());
        break;
    case QAbstractItemView::MovePageUp:
        currentDate = currentDate.addMonths(-1);
        break;
    case QAbstractItemView::MovePageDown:
        currentDate = currentDate.addMonths(1);
        break;
    case QAbstractItemView::MoveNext:
    case QAbstractItemView::MovePrevious:
        return currentIndex();
    default:
        break;
    }
    emit changeDate(currentDate, true);
    return currentIndex();
}

void GLDCalendarView::keyPressEvent(QKeyEvent *event)
{
#ifdef QT_KEYPAD_NAVIGATION
    if (event->key() == Qt::Key_Select)
    {
        if (QApplication::keypadNavigationEnabled())
        {
            if (!hasEditFocus())
            {
                setEditFocus(true);
                return;
            }
        }
    }
    else if (event->key() == Qt::Key_Back)
    {
        if (QApplication::keypadNavigationEnabled() && hasEditFocus())
        {
            if (qobject_cast<GLDCalendarModel *>(model()))
            {
                emit changeDate(origDate, true); //changes selection back to origDate, but doesn't activate
                setEditFocus(false);
                return;
            }
        }
    }
#endif

    if (!m_readOnly)
    {
        switch (event->key())
        {
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Select:
            emit editingFinished();
            return;
        default:
            break;
        }
    }
    QTableView::keyPressEvent(event);
}

#ifndef QT_NO_WHEELEVENT
void GLDCalendarView::wheelEvent(QWheelEvent *event)
{
    const int numDegrees = event->delta() / 8;
    const int numSteps = numDegrees / 15;
    const QModelIndex index = currentIndex();
    QDate currentDate = static_cast<GLDCalendarModel*>(model())->dateForCell(index.row(), index.column());
    currentDate = currentDate.addMonths(-numSteps);
    emit showDate(currentDate);
}
#endif

bool GLDCalendarView::event(QEvent *event)
{
#ifdef QT_KEYPAD_NAVIGATION
    if (event->type() == QEvent::FocusIn)
    {
        if (GLDCalendarModel *calendarModel = qobject_cast<GLDCalendarModel *>(model()))
        {
            origDate = calendarModel->m_date;
        }
    }
#endif

    return QTableView::event(event);
}

QDate GLDCalendarView::handleMouseEvent(QMouseEvent *event)
{
    GLDCalendarModel *calendarModel = qobject_cast<GLDCalendarModel *>(model());
    if (!calendarModel)
    {
        return QDate();
    }

    QPoint pos = event->pos();
    QModelIndex index = indexAt(pos);
    QDate date = calendarModel->dateForCell(index.row(), index.column());
    if (date.isValid() && date >= calendarModel->m_minimumDate
            && date <= calendarModel->m_maximumDate)
    {
        return date;
    }
    return QDate();
}

void GLDCalendarView::mouseDoubleClickEvent(QMouseEvent *event)
{
    GLDCalendarModel *calendarModel = qobject_cast<GLDCalendarModel *>(model());
    if (!calendarModel)
    {
        QTableView::mouseDoubleClickEvent(event);
        return;
    }

    if (m_readOnly)
    {
        return;
    }

    QDate date = handleMouseEvent(event);
    m_validDateClicked = false;
    if (date == calendarModel->m_date && !style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick))
    {
        emit editingFinished();
    }
}

void GLDCalendarView::mousePressEvent(QMouseEvent *event)
{
    GLDCalendarModel *calendarModel = qobject_cast<GLDCalendarModel *>(model());
    if (!calendarModel)
    {
        QTableView::mousePressEvent(event);
        return;
    }

    if (m_readOnly)
    {
        return;
    }

    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    QDate date = handleMouseEvent(event);
    if (date.isValid())
    {
        m_validDateClicked = true;
        int row = -1, col = -1;
        static_cast<GLDCalendarModel *>(model())->cellForDate(date, &row, &col);
        if (row != -1 && col != -1)
        {
            selectionModel()->setCurrentIndex(model()->index(row, col), QItemSelectionModel::NoUpdate);
        }
    }
    else
    {
        m_validDateClicked = false;
        event->ignore();
    }
}

void GLDCalendarView::mouseMoveEvent(QMouseEvent *event)
{
    GLDCalendarModel *calendarModel = qobject_cast<GLDCalendarModel *>(model());
    if (!calendarModel)
    {
        QTableView::mouseMoveEvent(event);
        return;
    }

    if (m_readOnly)
    {
        return;
    }

    if (m_validDateClicked)
    {
        QDate date = handleMouseEvent(event);
        if (date.isValid())
        {
            int row = -1, col = -1;
            static_cast<GLDCalendarModel *>(model())->cellForDate(date, &row, &col);
            if (row != -1 && col != -1)
            {
                selectionModel()->setCurrentIndex(model()->index(row, col), QItemSelectionModel::NoUpdate);
            }
        }
    }
    else
    {
        event->ignore();
    }
}

void GLDCalendarView::mouseReleaseEvent(QMouseEvent *event)
{
    GLDCalendarModel *calendarModel = qobject_cast<GLDCalendarModel *>(model());
    if (!calendarModel)
    {
        QTableView::mouseReleaseEvent(event);
        return;
    }

    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    if (m_readOnly)
    {
        return;
    }

    if (m_validDateClicked)
    {
        QDate date = handleMouseEvent(event);
        if (date.isValid())
        {
            emit changeDate(date, true);
            emit clicked(date);
            if (style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick))
            {
                emit editingFinished();
            }
        }
        m_validDateClicked = false;
    }
    else
    {
        event->ignore();
    }
}

void GLDCalendarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const
{
    QDate date = calendarWidgetPrivate->m_model->dateForCell(index.row(), index.column());
    if (date.isValid())
    {
        storedOption = option;
        QRect rect = option.rect;
        calendarWidgetPrivate->paintCell(painter, rect, date);
    }
    else
    {
        QItemDelegate::paint(painter, option, index);
    }
}

void GLDCalendarDelegate::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    storedOption.rect = rect;
    int row = -1;
    int col = -1;
    calendarWidgetPrivate->m_model->cellForDate(date, &row, &col);
    QModelIndex idx = calendarWidgetPrivate->m_model->index(row, col);
    QItemDelegate::paint(painter, storedOption, idx);
}

GLDCalendarWidgetPrivate::GLDCalendarWidgetPrivate()
    : QWidgetPrivate()
{
    m_model = 0;
    m_view = 0;
    m_delegate = 0;
    m_selection = 0;
    m_navigator = 0;
    m_dateEditEnabled = false;
    m_navBarVisible = true;
    m_oldFocusPolicy = Qt::StrongFocus;
}

void GLDCalendarWidgetPrivate::setNavigatorEnabled(bool enable)
{
    Q_Q(GLDCalendarWidget);

    bool navigatorEnabled = (m_navigator->widget() != 0);
    if (enable == navigatorEnabled)
    {
        return;
    }

    if (enable)
    {
        m_navigator->setWidget(q);
        q->connect(m_navigator, SIGNAL(dateChanged(QDate)),
                q, SLOT(_q_slotChangeDate(QDate)));
        q->connect(m_navigator, SIGNAL(editingFinished()),
                q, SLOT(_q_editingFinished()));
        m_view->installEventFilter(m_navigator);
    }
    else
    {
        m_navigator->setWidget(0);
        q->disconnect(m_navigator, SIGNAL(dateChanged(QDate)),
                q, SLOT(_q_slotChangeDate(QDate)));
        q->disconnect(m_navigator, SIGNAL(editingFinished()),
                q, SLOT(_q_editingFinished()));
        m_view->removeEventFilter(m_navigator);
    }
}

void GLDCalendarWidgetPrivate::createNavigationBar(QWidget *widget)
{
    Q_Q(GLDCalendarWidget);
    m_navBarBackground = new QWidget(widget);
    m_navBarBackground->setProperty("GLDNavBackground", true);
    m_navBarBackground->setFixedSize(c_navBarWidth, c_navBarHeight);
    m_navBarBackground->setObjectName(QLatin1String("qt_calendar_navigationbar"));
    m_navBarBackground->setAutoFillBackground(true);

    m_prevMonth = new QPushButton(m_navBarBackground);
    m_nextMonth = new QPushButton(m_navBarBackground);
    updateButtonIcons();
    m_prevMonth->setAutoRepeat(true);
    m_nextMonth->setAutoRepeat(true);
    m_prevMonth->setFlat(true);
    m_nextMonth->setFlat(true);

    m_monthComboBox = new QComboBox(m_navBarBackground);
    m_monthComboBox->setView(new QListView());
    m_monthComboBox->setFixedSize(QSize(c_monthComboBoxWidth, c_monthComboBoxHeight));
    m_monthComboBox->view()->setFixedHeight(c_monthComboBoxViewHeight);
    for (int i = 1; i <= c_monthsPerYear; i++)
    {
        QString monthName(QString("%1").arg(i) + getGLDi18nStr((g_rsMonth)));
        m_monthComboBox->addItem(monthName);
    }
    m_yearButton = new GLDCalToolButton(m_navBarBackground);
    m_yearButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    m_yearButton->setAutoRaise(true);
    m_yearEdit = new QSpinBox(m_navBarBackground);
    m_yearText = new QLabel(m_navBarBackground);
    m_yearText->setText(getGLDi18nStr(g_rsYear));

    m_yearButton->setProperty("GLDYearToolBtn", true);
    m_yearEdit->setProperty("GLDYearSpinBox", true);
    m_monthComboBox->setProperty("GLDMonthComboBox", true);

    QFont font = q->font();
    font.setBold(true);
    m_yearButton->setFont(font);
    m_yearEdit->setFrame(false);
    m_yearEdit->setFixedSize(c_yearEditWidth, c_yearEditHeight);
    m_yearEdit->setMinimum(m_model->m_minimumDate.year());
    m_yearEdit->setMaximum(m_model->m_maximumDate.year());
    m_yearEdit->hide();
    m_yearText->setFont(font);
    m_spaceHolder = new QSpacerItem(0,0);

    QHBoxLayout *pheaderLayout = new QHBoxLayout;
    pheaderLayout->setMargin(0);
    pheaderLayout->setSpacing(0);
    pheaderLayout->addWidget(m_prevMonth);
    pheaderLayout->insertStretch(pheaderLayout->count());
    pheaderLayout->addWidget(m_yearButton);
    pheaderLayout->addWidget(m_yearText);
    pheaderLayout->addItem(m_spaceHolder);
    pheaderLayout->addWidget(m_monthComboBox);
    pheaderLayout->insertStretch(pheaderLayout->count());
    pheaderLayout->addWidget(m_nextMonth);
    m_navBarBackground->setLayout(pheaderLayout);

    m_yearEdit->setFocusPolicy(Qt::StrongFocus);
    m_prevMonth->setFocusPolicy(Qt::NoFocus);
    m_nextMonth->setFocusPolicy(Qt::NoFocus);
    m_yearButton->setFocusPolicy(Qt::NoFocus);
    m_monthComboBox->setFocusPolicy(Qt::NoFocus);
    m_yearText->setFocusPolicy(Qt::NoFocus);

    //set names for the header controls.
    m_prevMonth->setObjectName(QLatin1String("qt_calendar_prevmonth"));
    m_nextMonth->setObjectName(QLatin1String("qt_calendar_nextmonth"));
    m_monthComboBox->setObjectName(QLatin1String("qt_calendar_monthComboBox"));
    m_yearButton->setObjectName(QLatin1String("qt_calendar_yearbutton"));
    m_yearEdit->setObjectName(QLatin1String("qt_calendar_yearedit"));
    m_yearText->setObjectName(QLatin1String("qt_calendar_yearText"));

    showMonth(m_model->m_date.year(), m_model->m_date.month());
}

void GLDCalendarWidgetPrivate::updateButtonIcons()
{
    m_prevMonth->setIcon(QIcon(":/icons/arrowleft.png"));
    m_nextMonth->setIcon(QIcon(":/icons/arrowright.png"));
}

void GLDCalendarWidgetPrivate::updateMonthMenu()
{
    int beg = 1, end = c_monthsPerYear;
    bool prevEnabled = true;
    bool nextEnabled = true;
    if (m_model->m_shownYear == m_model->m_minimumDate.year())
    {
        beg = m_model->m_minimumDate.month();
        if (m_model->m_shownMonth == m_model->m_minimumDate.month())
        {
            prevEnabled = false;
        }
    }
    if (m_model->m_shownYear == m_model->m_maximumDate.year())
    {
        end = m_model->m_maximumDate.month();
        if (m_model->m_shownMonth == m_model->m_maximumDate.month())
        {
            nextEnabled = false;
        }
    }
    m_prevMonth->setEnabled(prevEnabled);
    m_nextMonth->setEnabled(nextEnabled);
}

void GLDCalendarWidgetPrivate::updateCurrentPage(const QDate &date)
{
    Q_Q(GLDCalendarWidget);

    QDate newDate = date;
    QDate minDate = q->minimumDate();
    QDate maxDate = q->maximumDate();
    if (minDate.isValid()&& minDate.daysTo(newDate) < 0)
    {
        newDate = minDate;
    }
    if (maxDate.isValid()&& maxDate.daysTo(newDate) > 0)
    {
        newDate = maxDate;
    }
    showMonth(newDate.year(), newDate.month());
    int row = -1, col = -1;
    m_model->cellForDate(newDate, &row, &col);
    if (row != -1 && col != -1)
    {
        m_view->selectionModel()->setCurrentIndex(m_model->index(row, col),
                                                  QItemSelectionModel::NoUpdate);
    }
}

void GLDCalendarWidgetPrivate::_q_monthChanged(int monthNum)
{
    QDate currentDate = getCurrentDate();
    QDate newDate = currentDate.addMonths(monthNum - currentDate.month() + 1);
    updateCurrentPage(newDate);
}

QDate GLDCalendarWidgetPrivate::getCurrentDate()
{
    QModelIndex index = m_view->currentIndex();
    return m_model->dateForCell(index.row(), index.column());
}

void GLDCalendarWidgetPrivate::_q_prevMonthClicked()
{
    QDate currentDate = getCurrentDate().addMonths(-1);
    updateCurrentPage(currentDate);
}

void GLDCalendarWidgetPrivate::_q_nextMonthClicked()
{
    QDate currentDate = getCurrentDate().addMonths(1);
    updateCurrentPage(currentDate);
}

void GLDCalendarWidgetPrivate::_q_yearEditingFinished()
{
    Q_Q(GLDCalendarWidget);
    int nYear = m_yearEdit->text().toInt();

    if (nYear < c_MinYear || nYear > c_MaxYear)
    {
        nYear = m_model->m_shownYear;
    }
    m_yearEdit->setValue(nYear);
    m_yearButton->setText(QString("%1").arg(nYear));
    m_yearEdit->hide();
    q->setFocusPolicy(m_oldFocusPolicy);
    qApp->removeEventFilter(q);
    m_spaceHolder->changeSize(0, 0);
    m_yearButton->show();
    QDate currentDate = getCurrentDate();
    currentDate = currentDate.addYears(nYear - currentDate.year());
    updateCurrentPage(currentDate);
    q->setFocus();
}

void GLDCalendarWidgetPrivate::_q_yearClicked()
{
    Q_Q(GLDCalendarWidget);
    //show the spinbox on top of the button
    // 4为yearEdit的y坐标偏移量
    m_yearEdit->setGeometry(m_yearButton->x(), m_yearButton->y() + c_yearEditHeightErr,
                          m_yearEdit->sizeHint().width(), m_yearButton->height());
    m_spaceHolder->changeSize(m_yearButton->width(), 0);
    m_yearButton->hide();
    m_oldFocusPolicy = q->focusPolicy();
    q->setFocusPolicy(Qt::NoFocus);
    m_yearEdit->show();
    qApp->installEventFilter(q);
    m_yearEdit->raise();
    m_yearEdit->selectAll();
    m_yearEdit->setFocus(Qt::MouseFocusReason);

    // yearText的横坐标偏移量为2， 纵坐标偏移量为-4
    m_yearText->setGeometry(m_yearEdit->x() + m_yearEdit->width() + c_yearTextWidthErr,
              m_yearEdit->y() + c_yearTextHeightErr, m_yearText->sizeHint().width(), m_yearText->height());
    // monthComboBox的横坐标偏移量为2， 纵坐标偏移量为4
    m_monthComboBox->setGeometry(m_yearText->x() + m_yearText->width() + c_monthComboBoxWidthErr,
                   m_yearText->y() + c_monthComboBoxHeightErr, m_monthComboBox->sizeHint().width(),
                   m_monthComboBox->height());
}

void GLDCalendarWidgetPrivate::showMonth(int year, int month)
{
    if (m_model->m_shownYear == year && m_model->m_shownMonth == month)
    {
        return;
    }
    Q_Q(GLDCalendarWidget);
    m_model->showMonth(year, month);
    updateNavigationBar();
    emit q->currentPageChanged(year, month);
    m_view->internalUpdate();
    m_cachedSizeHint = QSize();
    update();
    updateMonthMenu();
}

void GLDCalendarWidgetPrivate::updateNavigationBar()
{
    m_monthComboBox->setCurrentIndex(m_model->m_shownMonth - 1);
    m_yearButton->setText(QString::number(m_model->m_shownYear));
    m_yearEdit->setValue(m_model->m_shownYear);
}

void GLDCalendarWidgetPrivate::update()
{
    QDate currentDate = m_model->m_date;
    int row, column;
    m_model->cellForDate(currentDate, &row, &column);
    QModelIndex idx;
    m_selection->clear();
    if (row != -1 && column != -1)
    {
        idx = m_model->index(row, column);
        m_selection->setCurrentIndex(idx, QItemSelectionModel::SelectCurrent);
    }
}

void GLDCalendarWidgetPrivate::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    Q_Q(const GLDCalendarWidget);
    q->paintCell(painter, rect, date);
}

void GLDCalendarWidgetPrivate::_q_slotShowDate(const QDate &date)
{
    updateCurrentPage(date);
}

void GLDCalendarWidgetPrivate::_q_slotChangeDate(const QDate &date)
{
    _q_slotChangeDate(date, true);
}

void GLDCalendarWidgetPrivate::_q_slotChangeDate(const QDate &date, bool changeMonth)
{
    QDate oldDate = m_model->m_date;
    m_model->setDate(date);
    QDate newDate = m_model->m_date;
    if (changeMonth)
    {
        showMonth(newDate.year(), newDate.month());
    }
    if (oldDate != newDate)
    {
        update();
        Q_Q(GLDCalendarWidget);
        m_navigator->setDate(newDate);
        emit q->selectionChanged();
    }
}

void GLDCalendarWidgetPrivate::_q_slotPassDateToEdit(const QDate &date, bool changeMonth)
{
    Q_Q(GLDCalendarWidget);
    emit q->passDateToEdit(date);
    Q_UNUSED(changeMonth);
}

void GLDCalendarWidgetPrivate::_q_editingFinished()
{
    Q_Q(GLDCalendarWidget);
    emit q->activated(m_model->m_date);
}

/*!
    \class QCalendarWidget
    \brief The QCalendarWidget class provides a monthly based
    calendar widget allowing the user to select a date.
    \since 4.2

    \ingroup advanced
    \inmodule QtWidgets

    \image fusion-calendarwidget.png

    The widget is initialized with the current month and year, but
    QCalendarWidget provides several public slots to change the year
    and month that is shown.

    By default, today's date is selected, and the user can select a
    date using both mouse and keyboard. The currently selected date
    can be retrieved using the selectedDate() function. It is
    possible to constrain the user selection to a given date range by
    setting the minimumDate and maximumDate properties.
    Alternatively, both properties can be set in one go using the
    setDateRange() convenience slot. Set the \l selectionMode
    property to NoSelection to prohibit the user from selecting at
    all. Note that a date also can be selected programmatically using
    the setSelectedDate() slot.

    The currently displayed month and year can be retrieved using the
    monthShown() and yearShown() functions, respectively.

    A newly created calendar widget uses abbreviated day names, and
    both Saturdays and Sundays are marked in red. The calendar grid is
    not visible. The week numbers are displayed, and the first column
    day is the first day of the week for the calendar's locale.

    The notation of the days can be altered to a single letter
    abbreviations ("M" for "Monday") by setting the
    horizontalHeaderFormat property to
    QCalendarWidget::SingleLetterDayNames. Setting the same property
    to QCalendarWidget::LongDayNames makes the header display the
    complete day names. The week numbers can be removed by setting
    the verticalHeaderFormat property to
    QCalendarWidget::NoVerticalHeader.  The calendar grid can be
    turned on by setting the gridVisible property to true using the
    setGridVisible() function:

    \table
    \row \li
        \image qcalendarwidget-grid.png
    \row \li
        \snippet code/src_gui_widgets_qcalendarwidget.cpp 0
    \endtable

    Finally, the day in the first column can be altered using the
    setFirstDayOfWeek() function.

    The QCalendarWidget class also provides three signals,
    selectionChanged(), activated() and currentPageChanged() making it
    possible to respond to user interaction.

    The rendering of the headers, weekdays or single days can be
    largely customized by setting QTextCharFormat's for some special
    weekday, a special date or for the rendering of the headers.

    Only a subset of the properties in QTextCharFormat are used by the
    calendar widget. Currently, the foreground, background and font
    properties are used to determine the rendering of individual cells
    in the widget.

    \sa QDate, QDateEdit, QTextCharFormat
*/

/*!
    \enum QCalendarWidget::SelectionMode

    This enum describes the types of selection offered to the user for
    selecting dates in the calendar.

    \value NoSelection      Dates cannot be selected.
    \value SingleSelection  Single dates can be selected.

    \sa selectionMode
*/

/*!
    Constructs a calendar widget with the given \a parent.

    The widget is initialized with the current month and year, and the
    currently selected date is today.

    \sa setCurrentPage()
*/
GLDCalendarWidget::GLDCalendarWidget(QDateTime &dt, QWidget *parent) :
    QWidget(*new GLDCalendarWidgetPrivate(), parent, 0),
    m_dateTime(dt)
{
    Q_D(GLDCalendarWidget);
    setFixedSize(c_calendarWidgetWidth, c_calendarWidgetHeight);
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Window);

    QVBoxLayout *layoutV = new QVBoxLayout(this);
    layoutV->setMargin(0);
    d->m_model = new GLDCalendarModel(dt.date(), this);
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(Qt::red));
    d->m_model->m_dayFormats.insert(Qt::Saturday, fmt);
    d->m_model->m_dayFormats.insert(Qt::Sunday, fmt);
    d->m_view = new GLDCalendarView(this);
    d->m_view->setObjectName(QLatin1String("qt_calendar_calendarview"));
    d->m_view->setModel(d->m_model);
    d->m_model->setView(d->m_view);
    d->m_view->setSelectionBehavior(QAbstractItemView::SelectItems);
    d->m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    d->m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    d->m_view->horizontalHeader()->setSectionsClickable(false);
    d->m_view->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    d->m_view->verticalHeader()->setSectionsClickable(false);
    d->m_selection = d->m_view->selectionModel();
    d->createNavigationBar(this);
    d->m_view->setFrameStyle(QFrame::NoFrame);
    d->m_delegate = new GLDCalendarDelegate(d, this);
    d->m_view->setItemDelegate(d->m_delegate);
    d->update();
    d->updateNavigationBar();
    setFocusPolicy(Qt::StrongFocus);
    setFocusProxy(d->m_view);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setGridVisible(true);
    setVerticalHeaderFormat(GLDCalendarWidget::NoVerticalHeader);
    setHorizontalHeaderFormat(GLDCalendarWidget::NoHorizontalHeader);

    setWeekdayTextColor();
    setWeekendTextColor();

    QTextCharFormat headFormat;
    QFont font;
    font.setBold(true);
    headFormat.setFont(font);
    headFormat.setForeground(QBrush(QColor("#000000")));
    setHeaderTextFormat(headFormat);

    layoutV->setMargin(0);
    layoutV->setSpacing(0);
    layoutV->addWidget(d->m_navBarBackground);
    // 除了导航栏，下面的部分
    QWidget *pCalendarWidget = new QWidget(this);
    pCalendarWidget->setProperty("GLDCalendarWidget", true);
    pCalendarWidget->setFixedSize(c_calendarWidgetWidth, c_calendarWidgetHeight - 30); // 30为出去导航栏后下面的日历部分
    layoutV->addWidget(pCalendarWidget);

    QHBoxLayout *pCalendarLayout = new QHBoxLayout(pCalendarWidget);
    pCalendarLayout->setContentsMargins(0, 0, 0, 0);
    // 主要是为了实现圆角
    QWidget *pCalendarBody = new QWidget(this);
    pCalendarBody->setFixedSize(c_calendarBodyWidth, c_calendarBodyHeight);
    pCalendarBody->setProperty("GLDCalendarBody", true);
    pCalendarLayout->addWidget(pCalendarBody);

    QVBoxLayout *pCalendarBodyLayout = new QVBoxLayout(pCalendarBody);
    pCalendarBodyLayout->setContentsMargins(c_calendarBodyLayoutMargin, 0, c_calendarBodyLayoutMargin, c_calendarBodyLayoutMargin);
    // 日历头，包含了星期一~星期日
    addCalendarHead();
    pCalendarBodyLayout->addWidget(m_calendarHead);
    pCalendarBodyLayout->addWidget(d->m_view);

    d->m_navigator = new GLDCalendarTextNavigator(this);
    setDateEditEnabled(true);
    setSelectedDate(m_dateTime.date());

    connect(d->m_view, SIGNAL(showDate(QDate)),
            this, SLOT(_q_slotShowDate(QDate)));
    connect(d->m_view, SIGNAL(changeDate(QDate, bool)),
            this, SLOT(_q_slotChangeDate(QDate, bool)));
    connect(d->m_view, SIGNAL(changeDate(QDate, bool)),
            this, SLOT(_q_slotPassDateToEdit(QDate, bool)));
    connect(d->m_view, SIGNAL(clicked(QDate)),
            this, SIGNAL(clicked(QDate)));
    connect(d->m_view, SIGNAL(editingFinished()),
            this, SLOT(_q_editingFinished()));

    connect(d->m_prevMonth, SIGNAL(clicked(bool)),
            this, SLOT(_q_prevMonthClicked()));
    connect(d->m_nextMonth, SIGNAL(clicked(bool)),
            this, SLOT(_q_nextMonthClicked()));
    connect(d->m_yearButton, SIGNAL(clicked(bool)),
            this, SLOT(_q_yearClicked()));
    connect(d->m_monthComboBox, SIGNAL(activated(int)),
            this, SLOT(_q_monthChanged(int)));
    connect(d->m_yearEdit, SIGNAL(editingFinished()),
            this, SLOT(_q_yearEditingFinished()));
}

/*!
   Destroys the calendar widget.
*/
GLDCalendarWidget::~GLDCalendarWidget()
{

}

/*!
   \reimp
*/
QSize GLDCalendarWidget::sizeHint() const
{
    return minimumSizeHint();
}

/*!
   \reimp
*/
QSize GLDCalendarWidget::minimumSizeHint() const
{
    Q_D(const GLDCalendarWidget);
    if (d->m_cachedSizeHint.isValid())
    {
        return d->m_cachedSizeHint;
    }

    ensurePolished();

    int w = 0;
    int h = 0;

    int end = 53;
    int rows = RowCount + 1;
    int cols = ColumnCount + 1;

    const int marginH = (style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1) * 2;

    if (horizontalHeaderFormat() == GLDCalendarWidget::NoHorizontalHeader)
    {
        rows = RowCount;
    }
    else
    {
        for (int i = 1; i <= ColumnCount; i++)
        {
            QFontMetrics fm(d->m_model->formatForCell(0, i).font());
            w = qMax(w, fm.width(d->m_model->dayName(d->m_model->dayOfWeekForColumn(i))) + marginH);
            h = qMax(h, fm.height());
        }
    }

    if (verticalHeaderFormat() == GLDCalendarWidget::NoVerticalHeader)
    {
        cols = ColumnCount;
    }
    else
    {
        for (int i = 1; i <= RowCount; i++)
        {
            QFontMetrics fm(d->m_model->formatForCell(i, 0).font());
            for (int j = 1; j < end; j++)
            {
                w = qMax(w, fm.width(QString::number(j)) + marginH);
            }
            h = qMax(h, fm.height());
        }
    }

    QFontMetrics fm(d->m_model->formatForCell(1, 1).font());
    for (int i = 1; i <= end; i++)
    {
        w = qMax(w, fm.width(QString::number(i)) + marginH);
        h = qMax(h, fm.height());
    }

    if (d->m_view->showGrid())
    {
        // hardcoded in tableview
        w += 1;
        h += 1;
    }

    w += 1; // default column span

    h = qMax(h, d->m_view->verticalHeader()->minimumSectionSize());
    w = qMax(w, d->m_view->horizontalHeader()->minimumSectionSize());

    //add the size of the header.
    QSize headerSize(0, 0);
    if (d->m_navBarVisible)
    {
        int headerH = d->m_navBarBackground->sizeHint().height();
        int headerW = 0;

        headerW += d->m_prevMonth->sizeHint().width();
        headerW += d->m_nextMonth->sizeHint().width();

        QFontMetrics fm = d->m_monthComboBox->fontMetrics();
        int monthW = 0;
        for (int i = 1; i < c_monthsPerYear; i++)
        {
            QString monthName = QString("%1").arg(i) + getGLDi18nStr(g_rsMonth);//locale().standaloneMonthName(i, QLocale::LongFormat);
            monthW = qMax(monthW, fm.boundingRect(monthName).width());
        }
        headerW += monthW;

        fm = d->m_yearButton->fontMetrics();
        headerW += fm.boundingRect(QLatin1String("5555")).width()/* + buttonDecoMargin*/;

        headerSize = QSize(headerW, headerH);
    }
    w *= cols;
    w = qMax(headerSize.width(), w);
    h = (h * rows) + headerSize.height();
    d->m_cachedSizeHint = QSize(w, h);
    return d->m_cachedSizeHint;
}

/*!
    Paints the cell specified by the given \a date, using the given \a painter and \a rect.
*/

void GLDCalendarWidget::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    Q_D(const GLDCalendarWidget);
    d->m_delegate->paintCell(painter, rect, date);
}

/*!
    \property GLDCalendarWidget::selectedDate
    \brief the currently selected date.

    The selected date must be within the date range specified by the
    minimumDate and maximumDate properties. By default, the selected
    date is the current date.

    \sa setDateRange()
*/

QDate GLDCalendarWidget::selectedDate() const
{
    Q_D(const GLDCalendarWidget);
    return d->m_model->m_date;
}

void GLDCalendarWidget::setSelectedDate(const QDate &date)
{
    Q_D(GLDCalendarWidget);
    if (d->m_model->m_date == date && date == d->getCurrentDate())
    {
        return;
    }

    if (!date.isValid())
    {
        return;
    }

    d->m_model->setDate(date);
    d->update();
    QDate newDate = d->m_model->m_date;
    d->showMonth(newDate.year(), newDate.month());
    emit selectionChanged();
}

/*!
    Returns the year of the currently displayed month. Months are
    numbered from 1 to 12.

    \sa monthShown(), setCurrentPage()
*/

int GLDCalendarWidget::yearShown() const
{
    Q_D(const GLDCalendarWidget);
    return d->m_model->m_shownYear;
}

/*!
    Returns the currently displayed month. Months are numbered from 1 to
    12.

    \sa yearShown(), setCurrentPage()
*/

int GLDCalendarWidget::monthShown() const
{
    Q_D(const GLDCalendarWidget);
    return d->m_model->m_shownMonth;
}

/*!
    Displays the given \a month of the given \a year without changing
    the selected date. Use the setSelectedDate() function to alter the
    selected date.

    The currently displayed month and year can be retrieved using the
    monthShown() and yearShown() functions respectively.

    \sa yearShown(), monthShown(), showPreviousMonth(), showNextMonth(),
    showPreviousYear(), showNextYear()
*/

void GLDCalendarWidget::setCurrentPage(int year, int month)
{
    Q_D(GLDCalendarWidget);
    QDate currentDate = d->getCurrentDate();
    int day = currentDate.day();
    int daysInMonths = QDate(year, month, 1).daysInMonth();
    if (day > daysInMonths)
    {
        day = daysInMonths;
    }

    d->showMonth(year, month);

    QDate newDate(year, month, day);
    int row = -1, col = -1;
    d->m_model->cellForDate(newDate, &row, &col);
    if (row != -1 && col != -1)
    {
        d->m_view->selectionModel()->setCurrentIndex(d->m_model->index(row, col),
                                                  QItemSelectionModel::NoUpdate);
    }
}

/*!
    Shows the next month relative to the currently displayed
    month. Note that the selected date is not changed.

    \sa showPreviousMonth(), setCurrentPage(), setSelectedDate()
*/

void GLDCalendarWidget::showNextMonth()
{
    int year = yearShown();
    int month = monthShown();
    if (month == c_monthsPerYear)
    {
        ++year;
        month = 1;
    }
    else
    {
        ++month;
    }
    setCurrentPage(year, month);
}

/*!
    Shows the previous month relative to the currently displayed
    month. Note that the selected date is not changed.

    \sa showNextMonth(), setCurrentPage(), setSelectedDate()
*/
void GLDCalendarWidget::showPreviousMonth()
{
    int year = yearShown();
    int month = monthShown();
    if (month == 1)
    {
        --year;
        month = c_monthsPerYear;
    }
    else
    {
        --month;
    }
    setCurrentPage(year, month);
}

/*!
    Shows the currently displayed month in the \e next year relative
    to the currently displayed year. Note that the selected date is
    not changed.

    \sa showPreviousYear(), setCurrentPage(), setSelectedDate()
*/
void GLDCalendarWidget::showNextYear()
{
    int year = yearShown();
    int month = monthShown();
    ++year;
    setCurrentPage(year, month);
}

/*!
    Shows the currently displayed month in the \e previous year
    relative to the currently displayed year. Note that the selected
    date is not changed.

    \sa showNextYear(), setCurrentPage(), setSelectedDate()
*/
void GLDCalendarWidget::showPreviousYear()
{
    int year = yearShown();
    int month = monthShown();
    --year;
    setCurrentPage(year, month);
}

/*!
    Shows the month of the selected date.

    \sa selectedDate(), setCurrentPage()
*/
void GLDCalendarWidget::showSelectedDate()
{
    QDate currentDate = selectedDate();
    setCurrentPage(currentDate.year(), currentDate.month());
}

/*!
    Shows the month of the today's date.

    \sa selectedDate(), setCurrentPage()
*/
void GLDCalendarWidget::showToday()
{
    QDate currentDate = QDate::currentDate();
    setCurrentPage(currentDate.year(), currentDate.month());
}

void GLDCalendarWidget::dateTimeChanged(const QDateTime &dateTime)
{
    setSelectedDate(QDate(dateTime.date().year(), dateTime.date().month(), dateTime.date().day()));
}

/*!
    \property GLDCalendarWidget::minimumDate
    \brief the minimum date of the currently specified date range.

    The user will not be able to select a date that is before the
    currently set minimum date.

    \table
    \row
    \li \image qcalendarwidget-minimum.png
    \row
    \li
    \snippet code/src_gui_widgets_qcalendarwidget.cpp 1
    \endtable

    By default, the minimum date is the earliest date that the QDate
    class can handle.

    When setting a minimum date, the maximumDate and selectedDate
    properties are adjusted if the selection range becomes invalid. If
    the provided date is not a valid QDate object, the
    setMinimumDate() function does nothing.

    \sa setDateRange()
*/
QDate GLDCalendarWidget::minimumDate() const
{
    Q_D(const GLDCalendarWidget);
    return d->m_model->m_minimumDate;
}

void GLDCalendarWidget::setMinimumDate(const QDate &date)
{
    Q_D(GLDCalendarWidget);
    if (!date.isValid() || d->m_model->m_minimumDate == date)
    {
        return;
    }

    QDate oldDate = d->m_model->m_date;
    d->m_model->setMinimumDate(date);
    d->m_yearEdit->setMinimum(d->m_model->m_minimumDate.year());
    d->updateMonthMenu();
    QDate newDate = d->m_model->m_date;
    if (oldDate != newDate)
    {
        d->update();
        d->showMonth(newDate.year(), newDate.month());
        d->m_navigator->setDate(newDate);
        emit selectionChanged();
    }
}

/*!
    \property GLDCalendarWidget::maximumDate
    \brief the maximum date of the currently specified date range.

    The user will not be able to select a date which is after the
    currently set maximum date.

    \table
    \row
    \li \image qcalendarwidget-maximum.png
    \row
    \li
    \snippet code/src_gui_widgets_qcalendarwidget.cpp 2
    \endtable

    By default, the maximum date is the last day the QDate class can
    handle.

    When setting a maximum date, the minimumDate and selectedDate
    properties are adjusted if the selection range becomes invalid. If
    the provided date is not a valid QDate object, the
    setMaximumDate() function does nothing.

    \sa setDateRange()
*/
QDate GLDCalendarWidget::maximumDate() const
{
    Q_D(const GLDCalendarWidget);
    return d->m_model->m_maximumDate;
}

void GLDCalendarWidget::setMaximumDate(const QDate &date)
{
    Q_D(GLDCalendarWidget);
    if (!date.isValid() || d->m_model->m_maximumDate == date)
    {
        return;
    }

    QDate oldDate = d->m_model->m_date;
    d->m_model->setMaximumDate(date);
    d->m_yearEdit->setMaximum(d->m_model->m_maximumDate.year());
    d->updateMonthMenu();
    QDate newDate = d->m_model->m_date;
    if (oldDate != newDate)
    {
        d->update();
        d->showMonth(newDate.year(), newDate.month());
        d->m_navigator->setDate(newDate);
        emit selectionChanged();
    }
}

/*!
    Defines a date range by setting the minimumDate and maximumDate
    properties.

    The date range restricts the user selection, i.e. the user can
    only select dates within the specified date range. Note that

    \snippet code/src_gui_widgets_qcalendarwidget.cpp 3

    is analogous to

    \snippet code/src_gui_widgets_qcalendarwidget.cpp 4

    If either the \a min or \a max parameters are not valid QDate
    objects, this function does nothing.

    \sa setMinimumDate(), setMaximumDate()
*/
void GLDCalendarWidget::setDateRange(const QDate &min, const QDate &max)
{
    Q_D(GLDCalendarWidget);
    if (d->m_model->m_minimumDate == min && d->m_model->m_maximumDate == max)
    {
        return;
    }
    if (!min.isValid() || !max.isValid())
    {
        return;
    }

    QDate oldDate = d->m_model->m_date;
    d->m_model->setRange(min, max);
    d->m_yearEdit->setMinimum(d->m_model->m_minimumDate.year());
    d->m_yearEdit->setMaximum(d->m_model->m_maximumDate.year());
    d->updateMonthMenu();
    QDate newDate = d->m_model->m_date;
    if (oldDate != newDate)
    {
        d->update();
        d->showMonth(newDate.year(), newDate.month());
        d->m_navigator->setDate(newDate);
        emit selectionChanged();
    }
}

/*! \enum GLDCalendarWidget::HorizontalHeaderFormat

    This enum type defines the various formats the horizontal header can display.

    \value SingleLetterDayNames The header displays a single letter abbreviation for day names (e.g. M for Monday).
    \value ShortDayNames The header displays a short abbreviation for day names (e.g. Mon for Monday).
    \value LongDayNames The header displays complete day names (e.g. Monday).
    \value NoHorizontalHeader The header is hidden.

    \sa horizontalHeaderFormat(), VerticalHeaderFormat
*/

/*!
    \property GLDCalendarWidget::horizontalHeaderFormat
    \brief the format of the horizontal header.

    The default value is GLDCalendarWidget::ShortDayNames.
*/

void GLDCalendarWidget::setHorizontalHeaderFormat(GLDCalendarWidget::HorizontalHeaderFormat format)
{
    Q_D(GLDCalendarWidget);
    if (d->m_model->m_horizontalHeaderFormat == format)
    {
        return;
    }

    d->m_model->setHorizontalHeaderFormat(format);
    d->m_cachedSizeHint = QSize();
    d->m_view->viewport()->update();
    d->m_view->updateGeometry();
}

GLDCalendarWidget::HorizontalHeaderFormat GLDCalendarWidget::horizontalHeaderFormat() const
{
    Q_D(const GLDCalendarWidget);
    return d->m_model->m_horizontalHeaderFormat;
}

/*!
    \enum GLDCalendarWidget::VerticalHeaderFormat

    This enum type defines the various formats the vertical header can display.

    \value ISOWeekNumbers The header displays ISO week numbers as described by \l QDate::weekNumber().
    \value NoVerticalHeader The header is hidden.

    \sa verticalHeaderFormat(), HorizontalHeaderFormat
*/

/*!
    \property GLDCalendarWidget::verticalHeaderFormat
    \brief the format of the vertical header.

    The default value is GLDCalendarWidget::ISOWeekNumber.
*/

GLDCalendarWidget::VerticalHeaderFormat GLDCalendarWidget::verticalHeaderFormat() const
{
    Q_D(const GLDCalendarWidget);
    bool shown = d->m_model->weekNumbersShown();
    if (shown)
    {
        return GLDCalendarWidget::ISOWeekNumbers;
    }
    return GLDCalendarWidget::NoVerticalHeader;
}

void GLDCalendarWidget::setVerticalHeaderFormat(GLDCalendarWidget::VerticalHeaderFormat format)
{
    Q_D(GLDCalendarWidget);
    bool show = false;
    if (format == GLDCalendarWidget::ISOWeekNumbers)
    {
        show = true;
    }
    if (d->m_model->weekNumbersShown() == show)
    {
        return;
    }
    d->m_model->setWeekNumbersShown(show);
    d->m_cachedSizeHint = QSize();
    d->m_view->viewport()->update();
    d->m_view->updateGeometry();
}

/*!
    \property GLDCalendarWidget::gridVisible
    \brief whether the table grid is displayed.

    \table
    \row
        \li \inlineimage qcalendarwidget-grid.png
    \row
        \li
        \snippet code/src_gui_widgets_qcalendarwidget.cpp 5
    \endtable

    The default value is false.
*/

bool GLDCalendarWidget::isGridVisible() const
{
    Q_D(const GLDCalendarWidget);
    return d->m_view->showGrid();
}

void GLDCalendarWidget::setGridVisible(bool show)
{
    Q_D(GLDCalendarWidget);
    d->m_view->setShowGrid(show);
    d->m_cachedSizeHint = QSize();
    d->m_view->viewport()->update();
    d->m_view->updateGeometry();
}

/*!
    \property GLDCalendarWidget::selectionMode
    \brief the type of selection the user can make in the calendar

    When this property is set to SingleSelection, the user can select a date
    within the minimum and maximum allowed dates, using either the mouse or
    the keyboard.

    When the property is set to NoSelection, the user will be unable to select
    dates, but they can still be selected programmatically. Note that the date
    that is selected when the property is set to NoSelection will still be
    the selected date of the calendar.

    The default value is SingleSelection.
*/
GLDCalendarWidget::SelectionMode GLDCalendarWidget::selectionMode() const
{
    Q_D(const GLDCalendarWidget);
    return d->m_view->m_readOnly ? GLDCalendarWidget::NoSelection : GLDCalendarWidget::SingleSelection;
}

void GLDCalendarWidget::setSelectionMode(SelectionMode mode)
{
    Q_D(GLDCalendarWidget);
    d->m_view->m_readOnly = (mode == GLDCalendarWidget::NoSelection);
    d->setNavigatorEnabled(isDateEditEnabled() && (selectionMode() != GLDCalendarWidget::NoSelection));
    d->update();
}

/*!
    \property GLDCalendarWidget::firstDayOfWeek
    \brief a value identifying the day displayed in the first column.

    By default, the day displayed in the first column
    is the first day of the week for the calendar's locale.
*/
void GLDCalendarWidget::setFirstDayOfWeek(Qt::DayOfWeek dayOfWeek)
{
    Q_D(GLDCalendarWidget);
    if ((Qt::DayOfWeek)d->m_model->firstColumnDay() == dayOfWeek)
    {
        return;
    }

    d->m_model->setFirstColumnDay(dayOfWeek);
    d->update();
}

Qt::DayOfWeek GLDCalendarWidget::firstDayOfWeek() const
{
    Q_D(const GLDCalendarWidget);
    return (Qt::DayOfWeek)d->m_model->firstColumnDay();
}

/*!
    Returns the text char format for rendering the header.
*/
QTextCharFormat GLDCalendarWidget::headerTextFormat() const
{
    Q_D(const GLDCalendarWidget);
    return d->m_model->m_headerFormat;
}

/*!
    Sets the text char format for rendering the header to \a format.
    If you also set a weekday text format, this format's foreground and
    background color will take precedence over the header's format.
    The other formatting information will still be decided by
    the header's format.
*/
void GLDCalendarWidget::setHeaderTextFormat(const QTextCharFormat &format)
{
    Q_D(GLDCalendarWidget);
    d->m_model->m_headerFormat = format;
    d->m_cachedSizeHint = QSize();
    d->m_view->viewport()->update();
    d->m_view->updateGeometry();
}

/*!
    Returns the text char format for rendering of day in the week \a dayOfWeek.

    \sa headerTextFormat()
*/
QTextCharFormat GLDCalendarWidget::weekdayTextFormat(Qt::DayOfWeek dayOfWeek) const
{
    Q_D(const GLDCalendarWidget);
    return d->m_model->m_dayFormats.value(dayOfWeek);
}

/*!
    Sets the text char format for rendering of day in the week \a dayOfWeek to \a format.
    The format will take precedence over the header format in case of foreground
    and background color. Other text formatting information is taken from the headers format.

    \sa setHeaderTextFormat()
*/
void GLDCalendarWidget::setWeekdayTextFormat(Qt::DayOfWeek dayOfWeek, const QTextCharFormat &format)
{
    Q_D(GLDCalendarWidget);
    d->m_model->m_dayFormats[dayOfWeek] = format;
    d->m_cachedSizeHint = QSize();
    d->m_view->viewport()->update();
    d->m_view->updateGeometry();
}

/*!
    Returns a QMap from QDate to QTextCharFormat showing all dates
    that use a special format that alters their rendering.
*/
QMap<QDate, QTextCharFormat> GLDCalendarWidget::dateTextFormat() const
{
    Q_D(const GLDCalendarWidget);
    return d->m_model->m_dateFormats;
}

/*!
    Returns a QTextCharFormat for \a date. The char format can be be
    empty if the date is not renderd specially.
*/
QTextCharFormat GLDCalendarWidget::dateTextFormat(const QDate &date) const
{
    Q_D(const GLDCalendarWidget);
    return d->m_model->m_dateFormats.value(date);
}

/*!
    Sets the format used to render the given \a date to that specified by \a format.

    If \a date is null, all date formats are cleared.
*/
void GLDCalendarWidget::setDateTextFormat(const QDate &date, const QTextCharFormat &format)
{
    Q_D(GLDCalendarWidget);
    if (date.isNull())
    {
        d->m_model->m_dateFormats.clear();
    }
    else
    {
        d->m_model->m_dateFormats[date] = format;
    }
    d->m_view->viewport()->update();
    d->m_view->updateGeometry();
}

/*!
    \property GLDCalendarWidget::dateEditEnabled
    \brief whether the date edit popup is enabled
    \since 4.3

    If this property is enabled, pressing a non-modifier key will cause a
    date edit to popup if the calendar widget has focus, allowing the user
    to specify a date in the form specified by the current locale.

    By default, this property is enabled.

    The date edit is simpler in appearance than QDateEdit, but allows the
    user to navigate between fields using the left and right cursor keys,
    increment and decrement individual fields using the up and down cursor
    keys, and enter values directly using the number keys.

    \sa GLDCalendarWidget::dateEditAcceptDelay
*/
bool GLDCalendarWidget::isDateEditEnabled() const
{
    Q_D(const GLDCalendarWidget);
    return d->m_dateEditEnabled;
}

void GLDCalendarWidget::setDateEditEnabled(bool enable)
{
    Q_D(GLDCalendarWidget);
    if (isDateEditEnabled() == enable)
    {
        return;
    }

    d->m_dateEditEnabled = enable;

    d->setNavigatorEnabled(enable && (selectionMode() != GLDCalendarWidget::NoSelection));
}

/*!
    \property GLDCalendarWidget::dateEditAcceptDelay
    \brief the time an inactive date edit is shown before its contents are accepted
    \since 4.3

    If the calendar widget's \l{dateEditEnabled}{date edit is enabled}, this
    property specifies the amount of time (in millseconds) that the date edit
    remains open after the most recent user input. Once this time has elapsed,
    the date specified in the date edit is accepted and the popup is closed.

    By default, the delay is defined to be 1500 milliseconds (1.5 seconds).
*/
int GLDCalendarWidget::dateEditAcceptDelay() const
{
    Q_D(const GLDCalendarWidget);
    return d->m_navigator->dateEditAcceptDelay();
}

void GLDCalendarWidget::setDateEditAcceptDelay(int delay)
{
    Q_D(GLDCalendarWidget);
    d->m_navigator->setDateEditAcceptDelay(delay);
}

void GLDCalendarWidget::addCalendarHead()
{
    m_calendarHead = new QWidget(this);
    m_calendarHead->setFixedSize(QSize(c_calendarHeadWidth, c_calendarHeadHeight));
    m_calendarHead->setProperty("GLDCalendarHead", true);

    QHBoxLayout *pHeadLayout = new QHBoxLayout(m_calendarHead);
    pHeadLayout->setSpacing(0);
    pHeadLayout->setContentsMargins(0, 0, 0, 0);
    m_calendarHead->setLayout(pHeadLayout);

    QStringList weekdaysList;
    if (locale().firstDayOfWeek() == Qt::Sunday)
    {
        weekdaysList << getGLDi18nStr(g_rsSunDay) << getGLDi18nStr(g_rsMonDay)
                     << getGLDi18nStr(g_rsTueDay) << getGLDi18nStr(g_rsWedDay)
                     << getGLDi18nStr(g_rsThuDay) << getGLDi18nStr(g_rsFriDay)
                     << getGLDi18nStr(g_rsSatDay);
    }
    else if (locale().firstDayOfWeek() == Qt::Monday)
    {
        weekdaysList << getGLDi18nStr(g_rsMonDay) << getGLDi18nStr(g_rsTueDay)
                     << getGLDi18nStr(g_rsWedDay) << getGLDi18nStr(g_rsThuDay)
                     << getGLDi18nStr(g_rsFriDay) << getGLDi18nStr(g_rsSatDay)
                     << getGLDi18nStr(g_rsSunDay) ;
    }

    foreach (QString weedday, weekdaysList)
    {
        addButtonToHeader(weedday, pHeadLayout);
    }
}

void GLDCalendarWidget::addButtonToHeader(QString weekday, QHBoxLayout *headLayout)
{
    QPushButton *pWeekdayBtn = new QPushButton(weekday);
    pWeekdayBtn->setDisabled(true);
    pWeekdayBtn->setProperty("GLDHeadBtn", true);
    pWeekdayBtn->setFixedSize(QSize(c_calendarHearBtnWidth, c_calendarHearBtnHeight));
    pWeekdayBtn->setFlat(true);
    headLayout->addWidget(pWeekdayBtn);
}

void GLDCalendarWidget::setWeekdayTextColor(const QColor &foreColor, const QColor &backColor)
{
    QTextCharFormat weekdayFormat;
    weekdayFormat.setForeground(QBrush(foreColor));
    weekdayFormat.setBackground(QBrush(backColor));

    setWeekdayTextFormat(Qt::Monday, weekdayFormat);
    setWeekdayTextFormat(Qt::Tuesday, weekdayFormat);
    setWeekdayTextFormat(Qt::Wednesday, weekdayFormat);
    setWeekdayTextFormat(Qt::Thursday, weekdayFormat);
    setWeekdayTextFormat(Qt::Friday, weekdayFormat);
}

void GLDCalendarWidget::setWeekendTextColor(const QColor &foreColor, const QColor &backColor)
{
    QTextCharFormat weekendFormat;
    weekendFormat.setForeground(QBrush(foreColor));
    weekendFormat.setBackground(QBrush(backColor));

    setWeekdayTextFormat(Qt::Saturday, weekendFormat);
    setWeekdayTextFormat(Qt::Sunday, weekendFormat);
}

/*!
    \since 4.4

    Updates the cell specified by the given \a date unless updates
    are disabled or the cell is hidden.

    \sa updateCells(), yearShown(), monthShown()
*/
void GLDCalendarWidget::updateCell(const QDate &date)
{
    if (!date.isValid())
    {
        qWarning("GLDCalendarWidget::updateCell: Invalid date");
        return;
    }

    if (!isVisible())
        return;

    Q_D(GLDCalendarWidget);
    int row, column;
    d->m_model->cellForDate(date, &row, &column);
    if (row == -1 || column == -1)
    {
        return;
    }

    QModelIndex modelIndex = d->m_model->index(row, column);
    if (!modelIndex.isValid())
        return;

    d->m_view->viewport()->update(d->m_view->visualRect(modelIndex));
}

/*!
    \since 4.4

    Updates all visible cells unless updates are disabled.

    \sa updateCell()
*/
void GLDCalendarWidget::updateCells()
{
    Q_D(GLDCalendarWidget);
    if (isVisible())
    {
        d->m_view->viewport()->update();
    }
}

/*!
    \fn void GLDCalendarWidget::selectionChanged()

    This signal is emitted when the currently selected date is
    changed.

    The currently selected date can be changed by the user using the
    mouse or keyboard, or by the programmer using setSelectedDate().

    \sa selectedDate()
*/

/*!
    \fn void GLDCalendarWidget::currentPageChanged(int year, int month)

    This signal is emitted when the currently shown month is changed.
    The new \a year and \a month are passed as parameters.

    \sa setCurrentPage()
*/

/*!
    \fn void GLDCalendarWidget::activated(const QDate &date)

    This signal is emitted whenever the user presses the Return or
    Enter key or double-clicks a \a date in the calendar
    widget.
*/

/*!
    \fn void GLDCalendarWidget::clicked(const QDate &date)

    This signal is emitted when a mouse button is clicked. The date
    the mouse was clicked on is specified by \a date. The signal is
    only emitted when clicked on a valid date, e.g., dates are not
    outside the minimumDate() and maximumDate(). If the selection mode
    is NoSelection, this signal will not be emitted.

*/

/*!
    \property GLDCalendarWidget::navigationBarVisible
    \brief whether the navigation bar is shown or not

    \since 4.3

    When this property is \c true (the default), the next month,
    previous month, month selection, year selection controls are
    shown on top.

    When the property is set to false, these controls are hidden.
*/
bool GLDCalendarWidget::isNavigationBarVisible() const
{
    Q_D(const GLDCalendarWidget);
    return d->m_navBarVisible;
}


void GLDCalendarWidget::setNavigationBarVisible(bool visible)
{
    Q_D(GLDCalendarWidget);
    d->m_navBarVisible = visible;
    d->m_cachedSizeHint = QSize();
    d->m_navBarBackground->setVisible(visible);
    updateGeometry();
}

/*!
  \reimp
*/
bool GLDCalendarWidget::event(QEvent *event)
{
    Q_D(GLDCalendarWidget);
    switch (event->type())
    {
    case QEvent::LayoutDirectionChange:
        d->updateButtonIcons();
    case QEvent::LocaleChange:
        d->m_model->setFirstColumnDay(locale().firstDayOfWeek());
        d->m_cachedSizeHint = QSize();
        d->updateNavigationBar();
        d->m_view->updateGeometry();
        break;
    case QEvent::FontChange:
    case QEvent::ApplicationFontChange:
        d->m_cachedSizeHint = QSize();
        d->m_view->updateGeometry();
        break;
    case QEvent::StyleChange:
        d->m_cachedSizeHint = QSize();
        d->m_view->updateGeometry();
    default:
        break;
    }
    return QWidget::event(event);
}

/*!
  \reimp
*/
bool GLDCalendarWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(GLDCalendarWidget);
    if (event->type() == QEvent::MouseButtonPress && d->m_yearEdit->hasFocus())
    {
        QWidget *tlw = window();
        QWidget *widget = static_cast<QWidget*>(watched);
        //as we have a event filter on the whole application we first make sure that the top level widget
        //of both this and the watched widget are the same to decide if we should finish the year edition.
        if (widget->window() == tlw)
        {
            QPoint mousePos = widget->mapTo(tlw, static_cast<QMouseEvent *>(event)->pos());
            QRect geom = QRect(d->m_yearEdit->mapTo(tlw, QPoint(0, 0)), d->m_yearEdit->size());
            if (!geom.contains(mousePos))
            {
                event->accept();
                d->_q_yearEditingFinished();
                setFocus();
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

/*!
  \reimp
*/
void GLDCalendarWidget::mousePressEvent(QMouseEvent *event)
{
    setAttribute(Qt::WA_NoMouseReplay);
    QWidget::mousePressEvent(event);
    setFocus();
}

/*!
  \reimp
*/
void GLDCalendarWidget::resizeEvent(QResizeEvent * event)
{
    Q_D(GLDCalendarWidget);

    // XXX Should really use a QWidgetStack for yearEdit and yearButton,
    // XXX here we hide the year edit when the layout is likely to break
    // XXX the manual positioning of the yearEdit over the yearButton.
    if(d->m_yearEdit->isVisible() && event->size().width() != event->oldSize().width())
    {
        d->_q_yearEditingFinished();
    }

    QWidget::resizeEvent(event);
}

/*!
  \reimp
*/
void GLDCalendarWidget::keyPressEvent(QKeyEvent * event)
{
    Q_D(GLDCalendarWidget);
    if(d->m_yearEdit->isVisible()&& event->key() == Qt::Key_Escape)
    {
        d->m_yearEdit->setValue(yearShown());
        d->_q_yearEditingFinished();
        return;
    }
    QWidget::keyPressEvent(event);
}

QT_END_NAMESPACE

#endif //QT_NO_CALENDARWIDGET
