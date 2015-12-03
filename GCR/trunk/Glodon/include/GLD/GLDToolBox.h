#ifndef GLDTOOLBOX_H
#define GLDTOOLBOX_H

#include <QListWidget>
#include <QScrollArea>
#include <QTreeWidget>
#include <QStyleOption>
#include <QAbstractButton>

#include "GLDString.h"
#include "GLDSystem.h"
#include "GLDObjectList.h"
#include "GLDWidget_Global.h"

class QScrollArea;
class QVBoxLayout;
class GLDTreeWidgetItem;
class GLDToolBoxTreeItem;

class GLDWIDGETSHARED_EXPORT GLDToolBoxButton : public QAbstractButton
{
    Q_OBJECT
public:
    GLDToolBoxButton(QWidget *parent);

    inline void setSelected(bool b) { m_bSelected = b; update(); }
    inline void setIndex(int newIndex) { m_nIndexInPage = newIndex; }

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

protected:
    void initStyleOption(QStyleOptionToolBox *opt) const;
    void paintEvent(QPaintEvent *);

private:
    bool m_bSelected;
    int m_nIndexInPage;
};

class  GLDIconList : public QList<QIcon>
{
public:
    explicit GLDIconList(const QIcon &icon);
    explicit GLDIconList();
};

enum GLDToolBoxType
{
    gtbtHeader,
    gtbtItem
};

class GLDToolBox;
class GLDToolBoxHeader;

class GLDWIDGETSHARED_EXPORT GLDToolBoxItem : public QObject
{
    Q_OBJECT
public:
    explicit GLDToolBoxItem(GLDToolBox *toolBox, GLDToolBoxItem *parent);
    virtual ~GLDToolBoxItem();
public:
    enum ItemMode { ListWidget, TreeWidget };
    virtual GLDToolBoxItem *insertItem(int index, const QString &text, const QIcon &icon = QIcon());
    virtual GLDToolBoxType type() const = 0;
    virtual void updateItem(int index, const QString &text, const QIcon &icon = QIcon());
    virtual void deleteItem(int index);
    virtual void removeItem(GLDToolBoxItem *item);

    inline QString text() const { return m_text; }
    inline void setText(const QString &value) { m_text = value; }
    void setText(int index, const QString &text);
    QIcon icon() const { return m_icon; }
    void setIcon(const QIcon &value) { m_icon = value; }
    void setIcon(int index, const QIcon &icon);
    inline GLDToolBox *toolBox() const { return m_toolBox; }
    inline GLDToolBoxItem *parent() const { return m_parent; }
    GLDToolBoxHeader *header();
    GLDToolBoxItem *items(int index) const;
    inline int count() const { return m_items.count(); }
    void clearItems();

    inline GString name() const { return m_name; }
    void setName(const GString &value) { m_name = value; }

    inline PtrInt tag() const { return m_tag; }
    void setTag(const PtrInt &value) { m_tag = value; }

protected:
    virtual GLDToolBoxItem *createItem() = 0;
private:
    GLDToolBox *m_toolBox;
    GLDToolBoxItem *m_parent;
    GObjectList<GLDToolBoxItem *> m_items;
    GString m_text;
    GString m_name;
    PtrInt m_tag;
    QIcon m_icon;
    friend class GLDToolBox;
};

class GLDWIDGETSHARED_EXPORT GLDToolBoxHeader : public GLDToolBoxItem
{
    Q_OBJECT
public:
    explicit GLDToolBoxHeader(GLDToolBox *toolBox, GLDToolBoxItem *parent);
    ~GLDToolBoxHeader();
public:
    enum ItemMode { ListWidget, TreeWidget };
    void deleteItem(int index);
    void removeItem(GLDToolBoxItem *item);

    GLDToolBoxButton *button() const { return m_button; }

    QWidget *widget() const { return m_widget; }

    void setButtonText(const QString &text) { m_button->setText(text); setText(text); }

    void setButtonIcon(const QIcon &is) { m_button->setIcon(is); setIcon(is); }

    ItemMode itemMode() { return m_itemMode; }

#ifndef QT_NO_TOOLTIP
    void setToolTip(const QString &tip);
    QString toolTip() const;
#endif
    QString buttonText() const { return m_button->text(); }

    QIcon buttonIcon() const { return m_button->icon(); }

    inline bool operator==(const GLDToolBoxHeader& other) const
    {
        return widget() == other.widget();
    }

    void setViewMode(QListWidget::ViewMode value);
    void expandAll();
    GLDToolBoxItem *find(QListWidgetItem *value);
    /* GLDToolBoxItem */
    GLDToolBoxItem *insertItem(int index, const QString &text, const QIcon &icon = QIcon());
    GLDToolBoxType type() const;
    void setCurrentItem(GLDToolBoxItem *item);
    inline QListWidget *listWidget() const { return dynamic_cast<QListWidget*>(m_widget); }
    inline QTreeWidget *treeWidget() const { return dynamic_cast<QTreeWidget*>(m_widget); }
protected:
    GLDToolBoxItem *createItem();
private:
    void checkCanUseThisMethod();
//    GLDTreeWidgetItem *treeWidgetItem(int index);

    GLDToolBoxItem *insertItem(int index, const QIcon &icon, const QString &text);
    GLDToolBoxItem *addItem(const QIcon &icon, const QString &text);
    GLDToolBoxItem *addItem(const QString &text);

    GLDToolBoxItem *insertItem(int index, const GLDIconList &icon, const QStringList &text);
    GLDToolBoxItem *insertItem(int index, const QStringList &text);
    GLDToolBoxItem *addItem(const GLDIconList &icon, const QStringList &text);
    GLDToolBoxItem *addItem(const QStringList &text);

    void setButton(GLDToolBoxButton *value) { m_button = value; }

    void setWidget(QWidget *value);
    void setSv(QScrollArea *value) { m_sv = value; }

    QScrollArea *sv() const { return m_sv; }

    bool useOwnWidget() const;
    void setUseOwnWidget(bool useOwnWidget);
private:
    GLDToolBoxButton *m_button;
    QScrollArea *m_sv;
    QWidget *m_widget;
    ItemMode m_itemMode;
    bool m_useOwnWidget;

    friend class GLDToolBox;
    friend class GLDToolBoxItem;
};

class GLDWIDGETSHARED_EXPORT GLDToolBox : public QScrollArea
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
    Q_PROPERTY(int count READ count)

public:
    enum ItemMode { ListWidget, TreeWidget};
    explicit GLDToolBox(QWidget *parent = 0);
    ~GLDToolBox();

    inline ItemMode itemMode() { return m_itemMode; }
    void setItemMode(ItemMode mode);
    inline GLDToolBoxHeader *currentPage() { return m_pCurrentPage; }

    GLDToolBoxHeader *addItem(QWidget *widget, const QString &text);
    GLDToolBoxHeader *addItem(QWidget *widget, const QIcon &icon, const QString &text);
    GLDToolBoxHeader *addItem(const QString &text);
    GLDToolBoxHeader *addItem(const QIcon &icon, const QString &text);
    GLDToolBoxHeader *insertItem(int index, QWidget *widget, const QString &text);
    GLDToolBoxHeader *insertItem(int index, QWidget *widget, const QIcon &icon, const QString &text);
    GLDToolBoxHeader *insertItem(int index, const QString &text);
    GLDToolBoxHeader *insertItem(int index, const QIcon &icon, const QString &text);

    void removeItem(int index);

    void setItemEnabled(int index, bool enabled);
    bool isItemEnabled(int index) const;

    void setItemText(int index, const QString &text);
    QString itemText(int index) const;

    void setItemIcon(int index, const QIcon &icon);
    QIcon itemIcon(int index) const;

#ifndef QT_NO_TOOLTIP
    void setItemToolTip(int index, const QString &toolTip);
    QString itemToolTip(int index) const;
#endif

    int currentIndex() const;
    QWidget *currentWidget() const;
    QWidget *widget(int index) const;
    int indexOf(QWidget *widget) const;
    int count() const;

    int indexOf(const GString &name);
    GLDToolBoxHeader *header(int index);
    GLDToolBoxHeader *header(const GString &name);

public Q_SLOTS:
    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget *widget);
private Q_SLOTS:
    void itemChanging(QListWidgetItem *current, QListWidgetItem *previous);
    void itemChanged(QListWidgetItem *item);

    void itemChanging(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void itemChanged(QTreeWidgetItem *item, int column);
Q_SIGNALS:
    void currentChanged(int index);

    void headerChanging(GLDToolBoxHeader *headerCurrent, GLDToolBoxHeader *headerNext, bool &canChange);
    void headerChanged(GLDToolBoxHeader *headerPrevious, GLDToolBoxHeader *headerCurrent);

    void itemChanging(GLDToolBoxItem *itemCurrent, GLDToolBoxItem *itemNext, bool &canChange);
    void itemChanged(GLDToolBoxItem *itemPrevious, GLDToolBoxItem *itemCurrent);

protected:
    bool event(QEvent *e);
    virtual void itemInserted(int index);
    virtual void itemRemoved(int index);
    void showEvent(QShowEvent *e);
    void changeEvent(QEvent *);
private:
    QWidget *newItem();
private:
    typedef GObjectList<GLDToolBoxHeader*> PageList;
    QListWidgetItem *m_currentListWidgetItem;
    QTreeWidgetItem *m_currentTreeWidgetItem;
    GLDToolBoxHeader *page(QWidget *widget) const;
    const GLDToolBoxHeader *page(int index) const;
    GLDToolBoxHeader *page(int index);
    GLDToolBoxHeader *page(const GString &name);

    void updateTabs();
    void relayout();

    ItemMode m_itemMode;
    PageList m_pageList;
    QVBoxLayout *m_layout;
    GLDToolBoxHeader *m_pCurrentPage;
    bool m_useOwnWidget;
    typedef QScrollArea inherited;
private Q_SLOTS:
    void _q_buttonClicked();
    void _q_widgetDestroyed(QObject*);
};

class GLDWIDGETSHARED_EXPORT GLDToolBoxTreeItem : public GLDToolBoxItem
{
public:
    explicit GLDToolBoxTreeItem(GLDToolBox *toolBox, GLDToolBoxItem *parent);
    ~GLDToolBoxTreeItem();
    GLDToolBoxItem *insertItem(int index, const QString &text, const QIcon &icon = QIcon());
    GLDToolBoxType type() const;
    void deleteItem(int index);
    void removeItem(GLDToolBoxItem *item);

    GLDTreeWidgetItem *item() { return m_item; }

protected:
    GLDToolBoxItem *createItem();
private:
    GLDTreeWidgetItem *m_item;
};

#endif // GLDTOOLBOX_H
