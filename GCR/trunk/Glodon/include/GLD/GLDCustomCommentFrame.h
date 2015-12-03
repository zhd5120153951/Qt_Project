/*!
 *@file GLDCommentFrame.h
 *@brief 定义了一个可以移动comment，
 *@author liul
 *@date 2014.4.24
 *@remarks {remarks}
 *Copyright (c) 1998-2014 Glodon Corporation
 */

#ifndef GLDCUSTOMCOMMENTFRAME_H
#define GLDCUSTOMCOMMENTFRAME_H

#include <QDialog>
#include <QWidget>
#include <QPoint>
#include <QModelIndex>
#include <QString>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

#include "GLDTextEdit.h"
#include "GLDTableView_Global.h"

#define GLD_PI 3.14159265358979

class GLDTABLEVIEWSHARED_EXPORT GCustomCommentFrame : public QFrame
{
    Q_OBJECT
public:
    explicit GCustomCommentFrame(QWidget* parent = NULL, QPoint pt = QPoint(0, 0));
    ~GCustomCommentFrame();
    enum GCustomCommentFrameState
    {
        m_EnMoving = 0,
        m_EnUP,
        m_EnDown,
        m_EnLeft,
        m_EnRiht,
        m_EnLeftUp,
        m_EnbottomLeft,
        m_EnRightUp,
        m_EnRightDown,
        m_EnNormal
    };
    const static int c_AdjustWith = 6;

public:
    inline QModelIndex curIndex() { return m_curIndex; }
    inline void setCurIndex(const QModelIndex &index) { m_curIndex = index; }

    void setFramePosition(QPoint point);

    void resetBordrect();

    // 当鼠标不父亲上点击事件的时候，获取不到编辑状态，需要将这个编辑状态设置为false
    inline void setEllopseHide() {  m_isShowEllopse = false; }

    inline bool isShowCommentPersistent() { return m_isShowCommentPersistent; }
    inline QPoint topLeft(){ return m_topLeft; }

Q_SIGNALS:
    void textChanged(const QString &str);

    // 批注框失去焦点时会触发信号，外部写值的时机
    void editFocusOut();

public Q_SLOTS:
    void hide();
    void show(bool isPersistent = false);
    void move(int x, int y);
    void move(const QPoint &);
    void move(QModelIndex index);
    void moveTo(QPoint point);
    void setCommentText(const QString &str);
    QString commentText();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);
    void showEvent(QShowEvent *);
    void resizeEvent(QResizeEvent *);

private:
    void onMouseMoveEvent(QMouseEvent *e);
    void initPlaintEdit();
    void setInitStyle();
    void setCommentFrameSize();
    void setCommentFrameMask();
    void setEditTextPos();
    void resetBorderRect();
    // TODO: 如果别的项目也需要一些角度转弧度，弧度转角度，根据两个点计算先的时候，需要存放函数到公共类中去。
    double cacultAngle();
    inline double radianToAngle(const double Degree){ return (Degree*180/GLD_PI);}
    double getDistance(const QPoint border, const QPoint pos);

    void inMoveRange(QPoint pos);
    // TODO:如果以后需要频繁做一些两个点之间的判断关系的画，是要把这些放到一个公共的类中去
    bool containPosPoint(const QPoint pos, const QPoint border);
    bool containPos(const QPoint pos, const QRect subBorderRect);

    void repainLineAndArrow(QPainter *painter);
    void repainArrow(QPainter *painter);
    void repainArrowMoving(QPainter *painter);
    void repainAdjustRect(QPainter *painter);

    inline QPoint adjustRectLeftPoint();
    inline QPoint adjustRectRightPoint();
    inline QPoint adjustRectUpPoint();
    inline QPoint adjustRectBottomPoint();

    bool isPosInFrame(QPoint pos);
    void resetFramePosition();

private:
    QWidget *m_parent;        // 存放当commet的容器
    const int m_nMarginWidth; // 边框和编辑框之间的大小
    int m_nWidth;       // 编辑框的宽度
    int m_nHeight;      // 编辑框的高度
    int m_nDefaultWidth;       // 编辑框的宽度
    int m_nDefaultHeight;      // 编辑框的高度
    QModelIndex m_curIndex;
    bool m_isShowEllopse;       // 编辑框是否被选择
    bool m_starDraw;            // 描绘开始

    //窗体移动需要
//    QPoint m_windowPos;
    QPoint m_mousePos;
 //   QPoint m_dPos;
    GCustomCommentFrameState m_state;

    // 该控件的子控件，共有四个，箭头，箭头到编辑框的距离，编辑框，移动的图形。
    GLDPlainTextEdit *m_plainTextEdit;   // 编辑框
    QPoint m_topLeft;                     // 箭头的顶点位置
    QPoint m_bottomRight;                 // 箭头的箭尾位置，和矩形编辑框的连接位置
    QRect m_borderRect;
    double m_angle;
    QVector<QPoint> m_arrowPoints;
    bool m_isShowCommentPersistent;
};

QPoint GCustomCommentFrame::adjustRectLeftPoint()
{
    return QPoint(m_borderRect.left() - 10, (m_borderRect.top() + m_borderRect.bottom()) / 2);
}

QPoint GCustomCommentFrame::adjustRectRightPoint()
{
    return QPoint(m_borderRect.right(), (m_borderRect.top() + m_borderRect.bottom()) / 2);
}

QPoint GCustomCommentFrame::adjustRectUpPoint()
{
    return QPoint((m_borderRect.left() + m_borderRect.right()) / 2, m_borderRect.top() - 10);
}

QPoint GCustomCommentFrame::adjustRectBottomPoint()
{
    return QPoint((m_borderRect.left() + m_borderRect.right()) / 2, m_borderRect.bottom());
}

#endif // GLDCOMMENTFRAME_H
