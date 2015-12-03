#include "GLDScrollStyle.h"
#include "GLDShellTreeViewEx.h"

#include <QBrush>
#include <QFile>
#include <QPalette>
#include <QPoint>
#include <QRect>
#include <QString>

#include "GLDFileUtils.h"

const int c_itemIndentation  = 15;
//const int c_arroyOffset = 7;
const int c_arroyToIcon = 8;
//const int c_iconOffset = 12;
const QColor c_currentColor = QColor("#010101");
const QColor c_otherColor = QColor("#626262");
const QColor c_backgroundColor = QColor("#dbf3fc");
const QString c_collapseIconPath = "://icons/right.png";
const QString c_collapseHoverIconPath = "://icons/r-hover.png";
const QString c_expandIconPath = "://icons/hs.png";

const GString c_sShellTreeViewQssFile = ":/qsses/GLDShellTreeViewEx.qss";

GLDShellTreeViewEx::GLDShellTreeViewEx(QWidget *parent)
    : GLDShellTreeView(parent)
{
    init();
}

GLDShellTreeViewEx::GLDShellTreeViewEx(bool popup, GLDShellComboBox *combo):
    GLDShellTreeView(popup, combo)
{
}

GLDShellTreeViewEx::~GLDShellTreeViewEx()
{
}

QModelIndex GLDShellTreeViewEx::point2ModelIndex(const QPoint &pos)
{
    for (QMap<QModelIndex, QRect>::iterator it = m_map_arrowRect.begin();
         it != m_map_arrowRect.end(); ++it)
    {
        if ((*it).contains(pos))
        {
            return it.key();
        }
    }

    return QModelIndex();
}

void GLDShellTreeViewEx::setModelIndexRect(const QModelIndex &index, const QRect &rect)
{
    m_map_arrowRect[index] = rect;
}

int GLDShellTreeViewEx::countAllShownRows()
{
    return countAllShownRows(rootIndex());
}

void GLDShellTreeViewEx::init()
{
    setIndentation(c_itemIndentation);
    setHeaderHidden(true);

    this->setStyleSheet(loadQssFile(c_sShellTreeViewQssFile));
    setStyle(new GLDScrollStyle(this));
    connect(this, SIGNAL(expanded(QModelIndex)), this,
            SLOT(doAfterExpand(QModelIndex)));
}

QModelIndex GLDShellTreeViewEx::expandedIndex() const
{
    return m_expandedIndex;
}

void GLDShellTreeViewEx::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->pos();
    QModelIndex index = point2ModelIndex(point);

    if (visualRect(index).isValid())//可见
    {
        setHoverIndex(index);
    }
    else
    {
        setHoverIndex(QModelIndex());
    }

    GLDShellTreeView::mouseMoveEvent(event);
}

void GLDShellTreeViewEx::doAfterExpand(const QModelIndex &index)
{
    m_expandedIndex = index;
}

int GLDShellTreeViewEx::countAllShownRows(const QModelIndex &root)
{
    int nRows = model()->rowCount(root);

    for (int i = 0; i < nRows; ++i)
    {
        QModelIndex child = model()->index(i, 0, root);

        if (isExpanded(child))
        {
            nRows += countAllShownRows(child);
        }
    }

    return nRows;
}

void GLDShellTreeViewEx::setModel(QAbstractItemModel *model)
{
    GLDShellTreeView::setModel(model);
    setItemDelegate(new GLDShellTreeDelegate(this));
}

GLDShellTreeDelegate::GLDShellTreeDelegate(GLDShellTreeViewEx *parent)
    : QStyledItemDelegate(parent)
    , m_pTreeView(parent)
{
}

void GLDShellTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem optionItem = option;

    optionItem.rect.adjust(0, 0, -8, 0); //右侧距离边线或者滚动条8像素
    QRect rect = option.rect;
    QPixmap image = decorationIcon(index);
    int nPosx = rect.x()  - c_arroyToIcon - 4;//控制左侧图标X坐标
    int nPosy = rect.y() + rect.height() / 2 - image.height() / 2;//控制左侧图标Y坐标
    painter->drawPixmap(nPosx, nPosy, image.width(), image.height(), image);

    //设置颜色
    QPalette palette;
    QColor color = fontColor(index);
    palette.setColor(QPalette::Text, color);
    optionItem.palette = palette;

    QStyledItemDelegate::paint(painter, optionItem, index);

}

QColor GLDShellTreeDelegate::fontColor(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QColor();
    }

    QModelIndex expandIndex = m_pTreeView->expandedIndex();
    if (!expandIndex.isValid() || index.parent() == expandIndex)
    {
        return c_currentColor;
    }
    return c_otherColor;
}

QPixmap GLDShellTreeDelegate::decorationIcon(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QPixmap();
    }
    QModelIndex expandIndex = m_pTreeView->expandedIndex();
    if (index.model()->hasChildren(index))
    {
        if (expandIndex.isValid() && index.parent() == expandIndex)
        {
            return QPixmap(c_collapseHoverIconPath);
        }
        else if (!m_pTreeView->isExpanded(index))
        {
            return QPixmap(c_collapseIconPath);
        }
        else
        {
            return QPixmap(c_expandIconPath);
        }
    }
    return QPixmap();
}

