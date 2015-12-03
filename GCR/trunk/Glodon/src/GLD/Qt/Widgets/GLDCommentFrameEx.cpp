#include <QBitmap>
#include "GLDTextEdit.h"
#include "GLDAbstractItemView.h"
#include "GLDAbstractItemModel.h"
#include "GLDCommentFrameEx.h"

static const int c_ncSpace                                      = 20;
static const int c_nMinWidth                                    = 178;
static const int c_nMinHeight                                   = 95;
static const int c_nMinAnchorWidth                              = 4;
static const QMargins c_margEdit                                = QMargins(7, 2, 13, 7);
static const QPoint c_ptDefaultAnchor                           = QPoint(0, 60);

GCustomCommentFrameEx::GCustomCommentFrameEx(QWidget *parent, QPoint pt) :
    QWidget(parent)
    , m_dragPoint(0, 0)
    , m_initWinSize(c_nMinWidth + (4 * c_ncSpace), c_nMinHeight + (2 * c_ncSpace))
    , m_anchor(c_ptDefaultAnchor)
    , m_ptAnchorFirst(0, 0)
    , m_ptAnchorSecond(0, 0)
    , m_rcPos(m_anchor.x() + c_ncSpace, c_ncSpace, c_nMinWidth, c_nMinHeight)
    , m_mouseCurrentPos(GCustomCommentFrameEx::EMousePosNULL)
    , m_bShowStretch(false)
    , m_clBg(255, 255, 225)
    , m_clText(0, 0, 0)
    , m_clBorder(130, 130, 127)
    , m_mapbl(":/icons/bottom-left.png")
    , m_mapbr(":/icons/bottom-right.png")
    , m_mapbc(":/icons/bottom-conter.png")
    , m_maprt(":/icons/right-top.png")
    , m_maprc(":/icons/right-conter.png")
    , m_bWndState(false)
    , m_bIsMenuShow(false)
{
    setUpUI();
    initPlaintEdit();

    QPoint ptmove = QPoint(pt.x(), pt.y() - m_anchor.y());

    if (!setPosByParent(QRect(ptmove.x(), ptmove.y(), m_initWinSize.width(), m_initWinSize.height())))
    {
        resize(m_initWinSize);
        QWidget::move(ptmove);
        setWindowMask(true);
    }

    setMouseTracking(true);
}

GCustomCommentFrameEx::~GCustomCommentFrameEx()
{
}

void GCustomCommentFrameEx::setUpUI()
{
    if (m_rcPos.top() < 0)
    {
        m_rcPos.moveTo(m_rcPos.x(), c_ncSpace);
    }

    setAttribute(Qt::WA_TranslucentBackground);
    updatePaintPolygonDate(m_anchor, m_rcPos);
}

QRect GCustomCommentFrameEx::getMaxRect(const QPoint &pt, const QRect &rc) const
{
    QRect rcResult(rc);

    if (rcResult.left() > pt.x())
    {
        rcResult.setLeft(pt.x());
    }

    if (rcResult.right() < pt.x())
    {
        rcResult.setRight(pt.x());
    }

    if (rcResult.top() > pt.y())
    {
        rcResult.setTop(pt.y());
    }

    if (rcResult.bottom() < pt.y())
    {
        rcResult.setBottom(pt.y());
    }

    return rcResult;
}

void GCustomCommentFrameEx::updateWindow()
{
    QRect rcPos(m_rcPos.left() - c_ncSpace
                , m_rcPos.top() - c_ncSpace
                , m_rcPos.width() + (2 * c_ncSpace)
                , m_rcPos.height() + (2 * c_ncSpace));
    rcPos = getMaxRect(m_anchor, rcPos);

    m_anchor -= rcPos.topLeft();
    m_rcPos.moveTo(m_rcPos.left() - rcPos.left(), m_rcPos.top() - rcPos.top());
    updatePaintPolygonDate(m_anchor, m_rcPos);

    if ((rcPos.topLeft().x() == 0)
            && (rcPos.topLeft().y() == 0)
            && (rcPos.width() == width())
            && (rcPos.height() == height()))
    {
        updateEditPos();
        update();
    }
    else
    {
        QWidget::move(pos() + rcPos.topLeft());
        resize(rcPos.width(), rcPos.height());
    }
}

void GCustomCommentFrameEx::drawImageOnWin(QPainter *painter)
{
    painter->drawRect(m_rcPos.left()
                      , m_rcPos.top()
                      , m_rcPos.width() + 10
                      , m_rcPos.height() + 10);

    if (!m_rcPos.contains(m_anchor))
    {
        QPolygon polygonLine(m_verFreamPoint);
        painter->drawPolygon(polygonLine);
    }

    drawFreamRect(painter, false);
}

void GCustomCommentFrameEx::drawFreamRect(QPainter *painter, bool bPainterEvent)
{
    const int c_smallRectWidth = 7;
    const int c_lineLeng = 4;
    const int c_nOff = c_smallRectWidth / 2;

    QPoint ptLT(m_rcPos.left() - c_ncSpace, m_rcPos.top() - c_ncSpace);
    QPoint ptRB(m_rcPos.right() + c_ncSpace, m_rcPos.bottom() + c_ncSpace);

    if (!bPainterEvent)
    {
        painter->drawRect(m_rcPos.left() - c_ncSpace
                          , m_rcPos.top() - c_ncSpace
                          , m_rcPos.width() + (2 * c_ncSpace)
                          , m_rcPos.height() + (2 * c_ncSpace));
    }
    else
    {
        m_rectLT.setRect(ptLT.rx()
                         , ptLT.ry()
                         , c_smallRectWidth
                         , c_smallRectWidth);
        m_rectRT.setRect(ptRB.rx() - c_smallRectWidth
                         , ptLT.ry()
                         , c_smallRectWidth
                         , c_smallRectWidth);
        m_rectMT.setRect(ptLT.rx() + (((ptRB.rx() - ptLT.rx()) / 2) - ((c_smallRectWidth + 1) / 2))
                         , ptLT.ry()
                         , c_smallRectWidth
                         , c_smallRectWidth);
        m_rectMB.setRect(ptLT.rx() + (((ptRB.rx() - ptLT.rx()) / 2) - ((c_smallRectWidth + 1) / 2))
                         , ptRB.ry() - c_smallRectWidth
                         , c_smallRectWidth
                         , c_smallRectWidth);
        m_rectRB.setRect(ptRB.rx() - c_smallRectWidth
                         , ptRB.ry() - c_smallRectWidth
                         , c_smallRectWidth
                         , c_smallRectWidth);
        m_rectLB.setRect(ptLT.rx()
                         , ptRB.ry() - c_smallRectWidth
                         , c_smallRectWidth
                         , c_smallRectWidth);
        m_rectML.setRect(ptLT.rx()
                         , ptLT.ry() + (((ptRB.ry() - ptLT.ry()) / 2) - ((c_smallRectWidth + 1) / 2))
                         , c_smallRectWidth
                         , c_smallRectWidth);
        m_rectMR.setRect(ptRB.rx() - c_smallRectWidth
                         , ptLT.ry() + (((ptRB.ry() - ptLT.ry()) / 2) - ((c_smallRectWidth + 1) / 2))
                         , c_smallRectWidth
                         , c_smallRectWidth);

        painter->drawRect(m_rectLT);
        painter->drawRect(m_rectRT);
        painter->drawRect(m_rectMT);
        painter->drawRect(m_rectMB);
        painter->drawRect(m_rectRB);
        painter->drawRect(m_rectLB);
        painter->drawRect(m_rectML);
        painter->drawRect(m_rectMR);

        int n_index = 0;

        for (int i = ptLT.rx() + c_smallRectWidth + c_lineLeng; i < m_rectMT.left() - c_lineLeng; i += c_lineLeng)
        {
            ++n_index;

            if (0 == n_index % 2)
            {
                painter->drawLine(QPoint(i, ptLT.ry() + c_nOff), QPoint(i + c_lineLeng - 1, ptLT.ry() + c_nOff));
                painter->drawLine(QPoint(i, ptRB.ry() - c_nOff), QPoint(i + c_lineLeng - 1, ptRB.ry() - c_nOff));
            }
        }

        n_index = 0;

        for (int i = m_rectMT.left() + c_smallRectWidth + c_lineLeng; i < m_rectRB.left() - c_lineLeng; i += c_lineLeng)
        {
            ++n_index;

            if (0 == n_index % 2)
            {
                painter->drawLine(QPoint(i, ptLT.ry() + c_nOff), QPoint(i + c_lineLeng - 1, ptLT.ry() + c_nOff));
                painter->drawLine(QPoint(i, ptRB.ry() - c_nOff), QPoint(i + c_lineLeng - 1, ptRB.ry() - c_nOff));
            }
        }

        n_index = 0;

        for (int i = ptLT.ry() + c_smallRectWidth + c_lineLeng; i < m_rectML.top() - c_lineLeng; i += c_lineLeng)
        {
            ++n_index;

            if (0 == n_index % 2)
            {
                painter->drawLine(QPoint(ptLT.rx() + c_nOff, i), QPoint(ptLT.rx() + c_nOff, i + c_lineLeng - 1));
                painter->drawLine(QPoint(ptRB.rx() - c_nOff, i), QPoint(ptRB.rx() - c_nOff, i + c_lineLeng - 1));
            }
        }

        n_index = 0;

        for (int i = m_rectML.top() + c_smallRectWidth + c_lineLeng; i < m_rectRB.top() - c_lineLeng; i += c_lineLeng)
        {
            ++n_index;

            if (0 == n_index % 2)
            {
                painter->drawLine(QPoint(ptLT.rx() + c_nOff, i), QPoint(ptLT.rx() + c_nOff, i + c_lineLeng - 1));
                painter->drawLine(QPoint(ptRB.rx() - c_nOff, i), QPoint(ptRB.rx() - c_nOff, i + c_lineLeng - 1));
            }
        }
    }
}

void GCustomCommentFrameEx::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painterMem(this);

    painterMem.drawPixmap(m_rcPos.left(), m_rcPos.bottom() + 1, m_mapbl);
    painterMem.drawPixmap(m_rcPos.right() + 2 - 11, m_rcPos.bottom() + 2 - 11, m_mapbr);
    painterMem.drawPixmap(m_rcPos.left() + m_mapbl.width()
                          , m_rcPos.bottom() + 1
                          , m_rcPos.width() - m_mapbl.width() - 10
                          , m_mapbc.height()
                          , m_mapbc);
    painterMem.drawPixmap(m_rcPos.right() + 1, m_rcPos.top(), m_maprt);
    painterMem.drawPixmap(m_rcPos.right() + 1
                          , m_rcPos.top() + m_maprt.height()
                          , m_maprc.width()
                          , m_rcPos.height() - m_maprt.height() - 10
                          , m_maprc);

    painterMem.setPen(QPen(m_clBorder));
    painterMem.setBrush(QBrush(GColor(255, 255, 225)));

    if (m_rcPos.contains(m_anchor))
    {
        painterMem.drawRect(m_rcPos.left()
                            , m_rcPos.top()
                            , m_rcPos.width() - 1
                            , m_rcPos.height() - 1);// 此处的1像素是因为通过m_verFreamPoint画出的矩形，
                                                    // 与直接通过m_rcPos画出来的矩形差一个像素需要调整一下
    }
    else
    {
        QPolygon polygonLine(m_verFreamPoint);
        painterMem.drawPolygon(polygonLine);
        painterMem.setPen(QPen(m_clBorder));
        painterMem.setRenderHint(QPainter::Antialiasing, true);
        painterMem.drawLine(m_anchor, m_ptAnchorFirst);
        painterMem.drawLine(m_anchor, m_ptAnchorSecond);
        painterMem.setRenderHint(QPainter::Antialiasing, false);
    }

    if (m_bShowStretch)
    {
        painterMem.setPen(QPen(GColor(0, 0, 0)));
        painterMem.setBrush(QBrush(GColor(255, 255, 255)));
        drawFreamRect(&painterMem);
    }
}

void GCustomCommentFrameEx::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    updateEditPos();
}

void GCustomCommentFrameEx::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);

    if (e->button() == Qt::LeftButton)
    {
        if (!m_bWndState)
        {
            setWindowMask(false);
            update();
            m_bWndState = true;
        }

        m_dragPoint = e->globalPos();
        e->accept();

        EMousePos posmouse = getMousePos(e->pos());

        if (posmouse == EMousePosEdit)
        {
            m_plainTextEdit->activateWindow();
        }
    }
}

void GCustomCommentFrameEx::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);

    if (m_bWndState)
    {
        setWindowMask(true);
        update();
        m_bWndState = false;
    }
}

void GCustomCommentFrameEx::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() != Qt::LeftButton)
    {
        m_mouseCurrentPos = getMousePos(e->pos());
    }

    processMousePos(e, m_mouseCurrentPos);

    QWidget::mouseMoveEvent(e);
}

void GCustomCommentFrameEx::enterEvent(QEvent *e)
{
    QWidget::enterEvent(e);
    m_bShowStretch = true;
    update();
}

void GCustomCommentFrameEx::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);
    m_bShowStretch = false;
    update();
}

bool GCustomCommentFrameEx::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_plainTextEdit->viewport())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *pmouseEvent = (QMouseEvent *)event;

            if (pmouseEvent->button() == Qt::RightButton)
            {
                m_bIsMenuShow = true;
            }
        }
    }

    if (watched == m_plainTextEdit)
    {
        if (QEvent::FocusOut == event->type())
        {
            if (!m_bIsMenuShow)
            {
                hide();
            }

            m_bIsMenuShow = false;
        }
    }

    return false ;
}

GCustomCommentFrameEx::EAnchorPos GCustomCommentFrameEx::getAnchorPos(const QPoint &pt, const QRect &rc) const
{
    if (pt.x() <= rc.left())
    {
        if ((rc.left() - pt.x()) >= (rc.top() - pt.y()) && (pt.y() <= rc.top() + (rc.height() / 2)))
        {
            return ELeftUp;
        }
        else if ((pt.y() > rc.top() + (rc.height() / 2)) && (rc.left() - pt.x()) > (pt.y() - rc.bottom()))
        {
            return ELeftDown;
        }
    }

    if (pt.y() <= rc.top())
    {
        if ((rc.left() - pt.x()) < (rc.top() - pt.y()) && pt.x() <= rc.left() + (rc.width() / 2))
        {
            return ETopLeft;
        }
        else if (pt.x() > rc.left() + (rc.width() / 2) && (pt.x() - rc.right()) <= (rc.top() - pt.y()))
        {
            return ETopRight;
        }
    }

    if (pt.x() >= rc.right())
    {
        if ((pt.x() - rc.right()) > (rc.top() - pt.y()) && (pt.y() <= rc.top() + (rc.height() / 2)))
        {
            return ERightUp;
        }
        else if ((pt.y() > rc.top() + (rc.height() / 2)) && (pt.x() - rc.right()) >= (pt.y() - rc.bottom()))
        {
            return ERightDown;
        }
    }

    if (pt.y() >= rc.bottom())
    {
        if ((rc.left() - pt.x()) <= (pt.y() - rc.bottom()) && pt.x() <= rc.left() + (rc.width() / 2))
        {
            return EBottomLeft;
        }
        else if (pt.x() > rc.left() + (rc.width() / 2) && (pt.x() - rc.right()) < (pt.y() - rc.bottom()))
        {
            return EBottomRight;
        }
    }

    return ECenter;
}

GCustomCommentFrameEx::EAnchorPos GCustomCommentFrameEx::getTriangleByAnchorAndRect(const QPoint &ptAnchor
        , const QRect &rcPos
        , QPoint &ptFirst
        , QPoint &ptSecond) const
{
    EAnchorPos pos = getAnchorPos(ptAnchor, rcPos);
    const int c_nSpaceSize = 10;
    const int c_nWidth = 20;

    switch (pos)
    {
        case ELeftUp:
        {
            ptFirst.setX(rcPos.left());
            ptFirst.setY(rcPos.top() + c_nSpaceSize);
            ptSecond.setX(rcPos.left());
            ptSecond.setY(rcPos.top() + (c_nSpaceSize + c_nWidth));
            break;
        }

        case ELeftDown:
        {
            ptFirst.setX(rcPos.left());
            ptFirst.setY(rcPos.bottom() - (c_nSpaceSize + c_nWidth));
            ptSecond.setX(rcPos.left());
            ptSecond.setY(rcPos.bottom() - c_nSpaceSize);
            break;
        }

        case ETopLeft:
        {
            ptFirst.setX(rcPos.left() + c_nSpaceSize);
            ptFirst.setY(rcPos.top());
            ptSecond.setX(rcPos.left() + (c_nSpaceSize + c_nWidth));
            ptSecond.setY(rcPos.top());
            break;
        }

        case ETopRight:
        {
            ptFirst.setX(rcPos.right() - (c_nSpaceSize + c_nWidth));
            ptFirst.setY(rcPos.top());
            ptSecond.setX(rcPos.right() - c_nSpaceSize);
            ptSecond.setY(rcPos.top());
            break;
        }

        case ERightUp:
        {
            ptFirst.setX(rcPos.right());
            ptFirst.setY(rcPos.top() + c_nSpaceSize);
            ptSecond.setX(rcPos.right());
            ptSecond.setY(rcPos.top() + (c_nSpaceSize + c_nWidth));
            break;
        }

        case ERightDown:
        {
            ptFirst.setX(rcPos.right());
            ptFirst.setY(rcPos.bottom() - (c_nSpaceSize + c_nWidth));
            ptSecond.setX(rcPos.right());
            ptSecond.setY(rcPos.bottom() - c_nSpaceSize);
            break;
        }

        case EBottomLeft:
        {
            ptFirst.setX(rcPos.left() + c_nSpaceSize);
            ptFirst.setY(rcPos.bottom());
            ptSecond.setX(rcPos.left() + (c_nSpaceSize + c_nWidth));
            ptSecond.setY(rcPos.bottom());
            break;
        }

        case EBottomRight:
        {
            ptFirst.setX(rcPos.right() - (c_nSpaceSize + c_nWidth));
            ptFirst.setY(rcPos.bottom());
            ptSecond.setX(rcPos.right() - c_nSpaceSize);
            ptSecond.setY(rcPos.bottom());
            break;
        }

        default:
        {
            ptFirst.setX(0);
            ptFirst.setY(0);
            ptSecond.setX(0);
            ptSecond.setY(0);
            break;
        }
    }

    return pos;
}

void GCustomCommentFrameEx::updatePaintPolygonDate(const QPoint &ptAnchor, const QRect &rcPos)
{
    EAnchorPos pos = getTriangleByAnchorAndRect(ptAnchor, rcPos, m_ptAnchorFirst, m_ptAnchorSecond);
    m_verFreamPoint.clear();

    switch (pos)
    {
        case ELeftUp:
        case ELeftDown:
        {
            m_verFreamPoint.append(ptAnchor);
            m_verFreamPoint.append(m_ptAnchorFirst);
            m_verFreamPoint.append(rcPos.topLeft());
            m_verFreamPoint.append(rcPos.topRight());
            m_verFreamPoint.append(rcPos.bottomRight());
            m_verFreamPoint.append(rcPos.bottomLeft());
            m_verFreamPoint.append(m_ptAnchorSecond);
            break;
        }

        case ETopLeft:
        case ETopRight:
        {
            m_verFreamPoint.append(ptAnchor);
            m_verFreamPoint.append(m_ptAnchorSecond);
            m_verFreamPoint.append(rcPos.topRight());
            m_verFreamPoint.append(rcPos.bottomRight());
            m_verFreamPoint.append(rcPos.bottomLeft());
            m_verFreamPoint.append(rcPos.topLeft());
            m_verFreamPoint.append(m_ptAnchorFirst);
            break;
        }

        case ERightUp:
        case ERightDown:
        {
            m_verFreamPoint.append(ptAnchor);
            m_verFreamPoint.append(m_ptAnchorSecond);
            m_verFreamPoint.append(rcPos.bottomRight());
            m_verFreamPoint.append(rcPos.bottomLeft());
            m_verFreamPoint.append(rcPos.topLeft());
            m_verFreamPoint.append(rcPos.topRight());
            m_verFreamPoint.append(m_ptAnchorFirst);
            break;
        }

        case EBottomLeft:
        case EBottomRight:
        {
            m_verFreamPoint.append(ptAnchor);
            m_verFreamPoint.append(m_ptAnchorFirst);
            m_verFreamPoint.append(rcPos.bottomLeft());
            m_verFreamPoint.append(rcPos.topLeft());
            m_verFreamPoint.append(rcPos.topRight());
            m_verFreamPoint.append(rcPos.bottomRight());
            m_verFreamPoint.append(m_ptAnchorSecond);
            break;
        }

        default:
        {
            m_ptAnchorFirst.setX(0);
            m_ptAnchorFirst.setY(0);
            m_ptAnchorSecond.setX(0);
            m_ptAnchorSecond.setY(0);
            break;
        }
    }
}

GCustomCommentFrameEx::EMousePos GCustomCommentFrameEx::getMousePos(const QPoint &pos) const
{
    if (m_rcPos.contains(pos))
    {
        return EMousePosEdit;
    }
    else if (m_rectLT.contains(pos))
    {
        return EStretchLeftUp;
    }
    else if (m_rectRT.contains(pos))
    {
        return EStretchRightUp;
    }
    else if (m_rectMT.contains(pos))
    {
        return EStretchUp;
    }
    else if (m_rectMB.contains(pos))
    {
        return EStretchDown;
    }
    else if (m_rectRB.contains(pos))
    {
        return EStretchRightDown;
    }
    else if (m_rectLB.contains(pos))
    {
        return EStretchLeftDown;
    }
    else if (m_rectML.contains(pos))
    {
        return EStretchLeft;
    }
    else if (m_rectMR.contains(pos))
    {
        return EStretchRight;
    }
    else if (QRect(m_rcPos.left() - c_ncSpace
                   , m_rcPos.top() - c_ncSpace
                   , m_rcPos.width() + (2 * c_ncSpace)
                   , m_rcPos.height() + (2 * c_ncSpace)).contains(pos))
    {
        return EMoveWindPos;
    }

    return EMousePosNULL;
}

void GCustomCommentFrameEx::processMousePos(QMouseEvent *e, GCustomCommentFrameEx::EMousePos pos)
{
    switch (pos)
    {
        case EMoveWindPos:
        {
            processMoveWindPos(e);
            break;
        }

        case EStretchLeft:
        {
            processStretchLeft(e);
            break;
        }

        case EStretchRight:
        {
            processStretchRight(e);
            break;
        }

        case EStretchUp:
        {
            processStretchUp(e);
            break;
        }

        case EStretchDown:
        {
            processStretchDown(e);
            break;
        }

        case EStretchLeftUp:
        {
            processStretchLeftUp(e);
            break;
        }

        case EStretchLeftDown:
        {
            processStretchLeftDown(e);
            break;
        }

        case EStretchRightUp:
        {
            processStretchRightUp(e);
            break;
        }

        case EStretchRightDown:
        {
            processStretchRightDown(e);
            break;
        }

        case EMousePosEdit:
        default:
        {
            setCursor(Qt::ArrowCursor);
            break;
        }
    }
}

void GCustomCommentFrameEx::processMoveWindPos(QMouseEvent *e)
{
    setCursor(Qt::SizeAllCursor);

    if (e->buttons() == Qt::LeftButton)
    {
        int nwidth = e->globalPos().rx() - m_dragPoint.rx();
        int nheight = e->globalPos().ry() - m_dragPoint.ry();
        m_dragPoint = e->globalPos();
        m_rcPos.moveTo(m_rcPos.x() + nwidth, m_rcPos.y() + nheight);
        updateWindow();
    }
}

void GCustomCommentFrameEx::processStretchLeft(QMouseEvent *e)
{
    setCursor(Qt::SizeHorCursor);

    if (e->buttons() == Qt::LeftButton)
    {
        int nwidth = e->globalPos().rx() - m_dragPoint.rx();
        int nheight = 0;
        m_dragPoint = e->globalPos();

        bool bMinsize = false;
        if (m_rcPos.width() - nwidth <= c_nMinWidth)
        {
            nwidth = m_rcPos.width() - c_nMinWidth;
            bMinsize = true;
        }

        m_rcPos.moveTo(m_rcPos.left() + nwidth, m_rcPos.top() + nheight);
        m_rcPos.setRight(m_rcPos.right() - nwidth);

        updateWindow();
        if (bMinsize == true)
        {
            m_dragPoint.setX(mapToGlobal(QPoint(m_rcPos.left() - c_ncSpace, 0)).x());
        }
    }
}

void GCustomCommentFrameEx::processStretchRight(QMouseEvent *e)
{
    setCursor(Qt::SizeHorCursor);

    if (e->buttons() == Qt::LeftButton)
    {
        int nwidth = e->globalPos().rx() - m_dragPoint.rx();
        m_dragPoint = e->globalPos();

        if (m_rcPos.width() + nwidth <= c_nMinWidth)
        {
            nwidth = c_nMinWidth - m_rcPos.width();
            m_dragPoint.setX(mapToGlobal(QPoint(m_rcPos.right() + nwidth + c_ncSpace, 0)).x());
        }

        m_rcPos.setRight(m_rcPos.right() + nwidth);
        updateWindow();
    }
}

void GCustomCommentFrameEx::processStretchUp(QMouseEvent *e)
{
    setCursor(Qt::SizeVerCursor);

    if (e->buttons() == Qt::LeftButton)
    {
        int nheight = e->globalPos().ry() - m_dragPoint.ry();
        m_dragPoint = e->globalPos();

        bool bMinsize = false;
        if (m_rcPos.height() - nheight <= c_nMinHeight)
        {
            nheight = m_rcPos.height() - c_nMinHeight;
            bMinsize = true;
        }

        m_rcPos.moveTo(m_rcPos.left(), m_rcPos.top() + nheight);
        m_rcPos.setBottom(m_rcPos.bottom() - nheight);

        updateWindow();

        if (bMinsize)
        {
            m_dragPoint.setY(mapToGlobal(QPoint(0, m_rcPos.top() - c_ncSpace)).y());
        }
    }
}

void GCustomCommentFrameEx::processStretchDown(QMouseEvent *e)
{
    setCursor(Qt::SizeVerCursor);

    if (e->buttons() == Qt::LeftButton)
    {
        int nheight = e->globalPos().ry() - m_dragPoint.ry();
        m_dragPoint = e->globalPos();

        if (m_rcPos.height() + nheight <= c_nMinHeight)
        {
            nheight = c_nMinHeight - m_rcPos.height();
            m_dragPoint.setY(mapToGlobal(QPoint(0, m_rcPos.bottom() + nheight + c_ncSpace)).y());
        }

        m_rcPos.setBottom(m_rcPos.bottom() + nheight);
        updateWindow();
    }
}

void GCustomCommentFrameEx::processStretchLeftUp(QMouseEvent *e)
{
    setCursor(Qt::SizeFDiagCursor);

    if (e->buttons() == Qt::LeftButton)
    {
        int nwidth = e->globalPos().rx() - m_dragPoint.rx();
        int nheight = e->globalPos().ry() - m_dragPoint.ry();
        m_dragPoint = e->globalPos();

        bool bminHeight = false;
        if (m_rcPos.height() - nheight <= c_nMinHeight)
        {
            nheight = m_rcPos.height() - c_nMinHeight;
            bminHeight = true;
        }

        bool bminWidth = false;
        if (m_rcPos.width() - nwidth <= c_nMinWidth)
        {
            nwidth = m_rcPos.width() - c_nMinWidth;
            bminWidth = true;
        }

        m_rcPos.moveTo(m_rcPos.left() + nwidth, m_rcPos.top() + nheight);
        m_rcPos.setRight(m_rcPos.right() - nwidth);
        m_rcPos.setBottom(m_rcPos.bottom() - nheight);
        updateWindow();
        if (bminWidth)
        {
            m_dragPoint.setX(mapToGlobal(QPoint(m_rcPos.left() - c_ncSpace, 0)).x());
        }

        if (bminHeight)
        {
            m_dragPoint.setY(mapToGlobal(QPoint(0, m_rcPos.top() - c_ncSpace)).y());
        }
    }
}

void GCustomCommentFrameEx::processStretchLeftDown(QMouseEvent *e)
{
    setCursor(Qt::SizeBDiagCursor);

    if (e->buttons() == Qt::LeftButton)
    {
        int nwidth = e->globalPos().rx() - m_dragPoint.rx();
        int nheight = e->globalPos().ry() - m_dragPoint.ry();
        m_dragPoint = e->globalPos();

        bool bminWidth = false;
        if (m_rcPos.width() - nwidth <= c_nMinWidth)
        {
            nwidth = m_rcPos.width() - c_nMinWidth;
            bminWidth = true;
        }

        bool bminHeight = false;
        if (m_rcPos.height() + nheight <= c_nMinHeight)
        {
            nheight = c_nMinHeight - m_rcPos.height();
            bminHeight = true;
        }

        m_rcPos.moveTo(m_rcPos.left() + nwidth, m_rcPos.top());
        m_rcPos.setRight(m_rcPos.right() - nwidth);
        m_rcPos.setBottom(m_rcPos.bottom() + nheight);
        updateWindow();
        if (bminWidth)
        {
            m_dragPoint.setX(mapToGlobal(QPoint(m_rcPos.left() - c_ncSpace, 0)).x());
        }

        if (bminHeight)
        {
            m_dragPoint.setY(mapToGlobal(QPoint(0, m_rcPos.bottom() + c_ncSpace)).y());
        }
    }
}

void GCustomCommentFrameEx::processStretchRightUp(QMouseEvent *e)
{
    setCursor(Qt::SizeBDiagCursor);

    if (e->buttons() == Qt::LeftButton)
    {
        int nwidth = e->globalPos().rx() - m_dragPoint.rx();
        int nheight = e->globalPos().ry() - m_dragPoint.ry();
        m_dragPoint = e->globalPos();

        bool bminHeight = false;
        if (m_rcPos.height() - nheight <= c_nMinHeight)
        {
            nheight = m_rcPos.height() - c_nMinHeight;
            bminHeight = true;
        }

        bool bminWidth = false;
        if (m_rcPos.width() + nwidth <= c_nMinWidth)
        {
            nwidth = c_nMinWidth - m_rcPos.width();
            bminWidth = true;
        }

        m_rcPos.moveTo(m_rcPos.left(), m_rcPos.top() + nheight);
        m_rcPos.setRight(m_rcPos.right() + nwidth);
        m_rcPos.setBottom(m_rcPos.bottom() - nheight);
        updateWindow();
        if (bminWidth)
        {
            m_dragPoint.setX(mapToGlobal(QPoint(m_rcPos.right() + c_ncSpace, 0)).x());
        }

        if (bminHeight)
        {
            m_dragPoint.setY(mapToGlobal(QPoint(0, m_rcPos.top() - c_ncSpace)).y());
        }
    }
}

void GCustomCommentFrameEx::processStretchRightDown(QMouseEvent *e)
{
    setCursor(Qt::SizeFDiagCursor);

    if (e->buttons() == Qt::LeftButton)
    {
        int nwidth = e->globalPos().rx() - m_dragPoint.rx();
        int nheight = e->globalPos().ry() - m_dragPoint.ry();
        m_dragPoint = e->globalPos();

        bool bminWidth = false;
        if (m_rcPos.width() + nwidth <= c_nMinWidth)
        {
            nwidth = c_nMinWidth - m_rcPos.width();
            bminWidth = true;
        }

        bool bminHeight = false;
        if (m_rcPos.height() + nheight <= c_nMinHeight)
        {
            nheight = c_nMinHeight - m_rcPos.height();
            bminHeight = true;
        }

        m_rcPos.setRight(m_rcPos.right() + nwidth);
        m_rcPos.setBottom(m_rcPos.bottom() + nheight);
        updateWindow();
        if (bminWidth)
        {
            m_dragPoint.setX(mapToGlobal(QPoint(m_rcPos.right() + c_ncSpace, 0)).x());
        }

        if (bminHeight)
        {
            m_dragPoint.setY(mapToGlobal(QPoint(0, m_rcPos.bottom() + c_ncSpace)).y());
        }
    }
}

void GCustomCommentFrameEx::setWindowMask(bool birregularWnd)
{
    QBitmap bitMap(width(), height());
    QPainter painter(&bitMap);
    painter.setPen(QPen(GColor(255, 255, 255)));
    painter.setBrush(QBrush(GColor(255, 255, 255)));
    painter.drawRect(0, 0, width(), height());
    painter.setPen(QPen(GColor(0, 0, 0)));
    painter.setBrush(QBrush(GColor(0, 0, 0)));

    if (birregularWnd)
    {
        drawImageOnWin(&painter);
    }

    setMask(bitMap);
}

void GCustomCommentFrameEx::updateEditPos()
{
    m_plainTextEdit->setGeometry(m_rcPos.left() + c_margEdit.left()
                                 , m_rcPos.top() + c_margEdit.top()
                                 , m_rcPos.width() - c_margEdit.right()
                                 , m_rcPos.height() - c_margEdit.bottom());
}

/**
 * @brief 初始化编辑框的大小
 */
void GCustomCommentFrameEx::initPlaintEdit()
{
    m_plainTextEdit = new GLDPlainTextEdit(this);
    m_plainTextEdit->setFrameShape(QFrame::Panel);
    m_plainTextEdit->setLineWidth(1);
    m_plainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_plainTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_plainTextEdit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    updateEditPos();
    // 设置背景Frame和Label的背景色
    QPalette palet = palette();
    palet.setColor(QPalette::Window, m_clBg);
    palet.setColor(QPalette::Disabled, QPalette::Text, GColor(m_clText));
    palet.setColor(QPalette::Disabled, QPalette::WindowText, GColor(m_clText));
    m_plainTextEdit->setAutoFillBackground(true);
    m_plainTextEdit->setCenterOnScroll(true);
    m_plainTextEdit->setPalette(palet);
    m_plainTextEdit->viewport()->setBackgroundRole(QPalette::Window);
    m_plainTextEdit->setStyleSheet("border-style:none;");
    m_plainTextEdit->ensureCursorVisible();
    m_plainTextEdit->setFocus();
    m_plainTextEdit->moveCursor(QTextCursor::End);
    m_plainTextEdit->setReadOnly(true);
    setFocusPolicy(Qt::WheelFocus);
    setFocusProxy(m_plainTextEdit);
    connect(m_plainTextEdit, SIGNAL(textChanged(GString)), this, SIGNAL(textChanged(GString)));
}

void GCustomCommentFrameEx::setCommentText(const GString &str)
{
    m_plainTextEdit->setPlainText(str);

    if (isReadOnly())
    {
        m_plainTextEdit->moveCursor(QTextCursor::Start);
    }
    else
    {
        m_plainTextEdit->moveCursor(QTextCursor::End);
    }
}

GString GCustomCommentFrameEx::commentText()
{
    return m_plainTextEdit->toPlainText();
}

void GCustomCommentFrameEx::hide()
{
    if (!isVisible())
    {
        return;
    }

    m_plainTextEdit->removeEventFilter(this);
    m_plainTextEdit->viewport()->removeEventFilter(this);
    setWindowMask(false);

    QWidget::hide();
    GlodonAbstractItemView *pParent = dynamic_cast<GlodonAbstractItemView *>(parentWidget());

    if (pParent && !isReadOnly())
    {
        pParent->model()->setData(m_curIndex, m_plainTextEdit->toPlainText(), gidrCommentRole);
    }
}

void GCustomCommentFrameEx::setFramePosition(QPoint point)
{
    move(point);
}

void GCustomCommentFrameEx::show(bool bautohiden)
{
    if (true == bautohiden)
    {
        m_plainTextEdit->installEventFilter(this);
        m_plainTextEdit->viewport()->installEventFilter(this);
    }
    else
    {
        m_plainTextEdit->removeEventFilter(this);
        m_plainTextEdit->viewport()->removeEventFilter(this);
    }

    QWidget::show();
    QApplication::setActiveWindow(this);
    m_plainTextEdit->setFocus();
    QTextCursor oTextCursor = m_plainTextEdit->textCursor();
    oTextCursor.movePosition(QTextCursor::End);
    m_plainTextEdit->setTextCursor(oTextCursor);
}

void GCustomCommentFrameEx::move(int x, int y)
{
    move(QPoint(x, y));
}

void GCustomCommentFrameEx::move(const QPoint &pt)
{
    m_anchor = c_ptDefaultAnchor;
    QPoint ptmove = QPoint(pt.x(), pt.y() - m_anchor.y());
    m_rcPos = QRect(m_anchor.x() + c_ncSpace, c_ncSpace, c_nMinWidth, c_nMinHeight);

    updatePaintPolygonDate(m_anchor, m_rcPos);

    update();
    QWidget::move(ptmove);
    resize(m_initWinSize.width(), m_initWinSize.height());
    setWindowMask(true);
    setPosByParent(QRect(ptmove.x(), ptmove.y(), m_initWinSize.width(), m_initWinSize.height()));
    updateEditPos();
}

void GCustomCommentFrameEx::move(const QModelIndex &index, const GlodonAbstractItemView &instView)
{
    GlodonAbstractItemView *pParent = dynamic_cast<GlodonAbstractItemView *>(parentWidget());

    if (pParent)
    {
        move(instView.visualRect(index).topRight() + instView.viewport()->pos());
        m_curIndex = index;
    }
}

bool GCustomCommentFrameEx::isReadOnly() const
{
    return m_plainTextEdit->isReadOnly();
}

void GCustomCommentFrameEx::setReadOnly(bool bReadOnly)
{
    m_plainTextEdit->setReadOnly(bReadOnly);
}

bool GCustomCommentFrameEx::setPosByParent(const QRect &rcPos)
{
    QWidget *pParent = parentWidget();

    if (NULL == pParent)
    {
        return false;
    }

    if (rotationRight(rcPos))
    {
        return true;
    }

    if (rotationDown(rcPos))
    {
        return true;
    }

    if (rotationUp(rcPos))
    {
        return true;
    }

    if (rotationLeft(rcPos))
    {
        return true;
    }

    return false;
}

bool GCustomCommentFrameEx::rotationLeft(const QRect &rcPoswind)
{
    QWidget *pParent = parentWidget();
    int nHeight = 0;

    if (rcPoswind.top() + m_rcPos.height() + c_ncSpace - pParent->height() > 0)
    {
        nHeight = rcPoswind.top() + m_rcPos.height() + c_ncSpace - pParent->height();
    }

    if (rcPoswind.top() - nHeight + c_ncSpace < 0)
    {
        nHeight += rcPoswind.top() - nHeight + c_ncSpace;
    }

    int nSpace = rcPoswind.left() - m_rcPos.width() - c_ncSpace;

    if (nSpace >= 0 || nSpace + c_ncSpace < c_nMinAnchorWidth)
    {
        if (nSpace >= 0)
        {
            int nRight = rcPoswind.left() - m_anchor.x() - c_ncSpace - pParent->width();

            if (nRight > 0)
            {
                nSpace = c_ncSpace + nRight;
            }
            else
            {
                nSpace = c_ncSpace;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        nSpace = c_ncSpace + nSpace;
    }

    m_rcPos.moveTo(c_ncSpace, c_ncSpace);
    m_anchor += QPoint(m_rcPos.width() + c_ncSpace + nSpace, nHeight);

    updatePaintPolygonDate(m_anchor, m_rcPos);
    QWidget::move(rcPoswind.left() - m_anchor.x(), rcPoswind.top() - nHeight);
    QRect rcPos(m_rcPos.left() - c_ncSpace
                , m_rcPos.top() - c_ncSpace
                , m_rcPos.width() + (2 * c_ncSpace)
                , m_rcPos.height() + (2 * c_ncSpace));
    rcPos = getMaxRect(m_anchor, rcPos);
    resize(rcPos.width(), rcPos.height());

    updateEditPos();

    return true;
}

bool GCustomCommentFrameEx::rotationRight(const QRect &rcPoswind)
{
    QWidget *pParent = parentWidget();
    int nHeight = 0;

    if (rcPoswind.top() + m_rcPos.height() + c_ncSpace - pParent->height() > 0)
    {
        nHeight = rcPoswind.top() + m_rcPos.height() + c_ncSpace - pParent->height();
    }

    if (rcPoswind.top() - nHeight + c_ncSpace < 0)
    {
        nHeight += rcPoswind.top() - nHeight + c_ncSpace;
    }

    int nSpace = pParent->width() - (rcPoswind.left() + m_rcPos.width() + c_ncSpace);

    if (nSpace >= 0 || nSpace + c_ncSpace < c_nMinAnchorWidth)
    {
        if (nSpace >= 0)
        {
            if (rcPoswind.left() + m_rcPos.left() < 0)
            {
                nSpace = 0 - (rcPoswind.left() + m_rcPos.left());
            }
            else
            {
                nSpace = 0;
            }
        }
        else
        {
            return false;
        }
    }

    m_anchor.setX(m_anchor.x() - nSpace);
    m_anchor.setY(m_anchor.y() + nHeight);
    m_rcPos.moveTo(c_ncSpace, c_ncSpace);
    updatePaintPolygonDate(m_anchor, m_rcPos);
    QWidget::move(rcPoswind.left() + nSpace, rcPoswind.top() - nHeight);
    QRect rcPos(m_rcPos.left() - c_ncSpace
                , m_rcPos.top() - c_ncSpace
                , m_rcPos.width() + (2 * c_ncSpace)
                , m_rcPos.height() + (2 * c_ncSpace));
    rcPos = getMaxRect(m_anchor, rcPos);
    resize(rcPos.width(), rcPos.height());

    return true;
}

bool GCustomCommentFrameEx::rotationUp(const QRect &rcPoswind)
{
    QWidget *pParent = parentWidget();
    int nwidth = 0;

    if (rcPoswind.left() + m_rcPos.width() + c_ncSpace - pParent->width() > 0)
    {
        nwidth = rcPoswind.left() + m_rcPos.width() + c_ncSpace - pParent->width();
    }

    if (rcPoswind.left() - nwidth + c_ncSpace < 0)
    {
        nwidth += rcPoswind.left() - nwidth + c_ncSpace;
    }

    int nSpace = rcPoswind.top() + m_anchor.y() - m_rcPos.height() - c_ncSpace;

    if (nSpace >= 0 || nSpace + c_ncSpace < c_nMinAnchorWidth)
    {
        if (nSpace >= 0)
        {
            int nTop = rcPoswind.top() - rcPoswind.height()
                       + m_anchor.y()
                       + c_ncSpace
                       + m_rcPos.height()
                       - pParent->height();

            if (nTop > 0)
            {
                nSpace = c_ncSpace + nTop;
            }
            else
            {
                nSpace = c_ncSpace;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        nSpace = c_ncSpace + nSpace;
    }

    m_rcPos.moveTo(c_ncSpace, c_ncSpace);
    int nYpixs = m_anchor.y();
    m_anchor.setX(m_anchor.x() + nwidth);
    m_anchor.setY(c_ncSpace + m_rcPos.height() + nSpace);

    updatePaintPolygonDate(m_anchor, m_rcPos);
    QWidget::move(rcPoswind.left() - nwidth, rcPoswind.top() - (rcPoswind.height() - nYpixs) + (c_ncSpace - nSpace));
    QRect rcPos(m_rcPos.left() - c_ncSpace
                , m_rcPos.top() - c_ncSpace
                , m_rcPos.width() + (2 * c_ncSpace)
                , m_rcPos.height() + (2 * c_ncSpace));
    rcPos = getMaxRect(m_anchor, rcPos);
    resize(rcPos.width() + 1, rcPos.height());
    updateEditPos();
    return true;
}

bool GCustomCommentFrameEx::rotationDown(const QRect &rcPoswind)
{
    QWidget *pParent = parentWidget();
    int nwidth = 0;

    if (rcPoswind.left() + m_rcPos.width() + c_ncSpace - pParent->width() > 0)
    {
        nwidth = rcPoswind.left() + m_rcPos.width() + c_ncSpace - pParent->width();
    }

    if (rcPoswind.left() - nwidth + c_ncSpace < 0)
    {
        nwidth += rcPoswind.left() - nwidth + c_ncSpace;
    }

    int nSpace = pParent->height() - (rcPoswind.top() + m_anchor.y() + c_ncSpace + m_rcPos.height());

    if (nSpace >= 0 || nSpace + c_ncSpace < c_nMinAnchorWidth)
    {
        if (nSpace >= 0)
        {
            int nTop = rcPoswind.top() + m_anchor.y() + c_ncSpace;

            if (nTop < 0)
            {
                nSpace = c_ncSpace - nTop;
            }
            else
            {
                nSpace = c_ncSpace;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        nSpace = c_ncSpace + nSpace;
    }

    int nYpixs = m_anchor.y();
    m_anchor.setX(m_anchor.x() + nwidth);
    m_anchor.setY(0);
    m_rcPos.moveTo(c_ncSpace, nSpace);

    updatePaintPolygonDate(m_anchor, m_rcPos);
    QWidget::move(rcPoswind.left() - nwidth, rcPoswind.top() + nYpixs);
    QRect rcPos(m_rcPos.left() - c_ncSpace
                , m_rcPos.top() - c_ncSpace
                , m_rcPos.width() + (2 * c_ncSpace)
                , m_rcPos.height() + (2 * c_ncSpace));
    rcPos = getMaxRect(m_anchor, rcPos);
    resize(rcPos.width() + 1, rcPos.height());
    updateEditPos();
    return true;
}
