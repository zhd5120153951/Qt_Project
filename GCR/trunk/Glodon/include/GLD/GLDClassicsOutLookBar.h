#ifndef GLDCLASSICSOUTLOOKBAR_H
#define GLDCLASSICSOUTLOOKBAR_H

#include <QHBoxLayout>
#include <QPushButton>
#include <QToolBar>
#include <QToolButton>
#include <QObjectList>
#include <QSplitter>
#include <QSplitterHandle>
#include <QUuid>
#include <QLabel>
#include <QScrollArea>
#include <QAction>
#include <QStack>
#include "GLDString.h"
#include "GLDWidget_Global.h"

class GLDOutLookBarAction;
// 视图区
// 用于显示设置的widget
class GLDWIDGETSHARED_EXPORT GLDOutLookView: public QFrame
{
public:
    explicit GLDOutLookView(QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~GLDOutLookView();

    void addView(QWidget *widget);
    void setTitle(const GString &title);
    void setTitleHeight(int nHeight);
    void removeView(QWidget *widget);
private:
    QVBoxLayout *m_layout;
    QLabel *m_title;
    QWidget *m_titleWgt;
};

class GLDWIDGETSHARED_EXPORT GLDNavigationItem: public QPushButton
{
public:
    explicit GLDNavigationItem(QWidget *parent = 0);
    explicit GLDNavigationItem(const GString &text, QWidget *parent = 0);
    GLDNavigationItem(const GString &text, const QIcon& icon, QWidget *parent = 0);
    ~GLDNavigationItem();
public:
    void addOutLookBarAction(GLDOutLookBarAction *action);
    void setChecked(bool checked);
    inline GString uuid() { return m_uuid.toString(); }

private:
    void init();
private:
    QUuid m_uuid;
};

// 缩略按钮bar
class GLDWIDGETSHARED_EXPORT GLDThumbnailBar: public QToolBar
{
    Q_OBJECT
public:
    explicit GLDThumbnailBar(QWidget *parent = 0);
    ~GLDThumbnailBar();
public:
    GLDOutLookBarAction *addAction(const QIcon &icon, const GString &text);
    GLDOutLookBarAction *addAction(GLDOutLookBarAction *action);
    void removeAction(GLDOutLookBarAction *action);

    void addLeftAction(GLDOutLookBarAction *action);
    void removeLeftAction();

    bool hasAction(const GString &uId);
    void clearButtons();
private slots:
    void showMoreAction();
private:
    QAction *m_foldAction;
    QStack<GLDOutLookBarAction *> m_btnActions;
};

// 导航
class GLDWIDGETSHARED_EXPORT GLDOutLookNavigationBar: public QWidget
{
public:
    explicit GLDOutLookNavigationBar(QWidget *parent);
    ~GLDOutLookNavigationBar();

public:
    GLDNavigationItem *addItem(GLDNavigationItem *item);
    GLDNavigationItem *addItem(const GString &text);
    GLDNavigationItem *addItem(const GString &text, const QIcon &icon);
    bool removeItem(GLDNavigationItem *item);
    bool removeItem(int index);

    GLDNavigationItem *addLastAction(GLDOutLookBarAction *action);
    GLDNavigationItem *findByUuid(const GString &value);
    QAction *removeLastAction();

    void clearItems();
    int needItemCount(int offset);
    int itemCount();

private:
    void initLayout();

private:
    QObjectList *m_itemList;
    QVBoxLayout *m_itemLayout;
};

//分割条的按钮
class GLDWIDGETSHARED_EXPORT GLDNavigationSplitterHandle : public QSplitterHandle
{
    Q_OBJECT
public:
    explicit GLDNavigationSplitterHandle(Qt::Orientation o, QSplitter *parent);
signals:
    void splitterHandleChanged(bool downward);
protected:
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
private:
    double m_windowPosY;
};

// GLDNavigationSplitter
class GLDWIDGETSHARED_EXPORT GLDNavigationSplitter : public QSplitter
{
    Q_OBJECT
public:
    explicit GLDNavigationSplitter(Qt::Orientation, QWidget* parent = 0);

signals:
    void splitterChanged(bool downward);

protected:
    QSplitterHandle *createHandle();

private slots:
    void splitterHandleChanged(bool downward);
};

//GLDOutLookBarAction
class GLDWIDGETSHARED_EXPORT GLDOutLookBarAction : public QObject
{
    Q_OBJECT
public:
    explicit GLDOutLookBarAction(const GString &text, const QIcon &icon = QIcon(), QObject *parent = 0);
    ~GLDOutLookBarAction();
public:
    inline QAction *action(){ return m_action; }
    inline GString uuid(){ return m_uuid.toString(); }
    void setChecked(bool checked = false);
    inline GString text() { return m_action->text(); }
    inline QIcon icon() { return m_action->icon(); }

signals:
    void clicked(const GString &Uuid);

private slots:
    void triggered();

private:
    QAction *m_action;
    QUuid m_uuid;
};

// outLook
class GLDWIDGETSHARED_EXPORT GLDClassicsOutLookBar : public QScrollArea
{
    Q_OBJECT
public:
    explicit GLDClassicsOutLookBar(QWidget *parent = 0);
    ~GLDClassicsOutLookBar();

public:
    GLDNavigationItem *addBarItem(QWidget *view, const GString &text, const QIcon &icon = QIcon());
    QAction *addThumbnailButton(QWidget *view, const GString &text, const QIcon &icon = QIcon());
    void setTitleHeight(int nHeight);
    int	currentIndex() const;
    void setCurrentIndex(int nIndex);
    QWidget *currentWidget() const;
    QWidget *widget(int nIndex) const;
    void setCurrentWidget(QWidget * widget);
    GString currentText() const;
    void setToolButtonStyle(Qt::ToolButtonStyle style);
    void clearItems();
    void setIsHideThumbnailButton(bool isHide);

signals:
    void currentChanged(int nIndex);
private slots:
    void itemClick(const GString &itemID);
    void splitterChanged(bool downward);
private:
    void initLayout();
    GLDOutLookBarAction *findAction(const GString &uID);
    int findActionIndex(const GString &itemID) const;
    void setItemCheck(const GString &uID, bool checked);
private:
    GLDOutLookView *m_outLookView;
    GLDNavigationSplitter *m_splitter;
    GLDOutLookNavigationBar *m_outLookNavigBar;
    GLDThumbnailBar *m_thumbnailButtons;

    QHash<GString, QWidget *> m_itemViews;
    QList<GLDOutLookBarAction *> m_actions;

    GLDOutLookBarAction *m_curAction;
    QWidget *m_curView;
};

#endif // GLDOUTLOOKBAR_H
