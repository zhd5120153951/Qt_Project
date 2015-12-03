#ifndef GLDDOCKCONTAINER_H
#define GLDDOCKCONTAINER_H

#include <QStyleOptionDockWidget>
#include <QAbstractButton>
#include <QVBoxLayout>
#include <QToolButton>

#include "GLDString.h"
#include "GLDWidget_Global.h"

class GLDDockContainerTitleButton;
class GLDDockContainerTitleBar;
class GLDDockContainer;
class GLDDockContainerTabBar;

//GLDDockContainerTitleBar
class GLDWIDGETSHARED_EXPORT GLDDockContainerTitleBar : public QWidget
{
    Q_OBJECT
public:
    GLDDockContainerTitleBar(const GString &title, QWidget *parent);
    virtual ~GLDDockContainerTitleBar();
    void init();

    void setCloseBtnVisible(bool bShow);

protected:
    void paintEvent(QPaintEvent *evente);
    void resizeEvent(QResizeEvent *event);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    GLDDockContainerTitleButton *fixButton() ;
    GLDDockContainerTitleButton *closeButton() ;
    GString title();

private:
    void alignBtns(bool isCloseBtnShow = true);

private:
    bool m_showCloseBtn;
    GString m_title;
    GLDDockContainerTitleButton *m_fixedBtn;
    GLDDockContainerTitleButton *m_closeBtn;
    friend class GLDDockContainer;
};

//GLDDockContainerImageTitleBar
class GLDWIDGETSHARED_EXPORT GLDDockContainerImageTitleBar : public GLDDockContainerTitleBar
{
    Q_OBJECT
public:
    GLDDockContainerImageTitleBar(const GString &title, QWidget *parent);
    virtual ~GLDDockContainerImageTitleBar();

    void setCustomTitlePixmap(const QPixmap &leftPm,const QPixmap &centerPm,
                              const QPixmap &rightPm);
    void setUseCustomPixmap(bool used);

    void setTextColor(const QColor &textColor);
    void setTextAlignment(Qt::Alignment alignment);
    void setTextMargins(const QMargins &margins);

    QColor textColor() const;
    Qt::Alignment textAlignment() const;
    QMargins textMargins() const;

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

protected:
    QColor m_textColor;//(0,0,0)
    QPixmap m_leftPm, m_centerPm, m_rightPm;
    bool m_useCustomPixmap;//false;
    Qt::Alignment m_textAlignment;//vcenter,hleft
    QMargins m_textMargins;//top:1,bottom : 1, left : 30,right : 30
    friend class GLDDockContainer;
};

//GLDDockContainerTitleButton
class GLDWIDGETSHARED_EXPORT GLDDockContainerTitleButton : public QAbstractButton
{
    Q_OBJECT

public:
    GLDDockContainerTitleButton(QWidget *dockWidget);

    inline QIcon dockIcon() { return m_dockIcon; }
    inline void setNormIcon(QIcon value) { m_icon = value; } //泊靠时图标
    inline void setDockIcon(QIcon value) { m_dockIcon = value; } //泊靠时图标
    void setInDock(bool value);
    QSize sizeHint() const;
    inline QSize minimumSizeHint() const
    { return sizeHint(); }

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QIcon m_dockIcon;
    QIcon m_icon;
};

//GLDDockContainerTabBar
class GLDWIDGETSHARED_EXPORT GLDDockContainerTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit GLDDockContainerTabBar(QWidget *parent = 0);
    GLDDockContainerTabBar(QString title,QWidget *parent = 0);

    void setTabsClosable(bool closable);
    GString title() const;
	Qt::Orientation textOrientation();
	void setTextOrientation(Qt::Orientation orient);

Q_SIGNALS:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void initStyleOption(QStyleOptionTab *option, int tabIndex) const;
	//转换为垂直的字符串
	GString verticalString(const GString &str);

protected:
    GString m_title;

private:
    bool m_inResize;
	Qt::Orientation m_textOrientation;
};

//GLDDockContainerImageTabBar
class GLDWIDGETSHARED_EXPORT GLDDockContainerImageTabBar : public GLDDockContainerTabBar
{
    Q_OBJECT
public:
    explicit GLDDockContainerImageTabBar(QString title, QWidget *parent = 0);
    void setCustomTitlePixmap(const QPixmap &leftPm,const QPixmap &centerPm,
                              const QPixmap &rightPm);
    void setUseCustomPixmap(bool used);

    void setSize(const QSize &size);
    void setTextColor(const QColor &textColor);
    void setTextMargins(const QMargins &margins);
    void setTextAlignment(Qt::Alignment alignment);

    QSize size() const;
    QColor textColor() const;
    QMargins textMargins() const;
    Qt::Alignment textAlignment() const;

public:
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent *event);
    QString verticalTitle();

protected:
    QPixmap m_leftPm;
    QPixmap m_centerPm;
    QPixmap m_rightPm;
    bool m_usePixmap;//false
    QString m_title;
    QSize m_size;
    QColor m_textColor;//Black
    Qt::Alignment m_textAlignment; //hCenter,vTop;
    QMargins m_textMargins;//top:10,bottom:10,left:1,right 1;
};

class GLDWIDGETSHARED_EXPORT GLDDockContainer : public QWidget
{
    Q_OBJECT
public:
    explicit GLDDockContainer(const GString &title, QWidget *parent = 0, bool bWest = true);
    ~GLDDockContainer();

public:
    void addDockWidget(QWidget *w);
    void setCloseBtnVisible(bool value);
    void setTabsCloseAble(bool value);
    void setTitleBar(GLDDockContainerTitleBar *titleBar);
    void setTabBar(GLDDockContainerTabBar *tabBar);
    /*设置固定按钮图标*/
    QIcon fixedIcon();
    void setFixedIcon(QIcon &icon);
    /*设置固定按钮在泊靠状态下的图标*/
    void setFixedDockIcon(QIcon &icon);
	/**
	** tab control 字体方向
	**/
	Qt::Orientation tabTextOrientation();
	void setTabTextOrientation(Qt::Orientation orient);
    /*设置关闭按钮图标*/
    QIcon closeIcon();
    void setCloseIcon(QIcon &icon);
    inline void setMinDockWidth(int value) { m_minWidth = value; }

protected:
    void initStyleOption(QStyleOptionDockWidget *option) const;
    void doShow();
    void alignWidgets(bool bNeedRemove = true);
    void alignSplitter();
    void reInitTitleBar();
    void reInitTabBar();

protected:
    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    bool event(QEvent *event);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void doHideContainer();
    void doShowContainer();
    void doAddToSplitter();

protected slots:
    void doFixedClick();
    void doClickBar();
    void doShowFrame(bool bHideBar = true);

private:
    GLDDockContainerTitleBar *m_titleBar;
    QGridLayout *m_gridLayout;
    QWidget *m_dockWidget;
    GLDDockContainerTabBar *m_tabBar;
    QSize m_preSize;
    QSize m_fullSize; //包含tabBar和DockContainer，即鼠标悬浮在tabBar上面时应有的大小
    bool m_fixedFrame;
    bool m_barAtLeft;
    QMargins *m_oldMargin;
    int m_indexAtSplitter;
    QWidget *m_preParent;
    GString m_title;
    int m_minWidth;
};

#endif // GLDDOCKCONTAINER_H
