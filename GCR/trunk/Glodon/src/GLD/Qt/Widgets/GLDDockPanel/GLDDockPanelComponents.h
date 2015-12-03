#ifndef GLDDockPANELCONPONENTS_H
#define GLDDockPANELCONPONENTS_H

#include <QWidget>
#include "GLDWidget_Global.h"

class GLDWIDGETSHARED_EXPORT GLDDockPanelTitle : public QWidget
{
    Q_OBJECT

public:
    GLDDockPanelTitle(QWidget *parent);
    ~GLDDockPanelTitle() {}

    virtual void paintEvent(QPaintEvent *);

public slots:
    void setTitle(const QString &title);
    void startDrag();

private:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *e);

private:
    QString title_;

    bool isLBtnPressed_;
    QPoint pressedPos_;
    QPoint parentOldPos_;
};

class GLDWIDGETSHARED_EXPORT GLDDockPanelEdgeLeft : public QWidget
{
    Q_OBJECT

public:
    GLDDockPanelEdgeLeft(QWidget *parent);
    ~GLDDockPanelEdgeLeft() {}

private:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *e);

private:
    bool isLBtnPressed_;
    QPoint pressedPos_;
    QRect parentOldRect_;
};

class GLDWIDGETSHARED_EXPORT GLDDockPanelEdgeTop : public QWidget
{
    Q_OBJECT

public:
    GLDDockPanelEdgeTop(QWidget *parent);
    ~GLDDockPanelEdgeTop() {}

private:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *e);

private:
    bool isLBtnPressed_;
    QPoint pressedPos_;
    QRect parentOldRect_;
};

class GLDWIDGETSHARED_EXPORT GLDDockPanelEdgeRight : public QWidget
{
    Q_OBJECT

public:
    GLDDockPanelEdgeRight(QWidget *parent);
    ~GLDDockPanelEdgeRight() {}

private:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *e);

private:
    bool isLBtnPressed_;
    QPoint pressedPos_;
    int parentOldWidth_;
};

class GLDWIDGETSHARED_EXPORT GLDDockPanelEdgeBottom : public QWidget
{
    Q_OBJECT

public:
    GLDDockPanelEdgeBottom(QWidget *parent);
    ~GLDDockPanelEdgeBottom() {}

private:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *e);

private:
    bool isLBtnPressed_;
    QPoint pressedPos_;
    int parentOldHeight_;
};

class GLDWIDGETSHARED_EXPORT GLDDockPanelEdgeLeftTop : public QWidget
{
    Q_OBJECT

public:
    GLDDockPanelEdgeLeftTop(QWidget *parent);
    ~GLDDockPanelEdgeLeftTop() {}

private:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *e);

private:
    bool isLBtnPressed_;
    QPoint pressedPos_;
    QRect parentOldRect_;
};

class GLDWIDGETSHARED_EXPORT GLDDockPanelEdgeRightTop : public QWidget
{
    Q_OBJECT

public:
    GLDDockPanelEdgeRightTop(QWidget *parent);
    ~GLDDockPanelEdgeRightTop() {}

private:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *e);

private:
    bool isLBtnPressed_;
    QPoint pressedPos_;
    QRect parentOldRect_;
};

class GLDWIDGETSHARED_EXPORT GLDDockPanelEdgeRightBottom : public QWidget
{
    Q_OBJECT

public:
    GLDDockPanelEdgeRightBottom(QWidget *parent);
    ~GLDDockPanelEdgeRightBottom() {}

private:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *e);

private:
    bool isLBtnPressed_;
    QPoint pressedPos_;
    QSize parentOldSize_;
};

class GLDWIDGETSHARED_EXPORT GLDDockPanelEdgeLeftBottom : public QWidget
{
    Q_OBJECT

public:
    GLDDockPanelEdgeLeftBottom(QWidget *parent);
    ~GLDDockPanelEdgeLeftBottom() {}

private:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *e);

private:
    bool isLBtnPressed_;
    QPoint pressedPos_;
    QRect parentOldRect_;
};

#endif // GLDDockPANELCONPONENTS_H
