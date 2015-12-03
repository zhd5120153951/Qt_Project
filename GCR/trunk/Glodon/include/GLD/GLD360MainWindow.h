/****
 * @file   : GLD360MainWindow.h
 * @brief  : 360风格主窗体
 *
 * @date   : 2014-04-02
 * @author : duanb
 * @remarks:
 ****/
#ifndef GLD360MAINWINDOW_H
#define GLD360MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QToolButton>

#include "GLDStrUtils.h"
#include "GLDObjectList.h"

class QLabel;
class QHBoxLayout;
class QSignalMapper;

class GLDWIDGETSHARED_EXPORT GLD360PushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit GLD360PushButton(QWidget *parent = 0);
    ~GLD360PushButton();
    void loadPixmap(const QString &picName);
protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);
private:
    //枚举按钮的几种状态
    enum ButtonStatus {NORMAL, ENTER, PRESS, NOSTATUS};
    ButtonStatus m_status;
    QPixmap m_pixmap;
    int m_btnWidth; //按钮宽度
    int m_btnHeight; //按钮高度
    bool m_mousePress; //按钮左键是否按下
};

class GLDWIDGETSHARED_EXPORT GLD360ToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit GLD360ToolButton(const GString &text, const GString &picName, QWidget *parent = 0);
    ~GLD360ToolButton();
    void setMousePress(bool mousePress);
protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void painterInfo(int topColor, int middleColor, int bottomColor);
public:
    bool m_mouseOver;    // 鼠标是否移过
    bool m_mousePress;    // 鼠标是否按下
};

// 标题栏
class GLDWIDGETSHARED_EXPORT GLD360WindowTile : public QWidget
{
    Q_OBJECT
public:
    explicit GLD360WindowTile(QWidget *parent = 0);
    virtual ~GLD360WindowTile();
signals:
    void showSkin();
    void showMin();
    void showMax();
    void showMainMenu();
    void closeWidget();
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
private:
    bool m_isMove;
    QPoint m_pressPoint;//鼠标按下去的点

    QLabel *m_versionTitle; //标题
    GLD360PushButton *m_skinButton; //换肤
    GLD360PushButton *m_mainMenuButton; //主菜单
    GLD360PushButton *m_minButton; //最小化
    GLD360PushButton *m_maxButton; //最大化
    GLD360PushButton *m_closeButton; //关闭
};

// 工具栏
class GLDWIDGETSHARED_EXPORT GLD360MainToolBar : public QWidget
{
    Q_OBJECT
public:
    explicit GLD360MainToolBar(QWidget *parent = 0);
    virtual ~GLD360MainToolBar();
public:
    void add360Action(const GString &text, const GString &picName);
    void setLogo(const GString &picName);
    void setCurrentPage(const QString &currentPage);
public slots:
signals:
    void showMax();
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
private slots:
    void turnPage(const QString &currentPage);
private:
    bool m_isMove;
    QPoint m_pressPoint; // 鼠标按下去的点
    GLDVector<GLD360ToolButton *> m_buttonList;
    QSignalMapper *m_signalMapper;
    QHBoxLayout *m_buttonLayout;
    QLabel *m_logoLabel;
};

// 主窗体
class GLDWIDGETSHARED_EXPORT GLD360MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit GLD360MainWindow(QWidget *parent = 0);
    virtual ~GLD360MainWindow();
public:
    void init();
    bool isCloseWhenMinimized() const;
    void setIsCloseWhenMinimized(bool isCloseWhenMinimized);
    void setSkinName(const QString &skinName);
    bool allowMax() const;
    void setAllowMax(bool allowMax);

    GLD360MainToolBar *mainToolBar() const;
    void setMainToolBar(GLD360MainToolBar *mainToolBar);

    GLD360WindowTile *titleWidget() const;
    void setTitleWidget(GLD360WindowTile *titleWidget);

public slots:
    void showWidget();
protected:
    virtual void initialize();
signals:
    void showSkin();
protected:
    void paintEvent(QPaintEvent *);
private slots:
    void showMax();
    void showMainMenu();
    void changeSkin(const QString &skinName);
private:
    QRect m_location;
    GLD360WindowTile *m_titleWidget; //标题栏
    GLD360MainToolBar *m_mainToolBar; // 工具栏
    // todo 主界面还没有内容
    GObjectList<QWidget*> m_contentWidgetList; //主界面内容
    QHBoxLayout *m_mainLayOut;
    QString m_skinName;//主窗口背景图片的名称
    // todo
    QMenu *m_mainMenu; //主菜单
    bool m_isCloseWhenMinimized;
    bool m_allowMax;
};

#endif // GLD360MAINWINDOW_H

