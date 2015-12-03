#ifndef GPOPUPWINDOW_H
#define GPOPUPWINDOW_H

#include <QWidget>
#include <QVariant>
//#include <QAbstractslider>
#include <QApplication>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QHash>
#include <QPair>
#include <QTimer>
#include <private/qwidget_p.h>
#include <QPointer>
#include <QEvent>
#include <limits.h>

class QLineEdit;
class GLDPopupWindowPrivate;

class GLDPopupWindow : public QWidget
{
    Q_OBJECT

    Q_ENUMS(InsertPolicy)
    Q_ENUMS(SizeAdjustPolicy)
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable)
    Q_PROPERTY(QString currentText READ currentText)
    Q_PROPERTY(SizeAdjustPolicy sizeAdjustPolicy READ sizeAdjustPolicy WRITE setSizeAdjustPolicy)
    Q_PROPERTY(int minimumContentsLength READ minimumContentsLength WRITE setMinimumContentsLength)
    Q_PROPERTY(bool duplicatesEnabled READ duplicatesEnabled WRITE setDuplicatesEnabled)
    Q_PROPERTY(bool frame READ hasFrame WRITE setFrame)

public:
    explicit GLDPopupWindow(QWidget *parent = 0);
    ~GLDPopupWindow();

    int maxVisibleItems() const;
    void setMaxVisibleItems(int maxItems);

    int count() const;
    void setMaxCount(int max);
    int maxCount() const;

    bool duplicatesEnabled() const;
    void setDuplicatesEnabled(bool enable);

    void setFrame(bool);
    bool hasFrame() const;

    void setPopupWidget(QWidget *newWidget);
    QWidget *PopupWidget();

    enum InsertPolicy
    {
        NoInsert,
        InsertAtTop,
        InsertAtCurrent,
        InsertAtBottom,
        InsertAfterCurrent,
        InsertBeforeCurrent,
        InsertAlphabetically
    };

    InsertPolicy insertPolicy() const;
    void setInsertPolicy(InsertPolicy policy);

    enum SizeAdjustPolicy
    {
        AdjustToContents,
        AdjustToContentsOnFirstShow,
        AdjustToMinimumContentsLengthWithIcon
    };

    SizeAdjustPolicy sizeAdjustPolicy() const;
    void setSizeAdjustPolicy(SizeAdjustPolicy policy);
    int minimumContentsLength() const;
    void setMinimumContentsLength(int characters);
    QSize iconSize() const;
    void setIconSize(const QSize &size);

    bool isEditable() const;
    void setEditable(bool editable);
    void setLineEdit(QLineEdit *edit);
    QLineEdit *lineEdit() const;
#ifndef QT_NO_VALIDATOR
    void setValidator(const QValidator *v);
    const QValidator *validator() const;
#endif
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    bool event(QEvent *event);

    QString currentText() const;

public Q_SLOTS:
    void clear();
    void clearEditText();
    void setEditText(const QString &text);
    virtual void showPopup();
    virtual void hidePopup();

Q_SIGNALS:
    void editTextChanged(const QString &);
    void activated(int index);
    void activated(const QString &);
    void highlighted(int index);
    void highlighted(const QString &);

protected:
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *e);
#endif
    void contextMenuEvent(QContextMenuEvent *e);
    void inputMethodEvent(QInputMethodEvent *);
    QVariant inputMethodQuery(Qt::InputMethodQuery) const;
    void initStyleOption(QStyleOptionComboBox *option) const;

protected:
    GLDPopupWindow(GLDPopupWindowPrivate &, QWidget *);

private:
    Q_DECLARE_PRIVATE(GLDPopupWindow)
    Q_DISABLE_COPY(GLDPopupWindow)
    Q_PRIVATE_SLOT(d_func(), void _q_returnPressed())
    Q_PRIVATE_SLOT(d_func(), void _q_resetButton())
};

class QAction;

class GPopupWindowPrivateScroller : public QWidget
{
    Q_OBJECT

public:
    GPopupWindowPrivateScroller(QAbstractSlider::SliderAction action, QWidget *parent)
        : QWidget(parent), sliderAction(action)
    {
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        setAttribute(Qt::WA_NoMousePropagation);
    }
    QSize sizeHint() const
    {
        return QSize(20, style()->pixelMetric(QStyle::PM_MenuScrollerHeight));
    }

protected:
    inline void stopTimer()
    {
        timer.stop();
    }

    inline void startTimer()
    {
        timer.start(100, this);
        fast = false;
    }

    void enterEvent(QEvent *)
    {
        startTimer();
    }

    void leaveEvent(QEvent *)
    {
        stopTimer();
    }
    void timerEvent(QTimerEvent *e)
    {
        if (e->timerId() == timer.timerId())
        {
            emit doScroll(sliderAction);
            if (fast) {
                emit doScroll(sliderAction);
                emit doScroll(sliderAction);
            }
        }
    }
    void hideEvent(QHideEvent *)
    {
        stopTimer();
    }

    void mouseMoveEvent(QMouseEvent *e)
    {
        // Enable fast scrolling if the cursor is directly above or below the popup.
        const int mouseX = e->pos().x();
        const int mouseY = e->pos().y();
        const bool horizontallyInside = pos().x() < mouseX && mouseX < rect().right() + 1;
        const bool verticallyOutside = (sliderAction == QAbstractSlider::SliderSingleStepAdd) ?
                                        rect().bottom() + 1 < mouseY : mouseY < pos().y();

        fast = horizontallyInside && verticallyOutside;
    }

    void paintEvent(QPaintEvent *) {
        QPainter p(this);
        QStyleOptionMenuItem menuOpt;
        menuOpt.init(this);
        menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
        menuOpt.menuRect = rect();
        menuOpt.maxIconWidth = 0;
        menuOpt.tabWidth = 0;
        menuOpt.menuItemType = QStyleOptionMenuItem::Scroller;
        if (sliderAction == QAbstractSlider::SliderSingleStepAdd)
            menuOpt.state |= QStyle::State_DownArrow;
#ifndef Q_WS_S60
        p.eraseRect(rect());
#endif
        style()->drawControl(QStyle::CE_MenuScroller, &menuOpt, &p);
    }

Q_SIGNALS:
    void doScroll(int action);

private:
    QAbstractSlider::SliderAction sliderAction;
    QBasicTimer timer;
    bool fast;
};

class GPopupWindowPrivateContainer : public QFrame
{
    Q_OBJECT

public:
    GPopupWindowPrivateContainer(GLDPopupWindow *parent);
    int spacing() const;
    void updateTopBottomMargin();
    void setWidget(QWidget *newFrame);
    QWidget *widget();

    QTimer blockMouseReleaseTimer;
    QBasicTimer adjustSizeTimer;
    QPoint initialClickPosition;

protected:
    void changeEvent(QEvent *e);
    bool eventFilter(QObject *o, QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void timerEvent(QTimerEvent *timerEvent);
    void resizeEvent(QResizeEvent *e);
    QStyleOptionComboBox comboStyleOption() const;

Q_SIGNALS:
    void resetButton();

private:
    GLDPopupWindow *combo;
    QWidget *popupWidget;
};

class GLDPopupWindowPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(GLDPopupWindow)
public:
    GLDPopupWindowPrivate();
    ~GLDPopupWindowPrivate() {}
    void init();
    GPopupWindowPrivateContainer* viewContainer();
    void updateLineEditGeometry();
    Qt::MatchFlags matchFlags() const;
    void _q_returnPressed();
    void _q_resetButton();
    void _q_updateIndexBeforeChange();
    void updateArrow(QStyle::StateFlag state);
    bool updateHoverControl(const QPoint &pos);
    QRect popupGeometry(int screen = -1) const;
    QStyle::SubControl newHoverControl(const QPoint &pos);
    int computeWidthHint() const;
    QSize recomputeSizeHint(QSize &sh) const;
    void adjustComboBoxSize();
    void updateLayoutDirection();
    void updateViewContainerPaletteAndOpacity();

    QLineEdit *lineEdit;
    GPopupWindowPrivateContainer *container;
    GLDPopupWindow::SizeAdjustPolicy sizeAdjustPolicy;
    int minimumContentsLength;
    QSize iconSize;
    uint shownOnce : 1;
    uint duplicatesEnabled : 1;
    uint frame : 1;
    uint padding : 26;
    mutable QSize minimumSizeHint;
    mutable QSize sizeHint;
    QStyle::StateFlag arrowState;
    QStyle::SubControl hoverControl;
    QRect hoverRect;
    int indexBeforeChange;
    static QPalette viewContainerPalette(GLDPopupWindow *cmb)
    { return cmb->d_func()->viewContainer()->palette(); }
};


#endif // GPOPUPWINDOW_H
