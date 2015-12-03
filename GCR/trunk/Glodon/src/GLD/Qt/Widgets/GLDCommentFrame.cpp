#include "GLDWidget_Global.h"
#include "GLDMathUtils.h"
#include "GLDCommentFrame.h"
#include "GLDAbstractItemModel.h"

#define CommentFrameOffset 5
#define CommentFrameArrowInitLineLen 100

GArrowFrame::GArrowFrame(QFrame *parent) : QFrame(parent), m_topLeft(QPoint()), m_bottomRight(QPoint())
{
}

void GArrowFrame::setPosition(QPoint topLeft, QPoint bottomRight)
{
    m_topLeft = topLeft;
    m_bottomRight = bottomRight;
    this->update();
}

void GArrowFrame::setBottomRight(QPoint bottomRight)
{
    m_bottomRight = bottomRight;
    this->update();
}

QPoint GArrowFrame::bottomRight()
{
    return m_bottomRight;
}

void GArrowFrame::setTopLeft(QPoint topLeft)
{
    m_topLeft = topLeft;
    this->update();
}

QPoint GArrowFrame::topLeft()
{
    return m_topLeft;
}

void GArrowFrame::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QColor color = Qt::black;
    QPen pen(color, 1, Qt::SolidLine);
    painter.setPen(pen);
    m_topLeft = this->frameRect().bottomLeft();
    m_bottomRight = this->frameRect().topRight();
    painter.drawLine(m_topLeft, m_bottomRight);

    QPoint point[3];
    point[0] = m_topLeft;
    point[1] = m_topLeft + QPoint(3, -5);
    point[2] = m_topLeft + QPoint(6, 0);

    QBrush brush_(color);
    QBrush old = painter.brush();
    painter.setBrush(brush_);
    painter.drawConvexPolygon(point, 3);
    painter.setBrush(old);
}

GCommentFrame::GCommentFrame(QWidget *parent, QPoint pt)
    : QFrame(parent),
    m_arrow(0), m_nMarginWidth(5 * 2), m_nWidth(160), m_nHeight(80)
{
    m_parent = parent;
    m_arrow = new GArrowFrame();
    m_arrow->setFrameShape(NoFrame);
    m_arrow->setWindowFlags(Qt::ToolTip);

    // 去掉Frame的外边框
    setFrameShape(Panel);
    // 去掉最小化，关闭按钮
    setWindowFlags(Qt::ToolTip);

    //因为setWindowFlags(Qt::ToolTip)会导致父子对象间的updatesEnabled属性不同步，
    //进而引发批注框显示不稳定的问题，所以在这里强制将批注框的updatesEnabled属性置为true。
    setUpdatesEnabled(true);

    m_label = new QLabel();
    m_label->setParent(this);
    // Label内自动换行
    m_label->setWordWrap(true);
    m_label->setAlignment(Qt::AlignTop);

    // 设置背景Frame和Label的背景色
    QPalette p = palette();
    p.setColor(QPalette::Window, QColor(255, 255, 225));

    setAutoFillBackground(true);
    setPalette(p);

    m_label->setAutoFillBackground(true);
    m_label->setPalette(p);

    m_label->resize(m_nWidth - m_nMarginWidth, m_nHeight - m_nMarginWidth);
    m_label->setMinimumSize(m_nWidth - m_nMarginWidth, m_nHeight - m_nMarginWidth);
    resize(m_label->size().width() + m_nMarginWidth, m_label->size().height() + m_nMarginWidth);
    m_label->move(m_nMarginWidth / 2, m_nMarginWidth / 2);

    if ((pt.x() > 0 ) && (pt.y()))
    {
        move(pt);
    }
}

GCommentFrame::~GCommentFrame()
{
    freeAndNil(m_arrow);
    freeAndNil(m_label);
}

void GCommentFrame::hide()
{
    QFrame::hide();
    m_arrow->hide();
}

void GCommentFrame::show()
{
    QFrame::show();
    m_arrow->show();
}

void GCommentFrame::move(int x, int y)
{
    move(QPoint(x, y));
}

void GCommentFrame::move(const QPoint &pt)
{
    int nOffsetX = 15;
    int nOffsetY = -10;
    QPoint offset(nOffsetX, nOffsetY);

    QPoint oPointGlobal = m_parent->mapToGlobal(pt);
    QFrame::move(oPointGlobal + offset);

    offset.setX(0);
    m_arrow->resize(nOffsetX, abs(nOffsetY));
    m_arrow->setParent(m_parent);
    m_arrow->move(pt + offset);
}

void GCommentFrame::setCommentText(const QString &str)
{
    m_label->setText(str);
    m_label->adjustSize();
    resize(m_label->size().width() + m_nMarginWidth, m_label->size().height() + m_nMarginWidth);
}

/*GInfoFrame*/
GInfoFrame::GInfoFrame(QFrame *parent, QPoint pt) : QFrame(parent)
{
    m_label = new QLabel(this);

    // 去掉Frame的外边框
    setFrameShape(Panel);
    // 去掉最小化，关闭按钮
    setWindowFlags(Qt::ToolTip);

    // 设置背景Frame和Label的背景色
    QPalette palte = palette();
    palte.setColor(QPalette::Window, QColor(255, 255, 225));

    setAutoFillBackground(true);
    setPalette(palte);

    m_label->setAutoFillBackground(true);
    m_label->setPalette(palte);

    if ((pt.x() > 0 ) && 0 != (pt.y()))
    {
        QFrame::move(pt);
    }
}

GInfoFrame::~GInfoFrame()
{
    freeAndNil(m_label);
}

void GInfoFrame::move(const QPoint &point)
{
    if (orientation() == Qt::Vertical)
    {
        QFrame::move(point + QPoint(5, 0));
    }
    else
    {
        QFrame::move(point + QPoint(-5, 10));
    }
}

void GInfoFrame::setText(const QString &str)
{
    m_label->setText(str);
    m_label->adjustSize();
    m_label->update();
    adjustSize();
    update();
}

/* GHintFrame */

GToolTipFrame::GToolTipFrame(QFrame *parent, QPoint pt, const QString &text):
    m_parent(parent), m_label(new QLabel()), m_lastShowText(""),
    m_nMarginWidth(5 * 2)//, m_nWidth(160), m_nHeight(18)
{
    // 去掉Frame的外边框
    setFrameShape(Panel);
    // 去掉最小化，关闭按钮
    setWindowFlags(Qt::ToolTip);

    m_label->setParent(this);
    // Label内自动换行
    m_label->setWordWrap(true);
    m_label->setAlignment(Qt::AlignTop);

    // 设置背景Frame和Label的背景色
    QPalette palete = palette();
    palete.setColor(QPalette::Window, QColor(255, 255, 225));

    setAutoFillBackground(true);
    setPalette(palete);

    m_label->setAutoFillBackground(true);
    m_label->setPalette(palete);
    resize(m_label->size().width() + m_nMarginWidth, m_label->size().height() + m_nMarginWidth);
    m_label->move(m_nMarginWidth / 2, m_nMarginWidth / 2);

    if ((pt.x() > 0 ) && 0 != (pt.y()))
    {
        move(pt);
        setHintText(text);
    }
}

GToolTipFrame::~GToolTipFrame()
{
    freeAndNil(m_label);
}

void GToolTipFrame::hide()
{
    QFrame::hide();
//    m_lastShowText = "";
}

void GToolTipFrame::show()
{
    QFrame::show();
}

void GToolTipFrame::move(int x, int y)
{
    move(QPoint(x, y));
}

void GToolTipFrame::move(const QPoint &point)
{
//    QPoint parentPt = m_parent->mapFromGlobal(point);
    int nXOffset = 35;
    int nYOffset = 45;
    QPoint pOffset(nXOffset, nYOffset);
    QFrame::setParent(m_parent);
    QFrame::move(point + pOffset);
}

void GToolTipFrame::setHintText(const QString &text)
{
    if (text.isEmpty())
    {
        hide();
        return;
    }
//    if (m_lastShowText.isEmpty())
//    {
//        m_lastShowText = text;
//    }
//    else if (m_lastShowText == text)
//    {
        m_label->setText(text);
        m_label->adjustSize();
        resize(m_label->size().width() + m_nMarginWidth, m_label->size().height() + m_nMarginWidth);
        show();
//        m_lastShowText = text;
//    }
//    else
//    {
//        m_lastShowText = "";
//    }
}
