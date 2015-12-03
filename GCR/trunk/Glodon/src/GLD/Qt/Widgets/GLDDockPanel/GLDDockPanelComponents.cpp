#include "GLDDockPanelComponents.h"

#include "GLDDockPanel.h"
#include "GLDDockDataBuilder.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QMimeData>

GLDDockPanelTitle::GLDDockPanelTitle(QWidget *parent) : QWidget(parent), isLBtnPressed_(false)
{
    setPalette(QPalette(Qt::lightGray));
    setAutoFillBackground(true);
}

void GLDDockPanelTitle::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, title_);
}

void GLDDockPanelTitle::setTitle(const QString &title)
{
    title_ = title;
    repaint();
}

void GLDDockPanelTitle::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        isLBtnPressed_ = true;
        pressedPos_ = e->globalPos();
        parentOldPos_ = parentWidget()->pos();
    }
}

void GLDDockPanelTitle::mouseReleaseEvent(QMouseEvent *)
{
    isLBtnPressed_ = false;
}

void GLDDockPanelTitle::mouseMoveEvent(QMouseEvent *e)
{
    if (!isLBtnPressed_)
    {
        return;
    }

    GLDDockPanel *panel = qobject_cast<GLDDockPanel *>(parentWidget());

    if (panel && panel->isDocked_)
    {
        panel->undock();
    }

    if (QApplication::keyboardModifiers() != Qt::ControlModifier)
    {
        startDrag();
        return;
    }

    parentWidget()->move(parentOldPos_.x() + e->globalX() - pressedPos_.x(), parentOldPos_.y() + e->globalY() - pressedPos_.y());
}

void GLDDockPanelTitle::startDrag()
{
    QMimeData *mimeData = new QMimeData;
    GLDDockDataBuilder data;
    data.setWidget(parentWidget());
    mimeData->setData("dockpanellib/dockdata", data.toByteArray());
    QDrag drag(this);
    QPixmap pic(parentWidget()->size());
    parentWidget()->render(&pic);
    drag.setPixmap(pic);
    drag.setMimeData(mimeData);
    parentWidget()->hide();

    if (drag.start() == Qt::IgnoreAction)
    {
        parentWidget()->move(QCursor::pos());
    }

    parentWidget()->show();
    isLBtnPressed_ = false;
}

GLDDockPanelEdgeLeft::GLDDockPanelEdgeLeft(QWidget *parent) : QWidget(parent), isLBtnPressed_(false)
{
    setCursor(Qt::SizeHorCursor);
}

void GLDDockPanelEdgeLeft::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        isLBtnPressed_ = true;
        pressedPos_ = e->globalPos();
        parentOldRect_ = parentWidget()->geometry();
    }
}

void GLDDockPanelEdgeLeft::mouseReleaseEvent(QMouseEvent *)
{
    isLBtnPressed_ = false;
}

void GLDDockPanelEdgeLeft::mouseMoveEvent(QMouseEvent *e)
{
    if (!isLBtnPressed_)
    {
        return;
    }

    int x, y, w, h;
    x = parentOldRect_.x() + e->globalX() - pressedPos_.x();
    y = parentWidget()->pos().y();
    w = parentOldRect_.width() - (e->globalX() - pressedPos_.x());

    if (w < 20)
    {
        return;
    }

    h = parentWidget()->height();
    parentWidget()->move(x, y);
    parentWidget()->resize(w, h);
}

GLDDockPanelEdgeTop::GLDDockPanelEdgeTop(QWidget *parent) : QWidget(parent), isLBtnPressed_(false)
{
    setCursor(Qt::SizeVerCursor);
}

void GLDDockPanelEdgeTop::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        isLBtnPressed_ = true;
        pressedPos_ = e->globalPos();
        parentOldRect_ = parentWidget()->geometry();
    }
}

void GLDDockPanelEdgeTop::mouseReleaseEvent(QMouseEvent *)
{
    isLBtnPressed_ = false;
}

void GLDDockPanelEdgeTop::mouseMoveEvent(QMouseEvent *e)
{
    if (!isLBtnPressed_)
    {
        return;
    }

    int x, y, w, h;
    x = parentWidget()->pos().x();
    y = parentOldRect_.y() - (pressedPos_.y() - e->globalY());
    w = parentWidget()->width();
    h = parentOldRect_.height() - (e->globalY() - pressedPos_.y());

    if (h < 20)
    {
        return;
    }

    parentWidget()->move(x, y);
    parentWidget()->resize(w, h);
}

GLDDockPanelEdgeRight::GLDDockPanelEdgeRight(QWidget *parent) : QWidget(parent), isLBtnPressed_(false)
{
    setCursor(Qt::SizeHorCursor);
}

void GLDDockPanelEdgeRight::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        isLBtnPressed_ = true;
        pressedPos_ = e->globalPos();
        parentOldWidth_ = parentWidget()->width();
    }
}

void GLDDockPanelEdgeRight::mouseReleaseEvent(QMouseEvent *)
{
    isLBtnPressed_ = false;
}

void GLDDockPanelEdgeRight::mouseMoveEvent(QMouseEvent *e)
{
    if (!isLBtnPressed_)
    {
        return;
    }

    int w, h;
    w = parentOldWidth_ - (pressedPos_.x() - e->globalX());

    if (w < 20)
    {
        return;
    }

    h = parentWidget()->height();
    parentWidget()->resize(w, h);
}

GLDDockPanelEdgeBottom::GLDDockPanelEdgeBottom(QWidget *parent) : QWidget(parent)
{
    setCursor(Qt::SizeVerCursor);
}

void GLDDockPanelEdgeBottom::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        isLBtnPressed_ = true;
        pressedPos_ = e->globalPos();
        parentOldHeight_ = parentWidget()->height();
    }
}

void GLDDockPanelEdgeBottom::mouseReleaseEvent(QMouseEvent *)
{
    isLBtnPressed_ = false;
}

void GLDDockPanelEdgeBottom::mouseMoveEvent(QMouseEvent *e)
{
    if (!isLBtnPressed_)
    {
        return;
    }

    int w, h;
    w = parentWidget()->width();
    h = parentOldHeight_ - (pressedPos_.y() - e->globalY());

    if (h < 20)
    {
        return;
    }

    parentWidget()->resize(w, h);
}

GLDDockPanelEdgeLeftTop::GLDDockPanelEdgeLeftTop(QWidget *parent) : QWidget(parent), isLBtnPressed_(false)
{
    setCursor(Qt::SizeFDiagCursor);
}

void GLDDockPanelEdgeLeftTop::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        isLBtnPressed_ = true;
        pressedPos_ = e->globalPos();
        parentOldRect_ = parentWidget()->geometry();
    }
}

void GLDDockPanelEdgeLeftTop::mouseReleaseEvent(QMouseEvent *)
{
    isLBtnPressed_ = false;
}

void GLDDockPanelEdgeLeftTop::mouseMoveEvent(QMouseEvent *e)
{
    if (!isLBtnPressed_)
    {
        return;
    }

    int x, y, w, h;
    x = parentOldRect_.x() + e->globalX() - pressedPos_.x();
    y = parentOldRect_.y() - (pressedPos_.y() - e->globalY());
    w = parentOldRect_.width() - (e->globalX() - pressedPos_.x());

    if (w < 20)
    {
        w = 20;
        x = parentWidget()->pos().x();
    }

    h = parentOldRect_.height() - (e->globalY() - pressedPos_.y());

    if (h < 20)
    {
        h = 20;
        y = parentWidget()->pos().y();
    }

    parentWidget()->move(x, y);
    parentWidget()->resize(w, h);
}

GLDDockPanelEdgeRightTop::GLDDockPanelEdgeRightTop(QWidget *parent) : QWidget(parent), isLBtnPressed_(false)
{
    setCursor(Qt::SizeBDiagCursor);
}

void GLDDockPanelEdgeRightTop::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        isLBtnPressed_ = true;
        pressedPos_ = e->globalPos();
        parentOldRect_ = parentWidget()->geometry();
    }
}

void GLDDockPanelEdgeRightTop::mouseReleaseEvent(QMouseEvent *)
{
    isLBtnPressed_ = false;
}

void GLDDockPanelEdgeRightTop::mouseMoveEvent(QMouseEvent *e)
{
    if (!isLBtnPressed_)
    {
        return;
    }

    int x, y, w, h;
    x = parentWidget()->pos().x();
    y = parentOldRect_.y() - (pressedPos_.y() - e->globalY());
    w = parentOldRect_.width() - (pressedPos_.x() - e->globalX());

    if (w < 20)
    {
        w = 20;
    }

    h = parentOldRect_.height() - (e->globalY() - pressedPos_.y());

    if (h < 20)
    {
        h = 20;
        y = parentWidget()->pos().y();
    }

    parentWidget()->move(x, y);
    parentWidget()->resize(w, h);
}

GLDDockPanelEdgeRightBottom::GLDDockPanelEdgeRightBottom(QWidget *parent) : QWidget(parent), isLBtnPressed_(false)
{
    setCursor(Qt::SizeFDiagCursor);
}

void GLDDockPanelEdgeRightBottom::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        isLBtnPressed_ = true;
        pressedPos_ = e->globalPos();
        parentOldSize_ = parentWidget()->size();
    }
}

void GLDDockPanelEdgeRightBottom::mouseReleaseEvent(QMouseEvent *)
{
    isLBtnPressed_ = false;
}

void GLDDockPanelEdgeRightBottom::mouseMoveEvent(QMouseEvent *e)
{
    if (!isLBtnPressed_)
    {
        return;
    }

    int w, h;
    w = parentOldSize_.width() - (pressedPos_.x() - e->globalX());

    if (w < 20)
    {
        w = 20;
    }

    h = parentOldSize_.height() - (pressedPos_.y() - e->globalY());

    if (h < 20)
    {
        h = 20;
    }

    parentWidget()->resize(w, h);
}

GLDDockPanelEdgeLeftBottom::GLDDockPanelEdgeLeftBottom(QWidget *parent) : QWidget(parent), isLBtnPressed_(false)
{
    setCursor(Qt::SizeBDiagCursor);
}

void GLDDockPanelEdgeLeftBottom::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        isLBtnPressed_ = true;
        pressedPos_ = e->globalPos();
        parentOldRect_ = parentWidget()->geometry();
    }
}

void GLDDockPanelEdgeLeftBottom::mouseReleaseEvent(QMouseEvent *)
{
    isLBtnPressed_ = false;
}

void GLDDockPanelEdgeLeftBottom::mouseMoveEvent(QMouseEvent *e)
{
    if (!isLBtnPressed_)
    {
        return;
    }

    int x, y, w, h;
    x = parentOldRect_.x() + e->globalX() - pressedPos_.x();
    y = parentWidget()->pos().y();
    w = parentOldRect_.width() - (e->globalX() - pressedPos_.x());

    if (w < 20)
    {
        w = 20;
        x = parentWidget()->pos().x();
    }

    h = parentOldRect_.height() - (pressedPos_.y() - e->globalY());

    if (h < 20)
    {
        h = 20;
    }

    parentWidget()->move(x, y);
    parentWidget()->resize(w, h);
}
