/*!
 *@file GLDCommentFrame.h
 *@brief 定义了一个可以移动comment，
 *@author liul
 *@date 2014.4.24
 *@remarks {remarks}
 *Copyright (c) 1998-2014 Glodon Corporation
 */

#ifndef GLDCOMMENTFRAME_H
#define GLDCOMMENTFRAME_H

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
#include "GLDWidget_Global.h"

#define GLD_PI 3.14159265358979

class GLDWIDGETSHARED_EXPORT GArrowFrame : public QFrame
{
public:
    explicit GArrowFrame(QFrame* parent = NULL);
    void setPosition(QPoint topLeft, QPoint bottomRight);
    void setBottomRight(QPoint bottomRight);
    QPoint bottomRight();
    void setTopLeft(QPoint topLeft);
    QPoint topLeft();

protected:
    void paintEvent(QPaintEvent *);

private:
    QPoint m_topLeft;
    QPoint m_bottomRight;
};

class GLDWIDGETSHARED_EXPORT GCommentFrame : public QFrame
{
    Q_OBJECT
public:
    explicit GCommentFrame(QWidget* parent = NULL, QPoint pt = QPoint(0, 0));
    ~GCommentFrame();

public Q_SLOTS:
    void hide();
    void show();
    void move(int x, int y);
    void move(const QPoint &);
    void setCommentText(const QString &str);
public:
    GArrowFrame *m_arrow;

private:
    QLabel *m_label;
    QWidget *m_parent;
    const int m_nMarginWidth;
    const int m_nWidth;
    const int m_nHeight;
};

class GLDWIDGETSHARED_EXPORT GToolTipFrame : public QFrame
{
public:
    explicit GToolTipFrame(QFrame* parent = NULL, QPoint pt = QPoint(0, 0),
                        const QString &text = QString());
    ~GToolTipFrame();
    void hide();
    void show();
    void move(int x, int y);
    void move(const QPoint &);
    void setHintText(const QString &text);
private:
    QFrame *m_parent;
    QLabel *m_label;
    QString m_lastShowText;
    const int m_nMarginWidth;
//    const int m_nWidth;
//    const int m_nHeight;
};

/**
 * @brief 调整行列大小信息提示框，其中主要包含了label
 */
class GLDWIDGETSHARED_EXPORT GInfoFrame : public QFrame
{
public:
    explicit GInfoFrame(QFrame* parent = NULL, QPoint pt = QPoint(0, 0));
    ~GInfoFrame();

public:
    void move(const QPoint &);
    void setText(const QString &str);

    inline void setOrientation(Qt::Orientation orientation){ m_direction = orientation; }
    inline Qt::Orientation orientation(){ return m_direction; }

private:
    QLabel *m_label;
    Qt::Orientation m_direction;
};

#endif // GLDCOMMENTFRAME_H
