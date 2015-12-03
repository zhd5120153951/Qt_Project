#include "GLDDockPanel.h"
#include <QResizeEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QCursor>
#include <QByteArray>
#include <QDataStream>
#include "GLDDockDataBuilder.h"
#include "GLDDockNode.h"
#include <cassert>
#include "GLDDockFrame.h"
#include "GLDDockManager.h"
#include "GLDDockMaskWidget.h"
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

GLDDockPanel::GLDDockPanel(GLDDockManager *manager, QWidget *frame)
    : QWidget(frame, Qt::FramelessWindowHint | Qt::Tool),
      titleRectHeight_(20), contensWidget_(NULL), edgeWidth_(3),
      isDocked_(false), manager_(manager), arrows_(this), lastMaskArea_(NoneArea), isTabbed_(false),
      parentTabPanel_(NULL), panelType_(DockPanel)
{
    title_ = new GLDDockPanelTitle(this);
    connect(this, SIGNAL(windowTitleChanged(const QString &)), title_, SLOT(setTitle(const QString &)));
    leftEdge_ = new GLDDockPanelEdgeLeft(this);
    leftTopEdge_ = new GLDDockPanelEdgeLeftTop(this);
    topEdge_ = new GLDDockPanelEdgeTop(this);
    rightTopEdge_ = new GLDDockPanelEdgeRightTop(this);
    rightEdge_ = new GLDDockPanelEdgeRight(this);
    rightBottomEdge_ = new GLDDockPanelEdgeRightBottom(this);
    bottomEdge_ = new GLDDockPanelEdgeBottom(this);
    leftBottomEdge_ = new GLDDockPanelEdgeLeftBottom(this);

    maskWidget_ = new GLDDockMaskWidget(this);
    maskWidget_->hide();

    setAcceptDrops(true);

    setMinimumSize(50, 50);
}

void GLDDockPanel::paintEvent(QPaintEvent *)
{
    QPainter p(this);
}

void GLDDockPanel::resizeEvent(QResizeEvent *)
{
    relayout();
}

void GLDDockPanel::resetContensWidgetPosAndSize()
{
    if (!contensWidget_)
    {
        return;
    }

    if (contensWidget_->parentWidget() != this)
    {
        contensWidget_->setParent(this);
    }

    if (isDocked_)
    {
        if (isTabbed_)
        {
            contensWidget_->move(0, 0);
            contensWidget_->resize(width(), height());
        }
        else
        {
            contensWidget_->move(0, edgeWidth_ + titleRectHeight_);
            contensWidget_->resize(width(), height() - edgeWidth_ - titleRectHeight_);
        }
    }
    else
    {
        contensWidget_->move(edgeWidth_, edgeWidth_ * 2 + titleRectHeight_);
        contensWidget_->resize(width() - edgeWidth_ * 2, height() - edgeWidth_ * 3 - titleRectHeight_);
    }
}

void GLDDockPanel::relayout()
{
    if (isDocked_)
    {
        if (isTabbed_)
        {
            title_->hide();
        }
        else
        {
            title_->move(0, 0);
            title_->resize(width(), titleRectHeight_);
            title_->show();
        }

        leftEdge_->hide();
        leftTopEdge_->hide();
        topEdge_->hide();
        rightTopEdge_->hide();
        rightEdge_->hide();
        rightBottomEdge_->hide();
        bottomEdge_->hide();
        leftBottomEdge_->hide();
    }
    else
    {
        title_->move(edgeWidth_, edgeWidth_);
        title_->resize(width() - edgeWidth_ * 2, titleRectHeight_);
        title_->show();

        leftEdge_->move(0, edgeWidth_);
        leftEdge_->resize(edgeWidth_, height() - edgeWidth_ * 2);
        leftEdge_->show();

        leftTopEdge_->move(0, 0);
        leftTopEdge_->resize(edgeWidth_, edgeWidth_);
        leftTopEdge_->show();

        topEdge_->move(edgeWidth_, 0);
        topEdge_->resize(width() - edgeWidth_ * 2, edgeWidth_);
        topEdge_->show();

        rightTopEdge_->move(width() - edgeWidth_, 0);
        rightTopEdge_->resize(edgeWidth_, edgeWidth_);
        rightTopEdge_->show();

        rightEdge_->move(width() - edgeWidth_, edgeWidth_);
        rightEdge_->resize(edgeWidth_, height() - edgeWidth_ * 2);
        rightEdge_->show();

        rightBottomEdge_->move(width() - edgeWidth_, height() - edgeWidth_);
        rightBottomEdge_->resize(edgeWidth_, edgeWidth_);
        rightBottomEdge_->show();

        bottomEdge_->move(edgeWidth_, height() - edgeWidth_);
        bottomEdge_->resize(width() - edgeWidth_ * 2, edgeWidth_);
        bottomEdge_->show();

        leftBottomEdge_->move(0, height() - edgeWidth_);
        leftBottomEdge_->resize(edgeWidth_, edgeWidth_);
        leftBottomEdge_->show();
    }

    resetContensWidgetPosAndSize();
}

void GLDDockPanel::setDockStatus()
{
    floatSize_ = size();
    setWindowFlags(Qt::SubWindow);
    relayout();
    isDocked_ = true;
}

void GLDDockPanel::setFloatStatus()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    resize(floatSize_);
    relayout();
    isDocked_ = false;
    isTabbed_ = false;
}

bool GLDDockPanel::dockTo(QWidget *, DockArea )
{
    return false;
}

void GLDDockPanel::undock()
{
    manager_->undockPanel(this);
}

void GLDDockPanel::dragEnterEvent(QDragEnterEvent *e)
{
    manager_->onDragEnterPanel();
    const QMimeData *mimeData = e->mimeData();

    if (mimeData && mimeData->hasFormat("dockpanellib/dockdata"))
    {
        showArrow();
        e->accept();
    }
    else
    {
        e->ignore();
    }
}

void GLDDockPanel::dragMoveEvent(QDragMoveEvent *e)
{
    DockArea area = arrows_.getDockAreaByPos(mapFromGlobal(QCursor::pos()));

    if (area != lastMaskArea_)
    {
        maskWidget_->showOnDockArea(area);
        lastMaskArea_ = area;
    }

    e->accept();
}

void GLDDockPanel::dragLeaveEvent(QDragLeaveEvent *e)
{
    manager_->onDragLeavePanel();
    arrows_.show(NoneArea);
    lastMaskArea_ = NoneArea;
    maskWidget_->showOnDockArea(NoneArea);
    e->accept();
}

void GLDDockPanel::dropEvent(QDropEvent *e)
{
    const QMimeData *mimeData = e->mimeData();

    if (!mimeData->hasFormat("dockpanellib/dockdata"))
    {
        e->ignore();
        return;
    }

    QByteArray ba = mimeData->data("dockpanellib/dockdata");
    GLDDockDataBuilder builder;
    builder.fromByteArray(ba);

    GLDDockPanel *panel = qobject_cast<GLDDockPanel *>(builder.getWidget());

    if (panel && lastMaskArea_ != NoneArea)
    {
        e->accept();
        manager_->dockPanelToPanel(panel, this, lastMaskArea_);
    }
    else
    {
        e->ignore();
    }

    lastMaskArea_ = NoneArea;
    arrows_.show(NoneArea);
    maskWidget_->showOnDockArea(NoneArea);

    manager_->onEndDragAtPanel();
}

void GLDDockPanel::showArrow()
{
    arrows_.show(CenterArea | CenterLeftArea | CenterTopArea | CenterRightArea | CenterBottomArea);
}

void GLDDockPanel::startDrag()
{
    title_->startDrag();
}

void GLDDockPanel::setTabbedStatus(bool isTabbed, GLDDockPanel *parentTabPanel)
{
    isTabbed_ = isTabbed;
    parentTabPanel_ = parentTabPanel;
    relayout();
}
