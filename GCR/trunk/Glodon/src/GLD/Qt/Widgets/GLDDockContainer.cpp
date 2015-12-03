#include <QPaintEvent>
#include <QStylePainter>
#include <QSplitter>

#include "GLDGlobal.h"
#include "GLDDockContainer.h"

/*CDockWidget*/

GLDDockContainer::GLDDockContainer(const GString &title, QWidget *parent, bool bWest) : QWidget(parent),
    m_fixedFrame(true), m_barAtLeft(bWest), m_oldMargin(NULL), m_indexAtSplitter(-1), m_preParent(NULL),
    m_title(title), m_minWidth(-1)
{
    //@attention dockContainer 本身是一个Widget，但其实是里面的GLDDockContainerTabBar和加入的DockContainerWidget
    //作为显示的模块，dockContainer本身不要显示出来。这里需要visible设置为false，否则dockWidget会透明占用父窗口的一定空间
    setVisible(false);
    m_tabBar = new GLDDockContainerTabBar(this);
    m_tabBar->installEventFilter(this);
    m_tabBar->setShape(GLDDockContainerTabBar::RoundedWest);
    m_tabBar->addTab(title);
    m_tabBar->resize(m_tabBar->sizeHint());
    m_tabBar->hide();
    m_tabBar->setObjectName("m_tabBar");

    connect(m_tabBar, SIGNAL(clicked()), this, SLOT(doClickBar()));

    m_titleBar = new GLDDockContainerTitleBar(title, this);
    connect(m_titleBar->m_closeBtn, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_titleBar->m_fixedBtn, SIGNAL(clicked()), this, SLOT(doFixedClick()));

    m_gridLayout = new QGridLayout(this);
    if (m_barAtLeft)
    {
        m_gridLayout->addWidget(m_tabBar, 0, 0, 2, 1, Qt::AlignTop);
        m_gridLayout->addWidget(m_titleBar, 0, 1, 1, 1);
    }
    else
    {
        m_gridLayout->addWidget(m_tabBar, 0, 1, 2, 1, Qt::AlignTop);
        m_gridLayout->addWidget(m_titleBar, 0, 0, 1, 1);
    }
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    m_gridLayout->setSpacing(0);

    m_dockWidget = NULL;
    setWindowFlags(Qt::WindowStaysOnTopHint);
}

GLDDockContainer::~GLDDockContainer()
{
    freeAndNil(m_oldMargin)
}

void GLDDockContainer::addDockWidget(QWidget *w)
{
    if (m_dockWidget)
    {
        m_gridLayout->removeWidget(m_dockWidget);
    }

    m_dockWidget = w;

    if (m_barAtLeft)
    {
        m_gridLayout->addWidget(m_dockWidget, 1, 1, 1, 1);
    }
    else
    {
        m_gridLayout->addWidget(m_dockWidget, 1, 0, 1, 1);
    }
}

void GLDDockContainer::setCloseBtnVisible(bool value)
{
    m_titleBar->setCloseBtnVisible(value);
}

void GLDDockContainer::setTabsCloseAble(bool value)
{
    m_tabBar->setTabsClosable(value);
}

void GLDDockContainer::setTitleBar(GLDDockContainerTitleBar *titleBar)
{
    if (m_titleBar != titleBar)
    {
        m_gridLayout->removeWidget(m_titleBar);

        if (m_titleBar != NULL)
        {
            delete m_titleBar;
        }

        m_titleBar = titleBar;
        reInitTitleBar();
    }
}

void GLDDockContainer::setTabBar(GLDDockContainerTabBar *tabBar)
{
    if (m_tabBar != tabBar)
    {
        if (m_barAtLeft)
        {
            m_gridLayout->removeWidget(m_tabBar);
        }
        else
        {
            m_gridLayout->removeWidget(m_tabBar);
        }

        if (m_tabBar != NULL)
        {
            delete m_tabBar;
        }

        m_tabBar = tabBar;
        reInitTabBar();
    }
}

QIcon GLDDockContainer::fixedIcon()
{
    return m_titleBar->m_fixedBtn->icon();
}

void GLDDockContainer::setFixedIcon(QIcon &icon)
{
    m_titleBar->m_fixedBtn->setNormIcon(icon);
    m_titleBar->m_fixedBtn->setIcon(icon);
}

void GLDDockContainer::setFixedDockIcon(QIcon &icon)
{
    m_titleBar->m_fixedBtn->setDockIcon(icon);
}

QIcon GLDDockContainer::closeIcon()
{
    return m_titleBar->m_closeBtn->icon();
}

void GLDDockContainer::setCloseIcon(QIcon &icon)
{
    m_titleBar->m_closeBtn->setNormIcon(icon);
    m_titleBar->m_closeBtn->setIcon(icon);
}

void GLDDockContainer::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

void GLDDockContainer::initStyleOption(QStyleOptionDockWidget *option) const
{
    if (!option)
    {
        return;
    }

    option->initFrom(this);
    option->closable = true;
    option->movable = true;
    option->floatable = true;

    QStyleOptionDockWidgetV2 *v2 = qstyleoption_cast<QStyleOptionDockWidgetV2 *>(option);

    if (v2 != 0)
    {
        v2->verticalTitleBar = false;
    }
}

void GLDDockContainer::doShow()
{
    doShowFrame(false);
    m_fixedFrame = false;
    if (dynamic_cast<QSplitter *>(parentWidget()))
    {
        alignSplitter();
    }
    else
    {
        alignWidgets();
    }
}

void GLDDockContainer::alignWidgets(bool bNeedRemove)
{
    QLayout *layout = parentWidget()->layout();
    if (!layout)
    {
        return;
    }

    QRect barRet = m_tabBar->rect();
    if (bNeedRemove)
    {
        layout->removeWidget(this);

        if (!m_barAtLeft)
        {
            parentWidget()->installEventFilter(this);
        }
    }
    if (m_oldMargin == NULL)
    {
        QMargins margin = layout->contentsMargins();
        m_oldMargin = new QMargins(margin.left(), margin.top(), margin.right(), margin.bottom());
    }
    if (m_barAtLeft)
    {
        layout->setContentsMargins(barRet.width(), m_oldMargin->top(), m_oldMargin->right(), m_oldMargin->bottom());
        setGeometry(0, m_oldMargin->top(), width(),
                    parentWidget()->height() - m_oldMargin->top() - m_oldMargin->bottom());
    }
    else
    {
        layout->setContentsMargins(m_oldMargin->left(), m_oldMargin->top(), barRet.width(), m_oldMargin->bottom());
        setGeometry(parentWidget()->width() - width(), m_oldMargin->top(),
                    width(), parentWidget()->height() - m_oldMargin->top() - m_oldMargin->bottom());
    }
    raise();
}

void GLDDockContainer::alignSplitter()
{
    QWidget *pSplitter = parentWidget();
    if (m_preParent == NULL)
    {
        m_preParent = pSplitter;
    }

    QRect rParentRect = pSplitter->rect();
    QRect barRet = m_tabBar->rect();
    m_indexAtSplitter = dynamic_cast<QSplitter *>(pSplitter)->indexOf(this);
    setParent(pSplitter->parentWidget());

    if (!m_barAtLeft)
    {
        parentWidget()->installEventFilter(this);
    }

    if (parentWidget()->layout() == NULL)
    {
        if (m_barAtLeft)
        {
            int nLeft = rParentRect.left();
            setGeometry(nLeft, rParentRect.top(), width(), pSplitter->height());
            const_cast<QRect &>(pSplitter->geometry()).setLeft(nLeft + barRet.width());
        }
        else
        {
            int nRight = rParentRect.right();
            setGeometry(nRight - width(), rParentRect.top(), width(), pSplitter->height());
            const_cast<QRect &>(pSplitter->geometry()).setRight(nRight - barRet.width());
        }
        raise();
    }
    else
    {
        alignWidgets(false);
        show();
    }
}

void GLDDockContainer::reInitTitleBar()
{
    connect(m_titleBar->m_closeBtn, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_titleBar->m_fixedBtn, SIGNAL(clicked()), this, SLOT(doFixedClick()));

    if (m_barAtLeft)
    {
        m_gridLayout->addWidget(m_titleBar, 0, 1, 1, 1);
    }
    else
    {
        m_gridLayout->addWidget(m_titleBar, 0, 0, 1, 1);
    }
}

void GLDDockContainer::reInitTabBar()
{
    m_tabBar->installEventFilter(this);
    m_tabBar->setShape(GLDDockContainerTabBar::RoundedWest);
    m_tabBar->addTab(m_tabBar->title());
    m_tabBar->resize(m_tabBar->sizeHint());
    m_tabBar->hide();

    connect(m_tabBar, SIGNAL(clicked()), this, SLOT(doClickBar()));

    if (m_barAtLeft)
    {
        m_gridLayout->addWidget(m_titleBar, 0, 1, 1, 1);
    }
    else
    {
        m_gridLayout->addWidget(m_titleBar, 0, 0, 1, 1);
    }

}

bool GLDDockContainer::eventFilter(QObject *obj, QEvent *event)
{
    if ((m_tabBar == obj) && (m_tabBar->isVisible()))
    {
        switch (event->type())
        {
            case QEvent::Enter:
                doShow();
                event->accept();
                return true;

            case QEvent::Leave:
                if (!underMouse())
                {
                    doHideContainer();
                    event->accept();
                    return true;
                }
                else
                {
                    break;
                }

            default:
                break;
        }
    }
    else if (obj == parentWidget())
    {
        if ((event->type() == QEvent::Resize) && (!m_fixedFrame))
        {
            setGeometry(parentWidget()->width() - m_tabBar->width(), 0,
                        m_tabBar->width(), m_tabBar->height());
        }
    }
    return QWidget::eventFilter(obj, event);
}

bool GLDDockContainer::event(QEvent *event)
{
    if (event->type() == QEvent::Leave && !m_fixedFrame)
    {
        doHideContainer();
        event->accept();
        return true;
    }
    return QWidget::event(event);
}

QSize GLDDockContainer::sizeHint() const
{
    if (m_titleBar->isVisible() && m_tabBar->isHidden())
    {
        if (m_dockWidget)
        {
            return m_dockWidget->sizeHint();
        }
        else
        {
            return m_titleBar->sizeHint();
        }
    }
    else
    {
        return m_tabBar->sizeHint();
    }
}

QSize GLDDockContainer::minimumSizeHint() const
{
    if (m_titleBar->isVisible() && m_tabBar->isHidden())
    {
        if (-1 == m_minWidth)
        {
            return m_titleBar->sizeHint() * 2;
        }
        else
        {
            return QSize(m_minWidth, m_titleBar->height() * 2);
        }
    }
    else
    {
        return m_tabBar->sizeHint();
    }
}

void GLDDockContainer::doFixedClick()
{
    QSize tabSize = m_tabBar->size();
    if (m_tabBar->isVisible())
    {
        doClickBar();
        resize(size().width() - tabSize.width(), size().height());
    }
    else
    {
        m_preSize = size() + QSize(tabSize.width(), 0);
        doHideContainer();
        m_titleBar->m_fixedBtn->setInDock(true);
    }
}

void GLDDockContainer::doHideContainer()
{
    QRect rPreGeom = geometry();
    m_fixedFrame = false;

    if (!m_titleBar->isVisible())
    {
        return;
    }

//    m_preSize = size();
    m_titleBar->hide();

    if (m_dockWidget)
    {
        m_dockWidget->hide();
    }

    if (m_barAtLeft)
    {
        resize(m_tabBar->size());
    }
    else
    {
        setGeometry(parentWidget()->width() - m_tabBar->width(), rPreGeom.top(),
                    m_tabBar->width(), m_tabBar->height());
    }

    m_tabBar->show();

    if (dynamic_cast<QSplitter *>(parentWidget()))
    {
        alignSplitter();
    }
}

void GLDDockContainer::doShowContainer()
{
    doShowFrame(true);
    QLayout *layout = parentWidget()->layout();

    if (!layout)
    {
        return;
    }

    layout->setContentsMargins(*m_oldMargin);
    delete m_oldMargin;
    m_oldMargin = NULL;
    QList<QWidget *> list;
    int nWidgets = layout->count();
    for (int i = 0; i < nWidgets; i++)
    {
        list.append(layout->itemAt(i)->widget());
        layout->removeWidget(list.at(i));
    }
    if (m_barAtLeft)
    {
        layout->addWidget(this);
    }

    for (int i = 0; i < nWidgets; i++)
    {
        layout->addWidget(list.at(i));
    }

    if (!m_barAtLeft)
    {
        layout->addWidget(this);
        parentWidget()->removeEventFilter(this);
    }
}

void GLDDockContainer::doAddToSplitter()
{
    QSplitter *pSlitter = dynamic_cast<QSplitter *>(m_preParent);
    if (parentWidget()->layout())
    {
        parentWidget()->layout()->setContentsMargins(*m_oldMargin);

        if (!m_barAtLeft)
        {
            parentWidget()->removeEventFilter(this);
        }

        delete m_oldMargin;
        m_oldMargin = NULL;
    }
    pSlitter->insertWidget(m_indexAtSplitter, this);
    QList<int> sizes = pSlitter->sizes();
    if (m_indexAtSplitter + 1 < sizes.count())
    {
        sizes[m_indexAtSplitter + 1] += sizes[m_indexAtSplitter] - m_preSize.width() + 1;
    }    
    sizes[m_indexAtSplitter] = m_preSize.width() - 1;
    pSlitter->setSizes(sizes);
    doShowFrame(true);
    m_preParent = NULL;
}

void GLDDockContainer::doClickBar()
{
    m_titleBar->m_fixedBtn->setInDock(false);
    if (dynamic_cast<QSplitter *>(m_preParent))
    {
        doAddToSplitter();
    }
    else
    {
        doShowContainer();
    }
}

void GLDDockContainer::doShowFrame(bool bHideBar)
{
    m_fixedFrame = true;
    if (bHideBar)
    {
        m_tabBar->hide();
    }

    if (m_titleBar->isVisible())
    {
        return;
    }

    m_titleBar->show();
    if (m_dockWidget)
    {
        m_dockWidget->show();
    }
        resize(m_preSize);
}

Qt::Orientation GLDDockContainer::tabTextOrientation()
{
	return m_tabBar->textOrientation();
}

void GLDDockContainer::setTabTextOrientation( Qt::Orientation orient )
{
	m_tabBar->setTextOrientation(orient);
}

GLDDockContainerTitleButton::GLDDockContainerTitleButton(QWidget *dockWidget)
    : QAbstractButton(dockWidget)
{
    setFocusPolicy(Qt::NoFocus);
}

void GLDDockContainerTitleButton::setInDock(bool value)
{
    if (value && !m_dockIcon.isNull())
    {
        setIcon(m_dockIcon);
    }
    else
    {
        setIcon(m_icon);
    }
}

QSize GLDDockContainerTitleButton::sizeHint() const
{
    ensurePolished();

    int nsize = 2 * style()->pixelMetric(QStyle::PM_DockWidgetTitleBarButtonMargin, 0, this);
    if (!icon().isNull())
    {
        int niconSize = style()->pixelMetric(QStyle::PM_SmallIconSize, 0, this);
        QSize sz = icon().actualSize(QSize(niconSize, niconSize));
        nsize += qMax(sz.width(), sz.height());
    }

    return QSize(nsize * 1, nsize);
}

void GLDDockContainerTitleButton::enterEvent(QEvent *event)
{
    if (isEnabled())
    {
        update();
    }

    QAbstractButton::enterEvent(event);
}

void GLDDockContainerTitleButton::leaveEvent(QEvent *event)
{
    if (isEnabled())
    {
        update();
    }

    QAbstractButton::leaveEvent(event);
}

void GLDDockContainerTitleButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QStyleOptionToolButton opt;
    opt.init(this);
    opt.state |= QStyle::State_AutoRaise;

    if (0 != style()->styleHint(QStyle::SH_DockWidget_ButtonsHaveFrame, 0, this))
    {
        if (isEnabled() && underMouse() && !isChecked() && !isDown())
        {
            opt.state |= QStyle::State_Raised;
        }

        if (isChecked())
        {
            opt.state |= QStyle::State_On;
        }

        if (isDown())
        {
            opt.state |= QStyle::State_Sunken;
        }

        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &painter, this);
    }

    opt.icon = icon();
    opt.subControls = 0;
    opt.activeSubControls = 0;
    opt.features = QStyleOptionToolButton::None;
    opt.arrowType = Qt::NoArrow;
    int nSize = style()->pixelMetric(QStyle::PM_SmallIconSize, 0, this);
    opt.iconSize = QSize(nSize, nSize);
    style()->drawComplexControl(QStyle::CC_ToolButton, &opt, &painter, this);
    G_UNUSED(event)
}

GLDDockContainerTabBar::GLDDockContainerTabBar(QWidget *parent) : QTabBar(parent)
	,m_textOrientation(Qt::Horizontal)
{
    m_inResize = false;
}

GLDDockContainerTabBar::GLDDockContainerTabBar(QString title, QWidget *parent)
    : QTabBar(parent)
    , m_title(title)
    , m_textOrientation(Qt::Horizontal)
{
    m_inResize = false;
}

void GLDDockContainerTabBar::setTabsClosable(bool closable)
{
    QTabBar::setTabsClosable(closable);
}

GString GLDDockContainerTabBar::title() const
{
    return m_title;
}

void GLDDockContainerTabBar::paintEvent(QPaintEvent *event)
{
//    QTabBar::paintEvent(event);
    for (int i = 0; i < count(); i++)
    {
        QStylePainter painter(this);
        QStyleOptionTabV3 tab;
        initStyleOption(&tab, i);
        if (m_textOrientation == Qt::Vertical)
        {
            QString oldText = tab.text;
            tab.text = "";
            painter.drawControl(QStyle::CE_TabBarTab, tab);

            tab.text= verticalString(oldText);
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
    G_UNUSED(event)
}

void GLDDockContainerTabBar::mousePressEvent(QMouseEvent *event)
{
    emit clicked();
    QTabBar::mousePressEvent(event);
}

void GLDDockContainerTabBar::initStyleOption(QStyleOptionTab *option, int tabIndex) const
{
    QTabBar::initStyleOption(option, tabIndex);
    option->state = 0;
    option->state |= QStyle::State_Active;
    option->state |= QStyle::State_Enabled;
    if (underMouse())
    {
        option->state |= QStyle::State_MouseOver;
    }
}

GString GLDDockContainerTabBar::verticalString( const GString &str )
{
	QChar buffer[255];
	int nChar = 0;

	for (int i = 0; i < str.count(); ++i)
	{
		buffer[nChar] = str[i];
		buffer[++nChar] = '\n';
		++nChar;
	}

	buffer[nChar - 1] = '\0';
	return GString(buffer);
}

Qt::Orientation GLDDockContainerTabBar::textOrientation()
{
	return m_textOrientation;
}

void GLDDockContainerTabBar::setTextOrientation( Qt::Orientation orient )
{
	m_textOrientation = orient;
}

GLDDockContainerTitleBar::GLDDockContainerTitleBar(const GString &title, QWidget *parent) : QWidget(parent)
{
    m_title = title;
    m_showCloseBtn = true;
    this->setObjectName("titleBar");
    init();
}

GLDDockContainerTitleBar::~GLDDockContainerTitleBar()
{

}

void GLDDockContainerTitleBar::init()
{
    QStyleOptionDockWidgetV2 opt;
    m_fixedBtn = new GLDDockContainerTitleButton(this);
    m_fixedBtn->setIcon(style()->standardIcon(QStyle::SP_TitleBarNormalButton, &opt, this));
    m_fixedBtn->setNormIcon(m_fixedBtn->icon());
    m_fixedBtn->resize(m_fixedBtn->sizeHint());

    m_closeBtn = new GLDDockContainerTitleButton(this);
    m_closeBtn->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton, &opt, this));
    m_closeBtn->setNormIcon(m_closeBtn->icon());
    m_closeBtn->resize(m_closeBtn->sizeHint());

    alignBtns(m_showCloseBtn);
}

void GLDDockContainerTitleBar::setCloseBtnVisible(bool bShow)
{
    if (m_showCloseBtn == bShow)
    {
        return;
    }

    if (bShow)
    {
        m_closeBtn->show();
    }
    else
    {
        m_closeBtn->hide();
    }

    m_showCloseBtn = bShow;
    alignBtns(m_showCloseBtn);
}


void GLDDockContainerTitleBar::paintEvent(QPaintEvent *evente)
{
    QPainter painter(this);
    QStyleOptionDockWidget opt;
    opt.rect = this->rect();
    opt.rect.adjust(0, -2, 0, 3);
    opt.title = m_title;
    opt.closable = true;
    opt.floatable = true;
    opt.verticalTitleBar = false;

    painter.setBrush(opt.palette.background().color().darker(110));
    style()->drawControl(QStyle::CE_DockWidgetTitle, &opt, &painter, this);
    G_UNUSED(evente)
}

void GLDDockContainerTitleBar::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    alignBtns(m_showCloseBtn);
}

QSize GLDDockContainerTitleBar::sizeHint() const
{
    return minimumSizeHint();
}

QSize GLDDockContainerTitleBar::minimumSizeHint() const
{
    QSize size = QWidget::sizeHint();
    return QSize(qMax(m_closeBtn->width() * 2, size.width()), m_closeBtn->height() + 4);
}

GLDDockContainerTitleButton *GLDDockContainerTitleBar::fixButton()
{
    return m_fixedBtn;
}

GLDDockContainerTitleButton *GLDDockContainerTitleBar::closeButton()
{
    return m_closeBtn;
}

GString GLDDockContainerTitleBar::title()
{
    return m_title;
}

void GLDDockContainerTitleBar::alignBtns(bool isCloseBtnShow)
{
    if (isCloseBtnShow)
    {
        m_closeBtn->setGeometry(width() - m_closeBtn->width() - 2, 2, m_closeBtn->width(), m_closeBtn->height());
        m_fixedBtn->setGeometry(m_closeBtn->geometry().left() - m_fixedBtn->width() - 2,
                                2, m_fixedBtn->width(), m_fixedBtn->height());
    }
    else
    {
        m_fixedBtn->setGeometry(width() - m_closeBtn->width() - 2, 2, m_closeBtn->width(), m_closeBtn->height());
    }
}

//标题栏由图片构成的实现
GLDDockContainerImageTitleBar::GLDDockContainerImageTitleBar(const GString &title, QWidget *parent)
    : GLDDockContainerTitleBar(title, parent)
    , m_textColor(0,0,0)
    , m_useCustomPixmap(false)
    , m_textAlignment(Qt::AlignLeft | Qt::AlignVCenter)
    , m_textMargins(30,1,30,1)
{

}

GLDDockContainerImageTitleBar::~GLDDockContainerImageTitleBar()
{
}

void GLDDockContainerImageTitleBar::setCustomTitlePixmap(const QPixmap &leftPm, const QPixmap &centerPm, const QPixmap &rightPm)
{
    m_leftPm = leftPm;
    m_rightPm = rightPm;
    m_centerPm = centerPm;
}

void GLDDockContainerImageTitleBar::setUseCustomPixmap(bool used)
{
    m_useCustomPixmap = used;

    if (used)
    {
        fixButton()->setVisible(false);
        closeButton()->setVisible(false);
    }
}

void GLDDockContainerImageTitleBar::setTextColor(const QColor &textColor)
{
    m_textColor = textColor;
}

void GLDDockContainerImageTitleBar::setTextAlignment(Qt::Alignment alignment)
{
    m_textAlignment = alignment;
}

void GLDDockContainerImageTitleBar::setTextMargins(const QMargins &margins)
{
    m_textMargins = margins;
}

QColor GLDDockContainerImageTitleBar::textColor() const
{
    return m_textColor;
}

Qt::Alignment GLDDockContainerImageTitleBar::textAlignment() const
{
    return m_textAlignment;
}

QMargins GLDDockContainerImageTitleBar::textMargins() const
{
    return m_textMargins;
}

void GLDDockContainerImageTitleBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect rect(this->rect());

    if (m_useCustomPixmap)
    {
        painter.drawPixmap(rect.topLeft(), m_leftPm);
        painter.drawPixmap(rect.topRight() - QPoint(m_rightPm.width() - 1, 0), m_rightPm);
        QBrush brush(m_centerPm);
        QRect centerRect(rect.left() + m_leftPm.width(), rect.top(),
                         rect.width() - m_leftPm.width() - m_rightPm.width(),
                         rect.height());
        painter.fillRect(centerRect, brush);
        QTextOption textOpt;
        textOpt.setAlignment(m_textAlignment);

        //int nFontHeight = painter.fontMetrics().height();
        //int nHalfRemove = (centerRect.height() - nFontHeight) / 2;

        QRect textRect(m_textMargins.left(),m_textMargins.top(),rect.width(),rect.height() - m_textMargins.top()
                       - m_textMargins.bottom());

        if (m_textColor.isValid())
        {
            painter.setPen(m_textColor);
        }

        painter.setFont(font());
        painter.drawText(textRect, title(), textOpt);
    }
    else
    {
        GLDDockContainerTitleBar::paintEvent(event);
    }
}

void GLDDockContainerImageTitleBar::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    QRect rect = this->rect();

    if (m_useCustomPixmap)
    {
        const int buttonWidth = m_rightPm.width();
        QRect rightRect(rect.right() - buttonWidth, 0, buttonWidth, rect.height());

        if (rightRect.contains(pos))
        {
            fixButton()->click();
        }
    }
    else
    {
        GLDDockContainerTitleBar::mousePressEvent(event);
    }
}

GLDDockContainerImageTabBar::GLDDockContainerImageTabBar(QString title, QWidget *parent)
    : GLDDockContainerTabBar(parent)
    , m_usePixmap(false)
    , m_title(title)
    , m_textColor(0, 0, 0)
    , m_textAlignment(Qt::AlignHCenter | Qt::AlignTop)
    , m_textMargins(1, 10, 1, 10)
{

}

void GLDDockContainerImageTabBar::setCustomTitlePixmap(const QPixmap &leftPm, const QPixmap &centerPm, const QPixmap &rightPm)
{
    m_leftPm = leftPm;
    m_rightPm = rightPm;
    m_centerPm = centerPm;
}

void GLDDockContainerImageTabBar::setUseCustomPixmap(bool used)
{
    m_usePixmap = used;
}

void GLDDockContainerImageTabBar::paintEvent(QPaintEvent *event)
{
    if (m_usePixmap)
    {
        for (int i = 0; i < count(); i++)
        {
            QStyleOptionTabV3 tab;
            initStyleOption(&tab, i);
            QBrush centerBrush(m_centerPm);

            QRect vRect(tab.rect);//竖
            QSize vSize = this->sizeHint(); //竖
            QSize hSize(vSize);//横
            hSize.transpose();

            vRect.setSize(vSize);

            QRect hRect(vRect);
            hRect.setSize(hSize);

            QRect centerRect(hRect);
            QSize centerSize(hRect.width() - m_leftPm.width() - m_rightPm.width(), hRect.height());
            centerRect.setSize(centerSize);
            centerRect.translate(m_leftPm.width(), 0);

            // 转换坐标系
            QStylePainter vPainter(this);
            vPainter.translate(vRect.width(), 0);
            vPainter.rotate(90);

            QRect textRect(vRect.left() - m_textMargins.left(), vRect.top() + m_textMargins.top(),
                           vRect.width() - m_textMargins.left() - m_textMargins.right(), vRect.height() - m_textMargins.top() - m_textMargins.bottom());


            vPainter.drawPixmap(hRect.topLeft(), m_leftPm);

            vPainter.fillRect(centerRect, centerBrush);
            vPainter.drawPixmap(centerRect.topRight(), m_rightPm);
            vPainter.rotate(-90);
            vPainter.translate(-vRect.width(), 0);

            if (m_textColor.isValid())
            {
                vPainter.setPen(m_textColor);
            }

            QTextOption textOpt;
            textOpt.setAlignment(m_textAlignment);
            vPainter.drawText(textRect, verticalTitle(), textOpt);
        }
    }
    else
    {
        GLDDockContainerTabBar::paintEvent(event);
    }
}

QString GLDDockContainerImageTabBar::verticalTitle()
{
    QChar buffer[255];
    int nChar = 0;

    for (int i = 0; i < m_title.count(); ++i)
    {
        buffer[nChar] = m_title[i];
        buffer[++nChar] = '\n';
        ++nChar;
    }

    buffer[nChar - 1] = '\0';
    return QString(buffer);
}

QSize GLDDockContainerImageTabBar::sizeHint() const
{
    if (m_size.isValid())
    {
        return m_size;
    }

    return GLDDockContainerTabBar::sizeHint();
}

void GLDDockContainerImageTabBar::setSize(const QSize &size)
{
    m_size = size;
}

void GLDDockContainerImageTabBar::setTextColor(const QColor &textColor)
{
    m_textColor = textColor;
}

void GLDDockContainerImageTabBar::setTextMargins(const QMargins &margins)
{
    m_textMargins = margins;
}

void GLDDockContainerImageTabBar::setTextAlignment(Qt::Alignment alignment)
{
    m_textAlignment = alignment;
}

QSize GLDDockContainerImageTabBar::size() const
{
    return m_size;
}

QColor GLDDockContainerImageTabBar::textColor() const
{
    return m_textColor;
}

QMargins GLDDockContainerImageTabBar::textMargins() const
{
    return m_textMargins;
}

Qt::Alignment GLDDockContainerImageTabBar::textAlignment() const
{
    return m_textAlignment;
}
