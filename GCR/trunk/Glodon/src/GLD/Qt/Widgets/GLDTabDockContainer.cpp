#include "GLDTabDockContainer.h"
#include <QString>
#include <QStylePainter>
#include <QStyleOption>
#include <cassert>
#include <QMainWindow>
#include <QResizeEvent>
#include <QApplication>
#include <QHBoxLayout>
#include <QFrame>
#include <QAction>
#include <QTimer>
#include <QTime>
#include <QRect>
#include "GLDUIUtils.h"
#include "GLDException.h"
#include "GLDWidget_Global.h"
//////////////////////////////////////////////////////////////////////////

GLDTabDockContainerTitleWidget::GLDTabDockContainerTitleWidget(QWidget *parent)
    : QWidget(parent)
    , m_dockerWidget(NULL)
{
    m_toolBar = new QToolBar(this);
    m_title = new QLabel(this);
    m_anchorButton = new GLDTabDockContainerWidgetAnchorButton(this);
    m_layout = new QHBoxLayout(this);
    m_toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_toolBar->setFixedHeight(25);
    m_userToolBar = new QToolBar(this);

    m_title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    m_toolBar->addWidget(m_anchorButton);

    m_layout->setMargin(2);
    m_layout->setContentsMargins(5, 0, 0, 0);
    m_layout->addWidget(m_title);
    m_layout->addStretch();
    m_layout->addWidget(m_userToolBar);
    m_layout->addWidget(m_toolBar);


    setLayout(m_layout);
}

void GLDTabDockContainerTitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dockerWidget == NULL)
    {
        GLDTabDockContainerWidget *widget = qobject_cast<GLDTabDockContainerWidget *>(parent());

        if (widget != NULL)
        {
            m_dockerWidget = widget;
        }
    }

    if (m_dockerWidget != NULL)
    {
        if (!m_dockerWidget->isAnchored())
        {
            event->accept();
            return;
        }
    }

    event->ignore();
}

void GLDTabDockContainerTitleWidget::setDockWidget(GLDTabDockContainerWidget *dockerWidget)
{
    m_dockerWidget = dockerWidget;
}

GLDTabDockContainerWidgetAnchorButton *GLDTabDockContainerTitleWidget::addAnchorButton()
{
    GLDTabDockContainerWidgetAnchorButton *anchorButton = new GLDTabDockContainerWidgetAnchorButton(this);
    anchorButton->setFixedSize(QSize(15, 15));
    m_userToolBar->setMinimumWidth(m_userToolBar->minimumWidth() + 22);
    m_action = m_userToolBar->insertWidget(m_action, anchorButton);

    return anchorButton;
}

QToolBar *GLDTabDockContainerTitleWidget::toolBar()
{
    return m_userToolBar;
}

//////////////////////////////////////////////////////////////////////////
//GLDTabDockContainerWidget
GLDTabDockContainerWidget::GLDTabDockContainerWidget(const QString &title, QWidget *parent /*= 0*/)
    : QDockWidget(title, parent)
    , m_anchordIcon(":/icons/TabDockContainerAnchor.png")
    , m_notAnchoredIcon(":/icons/TabDockContainerNotAnchor.png")
    , m_currentAnchoredState(true)
    , m_floatable(true)
    , m_closable(false)
    , m_autoFillHeight(true)
    , m_anchoredToolTip(getGLDi18nStr(g_rsAutoHide))
    , m_notAnchoredToolTip(getGLDi18nStr(g_rsLocked))
    , m_timerID(0)
    , m_bisEntered(false)
    , m_bisHide(false)
    , m_bisFloating(false)
    , m_interval(500)
{
    m_titleWidget = new GLDTabDockContainerTitleWidget(this);
    m_titleWidget->m_anchorButton->setIconSize(QSize(15, 15));
    m_titleWidget->m_anchorButton->setFixedSize(QSize(15, 15));
    m_titleWidget->m_anchorButton->setIcon(m_anchordIcon);
    m_titleWidget->m_anchorButton->setToolTip(m_currentAnchoredState ? m_anchoredToolTip : m_notAnchoredToolTip);
    setTitle(title);
    setTitleBarWidget(m_titleWidget);
    updateFloatAndCloseState();
    connect(m_titleWidget->m_anchorButton, SIGNAL(clicked()), this, SLOT(anchorButtonClicked()));
}

GLDTabDockContainerWidget::GLDTabDockContainerWidget(QWidget *parent /*= 0*/)
    : QDockWidget(parent)
    , m_anchordIcon(":/icons/TabDockContainerAnchor.png")
    , m_notAnchoredIcon(":/icons/TabDockContainerNotAnchor.png")
    , m_currentAnchoredState(true)
    , m_floatable(true)
    , m_closable(false)
    , m_autoFillHeight(true)
    , m_anchoredToolTip(getGLDi18nStr(g_rsAutoHide))
    , m_notAnchoredToolTip(getGLDi18nStr(g_rsLocked))
    , m_timerID(0)
    , m_bisEntered(false)
    , m_bisHide(false)
    , m_bisFloating(false)
    , m_interval(500)
{
    m_titleWidget = new GLDTabDockContainerTitleWidget(this);
    m_titleWidget->m_anchorButton->setIconSize(QSize(15, 15));
    m_titleWidget->m_anchorButton->setFixedSize(QSize(15, 15));
    m_titleWidget->m_anchorButton->setIcon(m_anchordIcon);
    m_titleWidget->m_anchorButton->setToolTip(m_currentAnchoredState ? m_anchoredToolTip : m_notAnchoredToolTip);
    setTitleBarWidget(m_titleWidget);
    updateFloatAndCloseState();
    connect(m_titleWidget->m_anchorButton, SIGNAL(clicked()), this, SLOT(anchorButtonClicked()));
}

QToolBar *GLDTabDockContainerWidget::titleToolBar()
{
    return m_titleWidget->toolBar();
}

void GLDTabDockContainerWidget::setFloatable(bool floatable)
{
    m_floatable = floatable;
    updateFloatAndCloseState();
}

bool GLDTabDockContainerWidget::isFloatable() const
{
    return m_floatable;
}

void GLDTabDockContainerWidget::setAnchoredIcon(const QIcon &anchoredIcon)
{
    m_anchordIcon = anchoredIcon;
    updateFloatAndCloseState();
}

void GLDTabDockContainerWidget::setNotAnchoredIcon(const QIcon &notAnchoredIcon)
{
    m_notAnchoredIcon = notAnchoredIcon;
    updateFloatAndCloseState();
}

QIcon GLDTabDockContainerWidget::anchoredIcon() const
{
    return m_anchordIcon;
}

QIcon GLDTabDockContainerWidget::notAnchoredIcon() const
{
    return m_notAnchoredIcon;
}

void GLDTabDockContainerWidget::showWidget()
{
    if (m_currentAnchoredState)
    {
        show();
    }
}

void GLDTabDockContainerWidget::hideWidget()
{
    if (m_currentAnchoredState)
    {
        hide();
    }
}

bool GLDTabDockContainerWidget::isAnchored() const
{
    return m_currentAnchoredState;
}

void GLDTabDockContainerWidget::setAnchorToolTip(const QString &anchoredToolTip, const QString &notAnchoredToolTip)
{
    m_anchoredToolTip = anchoredToolTip;
    m_notAnchoredToolTip = notAnchoredToolTip;
}

void GLDTabDockContainerWidget::anchorToolTip(QString &anchoredToolTip, QString &notAnchoredToolTip) const
{
    anchoredToolTip = m_anchoredToolTip;
    notAnchoredToolTip = m_notAnchoredToolTip;
}

void GLDTabDockContainerWidget::setAnchored(bool anchored)
{
    m_currentAnchoredState = anchored;

    if (m_currentAnchoredState)
    {
        m_titleWidget->m_anchorButton->setIcon(m_anchordIcon);
        m_titleWidget->m_anchorButton->setToolTip(m_anchoredToolTip);
    }
    else
    {
        m_titleWidget->m_anchorButton->setIcon(m_notAnchoredIcon);
        m_titleWidget->m_anchorButton->setToolTip(m_notAnchoredToolTip);
    }

    m_bisFloating = !m_currentAnchoredState;
    emit anchorChanged(m_currentAnchoredState);
    m_bisFloating = m_currentAnchoredState;

}

void GLDTabDockContainerWidget::pauseTimer()
{
    killTimer(m_timerID);
}

void GLDTabDockContainerWidget::resumeTimer()
{
    m_timerID = startTimer(m_interval);
}

void GLDTabDockContainerWidget::anchorButtonClicked()
{
    setAnchored(!m_currentAnchoredState);
}

void GLDTabDockContainerWidget::leaveHide()
{
    hide();
}

void GLDTabDockContainerWidget::setLeaved(bool isLeaved)
{
    m_bisHide = isLeaved;
}

void GLDTabDockContainerWidget::updateFloatAndCloseState()
{
    if (m_closable)
    {
        setFeatures(features() | QDockWidget::DockWidgetClosable);
    }
    else
    {
        setFeatures(features() & ~QDockWidget::DockWidgetClosable);
    }

    if (m_floatable)
    {
        setFeatures(features() | QDockWidget::DockWidgetFloatable);
    }
    else
    {
        setFeatures(features() & ~QDockWidget::DockWidgetFloatable);
    }
}

void GLDTabDockContainerWidget::enterEvent(QEvent *)
{
    m_bisEntered = true;

    if (m_timerID != 0)
    {
        killTimer(m_timerID);
        m_timerID = 0;
    }

    if (m_bisHide)
    {
        emit setHideAndEntered(m_bisHide, m_bisEntered);
        m_bisEntered = false;
    }
}

void GLDTabDockContainerWidget::leaveEvent(QEvent *)
{
    if (!m_currentAnchoredState)
    {
        if (m_timerID != 0)
        {
            killTimer(m_timerID);
            m_timerID = 0;
        }

        if (!m_bisFloating)
        {
            m_timerID = startTimer(m_interval);
        }
    }
}

void GLDTabDockContainerWidget::setTimerInterval(int interval)
{
    m_interval = interval;
}

int GLDTabDockContainerWidget::timerInterval()
{
    return m_interval;
}

void GLDTabDockContainerWidget::timerEvent(QTimerEvent *e)
{
    if (!m_currentAnchoredState)
    {
        if (e->timerId() == m_timerID)
        {
            QPoint mousePoint = QCursor::pos();

            mousePoint = mapFromGlobal(mousePoint);

            if (!rect().contains(mousePoint))
            {
                hide();
                killTimer(m_timerID);
                m_timerID = 0;
            }
        }
    }
}

void GLDTabDockContainerWidget::setAutoFillHeight(bool autoHeight)
{
    m_autoFillHeight = autoHeight;
}

bool GLDTabDockContainerWidget::isAutoFillHeight() const
{
    return m_autoFillHeight;
}

void GLDTabDockContainerWidget::setTitle(const QString &title)
{
    m_titleWidget->m_title->setText(title);
}

QString GLDTabDockContainerWidget::title() const
{
    return m_titleWidget->m_title->text();
}

GLDTabDockContainerTabBar::GLDTabDockContainerTabBar(QWidget *parent)
    : QTabBar(parent), m_curIndex(-1), m_textOrientation(Qt::Horizontal), m_timerID(0)
    , m_bisHide(false), m_bisEntered(false), m_interval(500)
{
    setMouseTracking(true);
}

void GLDTabDockContainerTabBar::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pt = event->pos();
    int ndx = tabAt(pt);
    m_preIndex = m_curIndex;

    if (ndx != m_curIndex)
    {
        m_curIndex = ndx;

        if (m_curIndex != -1)
        {
            setCurrentIndex(m_curIndex);
        }
    }

    emit tabChanged(m_curIndex, m_preIndex);
}

void GLDTabDockContainerTabBar::leaveEvent(QEvent *)
{
    m_bisHide = true;

    if (m_timerID != 0)
    {
        killTimer(m_timerID);
    }

    m_timerID = startTimer(m_interval);
    emit setLeaved(m_bisHide);
}

void GLDTabDockContainerTabBar::setTimerInterval(int interval)
{
    m_interval = interval;
}

void GLDTabDockContainerTabBar::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == m_timerID)
    {
        if (true == m_bisHide && false == m_bisEntered)
        {
            emit tabLeave();
        }

        killTimer(m_timerID);
        m_timerID = 0;
        m_bisHide = false;
        m_bisEntered = false;
    }
}

void GLDTabDockContainerTabBar::setHideAndEntered(bool isHide, bool isEntered)
{
    m_bisHide = isHide;
    m_bisEntered = isEntered;
}

static QString verticalString(const QString &str)
{
    QChar buffer[255];
    int nChar = 0;

    for (int i = 0; i < str.count(); ++i)
    {
        buffer[nChar] = str[i];
        buffer[++nChar] = '\n';
        ++nChar;
    }

    if (nChar > 0)
    {
        buffer[nChar - 1] = '\0';
    }

    return QString(buffer);
}

void GLDTabDockContainerTabBar::paintEvent(QPaintEvent *event)
{

    G_UNUSED(event);
    for (int i = 0; i < count(); i++)
	{
        QStylePainter painter(this);
        QStyleOptionTabV3 tab;
        initStyleOption(&tab, i);

        if (m_textOrientation == Qt::Vertical)
        {
            QString strOldText = tab.text;
            tab.text = "";
            painter.drawControl(QStyle::CE_TabBarTab, tab);

            tab.text = verticalString(strOldText);

            QTextOption opt;
            opt.setAlignment(Qt::AlignCenter);
            painter.setPen(tab.palette.color(QPalette::WindowText));
            painter.drawText(tab.rect, tab.text, opt);
        }
        else
        {
            painter.drawControl(QStyle::CE_TabBarTab, tab);
        }
    }
}

/////////////////////////////////////////////////////////////////////////
//ContainerBar


GLDTabDockContainerBar::GLDTabDockContainerBar(QWidget *parent /*= 0*/)
    : QWidget(parent), m_timerLong(0)
{
    m_dockToolBar = new QToolBar("dockToolBar", this);
    m_tabBar = new GLDTabDockContainerTabBar(this);
    m_dockToolBar->addWidget(m_tabBar);
    m_dockToolBar->setVisible(false);
    m_dockToolBar->setMovable(false);
    m_dockToolBar->setFloatable(false);
    connect(m_tabBar, SIGNAL(tabChanged(int, int)), this, SLOT(currentTabChanged(int, int)));
}

void GLDTabDockContainerBar::selectTab(int idx)
{
    if (idx != -1)
    {
        assert(idx < m_dockWidgets.size());
        m_dockWidgets[idx]->setAnchored(false);
    }
}

void GLDTabDockContainerBar::addDockWidget(GLDTabDockContainerWidget *widget)
{

    if (m_dockWidgets.empty())
    {
        int ndx = m_tabBar->addTab(widget->title());
        m_dockWidgets.append(widget);
        m_tabIndexes.append(ndx);
        assert(ndx < m_dockWidgets.size());
        connect(m_dockWidgets[ndx], SIGNAL(setHideAndEntered(bool, bool)),
                m_tabBar, SLOT(setHideAndEntered(bool, bool)));
    }
    else
    {
        QList<GLDTabDockContainerWidget *>::const_iterator itr = qFind(m_dockWidgets, widget);

        if (itr == m_dockWidgets.end())
        {
            int ndx = m_tabBar->addTab(widget->title());
            m_dockWidgets.append(widget);
            m_tabIndexes.append(ndx);
            assert(ndx < m_dockWidgets.size());
            connect(m_dockWidgets[ndx], SIGNAL(setHideAndEntered(bool, bool)),
                    m_tabBar, SLOT(setHideAndEntered(bool, bool)));
        }
    }

    connect(m_tabBar, SIGNAL(tabLeave()), widget, SLOT(leaveHide()));
    connect(m_tabBar, SIGNAL(setLeaved(bool)), widget, SLOT(setLeaved(bool)));

    m_dockToolBar->show();
}

void GLDTabDockContainerBar::removeDockWidget(GLDTabDockContainerWidget *widget)
{
    int ndx = -1;

    for (int i = 0; i < m_tabIndexes.size(); ++i)
    {
        if (m_dockWidgets[i] == widget)
        {
            ndx = i;
            break;
        }
    }

    if (ndx != -1)
    {
        m_tabBar->removeTab(ndx);
        m_dockWidgets.removeAt(ndx);
        m_tabIndexes.removeAt(ndx);
    }

    if (m_tabIndexes.empty())
    {
        m_dockToolBar->hide();
    }
}

void GLDTabDockContainerBar::currentTabChanged(int cur, int pre)
{
    if (cur != pre)
    {
        if (cur == -1 && pre < m_dockWidgets.size())
        {
            m_dockWidgets[pre]->hide();
        }
        else
        {
            if (pre != -1 && pre < m_dockWidgets.size())
            {
                m_dockWidgets[pre]->hide();
            }

            if (cur < m_dockWidgets.size())
            {
                assert(cur >= 0);
                m_dockWidgets[cur]->setAnchored(false);
            }
        }
    }
    else
    {
        if (cur != -1 && cur < m_dockWidgets.size())
        {
            assert(cur >= 0);

            if (!m_dockWidgets[cur]->isVisible())
            {
                m_dockWidgets[cur]->setAnchored(false);
            }
        }
    }

    if (cur >= 0 && cur < m_dockWidgets.size())
    {
        int ntimervalue = m_dockWidgets[cur]->timerInterval();
        m_tabBar->setTimerInterval(ntimervalue);
    }
}

//////////////////////////////////////////////////////////////////////////
//GLDTabDockContainer
static Qt::ToolBarArea dockAreaToToolBarArea(Qt::DockWidgetArea area)
{
    if (area == Qt::LeftDockWidgetArea)
    {
        return Qt::LeftToolBarArea;
    }

    if (area == Qt::RightDockWidgetArea)
    {
        return Qt::RightToolBarArea;
    }

    if (area == Qt::TopDockWidgetArea)
    {
        return Qt::TopToolBarArea;
    }

    if (area == Qt::BottomDockWidgetArea)
    {
        return Qt::BottomToolBarArea;
    }

#ifdef _DEBUG
    else
    {
        assert(false);
    }

#endif
    return Qt::BottomToolBarArea;
}

static QTabBar::Shape dockAreaToTabBarShape(Qt::DockWidgetArea area)
{
    if (area == Qt::LeftDockWidgetArea)
    {
        return QTabBar::RoundedWest;
    }

    if (area == Qt::RightDockWidgetArea)
    {
        return QTabBar::RoundedEast;
    }

    if (area == Qt::TopDockWidgetArea)
    {
        return QTabBar::RoundedNorth;
    }

    if (area == Qt::BottomDockWidgetArea)
    {
        return QTabBar::RoundedSouth;
    }

#ifdef _DEBUG
    else
    {
        assert(false);
    }

#endif
    return QTabBar::RoundedWest;
}

GLDTabDockContainer::GLDTabDockContainer(Qt::DockWidgetArea dockArea, QMainWindow *mainWindow, QWidget *parent)
    : QWidget(parent)
    , m_dockArea(dockArea)
    , m_mainWindow(mainWindow)
    , m_textForceVertical(false)
    , m_interval(500)
{
    parent->setContextMenuPolicy(Qt::NoContextMenu);
#ifdef _DEBUG

    if (dockArea != Qt::LeftDockWidgetArea && dockArea != Qt::RightDockWidgetArea
            && dockArea != Qt::TopDockWidgetArea && dockArea != Qt::BottomDockWidgetArea)
    {
        assert(false);
    }

#endif
//  QMainWindow *mw = qobject_cast<QMainWindow*>(parent);
//  if (mw != NULL)
//  {
//      m_mainWindow = mw;
//  }
    m_bar = new GLDTabDockContainerBar(this);
    m_bar->m_tabBar->setShape(dockAreaToTabBarShape(m_dockArea));

    if (m_mainWindow != NULL)
    {
        Qt::ToolBarArea area = dockAreaToToolBarArea(m_dockArea);
        m_mainWindow->addToolBarBreak(area);
        m_mainWindow->addToolBar(area, m_bar->m_dockToolBar);
    }
}

void GLDTabDockContainer::addTabDockContainerWidget(GLDTabDockContainerWidget *widget)
{
    //if main window is null,try to examine the parent widget.
    if (m_mainWindow == NULL)
    {
        QMainWindow *mw = qobject_cast<QMainWindow *>(parent());

        if (NULL != mw)
        {
            m_mainWindow = mw;
        }
    }

    assert(NULL != m_mainWindow);

    widget->setParent(this);
    widget->setAllowedAreas(m_dockArea);
    connect(widget, SIGNAL(anchorChanged(bool)), this, SLOT(anchorChanged(bool)));
    bool bisAnchored = widget->isAnchored();

    if (bisAnchored == false)
    {
        widget->setAnchored(true);
        QApplication::processEvents();
    }

    widget->setAnchored(bisAnchored);

    if (!widget->isAnchored())
    {
        widget->hide();
    }

    m_dockWidgets.append(widget);
    updateAllWidget();
}

void GLDTabDockContainer::hideWidget()
{
    for (int i = 0; i < m_dockWidgets.size(); ++i)
    {
        m_dockWidgets[i]->hideWidget();
    }

    m_bar->m_dockToolBar->hide();
    m_bar->hide();
}

void GLDTabDockContainer::showWidget()
{
    for (int i = 0; i < m_dockWidgets.size(); ++i)
    {
        m_dockWidgets[i]->showWidget();
    }

    if (!m_bar->m_tabIndexes.empty())
    {
        m_bar->m_dockToolBar->show();
    }

    m_bar->show();
}

void GLDTabDockContainer::setTimerInterval(int interval)
{
    m_interval = interval;
    updateAllWidget();
}

void GLDTabDockContainer::anchorChanged(bool anchored)
{
    GLDTabDockContainerWidget *widget = qobject_cast<GLDTabDockContainerWidget *>(sender());

    if (NULL != widget)
    {
        if (m_bar->m_tabBar->textOrientation() == Qt::Horizontal && m_textForceVertical)
        {
            if (m_dockArea == Qt::LeftDockWidgetArea || m_dockArea == Qt::RightDockWidgetArea)
            {
                m_bar->m_tabBar->setTextOrientation(Qt::Vertical);
            }
        }

        if (anchored)
        {
            widget->setFeatures(widget->features() | QDockWidget::DockWidgetMovable);
            m_mainWindow->addDockWidget(m_dockArea, widget);
            m_bar->removeDockWidget(widget);
            widget->setFloating(false);
            widget->show();
        }
        else
        {
            m_mainWindow->removeDockWidget(widget);
            widget->setFloating(true);
            widget->setFeatures(widget->features() & ~QDockWidget::DockWidgetMovable);
            m_bar->addDockWidget(widget);
            widget->resize(widget->sizeHint());
            QApplication::processEvents();

            QPoint pt;
            QWidget *centerWidget = m_mainWindow->centralWidget();

            if (centerWidget == NULL)
            {
                //TODO need some functions to deal with the situation!....
                return ;
            }

            if (m_dockArea == Qt::LeftDockWidgetArea)
            {
                //pt = centerWidget->rect().topLeft();
                if (m_bar->isVisible())
                {
                    pt = m_bar->m_dockToolBar->rect().topRight();
                }
                else
                {
                    pt = m_bar->m_dockToolBar->rect().topLeft();
                }

                pt = m_bar->m_dockToolBar->mapToGlobal(pt);

                if (widget->isAutoFillHeight())
                {
                    widget->resize(widget->width(), centerWidget->height());
                }
            }
            else if (m_dockArea == Qt::RightDockWidgetArea)
            {
                //pt = centerWidget->rect().topRight() ;
                if (m_bar->isVisible())
                {
                    pt = m_bar->m_dockToolBar->rect().topLeft();
                }
                else
                {
                    pt = m_bar->m_dockToolBar->rect().topRight();
                }

                pt = m_bar->m_dockToolBar->mapToGlobal(pt);
                pt.setX(pt.x() - widget->rect().width());

                if (widget->isAutoFillHeight())
                {
                    widget->resize(widget->width(), centerWidget->height());
                }

            }
            else if (m_dockArea == Qt::TopDockWidgetArea)
            {
                //pt = centerWidget->rect().topRight();
                if (m_bar->isVisible())
                {
                    pt = m_bar->m_dockToolBar->rect().bottomLeft();
                }
                else
                {
                    pt = m_bar->m_dockToolBar->rect().topLeft();
                }

                pt = m_bar->m_dockToolBar->mapToGlobal(pt);

                if (widget->isAutoFillHeight())
                {
                    widget->resize(centerWidget->width(), widget->height());
                }
            }
            else if (m_dockArea == Qt::BottomDockWidgetArea)
            {
                if (!m_bar->isVisible())
                {
                    pt = m_bar->m_dockToolBar->rect().bottomLeft();
                }
                else
                {
                    pt = m_bar->m_dockToolBar->rect().topLeft();
                }

                pt = m_bar->m_dockToolBar->mapToGlobal(pt);
                //pt = centerWidget->rect().bottomLeft();
                pt.setY(pt.y() - widget->rect().height());

                if (widget->isAutoFillHeight())
                {
                    widget->resize(centerWidget->width(), widget->height());
                }
            }

#ifdef _DEBUG
            else
            {
                //see the constructor description.
                assert(false);
            }

#endif

            pt = m_mainWindow->mapFromGlobal(pt);
            pt = m_mainWindow->mapToGlobal(pt);
            widget->move(pt);
            widget->show();
            widget->activateWindow();
        }
    }
}

void GLDTabDockContainer::setMainWindow(QMainWindow *mainWindow)
{
    m_mainWindow = mainWindow;

    if (m_mainWindow != NULL)
    {
        m_mainWindow->addToolBar(dockAreaToToolBarArea(m_dockArea), m_bar->m_dockToolBar);
    }
}

void GLDTabDockContainer::updateAllWidget()
{
    for (int i = 0; i < m_dockWidgets.size(); ++i)
    {
        m_dockWidgets[i]->setTimerInterval(m_interval);
    }
}

void GLDTabDockContainer::setTabTextForceVertical(bool isTextVertical)
{
    m_textForceVertical = isTextVertical;
}

bool GLDTabDockContainer::isTabTextForceVertical() const
{
    return m_textForceVertical;
}
