#ifndef GLDTABLECORNERBUTTON_H
#define GLDTABLECORNERBUTTON_H

#include <QAbstractButton>
#include <QGraphicsEffect>
#include "GLDWidget_Global.h"

class QAbstractItemModel;

class GLDWIDGETSHARED_EXPORT GTableCornerWidget
{
public:
    virtual void setModel(QAbstractItemModel *model)
    {
        m_model = model;
    }

protected:
    QAbstractItemModel *m_model;

};

class GLDWIDGETSHARED_EXPORT GTableCornerbutton : public QAbstractButton, public GTableCornerWidget
{
    Q_OBJECT

public:
    explicit GTableCornerbutton(QWidget *parent = 0);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);
};

class GLDWIDGETSHARED_EXPORT GTableCornerbuttonGraphicsEffect : public QGraphicsEffect
{
public:
    GTableCornerbuttonGraphicsEffect(QObject *parent)
        : QGraphicsEffect(parent)
        , m_clrStart("#A5AAB3")
        , m_clrMid("#A5AAB3")
        , m_clrEnd("#A5AAB3")
        , m_shadowHeight(3)
    {
        m_clrMid.setAlpha(40);
        m_clrEnd.setAlpha(20);
    }
    virtual void draw(QPainter *painter);

    virtual QRectF boundingRectFor(const QRectF &rect) const;

    void setStartColor(const QColor &color)
    {
        m_clrStart = color;
    }
    void setMidColor(const QColor &color)
    {
        m_clrMid = color;
    }
    void setEndColor(const QColor &color)
    {
        m_clrEnd = color;
    }

    void setShadowHeight(int height)
    {
        m_shadowHeight = height;
    }

private:
    QColor m_clrStart;
    QColor m_clrMid;
    QColor m_clrEnd;
    int    m_shadowHeight;
};

#endif // GLDTABLECORNERBUTTON_H
