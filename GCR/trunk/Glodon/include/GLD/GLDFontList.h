#ifndef GLDFONTLIST_H
#define GLDFONTLIST_H

#include <QFontComboBox>
#include "GLDWidget_Global.h"

#define ITEM_WIDTH 70
#define ITEM_HEIGHT 24
class GlodonFontFamilyDelegate;
class GLDWIDGETSHARED_EXPORT GFontList : public QFontComboBox
{
public:
    explicit GFontList(QWidget *parent);
    ~GFontList();
public:
    virtual void showPopup();
protected:
    int m_bFirstPopup;
};

class GLDWIDGETSHARED_EXPORT GlodonFontFamilyDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    explicit GlodonFontFamilyDelegate(QObject *parent);

    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

private:
    QIcon truetype;
    QIcon bitmap;
    QFontDatabase::WritingSystem writingSystem;
};

#endif // GLDFONTLIST_H
