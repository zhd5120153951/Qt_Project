#include "GLDClassicsOutLookBar.h"

#include <QAction>
#include <QMouseEvent>
#include <QMenu>
#include "GLDWidget_Global.h"
#include "GLDFileUtils.h"

const int c_maxToolButtonShow    = 3;

// 导航项目
static const int conNavigationItemHeight = 34;

const GString c_sOutLookBarQssFile = ":qsses/outLookBar.qss";

/* GLDOutLookBar */
GLDClassicsOutLookBar::GLDClassicsOutLookBar(QWidget *parent) :
    QScrollArea(parent), m_curAction(NULL), m_curView(NULL)
{
    m_splitter = new GLDNavigationSplitter(Qt::Vertical, this);
    connect(m_splitter, SIGNAL(splitterChanged(bool)),
            this, SLOT(splitterChanged(bool)));

    m_outLookView = new GLDOutLookView(m_splitter);
    m_outLookNavigBar = new GLDOutLookNavigationBar(m_splitter);
    m_thumbnailButtons = new GLDThumbnailBar(this);
    initLayout();

    this->setStyleSheet(loadQssFile(c_sOutLookBarQssFile));
}

GLDClassicsOutLookBar::~GLDClassicsOutLookBar()
{
    for (int i = 0; i < m_actions.count(); ++i)
    {
        GLDOutLookBarAction *action = m_actions.at(i);
        freeAndNil(action);
    }

    m_actions.clear();

    m_curAction = NULL;
    m_curView = NULL;
}

GLDNavigationItem *GLDClassicsOutLookBar::addBarItem(QWidget *view, const GString &text, const QIcon &icon)
{
    GLDOutLookBarAction *action = new GLDOutLookBarAction(text, icon, this);
    m_actions.append(action);

    GLDNavigationItem *item = m_outLookNavigBar->addItem(text, icon);
    item->addOutLookBarAction(action);
    m_outLookView->addView(view);
    view->hide();
    m_itemViews.insert(action->uuid(), view);
    connect(action, SIGNAL(clicked(GString)), this, SLOT(itemClick(GString)));
    return item;
}

QAction *GLDClassicsOutLookBar::addThumbnailButton(QWidget *view, const GString &text, const QIcon &icon)
{
    GLDOutLookBarAction *action = new GLDOutLookBarAction(text, icon, this);
    m_actions.append(action);

    m_thumbnailButtons->addLeftAction(action);
    m_outLookView->addView(view);
    view->hide();
    m_itemViews.insert(action->uuid(), view);
    connect(action, SIGNAL(clicked(GString)), this, SLOT(itemClick(GString)));
    return action->action();
}

void GLDClassicsOutLookBar::setTitleHeight(int nHeight)
{
    m_outLookView->setTitleHeight(nHeight);
}

int GLDClassicsOutLookBar::currentIndex() const
{
    return findActionIndex(m_curAction->uuid());
}

void GLDClassicsOutLookBar::setCurrentIndex(int nIndex)
{
    if (m_actions.count() == 0)
    {
        return;
    }

    if ((nIndex < 0) || (nIndex >= m_actions.count()))
    {
        nIndex = 0;
    }

    itemClick(m_actions[nIndex]->uuid());
}

QWidget *GLDClassicsOutLookBar::currentWidget() const
{
    return m_itemViews.value(m_curAction->uuid());
}

QWidget *GLDClassicsOutLookBar::widget(int nIndex) const
{
    if ((nIndex < 0) || (nIndex >= m_actions.count()))
    {
        return NULL;
    }

    return m_itemViews.value(m_actions[nIndex]->uuid());
}

void GLDClassicsOutLookBar::setCurrentWidget(QWidget *widget)
{
    GString uuid = m_curAction->uuid();
    QWidget *preWidget = m_itemViews[uuid];

    if (preWidget != widget)
    {
        m_outLookView->removeView(preWidget);
        freeAndNil(preWidget);
        m_itemViews[uuid] = widget;
        m_outLookView->addView(widget);
    }

    widget->show();
    m_curView = widget;
}

GString GLDClassicsOutLookBar::currentText() const
{
    return m_curAction->text();
}

void GLDClassicsOutLookBar::setToolButtonStyle(Qt::ToolButtonStyle style)
{
    m_thumbnailButtons->setToolButtonStyle(style);
}

void GLDClassicsOutLookBar::clearItems()
{
    m_outLookNavigBar->clearItems();
    m_thumbnailButtons->clear();

    if (NULL != m_curView)
    {
        m_curView->hide();
        m_curView = NULL;
    }

    m_outLookView->setTitle("");
    m_itemViews.clear();
    m_curAction = NULL;

    for (int i = 0; i < m_actions.count(); ++i)
    {
        GLDOutLookBarAction *action = m_actions.at(i);
        freeAndNil(action);
    }

    m_actions.clear();
}

//隐藏ThumbnailButton的接口.
void GLDClassicsOutLookBar::setIsHideThumbnailButton(bool isHide)
{
    if (isHide)
    {
        m_thumbnailButtons->hide();
        m_outLookNavigBar->hide();
    }
    else
    {
        m_thumbnailButtons->show();
        m_outLookNavigBar->show();
    }
}

void GLDClassicsOutLookBar::itemClick(const GString &itemID)
{
    QHash<GString, QWidget *>::const_iterator it = m_itemViews.find(itemID);

    if (it != m_itemViews.end())
    {
        GLDOutLookBarAction *act = this->findAction(itemID);

        if (m_curAction == act)
        {
            return;
        }

        if (NULL != m_curView)
        {
            m_curView->hide();
        }

        if (NULL != m_curAction)
        {
            setItemCheck(m_curAction->uuid(), false);
        }

        QWidget *widget = it.value();
        widget->show();
        m_outLookView->setTitle(act->text());
        act->setChecked(true);
        setItemCheck(act->uuid(), true);
        m_curAction = act;
        m_curView = widget;
        emit currentChanged(findActionIndex(itemID));
    }
}

void GLDClassicsOutLookBar::splitterChanged(bool downward)
{
    if (downward)
    {
        if (m_outLookNavigBar->itemCount() == 0)
        {
            return;
        }

        m_outLookNavigBar->removeLastAction();
        int nCount = m_outLookNavigBar->itemCount();
        GLDOutLookBarAction *action = m_actions.at(nCount);
        m_thumbnailButtons->addLeftAction(action);
    }
    else
    {
        if (m_outLookNavigBar->itemCount() == m_actions.count())
        {
            return;
        }

        GLDOutLookBarAction *action = m_actions.at(m_outLookNavigBar->itemCount());
        GLDNavigationItem *item = m_outLookNavigBar->addItem(action->text(), action->icon());
        item->addOutLookBarAction(action);
        m_thumbnailButtons->removeAction(action);
    }
}

void GLDClassicsOutLookBar::initLayout()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    QSplitter *splitter = new QSplitter();
    layout->addWidget(splitter);

    QWidget *navigation = new QWidget(splitter);

    QVBoxLayout *navigatLayout = new QVBoxLayout(splitter);
    navigatLayout->setSpacing(0);
    navigatLayout->setContentsMargins(0, 0, 0, 0);

    navigation->setLayout(navigatLayout);

    navigatLayout->addWidget(m_splitter);
    navigatLayout->addWidget(m_thumbnailButtons);
}

GLDOutLookBarAction *GLDClassicsOutLookBar::findAction(const GString &uID)
{
    for (int i = 0; i < m_actions.count(); ++i)
    {
        GLDOutLookBarAction *olAct = m_actions.at(i);

        if (olAct->uuid() == uID)
        {
            return olAct;
        }
    }

    return NULL;
}

int GLDClassicsOutLookBar::findActionIndex(const GString &itemID) const
{
    for (int i = 0; i < m_actions.count(); ++i)
    {
        GLDOutLookBarAction *olAct = m_actions.at(i);

        if (sameText(olAct->uuid(), itemID))
        {
            return i;
        }
    }

    return -1;
}

void GLDClassicsOutLookBar::setItemCheck(const GString &uID, bool checked)
{
    if (! m_thumbnailButtons->hasAction(uID))
    {
        m_outLookNavigBar->findByUuid(uID)->setChecked(checked);
    }

    GLDOutLookBarAction *action = findAction(uID);

    if (NULL != action)
    {
        action->setChecked(checked);
    }
}

/* GLDOutLookNavigationBar */
GLDOutLookNavigationBar::GLDOutLookNavigationBar(QWidget *parent)
    : QWidget(parent)
{
    m_itemList = new QObjectList();
    this->setMaximumHeight(0);
    this->setMinimumHeight(0);
    initLayout();
}

GLDOutLookNavigationBar::~GLDOutLookNavigationBar()
{
    clearItems();
    freeAndNil(m_itemList);
}

GLDNavigationItem *GLDOutLookNavigationBar::addItem(GLDNavigationItem *item)
{
    m_itemList->push_back(item);
    m_itemLayout->addWidget(item);
    this->setMaximumHeight(itemCount() * conNavigationItemHeight);
    this->setMinimumHeight(itemCount() * conNavigationItemHeight);
    return item;
}

GLDNavigationItem *GLDOutLookNavigationBar::addItem(const GString &text)
{
    GLDNavigationItem *item = new GLDNavigationItem(this);
    item->setText(text);
    return addItem(item);
}

GLDNavigationItem *GLDOutLookNavigationBar::addItem(const GString &text, const QIcon &icon)
{
    GLDNavigationItem *item = new GLDNavigationItem(text, icon, this);
    item->setText(text);
    item->setIcon(icon);
    return addItem(item);
}

bool GLDOutLookNavigationBar::removeItem(GLDNavigationItem *item)
{
    return m_itemList->removeOne(item);
}

bool GLDOutLookNavigationBar::removeItem(int index)
{
    if (0 == m_itemList->count())
    {
        return false;
    }

    m_itemList->removeAt(index);
    return true;
}

GLDNavigationItem *GLDOutLookNavigationBar::addLastAction(GLDOutLookBarAction *action)
{
    GLDNavigationItem *item = new GLDNavigationItem(action->text(), action->icon(), this);
    item->addOutLookBarAction(action);
    addItem(item);
    return item;
}

GLDNavigationItem *GLDOutLookNavigationBar::findByUuid(const GString &value)
{
    for (int i = 0; i < m_itemList->count(); i++)
    {
        GLDNavigationItem *item = static_cast<GLDNavigationItem *>(m_itemList->at(i));

        if (item->uuid() == value)
        {
            return item;
        }
    }

    return NULL;
}

QAction *GLDOutLookNavigationBar::removeLastAction()
{
    if (0 == m_itemList->count())
    {
        return NULL;
    }

    GLDNavigationItem *item = (GLDNavigationItem *)m_itemList->last();
    QAction *action = item->actions().first();
    m_itemList->removeOne(item);
    freeAndNil(item);

    this->setMaximumHeight(itemCount() * conNavigationItemHeight);
    this->setMinimumHeight(itemCount() * conNavigationItemHeight);
    return action;
}

void GLDOutLookNavigationBar::clearItems()
{
    for (int i = 0; i < m_itemList->count(); ++i)
    {
        GLDNavigationItem *item = (GLDNavigationItem *)m_itemList->at(i);
        freeAndNil(item);
    }

    this->setMaximumHeight(0);
    this->setMinimumHeight(0);
    m_itemList->clear();
}

int GLDOutLookNavigationBar::needItemCount(int offset)
{
    return (this->geometry().height() - offset) / conNavigationItemHeight;
}

int GLDOutLookNavigationBar::itemCount()
{
    return m_itemList->count();
}

void GLDOutLookNavigationBar::initLayout()
{
    m_itemLayout = new QVBoxLayout(this);
    m_itemLayout->setSpacing(0);
    m_itemLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(m_itemLayout);
}

/*GLDNavigationItem*/
GLDNavigationItem::GLDNavigationItem(QWidget *parent) : QPushButton(parent)
{
    init();
}

GLDNavigationItem::GLDNavigationItem(const GString &text, const QIcon &icon, QWidget *parent)
    : QPushButton(icon, text, parent)
{
    init();
}

GLDNavigationItem::~GLDNavigationItem()
{
}

void GLDNavigationItem::addOutLookBarAction(GLDOutLookBarAction *action)
{
    setText(action->text());
    setIcon(action->icon());
    m_uuid = action->uuid();
    connect(this, SIGNAL(clicked()), action->action(), SLOT(trigger()));
    setChecked(action->action()->isChecked());
    addAction(action->action());
}

void GLDNavigationItem::setChecked(bool checked)
{
    setCheckable(checked);
    QPushButton::setChecked(checked);
    QPushButton::update();
}

void GLDNavigationItem::init()
{

}

/* GLDOutLookView */
GLDOutLookView::GLDOutLookView(QWidget *parent, Qt::WindowFlags f)
    : QFrame(parent, f)
{
    m_titleWgt = new QWidget(this);
    QHBoxLayout *titleLayout = new QHBoxLayout(this);
    m_titleWgt->setLayout(titleLayout);

    m_titleWgt->setFixedHeight(30);
    m_titleWgt->setObjectName("titleWidget");
    m_title = new QLabel(m_titleWgt);
    m_title->setAlignment(Qt::AlignLeft);
    titleLayout->addWidget(m_title);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    QWidget *container = new QWidget(this);
    container->setObjectName("container");

    layout->addWidget(m_titleWgt);
    layout->addWidget(container);

    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    container->setLayout(m_layout);
}

GLDOutLookView::~GLDOutLookView()
{
}

void GLDOutLookView::addView(QWidget *widget)
{
    widget->setParent(this);//归outLookView来管释放
    m_layout->addWidget(widget);
}

void GLDOutLookView::setTitle(const GString &title)
{
    m_title->setText(title);
}

void GLDOutLookView::setTitleHeight(int nHeight)
{
    m_titleWgt->setFixedHeight(nHeight);
}

void GLDOutLookView::removeView(QWidget *widget)
{
    m_layout->removeWidget(widget);
}

/* GLDThumbnailBar */
GLDThumbnailBar::GLDThumbnailBar(QWidget *parent) : QToolBar(parent)
{
    m_foldAction = new QAction(QIcon(":/icons/more.png"), QString(""), this);
    connect(m_foldAction, SIGNAL(triggered()), this, SLOT(showMoreAction()));
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);//默认是图标加文字
}

GLDThumbnailBar::~GLDThumbnailBar()
{
    m_btnActions.clear();
}

GLDOutLookBarAction *GLDThumbnailBar::addAction(const QIcon &icon, const GString &text)
{
    GLDOutLookBarAction *action = new GLDOutLookBarAction(text, icon, this);
    this->addAction(action);
    return action;
}

GLDOutLookBarAction *GLDThumbnailBar::addAction(GLDOutLookBarAction *action)
{
    this->insertAction(m_btnActions.top()->action(), action->action());
    m_btnActions.push(action);
    return action;
}

void GLDThumbnailBar::removeAction(GLDOutLookBarAction *action)
{
    if (m_btnActions.count() > c_maxToolButtonShow)
    {
        m_btnActions.pop();
        int nIndex = m_btnActions.count() - c_maxToolButtonShow;
        QToolBar::removeAction(action->action());
        QToolBar::insertAction(m_foldAction, m_btnActions.at(nIndex)->action());

        if (m_btnActions.count() == c_maxToolButtonShow)
        {
            QToolBar::removeAction(m_foldAction);
        }
    }
    else
    {
        m_btnActions.pop();
        QToolBar::removeAction(action->action());
    }
}

void GLDThumbnailBar::addLeftAction(GLDOutLookBarAction *action)
{
    if (m_btnActions.empty())
    {
        QToolBar::insertAction(NULL, action->action());
    }
    else
    {
        if (c_maxToolButtonShow == m_btnActions.count())
        {
            GLDOutLookBarAction *oAction = static_cast<GLDOutLookBarAction *>(m_btnActions.at(0));
            QToolBar::removeAction(oAction->action());
            QToolBar::insertAction(NULL, m_foldAction);
            QToolBar::insertAction(m_btnActions.top()->action(), action->action());
        }
        else if (c_maxToolButtonShow < m_btnActions.count())
        {
            int nIndex = m_btnActions.count() - c_maxToolButtonShow;
            GLDOutLookBarAction *oAction = static_cast<GLDOutLookBarAction *>(m_btnActions.at(nIndex));
            QToolBar::removeAction(oAction->action());
            QToolBar::insertAction(m_btnActions.top()->action(), action->action());
        }
        else
        {
            QToolBar::insertAction(m_btnActions.top()->action(), action->action());
        }
    }

    m_btnActions.push(action);
}

void GLDThumbnailBar::removeLeftAction()
{
    this->removeAction(m_btnActions.top());
}

bool GLDThumbnailBar::hasAction(const GString &uId)
{
    for (int i = 0; i < m_btnActions.count(); i++)
    {
        GLDOutLookBarAction *oAction = static_cast<GLDOutLookBarAction *>(m_btnActions.at(i));

        if (oAction->uuid() == uId)
        {
            return true;
        }
    }

    return false;
}

void GLDThumbnailBar::clearButtons()
{
    m_btnActions.clear();
}

void GLDThumbnailBar::showMoreAction()
{
    QMenu *menu = new QMenu(this);

    for (int i = 0; i < m_btnActions.count() - c_maxToolButtonShow; i++)
    {
        menu->addAction(m_btnActions.at(i)->action());
    }

    menu->popup(QCursor::pos());
}

/* GLDNavigationSplitter */
GLDNavigationSplitter::GLDNavigationSplitter(Qt::Orientation orientation,
        QWidget *parent):
    QSplitter(orientation, parent)
{
    setChildrenCollapsible(false);
    setFixedHeight(5);
}

QSplitterHandle *GLDNavigationSplitter::createHandle()
{
    GLDNavigationSplitterHandle *handle = new GLDNavigationSplitterHandle(this->orientation(), this);
    connect(handle, SIGNAL(splitterHandleChanged(bool)),
            this, SLOT(splitterHandleChanged(bool)));
    return handle;
}

void GLDNavigationSplitter::splitterHandleChanged(bool downward)
{
    emit splitterChanged(downward);
}

GLDNavigationSplitterHandle::GLDNavigationSplitterHandle(Qt::Orientation o, QSplitter *parent):
    QSplitterHandle(o, parent), m_windowPosY(0)
{
    setFixedHeight(5);
}

void GLDNavigationSplitterHandle::mouseMoveEvent(QMouseEvent *e)
{
    double pos = e->windowPos().y();

    if (pos > m_windowPosY + conNavigationItemHeight)
    {
        emit splitterHandleChanged(true);
        m_windowPosY = pos;
    }
    else if (pos < m_windowPosY - conNavigationItemHeight)
    {
        emit splitterHandleChanged(false);
        m_windowPosY = pos;
    }

    QSplitterHandle::mouseMoveEvent(e);
}

void GLDNavigationSplitterHandle::mousePressEvent(QMouseEvent *e)
{
    m_windowPosY = e->windowPos().y();
    QSplitterHandle::mousePressEvent(e);
}

GLDOutLookBarAction::GLDOutLookBarAction(const GString &text, const QIcon &icon, QObject *parent)
{
    m_action = new QAction(icon, text, parent);
    connect(m_action, SIGNAL(triggered()), this, SLOT(triggered()));

    m_uuid = QUuid::createUuid();
}

GLDOutLookBarAction::~GLDOutLookBarAction()
{
}

void GLDOutLookBarAction::triggered()
{
    emit clicked(m_uuid.toString());
}

void GLDOutLookBarAction::setChecked(bool checked)
{
    m_action->setCheckable(true);
    m_action->setChecked(checked);
}
