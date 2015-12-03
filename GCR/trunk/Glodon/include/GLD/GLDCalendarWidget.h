#ifndef GLDCALENDARWIDGET_H
#define GLDCALENDARWIDGET_H

#include <qabstractitemmodel.h>
#include <QComboBox>
#include <qdatetime.h>
#include <qevent.h>
#include <qheaderview.h>
#include <qitemdelegate.h>
#include <qlabel.h>
#include <qlayout.h>
#include <QLocale>
#include <qmenu.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qstylepainter.h>
#include <qtableview.h>
#include <qtoolbutton.h>
#include <QtCore/qbasictimer.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qobject.h>
#include <qtextformat.h>
#include <QtWidgets/qwidget.h>
#include <private/qwidget_p.h>
#include "GLDWidget_Global.h"

const QString c_weekdayTextForeColor        = "#585858";
const QString c_weekdayTextBackColor        = "#fbfbfb";
const QString c_weekendTextForeColor        = "#585858";
const QString c_weekendTextBackColor        = "#efefef";

#ifndef QT_NO_CALENDARWIDGET

QT_BEGIN_NAMESPACE

enum {
    RowCount = 6,
    ColumnCount = 7,
    HeaderColumn = 0,
    HeaderRow = 0,
    MinimumDayOffset = 1
};

class QDate;
class QTextCharFormat;

class GLDWIDGETSHARED_EXPORT GLDCalendarDateSectionValidator
{
public:

    enum Section {
        NextSection,
        ThisSection,
        PrevSection
    };

    GLDCalendarDateSectionValidator() {}
    virtual ~GLDCalendarDateSectionValidator() {}
    virtual Section handleKey(int key) = 0;
    virtual QDate applyToDate(const QDate &date) const = 0;
    virtual void setDate(const QDate &date) = 0;
    virtual QString text() const = 0;
    virtual QString text(const QDate &date, int repeat) const = 0;

    QLocale m_locale;

protected:
    QString highlightString(const QString &str, int pos) const;
};

class GLDWIDGETSHARED_EXPORT GLDCalendarDayValidator : public GLDCalendarDateSectionValidator
{
public:
    GLDCalendarDayValidator();
    virtual Section handleKey(int key);
    virtual QDate applyToDate(const QDate &date) const;
    virtual void setDate(const QDate &date);
    virtual QString text() const;
    virtual QString text(const QDate &date, int repeat) const;
private:
    int m_pos;
    int m_day;
    int m_oldDay;
};

class GLDWIDGETSHARED_EXPORT GLDCalendarMonthValidator : public GLDCalendarDateSectionValidator
{
public:
    GLDCalendarMonthValidator();
    virtual Section handleKey(int key);
    virtual QDate applyToDate(const QDate &date) const;
    virtual void setDate(const QDate &date);
    virtual QString text() const;
    virtual QString text(const QDate &date, int repeat) const;
private:
    int m_pos;
    int m_month;
    int m_oldMonth;
};

class GLDWIDGETSHARED_EXPORT GLDCalendarYearValidator : public GLDCalendarDateSectionValidator
{
public:
    GLDCalendarYearValidator();
    virtual Section handleKey(int key);
    virtual QDate applyToDate(const QDate &date) const;
    virtual void setDate(const QDate &date);
    virtual QString text() const;
    virtual QString text(const QDate &date, int repeat) const;
private:
    int pow10(int n);
    int m_pos;
    int m_year;
    int m_oldYear;
};

class GLDWIDGETSHARED_EXPORT GLDCalendarDateValidator
{
public:
    GLDCalendarDateValidator();
    ~GLDCalendarDateValidator();

    void handleKeyEvent(QKeyEvent *keyEvent);
    QString currentText() const;
    QDate currentDate() const { return m_currentDate; }
    void setFormat(const QString &format);
    void setInitialDate(const QDate &date);

    void setLocale(const QLocale &locale);

private:
    struct SectionToken
    {
        SectionToken(GLDCalendarDateSectionValidator *val, int rep) : validator(val), repeat(rep) {}
        GLDCalendarDateSectionValidator *validator;
        int repeat;
    };

    void toNextToken();
    void toPreviousToken();
    void applyToDate();

    int countRepeat(const QString &str, int index) const;
    void clear();

    QStringList m_separators;
    QList<SectionToken *> m_tokens;
    GLDCalendarDateSectionValidator *m_yearValidator;
    GLDCalendarDateSectionValidator *m_monthValidator;
    GLDCalendarDateSectionValidator *m_dayValidator;

    SectionToken *m_currentToken;

    QDate m_initialDate;
    QDate m_currentDate;

    GLDCalendarDateSectionValidator::Section m_lastSectionMove;
};

class GLDWIDGETSHARED_EXPORT GLDCalendarView : public QTableView
{
    Q_OBJECT
public:
    GLDCalendarView(QWidget *parent = 0);

    void internalUpdate() { updateGeometries(); }
    void setReadOnly(bool enable);
    virtual void keyboardSearch(const QString & search) { Q_UNUSED(search) }

signals:
    void showDate(const QDate &date);
    void changeDate(const QDate &date, bool changeMonth);
    void clicked(const QDate &date);
    void editingFinished();
protected:
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event);
#endif
    void keyPressEvent(QKeyEvent *event);
    bool event(QEvent *event);

    QDate handleMouseEvent(QMouseEvent *event);
public:
    bool m_readOnly;
private:
    bool m_validDateClicked;
#ifdef QT_KEYPAD_NAVIGATION
    QDate origDate;
#endif
};

class GLDCalendarWidgetPrivate;
class GLDWIDGETSHARED_EXPORT GLDCalendarDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    GLDCalendarDelegate(GLDCalendarWidgetPrivate *w, QObject *parent = 0)
        : QItemDelegate(parent), calendarWidgetPrivate(w)
            { }
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;
    void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;

private:
    GLDCalendarWidgetPrivate *calendarWidgetPrivate;
    mutable QStyleOptionViewItem storedOption;
};

//Private tool button class
class GLDWIDGETSHARED_EXPORT GLDCalToolButton: public QToolButton
{
public:
    GLDCalToolButton(QWidget * parent)
        : QToolButton(parent)
          {  }
protected:
    void paintEvent(QPaintEvent *e)
    {
        Q_UNUSED(e)

#ifndef Q_WS_MAC
        QStyleOptionToolButton opt;
        initStyleOption(&opt);

        if (opt.state & QStyle::State_MouseOver || isDown()) {
            //act as normal button
            setPalette(QPalette());
        } else {
            //set the highlight color for button text
            QPalette toolPalette = palette();
            toolPalette.setColor(QPalette::ButtonText, toolPalette.color(QPalette::HighlightedText));
            setPalette(toolPalette);
        }
#endif
        QToolButton::paintEvent(e);
    }
};

class GLDWIDGETSHARED_EXPORT GLDPrevNextCalButton : public QToolButton
{
    Q_OBJECT
public:
    GLDPrevNextCalButton(QWidget *parent) : QToolButton(parent) {}
protected:
    void paintEvent(QPaintEvent *) {
        QStylePainter painter(this);
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        opt.state &= ~QStyle::State_HasFocus;
        painter.drawComplexControl(QStyle::CC_ToolButton, opt);
    }
};

class QLabel;
class GLDCalendarDateValidator;
class QFrame;
class GLDWIDGETSHARED_EXPORT GLDCalendarTextNavigator: public QObject
{
    Q_OBJECT
public:
    GLDCalendarTextNavigator(QObject *parent = 0)
        : QObject(parent), m_dateText(0), m_dateFrame(0), m_dateValidator(0), m_widget(0), m_editDelay(1500), m_date(QDate::currentDate()) { }

    QWidget *widget() const;
    void setWidget(QWidget *widget);

    int dateEditAcceptDelay() const;
    void setDateEditAcceptDelay(int delay);

    QDate date() const;
    void setDate(const QDate &date);

    bool eventFilter(QObject *o, QEvent *e);
    void timerEvent(QTimerEvent *e);

signals:
    void dateChanged(const QDate &date);
    void editingFinished();

private:
    void applyDate();
    void updateDateLabel();
    void createDateLabel();
    void removeDateLabel();

    QLabel *m_dateText;
    QFrame *m_dateFrame;
    QBasicTimer m_acceptTimer;
    GLDCalendarDateValidator *m_dateValidator;
    QWidget *m_widget;
    int m_editDelay;

    QDate m_date;
};

class GLDWIDGETSHARED_EXPORT GLDCalendarWidget : public QWidget
{
    Q_OBJECT
    Q_ENUMS(Qt::DayOfWeek)
    Q_ENUMS(HorizontalHeaderFormat)
    Q_ENUMS(VerticalHeaderFormat)
    Q_ENUMS(SelectionMode)
    Q_PROPERTY(QDate selectedDate READ selectedDate WRITE setSelectedDate)
    Q_PROPERTY(QDate minimumDate READ minimumDate WRITE setMinimumDate)
    Q_PROPERTY(QDate maximumDate READ maximumDate WRITE setMaximumDate)
    Q_PROPERTY(Qt::DayOfWeek firstDayOfWeek READ firstDayOfWeek WRITE setFirstDayOfWeek)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible)
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(HorizontalHeaderFormat horizontalHeaderFormat READ horizontalHeaderFormat WRITE setHorizontalHeaderFormat)
    Q_PROPERTY(VerticalHeaderFormat verticalHeaderFormat READ verticalHeaderFormat WRITE setVerticalHeaderFormat)
    Q_PROPERTY(bool navigationBarVisible READ isNavigationBarVisible WRITE setNavigationBarVisible)
    Q_PROPERTY(bool dateEditEnabled READ isDateEditEnabled WRITE setDateEditEnabled)
    Q_PROPERTY(int dateEditAcceptDelay READ dateEditAcceptDelay WRITE setDateEditAcceptDelay)
public:
    enum HorizontalHeaderFormat {
        NoHorizontalHeader,
        SingleLetterDayNames,
        ShortDayNames,
        LongDayNames
    };

    enum VerticalHeaderFormat {
        NoVerticalHeader,
        ISOWeekNumbers
    };

    enum SelectionMode {
        NoSelection,
        SingleSelection
    };

    explicit GLDCalendarWidget(QDateTime &dt, QWidget *parent = 0);
    ~GLDCalendarWidget();

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

    QDate selectedDate() const;

    int yearShown() const;
    int monthShown() const;

    QDate minimumDate() const;
    void setMinimumDate(const QDate &date);

    QDate maximumDate() const;
    void setMaximumDate(const QDate &date);

    Qt::DayOfWeek firstDayOfWeek() const;
    void setFirstDayOfWeek(Qt::DayOfWeek dayOfWeek);

    bool isNavigationBarVisible() const;
    bool isGridVisible() const;

    SelectionMode selectionMode() const;
    void setSelectionMode(SelectionMode mode);

    HorizontalHeaderFormat horizontalHeaderFormat() const;
    void setHorizontalHeaderFormat(HorizontalHeaderFormat format);

    VerticalHeaderFormat verticalHeaderFormat() const;
    void setVerticalHeaderFormat(VerticalHeaderFormat format);

    QTextCharFormat headerTextFormat() const;
    void setHeaderTextFormat(const QTextCharFormat &format);

    QTextCharFormat weekdayTextFormat(Qt::DayOfWeek dayOfWeek) const;
    void setWeekdayTextFormat(Qt::DayOfWeek dayOfWeek, const QTextCharFormat &format);

    QMap<QDate, QTextCharFormat> dateTextFormat() const;
    QTextCharFormat dateTextFormat(const QDate &date) const;
    void setDateTextFormat(const QDate &date, const QTextCharFormat &format);

    bool isDateEditEnabled() const;
    void setDateEditEnabled(bool enable);

    int dateEditAcceptDelay() const;
    void setDateEditAcceptDelay(int delay);

    void addCalendarHead();

    void addButtonToHeader(QString weekday, QHBoxLayout *headLayout);

    void setWeekdayTextColor(const QColor &foreColor = QColor(c_weekdayTextForeColor),
                             const QColor &backColor = QColor(c_weekdayTextBackColor));
    void setWeekendTextColor(const QColor &foreColor = QColor(c_weekendTextForeColor),
                             const QColor &backColor = QColor(c_weekendTextBackColor));

protected:
    bool event(QEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent * event);
    void keyPressEvent(QKeyEvent * event);

    virtual void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;
    void updateCell(const QDate &date);
    void updateCells();

public Q_SLOTS:
    void setSelectedDate(const QDate &date);
    void setDateRange(const QDate &min, const QDate &max);
    void setCurrentPage(int year, int month);
    void setGridVisible(bool show);
    void setNavigationBarVisible(bool visible);
    void showNextMonth();
    void showPreviousMonth();
    void showNextYear();
    void showPreviousYear();
    void showSelectedDate();
    void showToday();
    void dateTimeChanged(const QDateTime &dateTime);

Q_SIGNALS:
    void selectionChanged();
    void clicked(const QDate &date);
    void activated(const QDate &date);
    void currentPageChanged(int year, int month);
    void passDateToEdit(const QDate &date);

private:
    QWidget *m_calendarHead;
    QDateTime m_dateTime;

private:
    Q_DECLARE_PRIVATE(GLDCalendarWidget)
    Q_DISABLE_COPY(GLDCalendarWidget)

    Q_PRIVATE_SLOT(d_func(), void _q_slotShowDate(const QDate &date))
    Q_PRIVATE_SLOT(d_func(), void _q_slotChangeDate(const QDate &date))
    Q_PRIVATE_SLOT(d_func(), void _q_slotChangeDate(const QDate &date, bool changeMonth))
    Q_PRIVATE_SLOT(d_func(), void _q_slotPassDateToEdit(const QDate &date, bool changeMonth))
    Q_PRIVATE_SLOT(d_func(), void _q_editingFinished())
    Q_PRIVATE_SLOT(d_func(), void _q_prevMonthClicked())
    Q_PRIVATE_SLOT(d_func(), void _q_nextMonthClicked())
    Q_PRIVATE_SLOT(d_func(), void _q_yearEditingFinished())
    Q_PRIVATE_SLOT(d_func(), void _q_yearClicked())
    Q_PRIVATE_SLOT(d_func(), void _q_monthChanged(int monthNum))
};

class GLDWIDGETSHARED_EXPORT GLDCalendarModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    GLDCalendarModel(const QDate &date = QDate::currentDate(), QObject *parent = 0);

    int rowCount(const QModelIndex &) const
        { return RowCount + m_firstRow; }
    int columnCount(const QModelIndex &) const
        { return ColumnCount + m_firstColumn; }
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex())
    {
        beginInsertRows(parent, row, row + count - 1);
        endInsertRows();
        return true;
    }

    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex())
    {
        beginInsertColumns(parent, column, column + count - 1);
        endInsertColumns();
        return true;
    }

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex())
    {
        beginRemoveRows(parent, row, row + count - 1);
        endRemoveRows();
        return true;
    }

    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex())
    {
        beginRemoveColumns(parent, column, column + count - 1);
        endRemoveColumns();
        return true;
    }

    void showMonth(int year, int month);
    void setDate(const QDate &d);

    void setMinimumDate(const QDate &date);
    void setMaximumDate(const QDate &date);

    void setRange(const QDate &min, const QDate &max);

    void setHorizontalHeaderFormat(GLDCalendarWidget::HorizontalHeaderFormat format);

    void setFirstColumnDay(Qt::DayOfWeek dayOfWeek);
    Qt::DayOfWeek firstColumnDay() const;

    bool weekNumbersShown() const;
    void setWeekNumbersShown(bool show);

    QTextCharFormat formatForCell(int row, int col) const;
    Qt::DayOfWeek dayOfWeekForColumn(int section) const;
    int columnForDayOfWeek(Qt::DayOfWeek day) const;
    QDate dateForCell(int row, int column) const;
    void cellForDate(const QDate &date, int *row, int *column) const;
    QString dayName(Qt::DayOfWeek day) const;

    void setView(GLDCalendarView *view)
        { m_view = view; }

    void internalUpdate();
    QDate referenceDate() const;
    int columnForFirstOfMonth(const QDate &date) const;

    int m_firstColumn;
    int m_firstRow;
    QDate m_date;
    QDate m_minimumDate;
    QDate m_maximumDate;
    int m_shownYear;
    int m_shownMonth;
    Qt::DayOfWeek m_firstDay;
    GLDCalendarWidget::HorizontalHeaderFormat m_horizontalHeaderFormat;
    bool m_weekNumbersShown;
    QMap<Qt::DayOfWeek, QTextCharFormat> m_dayFormats;
    QMap<QDate, QTextCharFormat> m_dateFormats;
    QTextCharFormat m_headerFormat;
    GLDCalendarView *m_view;
};

class GLDWIDGETSHARED_EXPORT GLDCalendarWidgetPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(GLDCalendarWidget)
public:
    GLDCalendarWidgetPrivate();

    void showMonth(int year, int month);
    void update();
    void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;

    void _q_slotShowDate(const QDate &date);
    void _q_slotChangeDate(const QDate &date);
    void _q_slotChangeDate(const QDate &date, bool changeMonth);
    void _q_slotPassDateToEdit(const QDate &date, bool changeMonth);
    void _q_editingFinished();
    void _q_monthChanged(int monthNum);
    void _q_prevMonthClicked();
    void _q_nextMonthClicked();
    void _q_yearEditingFinished();
    void _q_yearClicked();

    void createNavigationBar(QWidget *widget);
    void updateButtonIcons();
    void updateMonthMenu();
    void updateNavigationBar();
    void updateMonthMenuNames();
    void updateCurrentPage(const QDate &newDate);
    inline QDate getCurrentDate();
    void setNavigatorEnabled(bool enable);

    GLDCalendarModel *m_model;
    GLDCalendarView *m_view;
    GLDCalendarDelegate *m_delegate;
    QItemSelectionModel *m_selection;
    GLDCalendarTextNavigator *m_navigator;
    bool m_dateEditEnabled;

    QPushButton *m_nextMonth;
    QPushButton *m_prevMonth;
    QComboBox *m_monthComboBox;
    GLDCalToolButton *m_yearButton;
    QSpinBox *m_yearEdit;
    QWidget *m_navBarBackground;
    QSpacerItem *m_spaceHolder;
    QLabel *m_yearText;
    bool m_navBarVisible;
    mutable QSize m_cachedSizeHint;
    Qt::FocusPolicy m_oldFocusPolicy;
};

QT_END_NAMESPACE

#endif // QT_NO_CALENDARWIDGET

#endif // GLDCALENDARWIDGET_H
