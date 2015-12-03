#ifndef GLDDateTimeEditEx_H
#define GLDDateTimeEditEx_H

#include "GLDDateTimeEdit.h"
#include "GLDCalendarWidget.h"

#include <QAction>
#include <QComboBox>
#include <QHBoxLayout>
#include <QHoverEvent>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QMouseEvent>
#include <QSpacerItem>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWheelEvent>
#include "GLDWidget_Global.h"


/**
 * @brief The GLDDateTimeEditEx class 日历类
 */
class GLDWIDGETSHARED_EXPORT GLDDateTimeEditEx : public GLDDateTimeEdit
{
    Q_OBJECT
public:
    enum ButtonTypes { None, ComboBoxArrow, Ellipsis };

    explicit GLDDateTimeEditEx(const QDateTime &dt = QDateTime::currentDateTime(), QWidget *parent = 0);
    ~ GLDDateTimeEditEx();

public:

    /**
     * @brief plainText 返回编辑框的内容
     * @return
     */
    QString plainText();

    bool calendarPopup() const;

    /**
     * @brief setCalendarPopup 设置是否有弹出框
     * @param enable
     */
    void setCalendarPopup(bool enable);

    /**
     * @brief setHasBorder 设置是否有边线
     * @param bValue
     */
    void setHasBorder(const bool bValue);

    /**
     * @brief 获取QSS文件路径
     * @param path
     */
    virtual const QString qssFilePath(const QString &path);

    /**
     * @brief 日期不能包含字母消息提示
     * @param 无
     */
    virtual void excludeLetterMessageBox();

    /**
     * @brief 日期输入方式有误消息提示
     * @param 无
     */
    virtual void errorDateMessageBox();

    /**
     * @brief 选择的日期小于最小日期消息提示
     * @param 无
     */
    virtual void dateLessThanSysDateMessageBox();

    /**
     * @brief 选择的日期大于最大日期消息提示
     * @param 无
     */
    virtual void dateLargerThanSysDateMessageBox();

    void updateValue();

    /** 返回日期的显示格式
     *  yyyy:代表年  MM:代表月份  dd:代表天
     *  hh:代表时 mm:代表分 ss:代表秒
     *  格式一般为: yyyy-MM-dd hh:mm:ss
     * @brief displayFormat
     * @return
     */
    QString displayFormat() const;

    /** 由于需求的变化，需要设置显示格式的时候按正规的格式来设置(请严格遵守, 区分大小写)
     *  yyyy:代表年  MM:代表月份  dd:代表天
     *  hh:代表时 mm:代表分 ss:代表秒
     *  格式一般为: yyyy-MM-dd hh:mm:ss
     * @brief setDisplayFormat
     * @param format
     */
    void setDisplayFormat(const QString &format);

    /**
     * @brief setCalendarPopupOffset 设置日历弹出窗体的偏移量
     * @param xOffset
     * @param yOffset
     */
    void setCalendarPopupOffset(int xOffset = 0, int yOffset = 0);

signals:
    void inputInvalid(bool bIsInputInvalid);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    QDateTime dateTimeFromText(const QString &text) const;
    QString textFromDateTime(const QDateTime &dt) const;
    QValidator::State validate(QString &input, int &pos) const;
    void focusOutEvent(QFocusEvent *event);

private:
    /**
     * @brief initCalendarPopup
     */
    void initCalendarPopup();

    /**
     * @brief positionCalendarPopup 定位弹出框的位置，特别是在屏幕以外时的特殊位置定位
     */
    void positionCalendarPopup();

    /**
     * @brief showPopUp 显示日历下拉框
     */
    void showPopUp();

    /**
     * @brief hidePopUp 隐藏日历下拉框
     */
    void hidePopUp();

    QStyle::SubControl newHoverControl(const QPoint &pos);
    /**
     * @brief parseText 采用补全的方式，对用户输入的不完整的日期格式进行补全修正, 只做补0操作
     *                  如2014-1-2 3:4:5修正为2014-01-02 03:04:05
     * @param input 用户输入
     * @return  经补全的日期格式
     */
    QString parseText(const QString &input);

    QString formatDate(const QString &input);

Q_SIGNALS:
    void onshowPopup(bool &value);

private slots:
    /**
     * @brief GLDDateTimeEditEx::selectDate 显示给定的日期
     * @param date
     */
    void selectDate(const QDate &date);
    void selectDateTime(const QDateTime &dateTime);

private:
    bool m_isCalendarPopup; // 是否有弹出框
    bool m_hasBorder;
    bool m_curKeyIsEnter; // 判断是否是Key_Enter或Key_Return键
    // m_justKeyPressed 和 m_keyPressing用来控制编辑框删除后的第一次和之后的输入
    bool m_justKeyPressed;
    bool m_keyPressing;
    QString m_curStr; // 保存当前输入
    mutable QString m_lastStrForDatetime;
    int m_curPos; //保存当前输入的position
    GLDCalendarWidget *m_calendarWidget;
    ButtonTypes m_buttonType;
    QDateTime m_dateTime;
    int m_xOffset;
    int m_yOffset;
};

#endif // GLDDateTimeEditEx_H
