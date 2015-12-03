/*!
 *@file GLDCommentFrameEx.h
 *@brief {注释框控件}
 *
 *@author mazq-a
 *@date 2014.07.07
 *@remarks
 *Copyright (c) 1998-2012 Glodon Corporation
 */

#ifndef GLDCOMMENTFRAMEEX_H
#define GLDCOMMENTFRAMEEX_H

#include <QWidget>
#include <QModelIndex>
#include "GLDString.h"
#include "GLDUITypes.h"
#include "GLDTableView_Global.h"

class GLDPlainTextEdit;
class GlodonAbstractItemView;
class GLDTABLEVIEWSHARED_EXPORT GCustomCommentFrameEx : public QWidget
{
    Q_OBJECT

    enum EAnchorPos
    {
        ELeftUp,
        ELeftDown,
        EBottomLeft,
        EBottomRight,
        ERightUp,
        ERightDown,
        ETopLeft,
        ETopRight,
        ECenter
    };

    enum EMousePos
    {
        EMoveWindPos,
        EStretchLeft,
        EStretchRight,
        EStretchUp,
        EStretchDown,
        EStretchLeftUp,
        EStretchLeftDown,
        EStretchRightUp,
        EStretchRightDown,
        EMousePosEdit,
        EMousePosNULL
    };

public:
    explicit GCustomCommentFrameEx(QWidget *parent = NULL, QPoint pt = QPoint(0, 0));
    ~GCustomCommentFrameEx();

Q_SIGNALS:
    void textChanged(const GString &str);
    void editFocusOut();

public Q_SLOTS:
    void setCommentText(const GString &str);
    GString commentText();
public:
    void move(int x, int y);
    void move (const QPoint& pt);
    void move(const QModelIndex& index, const GlodonAbstractItemView& instView );
    void hide();
    void show(bool bautohiden = false);
    void setFramePosition(QPoint point);
	bool isReadOnly() const;
    void setReadOnly(bool bReadOnly);

protected:
    void paintEvent(QPaintEvent *e);
    void drawImageOnWin(QPainter *painter);
    void drawFreamRect(QPainter *painter, bool bPainterEvent = true);
    void resizeEvent(QResizeEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    EAnchorPos getAnchorPos(const QPoint& pt, const QRect& rc) const;
    EAnchorPos getTriangleByAnchorAndRect(const QPoint& ptAnchor
                                          , const QRect& rcPos
                                          , QPoint& ptFirst
                                          , QPoint& ptSecond ) const;

    void updatePaintPolygonDate(const QPoint& ptAnchor, const QRect& rcPos);
    EMousePos getMousePos(const QPoint& pos) const;
    void processMousePos(QMouseEvent *e, GCustomCommentFrameEx::EMousePos pos);
    void processMoveWindPos(QMouseEvent *e);
    void processStretchLeft(QMouseEvent *e);
    void processStretchRight(QMouseEvent *e);
    void processStretchUp(QMouseEvent *e);
    void processStretchDown(QMouseEvent *e);
    void processStretchLeftUp(QMouseEvent *e);
    void processStretchLeftDown(QMouseEvent *e);
    void processStretchRightUp(QMouseEvent *e);
    void processStretchRightDown(QMouseEvent *e);
    void setWindowMask(bool birregularWnd);
    void updateEditPos();

private:
    bool rotationLeft(const QRect& rcPos);
    bool rotationRight(const QRect& rcPos);
    bool rotationUp(const QRect& rcPos);
    bool rotationDown(const QRect& rcPos);
    bool setPosByParent(const QRect& rcPos);
    void initPlaintEdit();
    void setUpUI();
    QRect getMaxRect(const QPoint& pt, const QRect& rc) const;
    void updateWindow();

private:
    QPoint m_dragPoint;
    QSize m_initWinSize;
    // 调整大小的矩形框
    QRect m_rectLT;
    QRect m_rectRT;
    QRect m_rectMT;
    QRect m_rectMB;
    QRect m_rectRB;
    QRect m_rectLB;
    QRect m_rectML;
    QRect m_rectMR;

    // 注释框指示的坐标点
    QPoint m_anchor;
    QPoint m_ptAnchorFirst;
    QPoint m_ptAnchorSecond;

    QRect m_rcPos;
    // 其它所有边框的点
    QVector<QPoint> m_verFreamPoint;
    EMousePos m_mouseCurrentPos;
    bool m_bShowStretch;
    GLDPlainTextEdit *m_plainTextEdit;   // 编辑框
    GColor m_clBg;
    GColor m_clText;
    GColor m_clBorder;
    QModelIndex m_curIndex;
    const QPixmap m_mapbl;
    const QPixmap m_mapbr;
    const QPixmap m_mapbc;
    const QPixmap m_maprt;
    const QPixmap m_maprc;
    bool m_bWndState;
    bool m_bIsMenuShow;
};

#endif // MAINWINDOW_H
