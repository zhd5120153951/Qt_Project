#include "GLDDockWidget.h"
#include <QTabBar>
#include <QEvent>
#include <QMouseEvent>
#include <QRect>
#include <QObject>
#include <QApplication>

/**
  ��������������tabBars�¼�
*/
class GLDTabBarsControl: public QObject
{
public:
    explicit GLDTabBarsControl(QObject *parent = 0);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
private:
    void pressEventOnTabBar(QTabBar *bar, QMouseEvent *event);
    void moveEventOnTabBar(QTabBar *bar, QMouseEvent *event);
    void releaseEventOnDock(GLDDockWidget *dockWidget, QMouseEvent *event);
    void leaveDockWidget(GLDDockWidget *dockWidget);
private:
    int m_xOffset;
    int m_yOffset;

    QPoint m_pressPosOnTab;
    QPoint m_releasePosOnTab;

    GLDDockWidget *m_nowDraggingDock;
    QTabBar *m_nowTabBar;
    friend class GLDDockWidget;
};
/**
  GLDTabBarsControl��ȫ�ֱ��?
*/
GLDTabBarsControl gTabBarsControl;

GLDTabBarsControl::GLDTabBarsControl(QObject *parent):
    QObject(parent), m_pressPosOnTab(-1, -1), m_releasePosOnTab(-1, -1),
    m_nowDraggingDock(NULL), m_nowTabBar(NULL), m_xOffset(0), m_yOffset(0)
{}

/**
  �¼����������¼���������
*/
bool GLDTabBarsControl::eventFilter(QObject *obj, QEvent *event)
{
    if (QTabBar *bar = dynamic_cast<QTabBar *>(obj))
    {
        m_nowTabBar = bar;
        switch (event->type())
        {
        case QEvent::MouseButtonPress:
        {
            QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent *>(event);
            pressEventOnTabBar(bar, mouseEvent);
            break;
        }
        case QEvent::MouseMove:
        {
            QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent *>(event);
            moveEventOnTabBar(bar, mouseEvent);
            break;
        }
        default:
            break;
        }
    }

    return QObject::eventFilter(obj, event);
}

void GLDTabBarsControl::pressEventOnTabBar(QTabBar *bar, QMouseEvent *event)
{
    m_pressPosOnTab = QPoint(-1, -1);
    for (int index = 0; NULL != bar && index < bar->count(); ++index)
    {
        QRect tabRect = bar->tabRect(index);
        QPoint pressPos = event->pos();
        if (tabRect.contains(pressPos))
        {
            m_pressPosOnTab = pressPos;
            break;
        }
    }
}

void GLDTabBarsControl::moveEventOnTabBar(QTabBar *bar, QMouseEvent *event)
{
    if (NULL != m_nowDraggingDock)
    {
        QPoint gPoint = event->globalPos();
        gPoint.setX(gPoint.x() - m_xOffset);
        gPoint.setY(gPoint.y() - m_yOffset);
        m_nowDraggingDock->move(gPoint);
        return;
    }
    for (int index = 0; NULL != bar && index < bar->count(); ++index)
    {
        QRect tabRect = bar->tabRect(index);

        QPoint point = event->pos();
        if (!tabRect.contains(point) && (tabRect.contains(m_pressPosOnTab))
                && !bar->rect().contains(point))
        {
            QVariant tmp = bar->tabData(index);
            GLDDockWidget *dockWidget = reinterpret_cast<GLDDockWidget *>(tmp.toULongLong());
            QRect dockRect = dockWidget->geometry();
            dockWidget->setFloating(true);
            m_nowDraggingDock = dockWidget;
            bar->removeTab(index);
            // ���ָ���֮ǰ�Ĵ�С
            dockWidget->setGeometry(dockRect);
            QPoint gPoint = event->globalPos();
            // ����ƫ������ǵ�����������?
            m_xOffset = m_pressPosOnTab.x() - 3;
            m_yOffset = dockRect.height() - 3;
            gPoint.setX(gPoint.x() - m_xOffset);
            gPoint.setY(gPoint.y() - m_yOffset);
            dockWidget->move(gPoint);
            m_pressPosOnTab = QPoint(-1, -1);

            dockWidget->installEventFilter(this);
        }
    }
}

void GLDTabBarsControl::releaseEventOnDock(GLDDockWidget *dockWidget, QMouseEvent *event)
{
    if (NULL == m_nowTabBar)
    {
        return;
    }
    m_releasePosOnTab = event->pos();
    m_nowTabBar->removeEventFilter(dockWidget);
    m_nowDraggingDock = NULL;
    m_nowTabBar = NULL;
}

void GLDTabBarsControl::leaveDockWidget(GLDDockWidget *dockWidget)
{
    if (NULL == m_nowTabBar)
    {
        return;
    }
    m_nowTabBar->removeEventFilter(dockWidget);
    m_nowDraggingDock = NULL;
    m_nowTabBar = NULL;
}

/**
  GLDDockWidget
*/
GLDDockWidget::GLDDockWidget(const QString &title, QWidget *parent, Qt::WindowFlags flags) :
    QDockWidget(title, parent, flags)
{
    initTitleBar();
    m_title->setText(title);

    connect(this, SIGNAL(allowedAreasChanged(Qt::DockWidgetAreas)),
            this, SLOT(allowedAreasChanged(Qt::DockWidgetAreas)));
    connect(this, SIGNAL(featuresChanged(QDockWidget::DockWidgetFeatures)),
            this, SLOT(featuresChanged(QDockWidget::DockWidgetFeatures)));
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(dockLocationChanged(Qt::DockWidgetArea)));
    connect(this, SIGNAL(topLevelChanged(bool)),
            this, SLOT(topLevelChanged(bool)));
    connect(this, SIGNAL(visibilityChanged(bool)),
            this, SLOT(visibilityChanged(bool)));
}

QAction *GLDDockWidget::addAction(const QIcon &icon, const QString &text)
{
    QAction *act = new QAction(icon, text, m_titleBar);
    m_titleBar->insertAction(m_actLock, act);
    return act;
}

void GLDDockWidget::setIcon(const QIcon &icon)
{
    setWindowIcon(icon);
}

void GLDDockWidget::resizeEvent(QResizeEvent *)
{
    // �����ı���С��ʱ�����ڵ�����
    m_titleBar->setAutoFillBackground(true);
    m_titleBar->setBackgroundRole(QPalette::Highlight);
}

void GLDDockWidget::mouseReleaseEvent(QMouseEvent *event)
{
    gTabBarsControl.releaseEventOnDock(this, event);
    QDockWidget::mouseReleaseEvent(event);
}

void GLDDockWidget::leaveEvent(QEvent *event)
{
    gTabBarsControl.leaveDockWidget(this);
    QDockWidget::leaveEvent(event);
}


void GLDDockWidget::allowedAreasChanged(Qt::DockWidgetAreas)
{
}

void GLDDockWidget::featuresChanged(QDockWidget::DockWidgetFeatures)
{
}

void GLDDockWidget::dockLocationChanged(Qt::DockWidgetArea)
{
    m_actFloat->setVisible(!isFloating());
    m_actFloat->setDisabled(isFloating());
    m_actLock->setVisible(isFloating());
    m_actLock->setEnabled(isFloating());
}

void GLDDockWidget::topLevelChanged(bool)
{
    m_actFloat->setVisible(!isFloating());
    m_actFloat->setDisabled(isFloating());
    m_actLock->setVisible(isFloating());
    m_actLock->setEnabled(isFloating());
    this->activateWindow();
}

void GLDDockWidget::visibilityChanged(bool)
{
    QObject *widget = this->nativeParentWidget();
    if (NULL == widget)
    {
        return;
    }
    // �����е�bars��װһ���¼�������������������е�bars�ϵ��¼���
    QList<QTabBar *> bars = widget->findChildren<QTabBar *>(QString(), Qt::FindDirectChildrenOnly);
    for (int i = 0; i < bars.count(); ++i)
    {
        QTabBar *tabs = bars.at(i);
        tabs->installEventFilter(&gTabBarsControl);
    }
}

void GLDDockWidget::onFloat()
{
    setFloating(true);
}

void GLDDockWidget::onLock()
{
    setFloating(false);
}

void GLDDockWidget::onClose()
{
    close();
}

void GLDDockWidget::initTitleBar()
{
    m_titleBar = new QToolBar(this);
    this->setTitleBarWidget(m_titleBar);
    m_titleBar->setStyleSheet("border: 0px");
    m_titleBar->setAutoFillBackground(true);
    m_titleBar->setBackgroundRole(QPalette::Highlight);
    m_titleBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    m_title = new QLabel(this);
    m_title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    m_actTitle = m_titleBar->addWidget(m_title);

    m_actLock = m_titleBar->addAction(QIcon(":Res/Floating16.png"), tr("&Lock"));
    connect(m_actLock, SIGNAL(triggered()), this, SLOT(onLock()));
    m_actFloat = m_titleBar->addAction(QIcon(":Res/Locked16.png"), tr("&Float"));
    connect(m_actFloat, SIGNAL(triggered()), this, SLOT(onFloat()));
    m_actClose = m_titleBar->addAction(QIcon(":Res/Close16.png"), tr("&Close"));
    connect(m_actClose, SIGNAL(triggered()), this, SLOT(onClose()));
}


QAction *GLDDockWidget::addWidget(QWidget *widget)
{
    return m_titleBar->insertWidget(m_actLock, widget);
}
