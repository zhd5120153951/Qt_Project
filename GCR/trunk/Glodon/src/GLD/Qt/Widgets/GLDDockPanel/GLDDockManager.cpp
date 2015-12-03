#include "GLDDockManager.h"
#include "GLDDockFrame.h"
#include "GLDDockPanel.h"
#include <cassert>
#include "GLDDockNode.h"
#include <QList>
#include "GLDDockMaskWidget.h"
#include "GLDDockTabWidget.h"

GLDDockManager::GLDDockManager(QWidget *parent)
    : QObject(parent), dockFrmae_(new GLDDockFrame(this, parent))
{

}

GLDDockManager::~GLDDockManager()
{

}

GLDDockPanel *GLDDockManager::addPanel(int id, const QString &title, QPoint pos, QSize size, QWidget *contensWidget /*= NULL*/)
{
    if (dockPanels_.find(id) != dockPanels_.end())
    {
        return NULL;
    }

    GLDDockPanel *p = new GLDDockPanel(this, dockFrmae_);
    p->setId(id);
    p->setWindowTitle(title);
    p->resetContensWidget(contensWidget);
    p->move(pos);
    p->resize(size);
    p->show();
    dockPanels_.insert(std::make_pair(id, p));

    return p;
}

GLDDockPanel *GLDDockManager::getDockPanelByID(int id)
{
    std::map<int, GLDDockPanel *>::iterator it = dockPanels_.find(id);

    if (it == dockPanels_.end())
    {
        return NULL;
    }

    return it->second;
}

bool GLDDockManager::dockPanelToFrame(GLDDockPanel *panel, DockArea area)
{
    if (panel->panelType_ == GLDDockPanel::SplitContainer)
    {
        GLDDockNode *fromNode = qobject_cast<GLDDockNode *>(panel->contensWidget_);
        assert(fromNode);
        panel->deleteLater();

        switch (area)
        {
            case LeftArea:
            {
                QList<int> sizes;
                sizes.push_back(30);
                sizes.push_back(70);

                int c = dockFrmae_->rootNode_->count();

                if (c == 1)
                {
                    dockFrmae_->rootNode_->setOrientation(Qt::Horizontal);
                    dockFrmae_->rootNode_->insertWidget(0, fromNode);
                    dockFrmae_->rootNode_->setSizes(sizes);
                }
                else if (c == 2)
                {
                    GLDDockNode *tmpNode = dockFrmae_->rootNode_;
                    dockFrmae_->rootNode_ = new GLDDockNode(dockFrmae_);
                    dockFrmae_->rootNode_->setOrientation(Qt::Horizontal);
                    dockFrmae_->rootNode_->insertWidget(0, fromNode);
                    dockFrmae_->rootNode_->insertWidget(1, tmpNode);
                    dockFrmae_->relayout();
                    dockFrmae_->rootNode_->setSizes(sizes);
                }
                else
                {
                    assert(false);
                }

                break;
            }

            case TopArea:
            {
                QList<int> sizes;
                sizes.push_back(30);
                sizes.push_back(70);

                int c = dockFrmae_->rootNode_->count();

                if (c == 1)
                {
                    dockFrmae_->rootNode_->setOrientation(Qt::Vertical);
                    dockFrmae_->rootNode_->insertWidget(0, fromNode);
                    dockFrmae_->rootNode_->setSizes(sizes);
                }
                else if (c == 2)
                {
                    GLDDockNode *tmpNode = dockFrmae_->rootNode_;
                    dockFrmae_->rootNode_ = new GLDDockNode(dockFrmae_);
                    dockFrmae_->rootNode_->setOrientation(Qt::Vertical);
                    dockFrmae_->rootNode_->insertWidget(0, fromNode);
                    dockFrmae_->rootNode_->insertWidget(1, tmpNode);
                    dockFrmae_->relayout();
                    dockFrmae_->rootNode_->setSizes(sizes);
                }
                else
                {
                    assert(false);
                }

                break;
            }

            case RightArea:
            {
                QList<int> sizes;
                sizes.push_back(70);
                sizes.push_back(30);

                int c = dockFrmae_->rootNode_->count();

                if (c == 1)
                {
                    dockFrmae_->rootNode_->setOrientation(Qt::Horizontal);
                    dockFrmae_->rootNode_->insertWidget(1, fromNode);
                    dockFrmae_->rootNode_->setSizes(sizes);
                }
                else if (c == 2)
                {
                    GLDDockNode *tmpNode = dockFrmae_->rootNode_;
                    dockFrmae_->rootNode_ = new GLDDockNode(dockFrmae_);
                    dockFrmae_->rootNode_->setOrientation(Qt::Horizontal);
                    dockFrmae_->rootNode_->insertWidget(0, tmpNode);
                    dockFrmae_->rootNode_->insertWidget(1, fromNode);
                    dockFrmae_->relayout();
                    dockFrmae_->rootNode_->setSizes(sizes);
                }
                else
                {
                    assert(false);
                }

                break;
            }

            case BottomArea:
            {
                QList<int> sizes;
                sizes.push_back(70);
                sizes.push_back(30);

                int c = dockFrmae_->rootNode_->count();

                if (c == 1)
                {
                    dockFrmae_->rootNode_->setOrientation(Qt::Vertical);
                    dockFrmae_->rootNode_->insertWidget(0, fromNode);
                    dockFrmae_->rootNode_->setSizes(sizes);
                }
                else if (c == 2)
                {
                    GLDDockNode *tmpNode = dockFrmae_->rootNode_;
                    dockFrmae_->rootNode_ = new GLDDockNode(dockFrmae_);
                    dockFrmae_->rootNode_->setOrientation(Qt::Vertical);
                    dockFrmae_->rootNode_->insertWidget(0, tmpNode);
                    dockFrmae_->rootNode_->insertWidget(1, fromNode);
                    dockFrmae_->relayout();
                    dockFrmae_->rootNode_->setSizes(sizes);
                }
                else
                {
                    assert(false);
                }

                break;
            }

            case CenterArea:
                assert(dockFrmae_->rootNode_->count() == 0);
                dockFrmae_->rootNode_->deleteLater();
                dockFrmae_->rootNode_ = fromNode;
                fromNode->show();
                fromNode->setParent(dockFrmae_);
                dockFrmae_->relayout();
                break;

            default:
                return false;
        }

        return true;
    }


    switch (area)
    {
        case LeftArea:
        {
            QList<int> sizes;
            sizes.push_back(30);
            sizes.push_back(70);

            int c = dockFrmae_->rootNode_->count();

            if (c == 1)
            {
                dockFrmae_->rootNode_->setOrientation(Qt::Horizontal);
                panel->setDockStatus();
                dockFrmae_->rootNode_->insertWidget(0, panel);
                dockFrmae_->rootNode_->setSizes(sizes);
            }
            else if (c == 2)
            {
                GLDDockNode *tmpNode = dockFrmae_->rootNode_;
                panel->setDockStatus();
                dockFrmae_->rootNode_ = new GLDDockNode(dockFrmae_);
                dockFrmae_->rootNode_->setOrientation(Qt::Horizontal);
                dockFrmae_->rootNode_->insertWidget(0, panel);
                dockFrmae_->rootNode_->insertWidget(1, tmpNode);
                dockFrmae_->relayout();
                dockFrmae_->rootNode_->setSizes(sizes);
            }
            else
            {
                assert(false);
            }

            break;
        }

        case TopArea:
        {
            QList<int> sizes;
            sizes.push_back(30);
            sizes.push_back(70);

            int c = dockFrmae_->rootNode_->count();

            if (c == 1)
            {
                dockFrmae_->rootNode_->setOrientation(Qt::Vertical);
                panel->setDockStatus();
                dockFrmae_->rootNode_->insertWidget(0, panel);
                dockFrmae_->rootNode_->setSizes(sizes);
            }
            else if (c == 2)
            {
                GLDDockNode *tmpNode = dockFrmae_->rootNode_;
                panel->setDockStatus();
                dockFrmae_->rootNode_ = new GLDDockNode(dockFrmae_);
                dockFrmae_->rootNode_->setOrientation(Qt::Vertical);
                dockFrmae_->rootNode_->insertWidget(0, panel);
                dockFrmae_->rootNode_->insertWidget(1, tmpNode);
                dockFrmae_->relayout();
                dockFrmae_->rootNode_->setSizes(sizes);
            }
            else
            {
                assert(false);
            }

            break;
        }

        case RightArea:
        {
            QList<int> sizes;
            sizes.push_back(70);
            sizes.push_back(30);

            int c = dockFrmae_->rootNode_->count();

            if (c == 1)
            {
                dockFrmae_->rootNode_->setOrientation(Qt::Horizontal);
                panel->setDockStatus();
                dockFrmae_->rootNode_->insertWidget(1, panel);
                dockFrmae_->rootNode_->setSizes(sizes);
            }
            else if (c == 2)
            {
                GLDDockNode *tmpNode = dockFrmae_->rootNode_;
                panel->setDockStatus();
                dockFrmae_->rootNode_ = new GLDDockNode(dockFrmae_);
                dockFrmae_->rootNode_->setOrientation(Qt::Horizontal);
                dockFrmae_->rootNode_->insertWidget(0, tmpNode);
                dockFrmae_->rootNode_->insertWidget(1, panel);
                dockFrmae_->relayout();
                dockFrmae_->rootNode_->setSizes(sizes);
            }
            else
            {
                assert(false);
            }

            break;
        }

        case BottomArea:
        {
            QList<int> sizes;
            sizes.push_back(70);
            sizes.push_back(30);

            int c = dockFrmae_->rootNode_->count();

            if (c == 1)
            {
                dockFrmae_->rootNode_->setOrientation(Qt::Vertical);
                panel->setDockStatus();
                dockFrmae_->rootNode_->insertWidget(0, panel);
                dockFrmae_->rootNode_->setSizes(sizes);
            }
            else if (c == 2)
            {
                GLDDockNode *tmpNode = dockFrmae_->rootNode_;
                panel->setDockStatus();
                dockFrmae_->rootNode_ = new GLDDockNode(dockFrmae_);
                dockFrmae_->rootNode_->setOrientation(Qt::Vertical);
                dockFrmae_->rootNode_->insertWidget(0, tmpNode);
                dockFrmae_->rootNode_->insertWidget(1, panel);
                dockFrmae_->relayout();
                dockFrmae_->rootNode_->setSizes(sizes);
            }
            else
            {
                assert(false);
            }

            break;
        }

        case CenterArea:
            assert(dockFrmae_->rootNode_->count() == 0);
            panel->setDockStatus();
            panel->setParent(dockFrmae_->rootNode_);
            break;

        default:
            return false;
    }

    return true;
}

bool GLDDockManager::dockPanelToPanel(GLDDockPanel *from, GLDDockPanel *target, DockArea area)
{
    if (target->isDocked())
    {
        return dockPanelToDockedPanel(from, target, area);
    }
    else
    {
        QTimer *t = new QTimer;
        QObject::connect(t, &QTimer::timeout, [ = ]()
        {
            t->deleteLater();
            dockPanelToFloatPanel(from, target, area);
        });
        t->start(1);

        return true;
    }

    return false;
}

bool GLDDockManager::isRootNode(GLDDockNode *node)
{
    return node == dockFrmae_->rootNode_;
}

void GLDDockManager::undockPanel(GLDDockPanel *panel)
{
    GLDDockNode *parentNode = qobject_cast<GLDDockNode *>(panel->parentWidget());

    if (parentNode)
    {
        panel->setParent(dockFrmae_);
        panel->setFloatStatus();

        if (isRootNode(parentNode))
        {
            GLDDockNode *otherChildNode = qobject_cast<GLDDockNode *>(parentNode->widget(0));

            if (otherChildNode)
            {
                QList<int> sizes = otherChildNode->sizes();
                parentNode->setOrientation(otherChildNode->orientation());
                parentNode->insertWidget(0, otherChildNode->widget(0));
                parentNode->insertWidget(1, otherChildNode->widget(0));
                parentNode->setSizes(sizes);
                otherChildNode->deleteLater();
            }

            return;
        }

        assert(parentNode->count() == 1);

        GLDDockPanel *containerPanel = qobject_cast<GLDDockPanel *>(parentNode->parentWidget());

        if (containerPanel)
        {
            QWidget *widget = parentNode->widget(0);
            GLDDockPanel *otherPanel = qobject_cast<GLDDockPanel *>(widget);

            if (otherPanel)
            {
                otherPanel->setParent(dockFrmae_);
                otherPanel->setFloatStatus();
                otherPanel->resize(containerPanel->size());
                otherPanel->move(containerPanel->pos());
                otherPanel->show();
                containerPanel->close();
                containerPanel->deleteLater();
            }
            else if (GLDDockNode *otherNode = qobject_cast<GLDDockNode *>(widget))
            {
                containerPanel->contensWidget_->deleteLater();
                containerPanel->resetContensWidget(otherNode);
                otherNode->show();
            }
            else
            {
                assert(false);
            }
        }
        else
        {
            GLDDockNode *grandParentNode = qobject_cast<GLDDockNode *>(parentNode->parentWidget());
            assert(grandParentNode);
            QList<int> sizes = grandParentNode->sizes();
            QWidget *widget = parentNode->widget(0);
            int index = grandParentNode->indexOf(parentNode);
            parentNode->close();
            parentNode->deleteLater();
            grandParentNode->insertWidget(index, widget);
            grandParentNode->setSizes(sizes);

        }

        return;
    }

    panel->setParent(dockFrmae_);
    panel->setFloatStatus();

    return;
}

void GLDDockManager::onDragEnterPanel()
{
    dockFrmae_->onDragEnterPanel();
}

void GLDDockManager::onDragLeavePanel()
{
    dockFrmae_->onDragLeavePanel();
}

void GLDDockManager::onEndDragAtPanel()
{
    dockFrmae_->onEndDragAtPanel();
}

bool GLDDockManager::dockPanelToFloatPanel(GLDDockPanel *from, GLDDockPanel *target, DockArea area)
{
    if (from->panelType_ == GLDDockPanel::TabContainer)
    {
        switch (area)
        {
            case CenterArea:
            {
                GLDDockTabWidget *tabWidget = qobject_cast<GLDDockTabWidget *>(from->contensWidget_);
                assert(tabWidget);
                from->move(target->pos());
                from->resize(target->size());
                tabWidget->addTab(target, target->windowTitle());
                target->setDockStatus();
                target->setTabbedStatus(true, from);
            }
            break;

            case CenterLeftArea:
            {
                GLDDockPanel *tabPanel = new GLDDockPanel(this, dockFrmae_);
                tabPanel->panelType_ = GLDDockPanel::SplitContainer;
                tabPanel->resize(target->size());
                tabPanel->move(target->pos());
                tabPanel->setAcceptDrops(false);
                GLDDockNode *node = new GLDDockNode(tabPanel);
                tabPanel->resetContensWidget(node);
                node->setOrientation(Qt::Horizontal);
                from->setDockStatus();
                target->setDockStatus();
                node->insertWidget(0, from);
                node->insertWidget(1, target);
                tabPanel->setWindowTitle(QString("%1 & %2").arg(from->windowTitle()).arg(target->windowTitle()));
                tabPanel->show();
            }
            break;

            case CenterTopArea:
            {
                GLDDockPanel *tabPanel = new GLDDockPanel(this, dockFrmae_);
                tabPanel->panelType_ = GLDDockPanel::SplitContainer;
                tabPanel->resize(target->size());
                tabPanel->move(target->pos());
                tabPanel->setAcceptDrops(false);
                GLDDockNode *node = new GLDDockNode(tabPanel);
                tabPanel->resetContensWidget(node);
                node->setOrientation(Qt::Vertical);
                from->setDockStatus();
                target->setDockStatus();
                node->insertWidget(0, from);
                node->insertWidget(1, target);
                tabPanel->setWindowTitle(QString("%1 & %2").arg(from->windowTitle()).arg(target->windowTitle()));
                tabPanel->show();
            }
            break;

            case CenterRightArea:
            {
                GLDDockPanel *tabPanel = new GLDDockPanel(this, dockFrmae_);
                tabPanel->panelType_ = GLDDockPanel::SplitContainer;
                tabPanel->resize(target->size());
                tabPanel->move(target->pos());
                tabPanel->setAcceptDrops(false);
                GLDDockNode *node = new GLDDockNode(tabPanel);
                tabPanel->resetContensWidget(node);
                node->setOrientation(Qt::Horizontal);
                from->setDockStatus();
                target->setDockStatus();
                node->insertWidget(0, target);
                node->insertWidget(1, from);
                tabPanel->setWindowTitle(QString("%1 & %2").arg(target->windowTitle()).arg(from->windowTitle()));
                tabPanel->show();
            }
            break;

            case CenterBottomArea:
            {
                GLDDockPanel *tabPanel = new GLDDockPanel(this, dockFrmae_);
                tabPanel->panelType_ = GLDDockPanel::SplitContainer;
                tabPanel->resize(target->size());
                tabPanel->move(target->pos());
                tabPanel->setAcceptDrops(false);
                GLDDockNode *node = new GLDDockNode(tabPanel);
                tabPanel->resetContensWidget(node);
                node->setOrientation(Qt::Vertical);
                from->setDockStatus();
                target->setDockStatus();
                node->insertWidget(0, target);
                node->insertWidget(1, from);
                tabPanel->setWindowTitle(QString("%1 & %2").arg(target->windowTitle()).arg(from->windowTitle()));
                tabPanel->show();
            }
            break;
            default:
                break;
        }

        return true;
    }


    if (from->panelType_ == GLDDockPanel::SplitContainer)
    {
        switch (area)
        {
            case CenterArea:
            {

            }
            break;

            case CenterLeftArea:
            {
                QWidget *tmp = from->contensWidget_;
                GLDDockNode *node = new GLDDockNode(from);
                from->resetContensWidget(node);
                node->setOrientation(Qt::Horizontal);
                node->show();

                from->move(target->pos());
                from->resize(target->size());

                node->insertWidget(0, tmp);
                target->setDockStatus();
                node->insertWidget(1, target);
            }
            break;

            case CenterTopArea:
            {
                QWidget *tmp = from->contensWidget_;
                GLDDockNode *node = new GLDDockNode(from);
                from->resetContensWidget(node);
                node->setOrientation(Qt::Vertical);
                node->show();

                from->move(target->pos());
                from->resize(target->size());

                node->insertWidget(0, tmp);
                target->setDockStatus();
                node->insertWidget(1, target);
            }
            break;

            case CenterRightArea:
            {
                QWidget *tmp = from->contensWidget_;
                GLDDockNode *node = new GLDDockNode(from);
                from->resetContensWidget(node);
                node->setOrientation(Qt::Horizontal);
                node->show();

                from->move(target->pos());
                from->resize(target->size());

                target->setDockStatus();
                node->insertWidget(0, target);
                node->insertWidget(1, tmp);
            }
            break;

            case CenterBottomArea:
            {
                QWidget *tmp = from->contensWidget_;
                GLDDockNode *node = new GLDDockNode(from);
                from->resetContensWidget(node);
                node->setOrientation(Qt::Vertical);
                node->show();

                from->move(target->pos());
                from->resize(target->size());

                target->setDockStatus();
                node->insertWidget(0, target);
                node->insertWidget(1, tmp);
            }
            break;
            default:
                break;
        }

        return true;
    }

    switch (area)
    {
        case CenterArea:
        {
            GLDDockPanel *tabPanel = new GLDDockPanel(this, dockFrmae_);
            tabPanel->panelType_ = GLDDockPanel::TabContainer;
            tabPanel->resize(target->size());
            tabPanel->move(target->pos());
            tabPanel->setAcceptDrops(false);
            GLDDockTabWidget *tabWidget = new GLDDockTabWidget(tabPanel);
            tabPanel->resetContensWidget(tabWidget);
            tabWidget->addTab(target, target->windowTitle());
            target->setDockStatus();
            target->setTabbedStatus(true, tabPanel);
            from->setDockStatus();
            from->setTabbedStatus(true, tabPanel);
            tabWidget->addTab(from, from->windowTitle());
            connect(tabWidget, SIGNAL(setBasePanelTitle(const QString &)), tabPanel, SLOT(setWindowTitle(const QString &)));
            tabPanel->setWindowTitle(tabWidget->currentWidget()->windowTitle());
            tabPanel->show();
        }
        break;

        case CenterLeftArea:
        {
            GLDDockPanel *tabPanel = new GLDDockPanel(this, dockFrmae_);
            tabPanel->panelType_ = GLDDockPanel::SplitContainer;
            tabPanel->resize(target->size());
            tabPanel->move(target->pos());
            tabPanel->setAcceptDrops(false);
            GLDDockNode *node = new GLDDockNode(tabPanel);
            tabPanel->resetContensWidget(node);
            node->setOrientation(Qt::Horizontal);
            from->setDockStatus();
            target->setDockStatus();
            node->insertWidget(0, from);
            node->insertWidget(1, target);
            tabPanel->setWindowTitle(QString("%1 & %2").arg(from->windowTitle()).arg(target->windowTitle()));
            tabPanel->show();
        }
        break;

        case CenterTopArea:
        {
            GLDDockPanel *tabPanel = new GLDDockPanel(this, dockFrmae_);
            tabPanel->panelType_ = GLDDockPanel::SplitContainer;
            tabPanel->resize(target->size());
            tabPanel->move(target->pos());
            tabPanel->setAcceptDrops(false);
            GLDDockNode *node = new GLDDockNode(tabPanel);
            tabPanel->resetContensWidget(node);
            node->setOrientation(Qt::Vertical);
            from->setDockStatus();
            target->setDockStatus();
            node->insertWidget(0, from);
            node->insertWidget(1, target);
            tabPanel->setWindowTitle(QString("%1 & %2").arg(from->windowTitle()).arg(target->windowTitle()));
            tabPanel->show();
        }
        break;

        case CenterRightArea:
        {
            GLDDockPanel *tabPanel = new GLDDockPanel(this, dockFrmae_);
            tabPanel->panelType_ = GLDDockPanel::SplitContainer;
            tabPanel->resize(target->size());
            tabPanel->move(target->pos());
            tabPanel->setAcceptDrops(false);
            GLDDockNode *node = new GLDDockNode(tabPanel);
            tabPanel->resetContensWidget(node);
            node->setOrientation(Qt::Horizontal);
            from->setDockStatus();
            target->setDockStatus();
            node->insertWidget(0, target);
            node->insertWidget(1, from);
            tabPanel->setWindowTitle(QString("%1 & %2").arg(target->windowTitle()).arg(from->windowTitle()));
            tabPanel->show();
        }
        break;

        case CenterBottomArea:
        {
            GLDDockPanel *tabPanel = new GLDDockPanel(this, dockFrmae_);
            tabPanel->panelType_ = GLDDockPanel::SplitContainer;
            tabPanel->resize(target->size());
            tabPanel->move(target->pos());
            tabPanel->setAcceptDrops(false);
            GLDDockNode *node = new GLDDockNode(tabPanel);
            tabPanel->resetContensWidget(node);
            node->setOrientation(Qt::Vertical);
            from->setDockStatus();
            target->setDockStatus();
            node->insertWidget(0, target);
            node->insertWidget(1, from);
            tabPanel->setWindowTitle(QString("%1 & %2").arg(target->windowTitle()).arg(from->windowTitle()));
            tabPanel->show();
        }
        break;
        default:
            break;
    }

    return true;
}

bool GLDDockManager::dockPanelToDockedPanel(GLDDockPanel *from, GLDDockPanel *target, DockArea area)
{
    if (target->isTabbed_)
    {
        switch (area)
        {
            case CenterArea:
            {
                if (from->panelType_ == GLDDockPanel::SplitContainer)
                {
                    return true;
                }

                if (from->panelType_ == GLDDockPanel::TabContainer)
                {
                    return true;
                }

                GLDDockTabWidget *tabWidget = qobject_cast<GLDDockTabWidget *>(target->parentTabPanel_->contensWidget_);
                from->setDockStatus();
                from->setTabbedStatus(true, target->parentTabPanel_);
                tabWidget->addTab(from, from->windowTitle());
            }
            break;

            case CenterLeftArea:
            case CenterTopArea:
            case CenterRightArea:
            case CenterBottomArea:
                dockPanelToPanel(from, target->parentTabPanel_, area);
                break;
            default:
                break;
        }

        return true;
    }

//  if (from->panelType_ == GLDDockPanel::TabPanel)
//  {
//      return true;
//  }

    if (from->panelType_ == GLDDockPanel::SplitContainer)
    {
        QList<int> sizes;
        sizes.push_back(5);
        sizes.push_back(5);

        GLDDockNode *parentNode = qobject_cast<GLDDockNode *>(target->parentWidget());
        QWidget *fromWidget = from->contensWidget_;
        from->deleteLater();

        assert(parentNode);

        if (parentNode->count() == 1)
        {
            assert(isRootNode(parentNode));

            switch (area)
            {
                case CenterLeftArea:
                    parentNode->setOrientation(Qt::Horizontal);
                    parentNode->insertWidget(0, fromWidget);
                    parentNode->setSizes(sizes);
                    break;

                case CenterTopArea:
                    parentNode->setOrientation(Qt::Vertical);
                    parentNode->insertWidget(0, fromWidget);
                    parentNode->setSizes(sizes);
                    break;

                case CenterRightArea:
                    parentNode->setOrientation(Qt::Horizontal);
                    parentNode->insertWidget(1, fromWidget);
                    parentNode->setSizes(sizes);
                    break;

                case CenterBottomArea:
                    parentNode->setOrientation(Qt::Vertical);
                    parentNode->insertWidget(1, fromWidget);
                    parentNode->setSizes(sizes);
                    break;

                case CenterArea:
                {
                }
                break;

                default:
                    return false;
            }

            return true;
        }

        //parentNode->count() != 1
        int parentIndex = parentNode->indexOf(target);
        QList<int> parentSizes = parentNode->sizes();

        GLDDockNode *node = new GLDDockNode(NULL);

        switch (area)
        {
            case CenterLeftArea:
                node->setOrientation(Qt::Horizontal);
                node->insertWidget(0, fromWidget);
                node->insertWidget(1, target);
                node->setSizes(sizes);
                parentNode->insertWidget(parentIndex, node);
                parentNode->setSizes(parentSizes);
                break;

            case CenterTopArea:
                node->setOrientation(Qt::Vertical);
                node->insertWidget(0, fromWidget);
                node->insertWidget(1, target);
                node->setSizes(sizes);
                parentNode->insertWidget(parentIndex, node);
                parentNode->setSizes(parentSizes);
                break;

            case CenterRightArea:
                node->setOrientation(Qt::Horizontal);
                node->insertWidget(0, target);
                node->insertWidget(1, fromWidget);
                node->setSizes(sizes);
                parentNode->insertWidget(parentIndex, node);
                parentNode->setSizes(parentSizes);
                break;

            case CenterBottomArea:
                node->setOrientation(Qt::Vertical);
                node->insertWidget(0, target);
                node->insertWidget(1, fromWidget);
                node->setSizes(sizes);
                parentNode->insertWidget(parentIndex, node);
                parentNode->setSizes(parentSizes);
                break;

            case CenterArea:
            {
            }
            break;

            default:
                return false;
        }

        return true;
    }

    QList<int> sizes;
    sizes.push_back(5);
    sizes.push_back(5);

    GLDDockNode *parentNode = qobject_cast<GLDDockNode *>(target->parentWidget());
    assert(parentNode);

    if (parentNode->count() == 1)
    {
        assert(isRootNode(parentNode));

        switch (area)
        {
            case CenterLeftArea:
                from->setDockStatus();
                parentNode->setOrientation(Qt::Horizontal);
                parentNode->insertWidget(0, from);
                parentNode->setSizes(sizes);
                break;

            case CenterTopArea:
                from->setDockStatus();
                parentNode->setOrientation(Qt::Vertical);
                parentNode->insertWidget(0, from);
                parentNode->setSizes(sizes);
                break;

            case CenterRightArea:
                from->setDockStatus();
                parentNode->setOrientation(Qt::Horizontal);
                parentNode->insertWidget(1, from);
                parentNode->setSizes(sizes);
                break;

            case CenterBottomArea:
                from->setDockStatus();
                parentNode->setOrientation(Qt::Vertical);
                parentNode->insertWidget(1, from);
                parentNode->setSizes(sizes);
                break;

            case CenterArea:
            {
                GLDDockPanel *tabPanel = new GLDDockPanel(this, dockFrmae_);
                tabPanel->panelType_ = GLDDockPanel::TabContainer;
                tabPanel->setAcceptDrops(false);
                GLDDockTabWidget *tabWidget = new GLDDockTabWidget(tabPanel);
                tabPanel->resetContensWidget(tabWidget);
                tabWidget->addTab(target, target->windowTitle());
                from->setDockStatus();
                from->setTabbedStatus(true, tabPanel);
                tabWidget->addTab(from, from->windowTitle());
                target->setTabbedStatus(true, tabPanel);
                tabPanel->resize(target->size());
                tabPanel->setDockStatus();
                parentNode->insertWidget(0, tabPanel);
                connect(tabWidget, SIGNAL(setBasePanelTitle(const QString &)), tabPanel, SLOT(setWindowTitle(const QString &)));
                tabPanel->setWindowTitle(tabWidget->currentWidget()->windowTitle());
            }
            break;

            default:
                return false;
        }

        return true;
    }

    int parentIndex = parentNode->indexOf(target);
    QList<int> parentSizes = parentNode->sizes();

    GLDDockNode *node = new GLDDockNode(NULL);

    switch (area)
    {
        case CenterLeftArea:
            from->setDockStatus();
            node->setOrientation(Qt::Horizontal);
            node->insertWidget(0, from);
            node->insertWidget(1, target);
            node->setSizes(sizes);
            parentNode->insertWidget(parentIndex, node);
            parentNode->setSizes(parentSizes);
            break;

        case CenterTopArea:
            from->setDockStatus();
            node->setOrientation(Qt::Vertical);
            node->insertWidget(0, from);
            node->insertWidget(1, target);
            node->setSizes(sizes);
            parentNode->insertWidget(parentIndex, node);
            parentNode->setSizes(parentSizes);
            break;

        case CenterRightArea:
            from->setDockStatus();
            node->setOrientation(Qt::Horizontal);
            node->insertWidget(0, target);
            node->insertWidget(1, from);
            node->setSizes(sizes);
            parentNode->insertWidget(parentIndex, node);
            parentNode->setSizes(parentSizes);
            break;

        case CenterBottomArea:
            from->setDockStatus();
            node->setOrientation(Qt::Vertical);
            node->insertWidget(0, target);
            node->insertWidget(1, from);
            node->setSizes(sizes);
            parentNode->insertWidget(parentIndex, node);
            parentNode->setSizes(parentSizes);
            break;

        case CenterArea:
        {
            GLDDockPanel *tabPanel = new GLDDockPanel(this, dockFrmae_);
            tabPanel->panelType_ = GLDDockPanel::TabContainer;
            tabPanel->setAcceptDrops(false);
            GLDDockTabWidget *tabWidget = new GLDDockTabWidget(tabPanel);
            tabPanel->resetContensWidget(tabWidget);
            tabWidget->addTab(target, target->windowTitle());
            from->setDockStatus();
            from->setTabbedStatus(true, tabPanel);
            tabWidget->addTab(from, from->windowTitle());
            target->setTabbedStatus(true, tabPanel);
            tabPanel->resize(target->size());
            tabPanel->setDockStatus();
            parentNode->insertWidget(parentIndex, tabPanel);
            parentNode->setSizes(sizes);
            connect(tabWidget, SIGNAL(setBasePanelTitle(const QString &)), tabPanel, SLOT(setWindowTitle(const QString &)));
            tabPanel->setWindowTitle(tabWidget->currentWidget()->windowTitle());
        }
        break;

        default:
            return false;
    }

    return true;
}
