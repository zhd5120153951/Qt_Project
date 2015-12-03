#include <QPainter>
#include <QStyleOptionHeader>

#include "GLDTableCornerButton.h"
#include "GLDAbstractItemModel.h"

GTableCornerbutton::GTableCornerbutton(QWidget *parent)
    : QAbstractButton(parent)
{
}

void GTableCornerbutton::paintEvent(QPaintEvent *)
{
    QStyleOptionHeader opt;
    opt.initFrom(this);
    QStyle::State state = QStyle::State_None | QStyle::State_Raised;

    if (isEnabled())
    {
        state |= QStyle::State_Enabled;
    }

    if (isActiveWindow())
    {
        state |= QStyle::State_Active;
    }

    if (isDown())
    {
        state |= QStyle::State_Sunken;
    }

    QModelIndex index = m_model->index(0, 0);

    Qt::Alignment align = static_cast<Qt::Alignment>(index.data(gidrConerTextAlignmentRole).toInt());
    opt.textAlignment = align;
    opt.text = index.data(gidrConerDataRole).toString();
    opt.state = state;
    opt.rect = rect();
    opt.position = QStyleOptionHeader::OnlyOneSection;

    QPainter painter(this);
    style()->drawControl(QStyle::CE_Header, &opt, &painter, this);
}

void GTableCornerbuttonGraphicsEffect::draw(QPainter *painter)
{
    QPoint offset;
    QPixmap pixmap = sourcePixmap(Qt::LogicalCoordinates, &offset);
//            || ((this->size().rwidth() - e->pos().y() > 1) && (this->size().rheight() - e->pos().x() > 1)))
    QPainter p(&pixmap);
    p.setBrush(Qt::NoBrush);

    QRect rt = pixmap.rect();
    int x1 = 0;
    int x2 = rt.width();

    int y = rt.height() - m_shadowHeight;
    p.setPen(m_clrStart);
    p.drawLine(QPoint(x1, y), QPoint(x2, y));

    ++y;
    p.setPen(m_clrMid);
    p.drawLine(QPoint(x1, y), QPoint(x2, y));

    ++y;
    p.setPen(m_clrEnd);
    p.drawLine(QPoint(x1, y), QPoint(x2, y));

    painter->drawPixmap(offset, pixmap);
}
QRectF GTableCornerbuttonGraphicsEffect::boundingRectFor(const QRectF &rect) const
{
    QRectF rectF = QGraphicsEffect::boundingRectFor(rect);
    rectF.setHeight(rectF.height() + m_shadowHeight);
    return rectF;
}
