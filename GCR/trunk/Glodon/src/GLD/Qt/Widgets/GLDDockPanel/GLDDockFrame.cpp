#include "GLDDockFrame.h"
#include <QDrag>
#include <QMimeData>
#include "GLDDockNode.h"
#include "GLDDockMaskWidget.h"
#include <QByteArray>
#include <QDataStream>
#include "GLDDockPanel.h"
#include "GLDDockDataBuilder.h"
#include "GLDDockManager.h"
#include "GLDDockArrows.h"

GLDDockFrame::GLDDockFrame(GLDDockManager *manager, QWidget *parent)
    : QWidget(parent)
    , m_pArrows(nullptr)
    , manager_(manager)
    , lastMaskArea_(NoneArea)
{
    rootNode_ = new GLDDockNode(this);
    maskWidget_ = new GLDDockMaskWidget(this);
    maskWidget_->hide();

    m_pArrows = new GLDDockArrows(this);
    setAcceptDrops(true);
}

GLDDockFrame::~GLDDockFrame()
{
    delete m_pArrows;
}

void GLDDockFrame::showArrow()
{
    if (rootNode_->count() == 0)
    {
        m_pArrows->show(CenterArea);
    }
    else
    {
        m_pArrows->show(LeftArea | TopArea | RightArea | BottomArea);
    }
}

void GLDDockFrame::dragEnterEvent(QDragEnterEvent *e)
{
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

void GLDDockFrame::dragMoveEvent(QDragMoveEvent *e)
{
    DockArea area = m_pArrows->getDockAreaByPos(mapFromGlobal(QCursor::pos()));

    if (area != lastMaskArea_)
    {
        maskWidget_->showOnDockArea(area);
        lastMaskArea_ = area;
    }

    e->accept();
}

void GLDDockFrame::dragLeaveEvent(QDragLeaveEvent *e)
{
    if (!rect().contains(mapFromGlobal(QCursor::pos())))
    {
        m_pArrows->show(NoneArea);
    }

    lastMaskArea_ = NoneArea;
    maskWidget_->showOnDockArea(NoneArea);
    e->accept();
}

void GLDDockFrame::dropEvent(QDropEvent *e)
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
        manager_->dockPanelToFrame(panel, lastMaskArea_);
        e->accept();
    }
    else
    {
        e->ignore();
    }

    lastMaskArea_ = NoneArea;
    m_pArrows->show(NoneArea);
    maskWidget_->showOnDockArea(NoneArea);
}

void GLDDockFrame::resizeEvent(QResizeEvent *)
{
    relayout();
}

void GLDDockFrame::relayout()
{
    rootNode_->setGeometry(rect());

    if (!rootNode_->isVisible())
    {
        rootNode_->show();
    }
}

void GLDDockFrame::onDragEnterPanel()
{
    showArrow();
    maskWidget_->showOnDockArea(NoneArea);
}

void GLDDockFrame::onDragLeavePanel()
{
    if (!rect().contains(mapFromGlobal(QCursor::pos())))
    {
        m_pArrows->show(NoneArea);
    }

    lastMaskArea_ = NoneArea;
    maskWidget_->showOnDockArea(NoneArea);
}

void GLDDockFrame::onEndDragAtPanel()
{
    m_pArrows->show(NoneArea);
    lastMaskArea_ = NoneArea;
    maskWidget_->showOnDockArea(NoneArea);
}
