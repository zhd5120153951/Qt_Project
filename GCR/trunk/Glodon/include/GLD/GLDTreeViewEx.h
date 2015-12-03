#ifndef GLDTREEVIEWEX_H
#define GLDTREEVIEWEX_H

#include <QVector>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QColor>
#include <QFont>
#include <QStyledItemDelegate>
#include <QPixmap>

#include "GLDTreeView.h"

class GlodonTreeViewEx;
class GLDTABLEVIEWSHARED_EXPORT GlodonTreeDelegateEx : public QStyledItemDelegate
{
    Q_OBJECT
public:
    GlodonTreeDelegateEx(GlodonTreeViewEx *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    inline void setRootFont(const QFont &font) { m_rootFont = font; }
    inline void setOtherFont(const QFont &font) { m_otherFont = font; }
    inline QFont rootFont() { return m_rootFont; }
    inline QFont otherFont() { return m_otherFont; }

protected:
    virtual QFont fontSize(const QModelIndex &index) const;
    virtual QPixmap decorationIcon(const QModelIndex &index) const;

private:
    GlodonTreeViewEx *m_pTreeView;
    QFont m_rootFont;
    QFont m_otherFont;

};

class GLDTABLEVIEWSHARED_EXPORT GlodonTreeViewEx : public GlodonTreeView
{
    Q_OBJECT
public:
    GlodonTreeViewEx(QWidget *parent = 0);

    void setModel(QAbstractItemModel *model);
    QModelIndex topParentIndex(const QModelIndex &index);
    int currentIndexlLevel(const QModelIndex &index);
    QModelIndex expandedIndex();

    inline void setRootFont(const QFont &font) { m_defaultDelegate->setRootFont(font); }
    inline void setOtherFont(const QFont &font) { m_defaultDelegate->setOtherFont(font); }
    inline QFont rootFont() { return m_defaultDelegate->rootFont(); }
    inline QFont otherFont() { return m_defaultDelegate->otherFont(); }

private:
    void init();

private slots:
    void doAfterExpand(const QModelIndex &index);

private:
    QModelIndex m_expandedIndex;
    GlodonTreeDelegateEx *m_defaultDelegate;
};


#endif // GLDTREEVIEWEX_H
