#include <QDir>
#include <QRect>
#include <QFile>
#include <QFont>
#include <QPoint>
#include <QColor>
#include <QPixmap>
#include <QString>
#include <QPainter>
#include <QVariant>
#include <QMouseEvent>
#include <QStandardItem>
#include <QStandardItemModel>

#include "GLDFileUtils.h"
#include "GLDTreeViewEx.h"
#include "GLDScrollStyle.h"

const QString c_sTreeViewQssFile = ":/qsses/GlodonTreeViewEx.qss";

const int c_itemIndentation  = 17;
const QString c_addIconPath = ":/icons/GlodonTreeView-add.png";
const QString c_subIconPath = ":/icons/GlodonTreeView-sub.png";
const QString c_bottomIconPath = ":/icons/GlodonTreeView-bottom.png";
const QString c_hoverIconPath = ":/icons/GlodonTreeView-hover.png";
const QFont c_rootFont = QFont("Simsun", 12, QFont::Bold);
const QFont c_otherFont = QFont("Simsun", 12);

GlodonTreeViewEx::GlodonTreeViewEx(QWidget *parent)
    : GlodonTreeView(parent),
      m_defaultDelegate(NULL)
{
    init();
}

void GlodonTreeViewEx::init()
{
    setIndentation(c_itemIndentation);
    setExpandsOnDoubleClick(true);
    setMouseTracking(true);

    this->setStyleSheet(loadQssFile(c_sTreeViewQssFile));
    setStyle(new GLDScrollStyle(this));
    connect(this, SIGNAL(expanded(QModelIndex)),
            this, SLOT(doAfterExpand(QModelIndex)));

    m_defaultDelegate = new GlodonTreeDelegateEx(this);
    setItemDelegate(m_defaultDelegate);
}

void GlodonTreeViewEx::doAfterExpand(const QModelIndex &index)
{
    m_expandedIndex = index;
}

void GlodonTreeViewEx::setModel(QAbstractItemModel *model)
{
    GlodonTreeView::setModel(model);
}

QModelIndex GlodonTreeViewEx::topParentIndex(const QModelIndex &index)
{
    QModelIndex modelIndex = index;
    if (!modelIndex.isValid())
    {
        return QModelIndex();
    }

    while(modelIndex.parent().isValid())
    {
        modelIndex = modelIndex.parent();
    }
    return modelIndex;
}

int GlodonTreeViewEx::currentIndexlLevel(const QModelIndex &index)
{
    QModelIndex modelIndex = index;
    if (!modelIndex.isValid())
    {
        return -1;
    }

    int level = 0;
    for ( ; modelIndex.parent().isValid(); ++level )
    {
        modelIndex = modelIndex.parent();
    }
    return level;
}

QModelIndex GlodonTreeViewEx::expandedIndex()
{
    return m_expandedIndex;
}

GlodonTreeDelegateEx::GlodonTreeDelegateEx(GlodonTreeViewEx *parent)
    : QStyledItemDelegate(parent)
    , m_pTreeView(parent)
    , m_rootFont(c_rootFont)
    , m_otherFont(c_otherFont)
{
}

void GlodonTreeDelegateEx::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem optionItem = option;
    QRect rect = optionItem.rect;

    if (m_pTreeView->currentIndexlLevel(index) == 0)
    {
        // 第一层缩进为5像素
        rect.adjust(5, 0, 0, 0);
    }
    // 右侧距离边线或者滚动条8像素
    rect.adjust(0, 0, -8, 0);
    optionItem.rect = rect;
    QPixmap image = decorationIcon(index);
    int nPosx = rect.x() - 14;
    int nPosy = rect.y() + rect.height() / 2 - image.height() / 2;
    painter->drawPixmap(nPosx, nPosy, image.width(), image.height(), image);

    QVariant value = fontSize(index);
    if (value.isValid() && !value.isNull())
    {
        optionItem.font = qvariant_cast<QFont>(value).resolve(optionItem.font);
        optionItem.fontMetrics = QFontMetrics(optionItem.font);
    }

    QStyledItemDelegate::paint(painter, optionItem, index);
}

QFont GlodonTreeDelegateEx::fontSize(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QFont();
    }

    QModelIndex currentIndex = m_pTreeView->expandedIndex();
    if (!currentIndex.isValid() && m_pTreeView->topParentIndex(currentIndex)  == index)
    {
        return m_rootFont;
    }
    return m_otherFont;
}

QPixmap GlodonTreeDelegateEx::decorationIcon(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QPixmap();
    }

    QModelIndex currentIndex = m_pTreeView->expandedIndex();

    if (index.model()->hasChildren(index))
    {
        if (m_pTreeView->isExpanded(index))
        {
            return QPixmap(c_subIconPath);
        }
        else
        {
            return QPixmap(c_addIconPath);
        }
    }
    else
    {
        if (currentIndex.isValid() && index.parent() == currentIndex)
        {
            return QPixmap(c_hoverIconPath);
        }
        else
        {
            return QPixmap(c_bottomIconPath);
        }
    }
    return QPixmap();
}
