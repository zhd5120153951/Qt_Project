#include "GLDDockTabWidget.h"
#include "GLDDockTabBar.h"
#include <QDrag>
#include <QMimeData>
#include "GLDDockNode.h"
#include "GLDDockPanel.h"
#include <QList>

GLDDockTabWidget::GLDDockTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    GLDDockTabBar *bar = new GLDDockTabBar(this);
    bar->setMovable(false);

    connect(bar, SIGNAL(dragTabOut(int)), this, SLOT(onDragTabOut(int)), Qt::QueuedConnection);
    setTabBar(bar);

    setTabPosition(South);

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrenChanged(int)));
}

GLDDockTabWidget::~GLDDockTabWidget()
{

}

void GLDDockTabWidget::onDragTabOut(int index)
{
    GLDDockPanel *currentPanel = qobject_cast<GLDDockPanel *>(widget(index));
    currentPanel->undock();
    currentPanel->show();
    currentPanel->startDrag();
}

void GLDDockTabWidget::onCurrenChanged(int)
{
    QWidget *cur = currentWidget();

    if (!cur)
    {
        return;
    }

    emit setBasePanelTitle(cur->windowTitle());
}

void GLDDockTabWidget::tabRemoved(int)
{
    if (count() == 1)
    {
        GLDDockPanel *panel = qobject_cast<GLDDockPanel *>(widget(0));
        GLDDockPanel *parentPanel = qobject_cast<GLDDockPanel *>(parentWidget());
        GLDDockNode *parentNode = qobject_cast<GLDDockNode *>(parentPanel->parentWidget());

        if (parentNode)
        {
            QList<int> sizes = parentNode->sizes();
            int thisIndex = parentNode->indexOf(parentPanel);
            parentPanel->setParent(NULL);
            parentPanel->close();
            parentPanel->deleteLater();
            panel->setTabbedStatus(false, NULL);
            parentNode->insertWidget(thisIndex, panel);
            parentNode->setSizes(sizes);
        }
        else
        {
            panel->undock();
            panel->show();
            panel->move(parentPanel->pos());
            panel->resize(parentPanel->size());

            parentPanel->close();
            parentPanel->deleteLater();
        }
    }
}
