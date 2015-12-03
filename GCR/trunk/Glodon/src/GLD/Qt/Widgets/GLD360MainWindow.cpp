#include "GLD360MainWindow.h"

#include <QMenu>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QSignalMapper>

const int c_titleHeight = 30;
const int c_toolBarHeight = 100;

/* GLD360PushButton */
GLD360PushButton::GLD360PushButton(QWidget *parent) : QPushButton(parent),
    m_status(NORMAL), m_btnWidth(0), m_btnHeight(0), m_mousePress(false)
{

}

GLD360PushButton::~GLD360PushButton()
{

}

void GLD360PushButton::loadPixmap(const QString &picName)
{
    m_pixmap.load(picName);
    m_btnWidth = m_pixmap.width() / 4;
    m_btnHeight = m_pixmap.height();
    setFixedSize(m_btnWidth, m_btnHeight);
}

void GLD360PushButton::enterEvent(QEvent *)
{
    m_status = ENTER;
    update();
}

void GLD360PushButton::leaveEvent(QEvent *)
{
    m_status = NORMAL;
    update();
}

void GLD360PushButton::mousePressEvent(QMouseEvent *event)
{
    // 若点击鼠标左键
    if (event->button() == Qt::LeftButton)
    {
        m_mousePress = true;
        m_status = PRESS;
        update();
    }
}

void GLD360PushButton::mouseReleaseEvent(QMouseEvent *)
{
    // 若点击鼠标左键
    if (m_mousePress)
    {
        m_mousePress = false;
        m_status = ENTER;
        update();
        emit clicked();
    }
}

void GLD360PushButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), m_pixmap.copy(m_btnWidth * m_status, 0, m_btnWidth, m_btnHeight));
}

/* GLD360ToolButton */
GLD360ToolButton::GLD360ToolButton(const GString &text, const GString &picName, QWidget *parent) : QToolButton(parent),
    m_mouseOver(false), m_mousePress(false)
{
    setCheckable(true);
    setText(text);
    //设置文本颜色
    QPalette text_palette = palette();
    text_palette.setColor(QPalette::ButtonText, QColor(230, 230, 230));
    setPalette(text_palette);

    //设置文本粗体
    QFont &text_font = const_cast<QFont &>(font());
    text_font.setWeight(QFont::Bold);

    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    //设置图标
    QPixmap pixmap(picName);
    setIcon(pixmap);
    setIconSize(pixmap.size());

    //设置大小
    setFixedSize(pixmap.width() + 25, pixmap.height() + 27);
    setAutoRaise(true);

    setStyleSheet("background:transparent;");
}

GLD360ToolButton::~GLD360ToolButton()
{

}

void GLD360ToolButton::setMousePress(bool mousePress)
{
    m_mousePress = mousePress;
    update();
}

void GLD360ToolButton::enterEvent(QEvent *)
{
    m_mouseOver = true;
}

void GLD360ToolButton::leaveEvent(QEvent *)
{
    m_mouseOver = false;
}

void GLD360ToolButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
}

void GLD360ToolButton::paintEvent(QPaintEvent *event)
{
    if (m_mouseOver || isChecked())
    {
        //绘制鼠标移到按钮上的按钮效果
        painterInfo(0, 100, 150);
    }

    QToolButton::paintEvent(event);
}

void GLD360ToolButton::painterInfo(int topColor, int middleColor, int bottomColor)
{
    QPainter painter(this);
    QPen pen(Qt::NoBrush, 1);
    painter.setPen(pen);
    QLinearGradient linear(rect().topLeft(), rect().bottomLeft());
    linear.setColorAt(0, QColor(230, 230, 230, topColor));
    linear.setColorAt(0.5, QColor(230, 230, 230, middleColor));
    linear.setColorAt(1, QColor(230, 230, 230, bottomColor));
    painter.setBrush(linear);
    painter.drawRect(rect());
}

/* GLD360WindowTile */
GLD360WindowTile::GLD360WindowTile(QWidget *parent): QWidget(parent), m_isMove(false)
{
    m_versionTitle = new QLabel();
    m_skinButton = new GLD360PushButton();
    m_mainMenuButton = new GLD360PushButton();
    m_minButton = new GLD360PushButton();
    m_maxButton = new GLD360PushButton();
    m_closeButton = new GLD360PushButton();
    m_versionTitle->setAlignment(Qt::AlignVCenter);
    m_versionTitle->setStyleSheet("color:white;");
    m_versionTitle->setText(parent->windowTitle());
    //设置图片
    m_skinButton->loadPixmap(":/icons/360skin/sysbutton/skin_button");
    m_mainMenuButton->loadPixmap(":/icons/360skin/sysbutton/main_menu.png");
    m_minButton->loadPixmap(":/icons/360skin/sysbutton/min_button.png");
    m_maxButton->loadPixmap(":/icons/360skin/sysbutton/max_button.png");
    m_closeButton->loadPixmap(":/icons/360skin/sysbutton/close_button");

    connect(m_skinButton, SIGNAL(clicked()), this, SIGNAL(showSkin()));
    connect(m_mainMenuButton, SIGNAL(clicked()), this, SIGNAL(showMainMenu()));
    connect(m_minButton, SIGNAL(clicked()), this, SIGNAL(showMin()));
    connect(m_maxButton, SIGNAL(clicked()), this, SIGNAL(showMax()));
    connect(m_closeButton, SIGNAL(clicked()), this, SIGNAL(closeWidget()));

    QHBoxLayout *title_layout = new QHBoxLayout();
    title_layout->addWidget(m_versionTitle, 0, Qt::AlignVCenter);
    title_layout->addStretch();
    title_layout->addWidget(m_skinButton, 0, Qt::AlignTop);
    title_layout->addWidget(m_mainMenuButton, 0, Qt::AlignTop);
    title_layout->addWidget(m_minButton, 0, Qt::AlignTop);
    title_layout->addWidget(m_maxButton, 0, Qt::AlignTop);
    title_layout->addWidget(m_closeButton, 0, Qt::AlignTop);
    title_layout->setSpacing(0);
    title_layout->setContentsMargins(0, 0, 5, 0);
    m_versionTitle->setContentsMargins(15, 0, 0, 0);
    m_skinButton->setContentsMargins(0, 0, 10, 0);

    setLayout(title_layout);
    setFixedHeight(c_titleHeight);
}

GLD360WindowTile::~GLD360WindowTile()
{

}

void GLD360WindowTile::mousePressEvent(QMouseEvent *e)
{
    m_pressPoint = e->pos();
    m_isMove = true;
}

void GLD360WindowTile::mouseMoveEvent(QMouseEvent *e)
{
    if ((e->buttons() == Qt::LeftButton) && m_isMove)
    {
        static QWidget *parent_widget = this->parentWidget();
        QPoint parent_point = parent_widget->pos();
        parent_point.setX(parent_point.x() + e->x() - m_pressPoint.x());
        parent_point.setY(parent_point.y() + e->y() - m_pressPoint.y());
        parent_widget->move(parent_point);
    }
}

void GLD360WindowTile::mouseReleaseEvent(QMouseEvent *)
{
    if (m_isMove)
    {
        m_isMove = false;
    }
}

void GLD360WindowTile::mouseDoubleClickEvent(QMouseEvent *)
{
    emit showMax();
}

/* GLD360MainToolBar */
GLD360MainToolBar::GLD360MainToolBar(QWidget *parent): QWidget(parent), m_isMove(false)
{
    m_buttonLayout = new QHBoxLayout();
    m_signalMapper = new QSignalMapper(this);
    connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(turnPage(const QString &)));

    m_logoLabel = new QLabel();
    m_logoLabel->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(m_buttonLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(m_logoLabel);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(15, 0, 0, 0);

    setLayout(mainLayout);
    setFixedHeight(c_toolBarHeight);
}

GLD360MainToolBar::~GLD360MainToolBar()
{
    m_buttonList.clear();
}

void GLD360MainToolBar::add360Action(const GString &text, const GString &picName)
{
    GLD360ToolButton *tool_button = new GLD360ToolButton(text, picName);
    m_buttonList.push_back(tool_button);
    connect(tool_button, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
    m_signalMapper->setMapping(tool_button, QString::number(m_buttonList.count() - 1, 10));
    m_buttonLayout->addWidget(tool_button, 0, Qt::AlignBottom);
}

void GLD360MainToolBar::setLogo(const GString &picName)
{
    QPixmap pixmap(picName);
    m_logoLabel->setPixmap(pixmap);
    m_logoLabel->setFixedSize(pixmap.size());
}

void GLD360MainToolBar::setCurrentPage(const QString &currentPage)
{
    turnPage(currentPage);
}

void GLD360MainToolBar::turnPage(const QString &currentPage)
{
    bool ok = false;
    int current_index = currentPage.toInt(&ok, 10);

    for (int i = 0; i < m_buttonList.count(); i++)
    {
        GLD360ToolButton *toolButton = m_buttonList.at(i);

        if (current_index == i)
        {
            toolButton->setMousePress(true);
            toolButton->setChecked(true);
        }
        else
        {
            toolButton->setMousePress(false);
            toolButton->setChecked(false);
        }
    }
}

void GLD360MainToolBar::mousePressEvent(QMouseEvent *e)
{
    m_pressPoint = e->pos();
    m_isMove = true;
}

void GLD360MainToolBar::mouseMoveEvent(QMouseEvent *e)
{
    if ((e->buttons() == Qt::LeftButton) && m_isMove)
    {
        static QWidget *parent_widget = this->parentWidget();
        QPoint parent_point = parent_widget->pos();
        parent_point.setX(parent_point.x() + e->x() - m_pressPoint.x());
        parent_point.setY(parent_point.y() + e->y() - m_pressPoint.y());
        parent_widget->move(parent_point);
    }
}

void GLD360MainToolBar::mouseReleaseEvent(QMouseEvent *)
{
    if (m_isMove)
    {
        m_isMove = false;
    }
}

void GLD360MainToolBar::mouseDoubleClickEvent(QMouseEvent *)
{
    emit showMax();
}

/* GLD360MainWindow */
GLD360MainWindow::GLD360MainWindow(QWidget *parent): QWidget(parent), m_mainMenu(NULL), m_isCloseWhenMinimized(true),
    m_allowMax(false)
{

}

GLD360MainWindow::~GLD360MainWindow()
{

}

void GLD360MainWindow::initialize()
{
    // do nothing
}

void GLD360MainWindow::init()
{
    setMinimumSize(900, 600);
    setWindowFlags(Qt::FramelessWindowHint);

    m_skinName = QString(":/icons/360skin/skin/17_big.jpg");

    m_titleWidget = new GLD360WindowTile(this);
    m_mainToolBar = new GLD360MainToolBar(this);

    QWidget *wgt = new QWidget();
    m_mainLayOut = new QHBoxLayout();
    m_mainLayOut->setSpacing(0);
    m_mainLayOut->setContentsMargins(0, 0, 0, 0);
    wgt->setLayout(m_mainLayOut);

    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->addWidget(m_titleWidget);
    main_layout->addWidget(m_mainToolBar);
    main_layout->addWidget(wgt);
    main_layout->setSpacing(0);
    main_layout->setContentsMargins(0, 0, 0, 0);

    setLayout(main_layout);

    initialize();

    connect(m_titleWidget, SIGNAL(showSkin()), this, SIGNAL(showSkin()));
    connect(m_titleWidget, SIGNAL(showMainMenu()), this, SLOT(showMainMenu()));
    connect(m_titleWidget, SIGNAL(showMax()), this, SLOT(showMax()));
    connect(m_titleWidget, SIGNAL(showMin()), this, SLOT(showMinimized()));
    connect(m_titleWidget, SIGNAL(closeWidget()), this, SLOT(close()));
}

void GLD360MainWindow::showWidget()
{
    m_contentWidgetList.clear();
}

void GLD360MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), QPixmap(m_skinName));

    QPainter painter2(this);
    painter2.setPen(Qt::gray);
    static const QPointF points[4] = {QPointF(0, 100), QPointF(0, this->height() - 1), QPointF(this->width() - 1, this->height() - 1), QPointF(this->width() - 1, 100)};
    painter2.drawPolyline(points, 4);
}

void GLD360MainWindow::showMax()
{
    // todo 要恢复原始大小
    if (m_allowMax)
    {
        showMaximized();
    }
}

void GLD360MainWindow::showMainMenu()
{
    if (m_mainMenu == NULL)
    {
        return;
    }

    // 设置主菜单出现的位置
    QPoint p = rect().topRight();
    p.setX(p.x() - 150);
    p.setY(p.y() + 22);
    m_mainMenu->exec(this->mapToGlobal(p));
}

void GLD360MainWindow::changeSkin(const QString &skinName)
{
    m_skinName = skinName;
    update();
}

GLD360WindowTile *GLD360MainWindow::titleWidget() const
{
    return m_titleWidget;
}

void GLD360MainWindow::setTitleWidget(GLD360WindowTile *titleWidget)
{
    m_titleWidget = titleWidget;
}

GLD360MainToolBar *GLD360MainWindow::mainToolBar() const
{
    return m_mainToolBar;
}

void GLD360MainWindow::setMainToolBar(GLD360MainToolBar *mainToolBar)
{
    m_mainToolBar = mainToolBar;
}

bool GLD360MainWindow::allowMax() const
{
    return m_allowMax;
}

void GLD360MainWindow::setAllowMax(bool allowMax)
{
    m_allowMax = allowMax;
}

bool GLD360MainWindow::isCloseWhenMinimized() const
{
    return m_isCloseWhenMinimized;
}

void GLD360MainWindow::setIsCloseWhenMinimized(bool isCloseWhenMinimized)
{
    m_isCloseWhenMinimized = isCloseWhenMinimized;

    if (m_isCloseWhenMinimized)
    {
        disconnect(m_titleWidget, SIGNAL(closeWidget()), this, SLOT(hide()));
        connect(m_titleWidget, SIGNAL(closeWidget()), this, SLOT(close()));
    }
    else
    {
        disconnect(m_titleWidget, SIGNAL(closeWidget()), this, SLOT(close()));
        connect(m_titleWidget, SIGNAL(closeWidget()), this, SLOT(hide()));
    }
}

void GLD360MainWindow::setSkinName(const QString &skinName)
{
    changeSkin(skinName);
}

