#ifndef GLDFILETREEVIEW_H
#define GLDFILETREEVIEW_H

#include <QVector>
#include <QModelIndex>
#include <QColor>
#include <QMouseEvent>
#include <QStyledItemDelegate>
#include <QPixmap>
#include "GLDShellComboBoxEx.h"
#include "GLDShellWidgets.h"
#include "GLDWidget_Global.h"


class GLDWIDGETSHARED_EXPORT GLDShellTreeViewEx : public GLDShellTreeView
{
    Q_OBJECT
public:
    explicit GLDShellTreeViewEx(QWidget *parent = 0);
    explicit GLDShellTreeViewEx(bool popup, GLDShellComboBox *combo);
    ~GLDShellTreeViewEx();

public:
    //GLDShellComBoBox π”√
    QModelIndex point2ModelIndex(const QPoint &pos);
    void setModelIndexRect(const QModelIndex &index, const QRect &rect);
    int countAllShownRows();
    inline QModelIndex hoverIndex(){ return m_indexHover; }
    inline void setHoverIndex(const QModelIndex &index){ m_indexHover = index; }

    void setModel(QAbstractItemModel *model);
    QModelIndex expandedIndex()const;

protected:
    void mouseMoveEvent(QMouseEvent *event);

private slots:
   void doAfterExpand(const QModelIndex &index);
   void init();

private:
    int countAllShownRows(const QModelIndex &root);

private:
    QMap<QModelIndex, QRect> m_map_arrowRect;
    QModelIndex m_indexHover;
    QModelIndex m_expandedIndex;

    friend class GLDShellComboBoxItemDelegate;
    friend class GLDShellComboBoxEx;
};

class GLDWIDGETSHARED_EXPORT GLDShellTreeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    GLDShellTreeDelegate(GLDShellTreeViewEx *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
protected:
   virtual QColor fontColor(const QModelIndex &index) const;
   virtual QPixmap decorationIcon(const QModelIndex &index) const;

private:
    GLDShellTreeViewEx *m_pTreeView;

};

#endif // GLDFILETREEVIEW_H


