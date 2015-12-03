#include "GLDToolBox.h"

#include <QList>
#include <QFile>
#include <QLayout>
#include <QPainter>
#include <QHeaderView>
#include <QApplication>

#include "GLDWidget_Global.h"
#include "GLDFileUtils.h"
#include "GLDException.h"

//const int c_nMargin = 1;
//const int c_nToolBoxNavigatorWidth = 250;
const int c_nToolBoxButtonHeight = 32;
const int c_nLayOutMargin = 1;
class GLDToolBoxTreeItem;

const GString c_sToolBoxQssFile = ":/qsses/GLDToolBox.qss";

/* GLDToolBoxButton */
GLDToolBoxButton::GLDToolBoxButton(QWidget *parent)
    : QAbstractButton(parent), m_bSelected(false), m_nIndexInPage(-1)
{
    setFixedHeight(c_nToolBoxButtonHeight);
    setBackgroundRole(QPalette::Window);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    setFocusPolicy(Qt::NoFocus);
}

QSize GLDToolBoxButton::sizeHint() const
{
    QSize iconSize(8, 8);
    if (!icon().isNull())
    {
        int ncone = style()->pixelMetric(QStyle::PM_SmallIconSize, 0, parentWidget() /* GLDToolBox */);
        iconSize += QSize(ncone + 2, ncone);
    }
    QSize textSize = fontMetrics().size(Qt::TextShowMnemonic, text()) + QSize(0, 8);

    QSize total(iconSize.width() + textSize.width(), qMax(iconSize.height(), textSize.height()));
    return total.expandedTo(QApplication::globalStrut());
}

QSize GLDToolBoxButton::minimumSizeHint() const
{
    if (icon().isNull())
    {
        return QSize();
    }
    int ncone = style()->pixelMetric(QStyle::PM_SmallIconSize, 0, parentWidget() /* GLDToolBox */);
    return QSize(ncone + 8, ncone + 8);
}

void GLDToolBoxButton::initStyleOption(QStyleOptionToolBox *option) const
{
    if (!option)
    {
        return;
    }
    option->initFrom(this);
    if (m_bSelected)
    {
        option->state |= QStyle::State_Selected;
    }
    if (isDown())
    {
        option->state |= QStyle::State_Sunken;
    }
    option->text = text();
    option->icon = icon();

    if (QStyleOptionToolBoxV2 *optionV2 = qstyleoption_cast<QStyleOptionToolBoxV2 *>(option))
    {
        GLDToolBox *toolBox = static_cast<GLDToolBox *>(parentWidget()); // I know I'm in a tool box.
        int nWidgetsCount = toolBox->count();
        int nCurrIndex = toolBox->currentIndex();
        if (nWidgetsCount == 1)
        {
            optionV2->position = QStyleOptionToolBoxV2::OnlyOneTab;
        }
        else if (m_nIndexInPage == 0)
        {
            optionV2->position = QStyleOptionToolBoxV2::Beginning;
        }
        else if (m_nIndexInPage == nWidgetsCount - 1)
        {
            optionV2->position = QStyleOptionToolBoxV2::End;
        }
        else
        {
            optionV2->position = QStyleOptionToolBoxV2::Middle;
        }
        if (nCurrIndex == m_nIndexInPage - 1)
        {
            optionV2->selectedPosition = QStyleOptionToolBoxV2::PreviousIsSelected;
        }
        else if (nCurrIndex == m_nIndexInPage + 1)
        {
            optionV2->selectedPosition = QStyleOptionToolBoxV2::NextIsSelected;
        }
        else
        {
            optionV2->selectedPosition = QStyleOptionToolBoxV2::NotAdjacent;
        }
    }
}

void GLDToolBoxButton::paintEvent(QPaintEvent *)
{
    QPainter paint(this);
//    QString text = QAbstractButton::text();
    QPainter *painter = &paint;
    QStyleOptionToolBoxV2 opt;
    initStyleOption(&opt);
    style()->drawControl(QStyle::CE_ToolBoxTab, &opt, painter, parentWidget());
}

/* GLDIconList */
GLDIconList::GLDIconList(const QIcon &icon)
    : QList()
{
    append(icon);
}

GLDIconList::GLDIconList()
    : QList()
{
}

class GLDTreeWidgetItem : public QTreeWidgetItem
{
public:
    explicit GLDTreeWidgetItem(const QStringList &strings, GLDToolBoxTreeItem *owner, int type = Type);
    ~GLDTreeWidgetItem();
public:
    GLDTreeWidgetItem* insertItem(int index, const GLDIconList &icon, const QStringList &text);
    GLDTreeWidgetItem* insertItem(int index, const QStringList &text);
    GLDTreeWidgetItem* addItem(const GLDIconList &icon, const QStringList &text);
    GLDTreeWidgetItem* addItem(const QStringList &text);
    GLDTreeWidgetItem *childItem(int index) const;
    GLDToolBoxTreeItem *owner() const { return m_owner; }

private:
    GLDToolBoxTreeItem *m_owner;
};

/* GLDTreeWidgetItem */
GLDTreeWidgetItem::GLDTreeWidgetItem(const QStringList &strings, GLDToolBoxTreeItem *owner, int type)
    : QTreeWidgetItem(strings, type), m_owner(owner)
{

}

GLDTreeWidgetItem::~GLDTreeWidgetItem()
{

}

GLDTreeWidgetItem *GLDTreeWidgetItem::insertItem(int index, const GLDIconList &icon, const QStringList &text)
{
    assert(icon.count() <= text.count());
    GLDTreeWidgetItem *item = new GLDTreeWidgetItem(text, m_owner);
    for (int i = 0; i < icon.count(); ++i)
    {
        item->setIcon(i, icon.at(i));
    }
    if ((index < 0) || (index > childCount()))
    {
        addChild(item);
    }
    else
    {
        insertChild(index, item);
    }
    return item;
}

GLDTreeWidgetItem *GLDTreeWidgetItem::insertItem(int index, const QStringList &text)
{
    return insertItem(index, GLDIconList(QIcon()), text);
}

GLDTreeWidgetItem *GLDTreeWidgetItem::addItem(const GLDIconList &icon, const QStringList &text)
{
    return insertItem(-1, icon, text);
}

GLDTreeWidgetItem *GLDTreeWidgetItem::addItem(const QStringList &text)
{
    return insertItem(-1, GLDIconList(QIcon()), text);
}

GLDTreeWidgetItem *GLDTreeWidgetItem::childItem(int index) const
{
    QTreeWidgetItem *item = child(index);
    if (item)
    {
        return dynamic_cast<GLDTreeWidgetItem*>(item);
    }
    return NULL;
}

/* GLDToolBoxItem */
GLDToolBoxItem::GLDToolBoxItem(GLDToolBox *toolBox, GLDToolBoxItem *parent)
    : m_toolBox(toolBox), m_parent(parent), m_name(""), m_tag(0)
{

}

GLDToolBoxItem::~GLDToolBoxItem()
{

}

GLDToolBoxItem *GLDToolBoxItem::insertItem(int index, const QString &text, const QIcon &icon)
{
    GLDToolBoxItem *item = createItem();
    item->setText(text);
    item->setIcon(icon);
    if ((index < 0) || (index > m_items.count()))
    {
        m_items.append(item);
    }
    else
    {
        m_items.insert(index, item);
    }
    return item;
}

void GLDToolBoxItem::updateItem(int index, const QString &text, const QIcon &icon)
{
    if ((index < 0) || (index >= m_items.count()))
    {
        return;
    }
    else
    {
        setText(index, text);
        setIcon(index, icon);
    }
}

void GLDToolBoxItem::deleteItem(int index)
{
    if ((index < 0) || (index >= m_items.count()))
    {
        return;
    }
    m_items.Delete(index);
}

void GLDToolBoxItem::removeItem(GLDToolBoxItem *item)
{
    m_items.remove(item);
    freeAndNil(item);
}

void GLDToolBoxItem::setText(int index, const QString &text)
{
    if ((index < 0) || (index >= m_items.count()))
    {
        return;
    }
    items(index)->setText(text);
    if ((int)ListWidget == (int)this->header()->itemMode())
    {
        header()->listWidget()->item(index)->setText(text);
    }
    else if ((int)TreeWidget == (int)this->header()->itemMode())
    {
        GLDToolBoxTreeItem *pTreeItem = dynamic_cast<GLDToolBoxTreeItem *>(this);
        if (NULL != pTreeItem)
        {
            pTreeItem->item()->child(index)->setText(0, text);
        }
        else
        {
            header()->treeWidget()->topLevelItem(index)->setText(0, text);
        }
    }
}

void GLDToolBoxItem::setIcon(int index, const QIcon &icon)
{
    if ((index < 0) || (index >= m_items.count()))
    {
        return;
    }
    items(index)->setIcon(icon);
    if ((int)ListWidget == (int)this->header()->itemMode())
    {
        header()->listWidget()->item(index)->setIcon(icon);
    }
    else if ((int)TreeWidget == (int)this->header()->itemMode())
    {
        GLDToolBoxTreeItem *pTreeItem = dynamic_cast<GLDToolBoxTreeItem *>(this);
        if (NULL != pTreeItem)
        {
            pTreeItem->item()->child(index)->setIcon(0, icon);
        }
        else
        {
            header()->treeWidget()->topLevelItem(index)->setIcon(0, icon);
        }
    }
}

GLDToolBoxItem *GLDToolBoxItem::items(int index) const
{
    if ((index < 0) || (index >= count()))
    {
        return NULL;
    }
    return m_items[index];
}

GLDToolBoxHeader *GLDToolBoxItem::header()
{
    GLDToolBoxItem *item = this;
    GLDToolBoxType toolBoxType = item->type();
    while ((toolBoxType != gtbtHeader) && (item->parent() != NULL))
    {
        item = item->parent();
        toolBoxType = item->type();
    }
    return dynamic_cast<GLDToolBoxHeader*>(item);
}

void GLDToolBoxItem::clearItems()
{
    for (int i = m_items.count() - 1; i >= 0; --i)
    {
        deleteItem(i);
    }
}

/* GLDToolBoxListItem */
class GLDToolBoxListItem : public GLDToolBoxItem
{
public:
    explicit GLDToolBoxListItem(GLDToolBox *toolBox, GLDToolBoxItem *parent);
    ~GLDToolBoxListItem();
    GLDToolBoxItem *insertItem(int index, const QString &text, const QIcon &icon = QIcon());
    GLDToolBoxType type() const;
    void deleteItem(int index);
    void removeItem(GLDToolBoxItem *item);

    QListWidgetItem *item() { return m_item; }

protected:
    GLDToolBoxItem *createItem();
private:
    QListWidgetItem *m_item;
};

GLDToolBoxListItem::GLDToolBoxListItem(GLDToolBox *toolBox, GLDToolBoxItem *parent)
    : GLDToolBoxItem(toolBox, parent), m_item(new QListWidgetItem())
{

}

GLDToolBoxListItem::~GLDToolBoxListItem()
{
}

GLDToolBoxItem *GLDToolBoxListItem::insertItem(int index, const QString &text, const QIcon &icon)
{
    assert(false);
    return NULL;
    G_UNUSED(index);
    G_UNUSED(text);
    G_UNUSED(icon);
}

GLDToolBoxType GLDToolBoxListItem::type() const
{
    return gtbtItem;
}

void GLDToolBoxListItem::deleteItem(int index)
{
    assert(false);
    G_UNUSED(index)
}

void GLDToolBoxListItem::removeItem(GLDToolBoxItem *item)
{
    assert(false);
    G_UNUSED(item)
}

GLDToolBoxItem *GLDToolBoxListItem::createItem()
{
    assert(false);
    return NULL;
}

/* GLDToolBoxTreeItem */
GLDToolBoxTreeItem::GLDToolBoxTreeItem(GLDToolBox *toolBox, GLDToolBoxItem *parent)
    : GLDToolBoxItem(toolBox, parent), m_item(new GLDTreeWidgetItem(QStringList(""), this))
{

}

GLDToolBoxTreeItem::~GLDToolBoxTreeItem()
{

}

GLDToolBoxItem *GLDToolBoxTreeItem::insertItem(int index, const QString &text, const QIcon &icon)
{
    GLDToolBoxTreeItem *result = (GLDToolBoxTreeItem *)GLDToolBoxItem::insertItem(index, text);
    GLDTreeWidgetItem *item = result->item();
    item->setText(0, text);
    item->setIcon(0, icon);
    if ((index < 0) || (index >= m_item->childCount()))
    {
        m_item->addChild(item);
    }
    else
    {
        m_item->insertChild(index, item);
    }

    return result;
}

GLDToolBoxType GLDToolBoxTreeItem::type() const
{
    return gtbtItem;
}

void GLDToolBoxTreeItem::deleteItem(int index)
{
    QTreeWidgetItem *item = m_item->takeChild(index);
    if (item)
    {
        freeAndNil(item);
        GLDToolBoxItem::deleteItem(index);
    }
}

void GLDToolBoxTreeItem::removeItem(GLDToolBoxItem *item)
{
    QTreeWidgetItem *removeItem
            = m_item->takeChild(m_item->indexOfChild((dynamic_cast<GLDToolBoxTreeItem*>(item)->item())));
    if (removeItem)
    {
        freeAndNil(removeItem);
        GLDToolBoxItem::removeItem(item);
    }
}

GLDToolBoxItem *GLDToolBoxTreeItem::createItem()
{
    return new GLDToolBoxTreeItem(toolBox(), this);
}

/* GLDToolBoxHeader */
GLDToolBoxHeader::GLDToolBoxHeader(GLDToolBox *toolBox, GLDToolBoxItem *parent)
    : GLDToolBoxItem(toolBox, parent), m_button(NULL), m_sv(NULL), m_widget(NULL),
      m_itemMode(TreeWidget), m_useOwnWidget(false)
{

}

GLDToolBoxHeader::~GLDToolBoxHeader()
{
}

void GLDToolBoxHeader::deleteItem(int index)
{
    checkCanUseThisMethod();
    switch (m_itemMode)
    {
    case ListWidget:
    {
        QListWidget *listWidget = dynamic_cast<QListWidget*>(m_widget);
        QListWidgetItem *item = listWidget->takeItem(index);
        freeAndNil(item);
        break;
    }
    case TreeWidget:
    {
        QTreeWidget *treeWidget = dynamic_cast<QTreeWidget*>(m_widget);
        QTreeWidgetItem *item = treeWidget->takeTopLevelItem(index);
        freeAndNil(item);
        break;
    }
    default:
        break;
    }
    GLDToolBoxItem::deleteItem(index);
}

void GLDToolBoxHeader::removeItem(GLDToolBoxItem *item)
{
    checkCanUseThisMethod();
    switch (m_itemMode)
    {
    case ListWidget:
    {
        QListWidget *listWidget = dynamic_cast<QListWidget*>(m_widget);
        QListWidgetItem *deleteItem = listWidget->takeItem(listWidget->row(((GLDToolBoxListItem*)(item))->item()));
        freeAndNil(deleteItem);
        break;
    }
    case TreeWidget:
    {
        QTreeWidget *treeWidget = dynamic_cast<QTreeWidget*>(m_widget);
        QTreeWidgetItem *deleteItem
                = treeWidget->takeTopLevelItem(treeWidget->indexOfTopLevelItem(((GLDToolBoxTreeItem*)(item))->item()));
        freeAndNil(deleteItem);
        break;
    }
    default:
        break;
    }
    GLDToolBoxItem::removeItem(item);
}

void GLDToolBoxHeader::setWidget(QWidget *value)
{
    if (m_widget)
    {
        assert(false);    // 不允许重新赋值
    }
    else
    {
        if (dynamic_cast<QListWidget*>(value))
        {
            m_itemMode = ListWidget;
        }
        else if (dynamic_cast<QTreeWidget*>(value))
        {
            m_itemMode = TreeWidget;
        }
        m_widget = value;
    }
}

#ifndef QT_NO_TOOLTIP
void GLDToolBoxHeader::setToolTip(const QString &tip)
{
    m_button->setToolTip(tip);
}

QString GLDToolBoxHeader::toolTip() const
{
    return m_button->toolTip();
}

#endif

GLDToolBoxItem *GLDToolBoxHeader::insertItem(int index, const QString &text, const QIcon &icon)
{
    checkCanUseThisMethod();
    switch (m_itemMode)
    {
    case ListWidget:
    {
        QListWidget *listWidget = dynamic_cast<QListWidget*>(m_widget);
        if (listWidget)
        {
            GLDToolBoxListItem *result = (GLDToolBoxListItem *)GLDToolBoxItem::insertItem(index, text, icon);
            result->item()->setText(text);
            result->item()->setIcon(icon);
            if ((index < 0) || (index >= listWidget->count()))
            {
                listWidget->addItem(result->item());
            }
            else
            {
                listWidget->insertItem(index, result->item());
            }
            return result;
        }
        break;
    }
    case TreeWidget:
    {
        return insertItem(index, GLDIconList(icon), QStringList(text));
    }
    default:
        assert(false);
        break;
    }
    return NULL;
}

GLDToolBoxType GLDToolBoxHeader::type() const
{
    return gtbtHeader;
}

void GLDToolBoxHeader::setCurrentItem(GLDToolBoxItem *item)
{
    switch (m_itemMode)
    {
    case ListWidget:
        listWidget()->setCurrentItem(dynamic_cast<GLDToolBoxListItem*>(item)->item());
        break;
    case TreeWidget:
        treeWidget()->setCurrentItem(dynamic_cast<GLDToolBoxTreeItem*>(item)->item());
        break;
    default:
        break;
    }
}

GLDToolBoxItem *GLDToolBoxHeader::createItem()
{
    checkCanUseThisMethod();
    switch (m_itemMode)
    {
    case ListWidget:
        return new GLDToolBoxListItem(toolBox(), this);
    case TreeWidget:
        return new GLDToolBoxTreeItem(toolBox(), this);
    default:
        assert(false);
        break;
    }
    return NULL;
}

void GLDToolBoxHeader::checkCanUseThisMethod()
{
    if (m_useOwnWidget)
    {
        gldError(tr("Custom widget classes should not invode this function"));//TRANS_STRING("自定义widget不能调用此方法"));
    }
}

GLDToolBoxItem *GLDToolBoxHeader::insertItem(int index, const QIcon &icon, const QString &text)
{
    return insertItem(index, text, icon);
}

//GLDToolBoxItem *GLDToolBoxHeader::insertItem(int index, const QString &text)
//{
//    return insertItem(index, QIcon(), text);
//}

GLDToolBoxItem *GLDToolBoxHeader::addItem(const QIcon &icon, const QString &text)
{
    return insertItem(-1, icon, text);
}

GLDToolBoxItem *GLDToolBoxHeader::addItem(const QString &text)
{
    return insertItem(-1, QIcon(), text);
}

GLDToolBoxItem *GLDToolBoxHeader::insertItem(int index, const GLDIconList &icon, const QStringList &text)
{
    checkCanUseThisMethod();
    switch (m_itemMode)
    {
    case ListWidget:
    {
        return insertItem(index, icon.at(0), text.at(0));
    }
    case TreeWidget:
    {
        assert(icon.count() <= text.count());
        QTreeWidget *treeWidget = dynamic_cast<QTreeWidget*>(m_widget);
        if (treeWidget)
        {
            GLDToolBoxTreeItem *result
                    = (GLDToolBoxTreeItem *)GLDToolBoxItem::insertItem(index, text.at(0), icon.at(0));
            result->item()->setText(0, text.at(0));
            result->item()->setIcon(0, icon.at(0));
            //            GLDTreeWidgetItem *item = new GLDTreeWidgetItem(QStringList(text));
            //            for (int i = 0; i < icon.count(); ++i)
            //            {
            //                result->setIcon(i, icon.at(i));
            //            }
            int nChildCount = treeWidget->topLevelItemCount();
            if ((index < 0) || (index >= nChildCount))
            {
                treeWidget->addTopLevelItem(result->item());
                index = nChildCount;
            }
            else
            {
                treeWidget->insertTopLevelItem(index, result->item());
            }
            //            // 增加头的列数
            //            for (int i = 0; i < text.count(); ++i)
            //            {
            //                treeWidget->headerItem()->setText(i, "header");
            //            }
            return result;
        }
        break;
    }
    default:
        assert(false);
        break;
    }
    return 0;
}

GLDToolBoxItem *GLDToolBoxHeader::insertItem(int index, const QStringList &text)
{
    return insertItem(index, GLDIconList(QIcon()), text);
}

GLDToolBoxItem *GLDToolBoxHeader::addItem(const GLDIconList &icon, const QStringList &text)
{
    return insertItem(-1, icon, text);
}

GLDToolBoxItem *GLDToolBoxHeader::addItem(const QStringList &text)
{
    return insertItem(-1, GLDIconList(QIcon()), text);
}

bool GLDToolBoxHeader::useOwnWidget() const
{
    return m_useOwnWidget;
}

void GLDToolBoxHeader::setUseOwnWidget(bool useOwnWidget)
{
    m_useOwnWidget = useOwnWidget;
}

//GLDTreeWidgetItem *GLDToolBoxHeader::treeWidgetItem(int index)
//{
//    QTreeWidget *treeWidget = dynamic_cast<QTreeWidget*>(m_widget);
//    if (treeWidget)
//    {
//        int nChildCount = treeWidget->topLevelItemCount();
//        if ((index < 0) || (index >= nChildCount))
//        {
//            return NULL;
//        }
//        GLDTreeWidgetItem *item = dynamic_cast<GLDTreeWidgetItem*>(treeWidget->topLevelItem(index));
//        return item;
//    }
//    return NULL;
//}

void GLDToolBoxHeader::setViewMode(QListView::ViewMode value)
{
    checkCanUseThisMethod();
    switch (m_itemMode)
    {
    case ListWidget:
    {
        QListWidget *listWidget = dynamic_cast<QListWidget*>(m_widget);
        if (listWidget)
        {
            listWidget->setViewMode(value);
            listWidget->setDragEnabled(false);
        }
        break;
    }
    case TreeWidget:
        assert(false);
        break;
    default:
        assert(false);
        break;
    }
}

void GLDToolBoxHeader::expandAll()
{
    checkCanUseThisMethod();
    switch (m_itemMode)
    {
    case ListWidget:
        break;
    case TreeWidget:
        dynamic_cast<QTreeWidget*>(m_widget)->expandAll();
        break;
    default:
        assert(false);
        break;
    }
}

GLDToolBoxItem *GLDToolBoxHeader::find(QListWidgetItem *value)
{
    if (!value)
    {
        return NULL;
    }
    QListWidget *widget = listWidget();
    if (widget && (widget == value->listWidget()))
    {
        for (int i = 0; i < widget->count(); ++i)
        {
            if (widget->item(i) == value)
            {
                return items(i);
            }
        }
    }
    return NULL;
}

/* GLDToolBox */
GLDToolBox::GLDToolBox(QWidget *parent)
    :  QScrollArea(parent), m_currentListWidgetItem(NULL), m_currentTreeWidgetItem(NULL),
      m_itemMode(TreeWidget), m_pCurrentPage(0), m_useOwnWidget(true)
{
    //setFixedWidth(nToolBoxNavigatorWidth + nMargin + 2);
    setFrameShape(QScrollArea::NoFrame);
    m_layout = new QVBoxLayout(this);
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    setBackgroundRole(QPalette::Button);
    this->setStyleSheet(loadQssFile(c_sToolBoxQssFile));
}

GLDToolBox::~GLDToolBox()
{

}

GLDToolBoxHeader *GLDToolBox::page(QWidget *widget) const
{
    if (!widget)
    {
        return NULL;
    }

    for (int i = 0; i < m_pageList.count(); ++i)
    {
        if (m_pageList.at(i)->widget() == widget)
        {
            return m_pageList.at(i);
        }
    }

    return NULL;
}

GLDToolBoxHeader *GLDToolBox::page(int index)
{
    if (index >= 0 && index < m_pageList.count())
    {
        return m_pageList[index];
    }
    return NULL;
}

GLDToolBoxHeader *GLDToolBox::page(const GString &name)
{
    for (int i = 0; i < m_pageList.count(); ++i)
    {
        if (compareText(name, m_pageList.at(i)->name()) == 0)
        {
            return m_pageList[i];
        }
    }
    return NULL;
}

const GLDToolBoxHeader *GLDToolBox::page(int index) const
{
    if (index >= 0 && index < m_pageList.count())
    {
        return m_pageList.at(index);
    }
    return NULL;
}

void GLDToolBox::updateTabs()
{
    GLDToolBoxButton *lastButton = m_pCurrentPage ? m_pCurrentPage->button() : 0;
    bool bAfter = false;
    int nindex = 0;
    for (nindex = 0; nindex < m_pageList.count(); ++nindex)
    {
        const GLDToolBoxHeader *page = m_pageList.at(nindex);
        GLDToolBoxButton *tB = page->button();
        // update indexes, since the updates are delayed, the indexes will be correct
        // when we actually paint.
        tB->setIndex(nindex);
        QWidget *tW = page->widget();
        if (bAfter)
        {
            QPalette palette = tB->palette();
            palette.setColor(tB->backgroundRole(), tW->palette().color(tW->backgroundRole()));
            tB->setPalette(palette);
            tB->update();
        }
        else if (tB->backgroundRole() != QPalette::Window)
        {
            tB->setBackgroundRole(QPalette::Window);
            tB->update();
        }
        bAfter = tB == lastButton;
    }
}

void GLDToolBox::setItemMode(GLDToolBox::ItemMode mode)
{
    m_itemMode = mode;
}

GLDToolBoxHeader* GLDToolBox::addItem(QWidget *item, const QString &text)
{
    return insertItem(-1, item, QIcon(), text);
}

GLDToolBoxHeader* GLDToolBox::addItem(QWidget *item, const QIcon &iconSet, const QString &text)
{
    return insertItem(-1, item, iconSet, text);
}

QWidget* GLDToolBox::newItem()
{
    m_useOwnWidget = false;
    QWidget* item = NULL;
    switch (m_itemMode)
    {
    case ListWidget:
        item = new QListWidget();
        connect(item, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
                this, SLOT(itemChanging(QListWidgetItem*, QListWidgetItem*)));
//        connect(item, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*)));
//        connect(item, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*)));
        connect(item, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*)));
        break;
    case TreeWidget:
    {
        QTreeWidget *treeWidget = new QTreeWidget();
        treeWidget->setSelectionMode(QTreeWidget::SingleSelection);
        connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
                this, SLOT(itemChanging(QTreeWidgetItem*, QTreeWidgetItem*)));
//  connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(itemChanged(QTreeWidgetItem *, int)));
//  connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(itemChanged(QTreeWidgetItem *, int)));
        connect(treeWidget, SIGNAL(itemPressed(QTreeWidgetItem *, int)),
                this, SLOT(itemChanged(QTreeWidgetItem *, int)));
        // 自适应宽度
        treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        // 隐藏标题
        treeWidget->setHeaderHidden(true);
        item = treeWidget;
        break;
    }
    default:
        assert(false);
        break;
    }
    return item;
}

GLDToolBoxHeader* GLDToolBox::addItem(const QString &text)
{
    QWidget* item = newItem();
    return insertItem(-1, item, QIcon(), text);
}

GLDToolBoxHeader* GLDToolBox::addItem(const QIcon &iconSet, const QString &text)
{
    QWidget* item = newItem();
    return insertItem(-1, item, iconSet, text);
}

GLDToolBoxHeader* GLDToolBox::insertItem(int index, QWidget *item, const QString &text)
{
    return insertItem(index, item, QIcon(), text);
}

GLDToolBoxHeader* GLDToolBox::insertItem(int index, const QString &text)
{
    QWidget* item = newItem();
    return insertItem(index, item, QIcon(), text);
}

GLDToolBoxHeader* GLDToolBox::insertItem(int index, const QIcon &icon, const QString &text)
{
    QWidget* item = newItem();
    return insertItem(index, item, icon, text);
}

GLDToolBoxHeader* GLDToolBox::insertItem(int index, QWidget *widget, const QIcon &icon, const QString &text)
{
    if (!widget)
    {
        m_useOwnWidget = true;
        return NULL;
    }

    connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(_q_widgetDestroyed(QObject*)));

    GLDToolBoxHeader *pHeader = new GLDToolBoxHeader(this, NULL);
    pHeader->setUseOwnWidget(m_useOwnWidget);
    m_useOwnWidget = true;
    pHeader->setWidget(widget);
    pHeader->setButton(new GLDToolBoxButton(this));
    pHeader->button()->setObjectName(QLatin1String("GLD_toolbox_toolboxbutton"));
    connect(pHeader->button(), SIGNAL(clicked()), this, SLOT(_q_buttonClicked()));
    QScrollArea *area = new QScrollArea(this);
    area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pHeader->setSv(area);
    pHeader->sv()->setWidget(widget);
    pHeader->sv()->setWidgetResizable(true);
    pHeader->sv()->hide();
    pHeader->sv()->setFrameStyle(inherited::NoFrame);

    pHeader->setButtonText(text);
    pHeader->setButtonIcon(icon);

    if (index < 0 || index >= (int)m_pageList.count())
    {
        index = m_pageList.count();
        m_pageList.append(pHeader);
        m_layout->addWidget(pHeader->button());
        m_layout->addWidget(pHeader->sv());
        if (index == 0)
        {
            setCurrentIndex(index);
        }
    }
    else
    {
        m_pageList.insert(index, pHeader);
        relayout();
        if (m_pCurrentPage)
        {
            QWidget *current = m_pCurrentPage->widget();
            int nOldindex = indexOf(current);
            if (index <= nOldindex)
            {
                m_pCurrentPage = 0; // trigger change
                setCurrentIndex(nOldindex);
            }
        }
    }

    pHeader->button()->show();

    updateTabs();
    itemInserted(index);
    return pHeader;
}

void GLDToolBox::_q_buttonClicked()
{
    GLDToolBoxButton *tb = dynamic_cast<GLDToolBoxButton*>(sender());
    QWidget* item = NULL;
    for (int i = 0; i < m_pageList.count(); ++i)
    {
        if (m_pageList.at(i)->button() == tb)
        {
            item = m_pageList.at(i)->widget();
            break;
        }
    }
    setCurrentIndex(indexOf(item));
}

int GLDToolBox::count() const
{
    return m_pageList.count();
}

int GLDToolBox::indexOf(const GString &name)
{
    for (int i = 0; i < m_pageList.count(); ++i)
    {
        if (compareText(name, m_pageList.at(i)->name()) == 0)
        {
            return i;
        }
    }
    return -1;
}

GLDToolBoxHeader *GLDToolBox::header(int index)
{
    return page(index);
}

GLDToolBoxHeader *GLDToolBox::header(const GString &name)
{
    return page(name);
}

void GLDToolBox::setCurrentIndex(int index)
{
    bool bCanChange = true;
    GLDToolBoxHeader *pHeader = page(index);

    if (!pHeader || m_pCurrentPage == pHeader)
    {
        return;
    }
    emit headerChanging(m_pCurrentPage, pHeader, bCanChange);
    if (!bCanChange)
    {
        return;
    }
    GLDToolBoxHeader *oldPage = m_pCurrentPage;
    pHeader->button()->setSelected(true);
    if (m_pCurrentPage)
    {
        m_pCurrentPage->sv()->hide();
        m_pCurrentPage->button()->setSelected(false);
        m_pCurrentPage->button()->setProperty("GLDCurrentBtn", "NoSelected");
    }
    pHeader->button()->setProperty("GLDCurrentBtn", "Selected");
    this->setStyleSheet(loadQssFile(c_sToolBoxQssFile));
    m_pCurrentPage = pHeader;
    m_pCurrentPage->sv()->show();
    updateTabs();
    emit currentChanged(index);
    emit headerChanged(oldPage, pHeader);
}

void GLDToolBox::relayout()
{
    delete m_layout;
    m_layout = new QVBoxLayout(this);
    m_layout->setMargin(c_nLayOutMargin);
    m_layout->setSpacing(0);
    for (int i = 0; i < m_pageList.count(); ++i)
    {
        m_layout->addWidget(m_pageList.at(i)->button());
        m_layout->addWidget(m_pageList.at(i)->sv());
    }
}

void GLDToolBox::_q_widgetDestroyed(QObject *object)
{
    // no verification - vtbl corrupted already
    QWidget *pWidget = (QWidget*)object;

    GLDToolBoxHeader *pHeader = page(pWidget);
    if (!pWidget || !pHeader)
        return;

    m_layout->removeWidget(pHeader->sv());
    m_layout->removeWidget(pHeader->button());
    pHeader->sv()->deleteLater(); // page might still be a child of sv
    delete pHeader->button();

    bool bRemoveCurrent = pHeader == m_pCurrentPage;
    m_pageList.remove(pHeader);

    if (0 == m_pageList.count())
    {
        m_pCurrentPage = 0;
        emit currentChanged(-1);
    }
    else if (bRemoveCurrent)
    {
        m_pCurrentPage = 0;
        setCurrentIndex(0);
    }
}

void GLDToolBox::removeItem(int index)
{
    if (QWidget *pWidget = widget(index))
    {
        disconnect(pWidget, SIGNAL(destroyed(QObject*)), this, SLOT(_q_widgetDestroyed(QObject*)));
        pWidget->setParent(this);
        // destroy internal data
        _q_widgetDestroyed(pWidget);
        itemRemoved(index);
    }
}

int GLDToolBox::currentIndex() const
{
    return m_pCurrentPage ? indexOf(m_pCurrentPage->widget()) : -1;
}

QWidget * GLDToolBox::currentWidget() const
{
    return m_pCurrentPage ? m_pCurrentPage->widget() : 0;
}

void GLDToolBox::setCurrentWidget(QWidget *widget)
{
    int nIndex = indexOf(widget);
    if (nIndex >= 0)
        setCurrentIndex(nIndex);
    else
        qWarning("GLDToolBox::setCurrentWidget: widget not contained in tool box");
}

void GLDToolBox::itemChanging(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
    {
        return;
    }
    QListWidget *listWidget = m_pCurrentPage->listWidget();
    GLDToolBoxItem *itemCurrent = m_pCurrentPage->find(current);
    GLDToolBoxItem *itemPrevious = m_pCurrentPage->find(previous);
    disconnect(listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
               this, SLOT(itemChanging(QListWidgetItem*, QListWidgetItem*)));

    if (previous)
    {
        listWidget->setCurrentItem(previous);
        m_currentListWidgetItem = previous;
    }

    bool bCanChanged = true;
    emit itemChanging(itemPrevious, itemCurrent, bCanChanged);

    if (bCanChanged)
    {
        listWidget->setCurrentItem(current);
        m_currentListWidgetItem = current;
        emit itemChanged(itemPrevious, itemCurrent);
    }
    connect(listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(itemChanging(QListWidgetItem*, QListWidgetItem*)));
}

void GLDToolBox::itemChanged(QListWidgetItem *item)
{
    QListWidget *listWidget = m_pCurrentPage->listWidget();
    if (listWidget && m_currentListWidgetItem)
    {
        listWidget->setCurrentItem(m_currentListWidgetItem);
        m_currentListWidgetItem = NULL;
    }
    G_UNUSED(item)
}

void GLDToolBox::itemChanging(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (!current)
    {
        return;
    }

    QTreeWidget *treeWidget = m_pCurrentPage->treeWidget();
    if ((treeWidget != NULL) && (treeWidget == current->treeWidget()))
    {
        GLDToolBoxTreeItem *itemCurrent = dynamic_cast<GLDTreeWidgetItem *>(current)->owner();
        GLDToolBoxTreeItem *itemPrevious = NULL;
        if (previous)
        {
            itemPrevious = dynamic_cast<GLDTreeWidgetItem *>(previous)->owner();
        }
        disconnect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
                   this, SLOT(itemChanging(QTreeWidgetItem*, QTreeWidgetItem*)));

        if (previous)
        {
            treeWidget->setCurrentItem(previous);
            m_currentTreeWidgetItem = previous;
        }

        bool bCanChanged = true;
        emit itemChanging(itemPrevious, itemCurrent, bCanChanged);

        if (bCanChanged)
        {
            treeWidget->setCurrentItem(current);
            m_currentTreeWidgetItem = current;
            emit itemChanged(itemPrevious, itemCurrent);
        }
        connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
                this, SLOT(itemChanging(QTreeWidgetItem*, QTreeWidgetItem*)));
    }
}

void GLDToolBox::itemChanged(QTreeWidgetItem *item, int column)
{
    QTreeWidget *treeWidget = m_pCurrentPage->treeWidget();
    if (treeWidget && m_currentTreeWidgetItem)
    {
        treeWidget->setCurrentItem(m_currentTreeWidgetItem);
        m_currentTreeWidgetItem = NULL;
    }
    G_UNUSED(item);
    G_UNUSED(column)
}

QWidget *GLDToolBox::widget(int index) const
{
    if (index < 0 || index >= (int) m_pageList.size())
        return 0;
    return m_pageList.at(index)->widget();
}

int GLDToolBox::indexOf(QWidget *widget) const
{
    GLDToolBoxHeader *pHeader = (widget ? page(widget) : 0);
    return pHeader ? m_pageList.indexOf(pHeader) : -1;
}

void GLDToolBox::setItemEnabled(int index, bool enabled)
{
    GLDToolBoxHeader *pHeader = page(index);
    if (!pHeader)
    {
        return;
    }

    pHeader->button()->setEnabled(enabled);
    if (!enabled && pHeader == m_pCurrentPage)
    {
        int nCurIndexUp = index;
        int nCurIndexDown = nCurIndexUp;
        const int c_nCount = m_pageList.count();
        while (nCurIndexUp > 0 || nCurIndexDown < c_nCount - 1)
        {
            if (nCurIndexDown < c_nCount - 1)
            {
                if (page(++nCurIndexDown)->button()->isEnabled())
                {
                    index = nCurIndexDown;
                    break;
                }
            }
            if (nCurIndexUp > 0)
            {
                if (page(--nCurIndexUp)->button()->isEnabled())
                {
                    index = nCurIndexUp;
                    break;
                }
            }
        }
        setCurrentIndex(index);
    }
}

void GLDToolBox::setItemText(int index, const QString &text)
{
    GLDToolBoxHeader *pHeader = page(index);
    if (pHeader)
    {
        pHeader->setButtonText(text);
    }
}

void GLDToolBox::setItemIcon(int index, const QIcon &icon)
{
    GLDToolBoxHeader *pHeader = page(index);
    if (pHeader)
    {
        pHeader->setButtonIcon(icon);
    }
}

#ifndef QT_NO_TOOLTIP
void GLDToolBox::setItemToolTip(int index, const QString &toolTip)
{
    GLDToolBoxHeader *pHeader = page(index);
    if (pHeader)
    {
        pHeader->setToolTip(toolTip);
    }
}

QString GLDToolBox::itemToolTip(int index) const
{
    const GLDToolBoxHeader *pHeader = page(index);
    return (pHeader ? pHeader->toolTip() : QString());
}

#endif // QT_NO_TOOLTIP

bool GLDToolBox::isItemEnabled(int index) const
{
    const GLDToolBoxHeader *pHeader = page(index);
    return pHeader && pHeader->button()->isEnabled();
}

QString GLDToolBox::itemText(int index) const
{
    const GLDToolBoxHeader *pHeader = page(index);
    return (pHeader ? pHeader->buttonText() : QString());
}

QIcon GLDToolBox::itemIcon(int index) const
{
    const GLDToolBoxHeader *pHeader = page(index);
    return (pHeader ? pHeader->buttonIcon() : QIcon());
}

void GLDToolBox::showEvent(QShowEvent *e)
{
    inherited::showEvent(e);
}

void GLDToolBox::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::StyleChange)
    {
        updateTabs();
    }
    inherited::changeEvent(ev);
}

void GLDToolBox::itemInserted(int index)
{
    Q_UNUSED(index)
}

void GLDToolBox::itemRemoved(int index)
{
    Q_UNUSED(index)
}

bool GLDToolBox::event(QEvent *e)
{
    return inherited::event(e);
}
