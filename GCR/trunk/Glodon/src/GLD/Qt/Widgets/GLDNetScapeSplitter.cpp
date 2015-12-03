#include <QResizeEvent>
#include <QTimer>
#include <QBitmap>

#include "GLDNetScapeSplitter.h"
#include "GLDStrUtils.h"

/* GLDNestScapSplitterTripleHelper */

QPoint GLDNestScapSplitterTripleHelper::vertexA() const
{
    switch (m_direction)
    {
        case leftToRight:
            return QPoint(m_midPoint.x(), m_midPoint.y() + m_radius);

        case rightToleft:
            return QPoint(m_midPoint.x(), m_midPoint.y() - m_radius);

        case TopToBottom:
            return QPoint(m_midPoint.x() - m_radius, m_midPoint.y());

        case bottomToTop:
            return QPoint(m_midPoint.x() + m_radius, m_midPoint.y());

        default:
            return m_midPoint;
    }
}

QPoint GLDNestScapSplitterTripleHelper::vertexB() const
{
    switch (m_direction)
    {
        case leftToRight:
            return QPoint(m_midPoint.x() - m_radius, m_midPoint.y());

        case rightToleft:
            return QPoint(m_midPoint.x() + m_radius, m_midPoint.y());

        case TopToBottom:
            return QPoint(m_midPoint.x(), m_midPoint.y() - m_radius);

        case bottomToTop:
            return QPoint(m_midPoint.x(), m_midPoint.y() + m_radius);

        default:
            return m_midPoint;
    }
}

QPoint GLDNestScapSplitterTripleHelper::vertexC() const
{
    switch (m_direction)
    {
        case leftToRight:
            return QPoint(m_midPoint.x(), m_midPoint.y() - m_radius);

        case rightToleft:
            return QPoint(m_midPoint.x(), m_midPoint.y() + m_radius);

        case TopToBottom:
            return QPoint(m_midPoint.x() + m_radius, m_midPoint.y());

        case bottomToTop:
            return QPoint(m_midPoint.x() - m_radius, m_midPoint.y());

        default:
            return m_midPoint;
    }
}

/* GLDNetScapeSplitter */

GLDNetScapeSplitter::GLDNetScapeSplitter(bool initCollapse, QWidget *parent) :
    GLDSplitter(parent),
    m_minHideLength(c_InitialHidenLength), m_align(alLeftBottom), m_bFirstTimeShow(true), m_bInitCollapse(initCollapse),
    m_bCollapseCompletable(false), m_bUsingLastExpandSize(true),
    m_nWantedExpandSize(-1), m_dWantedExpandFactor(-1)
{
    init();
}

GLDNetScapeSplitter::GLDNetScapeSplitter(Qt::Orientation original, bool initCollapse, QWidget *parent):
    GLDSplitter(original, parent),
    m_minHideLength(c_InitialHidenLength), m_align(alLeftBottom), m_bFirstTimeShow(true), m_bInitCollapse(initCollapse),
    m_bCollapseCompletable(false),m_bUsingLastExpandSize(true),
    m_nWantedExpandSize(-1), m_dWantedExpandFactor(-1), m_handle(NULL)
{
    init();
}

GLDNetScapeSplitter::GLDNetScapeSplitter(QWidget *parent) :
    GLDSplitter(parent), m_align(alLeftBottom),
    m_minHideLength(c_InitialHidenLength), m_bFirstTimeShow(true), m_bInitCollapse(false),
    m_bCollapseCompletable(false),m_bUsingLastExpandSize(true),
    m_nWantedExpandSize(-1), m_dWantedExpandFactor(-1), m_handle(NULL)
{
    init();
}

GLDNetScapeSplitter::GLDNetScapeSplitter(Qt::Orientation original, QWidget *parent) :
    GLDSplitter(original, parent), m_align(alLeftBottom),
    m_minHideLength(c_InitialHidenLength), m_bFirstTimeShow(true), m_bInitCollapse(false),
    m_bCollapseCompletable(false),m_bUsingLastExpandSize(true),
    m_nWantedExpandSize(-1), m_dWantedExpandFactor(-1), m_handle(NULL)
{
    init();
}

void GLDNetScapeSplitter::setCollpaseCompletable(bool enabled)
{
    m_bCollapseCompletable = enabled;

    if (!m_bFirstTimeShow)
    {
        emit splitterMoved(0, 1);
    }
}

bool GLDNetScapeSplitter::setCollpaseSideStretchFactor(double rate)
{
    if (!m_bFirstTimeShow)
    {
        return false;
    }

    if (rate < 1e-6 || rate > 1 - 1e-6)
    {
        return false;
    }

    m_dWantedExpandFactor = rate;
    m_nWantedExpandSize = -1;

    return true;
}

void GLDNetScapeSplitter::doTrigger(bool show)
{
    if (NULL == m_handle)
    {
        return;
    }

    bool bState = m_handle->getSplitterState();
    if (bState == show)
    {
        return;
    }
    m_handle->m_dragMoveState = GLDNetScapeSplitterHandle::pressed;
    m_handle->handlerButtonClick();
}

int GLDNetScapeSplitter::getHandleIndexOffset()
{
    switch (this->m_align)
    {
        case alLeftBottom:
            return orientation() == Qt::Vertical ? 1 : 0;

        case alRightTop:
            return orientation() == Qt::Vertical ? 0 : 1;
    }

    return 0;
}

void GLDNetScapeSplitter::changeHandleLength(GLDNetScapeSplitterHandle *handle, int handleLength)
{
    int nHandleIndex = indexOf(handle);

    if (nHandleIndex == -1)
    {
        return;
    }

    int nPrevIndex = nHandleIndex - 1;

    nHandleIndex = nHandleIndex - 1 + getHandleIndexOffset();

    if (nHandleIndex == -1)
        return;

    QList<int> lengths(sizes());

    if (handle->m_dragMoveState >= GLDNetScapeSplitterHandle::moved)
    {
        handle->saveExpandLength(qMax(handle->collapseLength(), lengths.at(nHandleIndex)));
    }
    else
    {
        if (nPrevIndex == nHandleIndex)
        {
            nPrevIndex++;
        }

        int nLengthOffset = handleLength - lengths.at(nHandleIndex);

        if (handle->collapseLength() == handleLength)
        {
            handle->saveExpandLength(qMax(handle->collapseLength(), lengths.at(nHandleIndex)));
        }

        if (nPrevIndex == -1)
            return;

        if ((this->m_align == alLeftBottom && this->orientation() == Qt::Horizontal)
                || (m_align == alRightTop && this->orientation() == Qt::Vertical))
        {
            lengths[nPrevIndex] = lengths.at(nPrevIndex) - nLengthOffset;
            lengths[nHandleIndex] = handleLength;
        }
        else
        {
            lengths[nPrevIndex] = lengths.at(nPrevIndex) - nLengthOffset;
            lengths[nHandleIndex] = handleLength;
        }

        this->setSizes(lengths);
    }
}

void GLDNetScapeSplitter::emitHandlerMoved(int length, int index)
{
    GLDNetScapeSplitterHandle *hand = dynamic_cast<GLDNetScapeSplitterHandle *>(handle(index));

    if (hand == NULL)
    {
        return;
    }

    int nMinIndexWidth = hand->collapseLength();
    int nMin(0);
    int nMax(1);

    getRange(index, &nMin, &nMax);

    bool bleftType = (this->m_align == alLeftBottom);

    if (bleftType != (this->orientation() == Qt::Vertical))  //TODOREVIEW
    {
        bleftType = true;
    }
    else
    {
        bleftType = false;
    }

    if (this->childrenCollapsible())
    {
        nMinIndexWidth = 0;
    }

    if (bleftType ? (length <= nMinIndexWidth) : (length + nMinIndexWidth >= nMax))
    {
        QList<int> childrenSizes = sizes();
        int nFixedDistance = bleftType ? (nMinIndexWidth - length) : (length + nMinIndexWidth - nMax);

        if (bleftType)
        {
            childrenSizes[index - 1] = nMinIndexWidth;
            childrenSizes[index] += nFixedDistance;
        }
        else
        {
            childrenSizes[index - 1] -= nFixedDistance;
            childrenSizes[index] = nMinIndexWidth;
        }

        setSizes(childrenSizes);

        if (true == hand->m_bExpanded)
        {
            hand->m_bExpanded = false;
            hand->adjustArrowDirection();
            hand->update();
        }
    }
    else
    {
        if (false == hand->m_bExpanded)
        {
            hand->m_bExpanded = true;
            hand->adjustArrowDirection();
        }

        hand->update();
    }
}

void GLDNetScapeSplitter::init()
{
    setHandleWidth(c_InitialHandleWidth);
    connect(this, SIGNAL(splitterMoved(int, int)),
            this, SLOT(emitHandlerMoved(int, int)));
}

void GLDNetScapeSplitter::updateSplitterExpendLength(GLDNetScapeSplitterHandle *splitterHandle)
{
    if (m_nWantedExpandSize > 0)
    {
        if (m_nWantedExpandSize < (orientation() == Qt::Horizontal ? width() : height()))
        {
            splitterHandle->m_expandedLength = m_nWantedExpandSize;
        }
    }
    else if (isInInterval(m_dWantedExpandFactor, 0.0, 1.0, 0))
    {
        splitterHandle->m_expandedLength = m_dWantedExpandFactor
                * (orientation() == Qt::Horizontal ? width() : height());
    }
}

void GLDNetScapeSplitter::updateSplitterSizes()
{
    QList<int> indexSizes = sizes();

    int nSize1 = indexSizes[0];
    int nSize2 = indexSizes[1];

    if (m_align == alLeftBottom)
    {
        if (orientation() == Qt::Horizontal)
        {
            indexSizes[0] = 0;
            indexSizes[1] += nSize1;
        }
        else
        {
            indexSizes[0] += nSize2;
            indexSizes[1] = 0;
        }
    }
    else    //alRightTop
    {
        if (orientation() == Qt::Horizontal)
        {
            indexSizes[0] += nSize2;
            indexSizes[1] = 0;
        }
        else
        {
            indexSizes[0] = 0;
            indexSizes[1] += nSize1;
        }
    }

    setSizes(indexSizes);
}

void GLDNetScapeSplitter::showSplitterHandle(GLDNetScapeSplitterHandle *splitterHandle)
{
    updateSplitterExpendLength(splitterHandle);

    if(m_bInitCollapse || m_bCollapseCompletable)
    {
        splitterHandle->m_collapseLength = 0;
    }

    if (m_bInitCollapse)
    {
        this->setChildrenCollapsible(true);

        splitterHandle->m_bExpanded = false;

        updateSplitterSizes();
    }

    splitterHandle->adjustArrowDirection();
}

void GLDNetScapeSplitter::onFirstTimeShowEvent()
{
    m_bFirstTimeShow = false;

    const int nWidgetsCount = count();

    emit resizeSignal(this->orientation() == Qt::Horizontal ? height() : width());

    for (int i = 0; i < nWidgetsCount; ++i)
    {
        GLDNetScapeSplitterHandle *splitterHandle = dynamic_cast<GLDNetScapeSplitterHandle *>(handle(i));

        if (splitterHandle == NULL)
        {
            continue;
        }

        if (splitterHandle->isVisibleTo(this))
        {
            showSplitterHandle(splitterHandle);
        }
    }
}

void GLDNetScapeSplitter::onShowEvent()
{
    if (count() > 2)
    {
        return;
    }

    if (m_bFirstTimeShow)
    {
        onFirstTimeShowEvent();
    }
}

void GLDNetScapeSplitter::initSignalSlot(GLDNetScapeSplitterHandle *handle)
{
    handle->initButton();
    handle->setCollapseLength(m_minHideLength);

    connect(this, SIGNAL(resizeSignal(QResizeEvent *)),
            handle, SLOT(onSplitterResized(QResizeEvent *)));
    connect(this, SIGNAL(resizeSignal(int)),
            handle, SLOT(onSplitterResized(int)));
    connect(handle, SIGNAL(splitterStateSwitched(GLDNetScapeSplitterHandle *, int)),
            this, SLOT(changeHandleLength(GLDNetScapeSplitterHandle *, int)));
}

GLDSplitterHandle *GLDNetScapeSplitter::createHandle()
{
    GLDNetScapeSplitterHandle *handle = new GLDNetScapeSplitterHandle(this->orientation(), this);
    m_handle = handle;
    if (NULL != handle)
    {
        initSignalSlot(handle);
    }

    return handle;
}


/* GLDNetScapeSplitterButton */

void GLDNetScapeSplitterButton::setButtonNameByArrowDirection()
{
    switch (m_arrowDirection)
    {
        case rightToleft:
        {
            this->setObjectName("left");
            break;
        }

        case leftToRight:
        {
            this->setObjectName("right");
            break;
        }

        case bottomToTop:
        {
            this->setObjectName("top");
            break;
        }

        case TopToBottom:
        {
            this->setObjectName("bottom");
            break;
        }

    default:
        Q_ASSERT(false);
    }
}

bool GLDNetScapeSplitterButton::hitButton(const QPoint &pos) const
{
    GLDNetScapeSplitterHandle *hand = dynamic_cast<GLDNetScapeSplitterHandle *>(parentWidget());

    if (NULL != hand)
    {
        if (GLDNetScapeSplitterHandle::moved == hand->dragMoveState())
        {
            return false;
        }
    }

    return QPushButton::hitButton(pos);
}

void GLDNetScapeSplitterButton::mousePressEvent(QMouseEvent *e)
{
    GLDNetScapeSplitterHandle *hand = dynamic_cast<GLDNetScapeSplitterHandle *>(parentWidget());

    if (NULL != hand)
    {
        if (Qt::LeftButton == (e->buttons() & Qt::LeftButton))
        {
            hand->setDragMoveState(0);
        }
    }

    return QPushButton::mousePressEvent(e);
}

void GLDNetScapeSplitterButton::paintEvent(QPaintEvent *e)
{
    QPixmap pixmap;
    QPushButton::paintEvent(e);
    QPainter painter(this);

    int nwidth = this->width();
    int nheight = this->height();
    int ntoBottom = c_TriangleBottomToBorderPixel;
    QImage image(nwidth, nheight, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);
    QPainter imagePainter(&image);

    QPolygon polyA;// left or top
    QPolygon polyB;//right or bottom
    int npaddingLen = 2;

    switch (m_arrowDirection)
    {
        case leftToRight:
        {
            int ntoSide = nheight / (double)c_TriangleToEndRate;
            int ntriangleWidth = nwidth - ntoBottom - npaddingLen;
            int nusingWidth = ntriangleWidth;
            QPoint midA(nwidth - ntoBottom, nheight - ntoSide - nusingWidth);
            GLDNestScapSplitterTripleHelper tmpA(midA, nusingWidth, 0);
            polyA.setPoints(3, tmpA.vertexA().x(), tmpA.vertexA().y(), tmpA.vertexB().x(),
                            tmpA.vertexB().y(), tmpA.vertexC().x(), tmpA.vertexC().y());
            QPoint midB(nwidth - ntoBottom, ntoSide + nusingWidth);
            GLDNestScapSplitterTripleHelper tmpB(midB, nusingWidth, 0);
            polyB.setPoints(3, tmpB.vertexA().x(), tmpB.vertexA().y(), tmpB.vertexB().x(),
                                tmpB.vertexB().y(), tmpB.vertexC().x(), tmpB.vertexC().y());
            break;
        }

        case rightToleft:
        {
            int ntoSide = nheight / (double)c_TriangleToEndRate;
            int ntriangleWidth = nwidth - ntoBottom - npaddingLen;
            int nusingWidth = ntriangleWidth;
            QPoint midA(ntoBottom, nheight - ntoSide - nusingWidth);
            GLDNestScapSplitterTripleHelper tmpA(midA, nusingWidth, 1);
            polyA.setPoints(3, tmpA.vertexA().x(), tmpA.vertexA().y(), tmpA.vertexB().x(),
                                tmpA.vertexB().y(), tmpA.vertexC().x(), tmpA.vertexC().y());
            QPoint midB(ntoBottom, ntoSide + nusingWidth);
            GLDNestScapSplitterTripleHelper tmpB(midB, nusingWidth, 1);
            polyB.setPoints(3, tmpB.vertexA().x(), tmpB.vertexA().y(), tmpB.vertexB().x(),
                                tmpB.vertexB().y(), tmpB.vertexC().x(), tmpB.vertexC().y());
            break;
         }

         case TopToBottom:
         {
            int ntoSide = nwidth / (double)c_TriangleToEndRate;
            int ntriangleHeight = nheight - ntoBottom - npaddingLen;
            int nusingHeight = ntriangleHeight - npaddingLen;
            polyA.setPoints(3, ntoSide, ntriangleHeight, ntoSide + nusingHeight, npaddingLen,
                                ntoSide + 2 * nusingHeight, ntriangleHeight);
            polyB.setPoints(3, nwidth - ntoSide, ntriangleHeight, nwidth - (ntoSide + nusingHeight), npaddingLen,
                                nwidth - (ntoSide + 2 * nusingHeight), ntriangleHeight);
            break;
         }

         case bottomToTop:
         {
            int ntoSide = nwidth / (double)c_TriangleToEndRate;
            int ntriangleHeight = nheight - ntoBottom - npaddingLen;
            int nusingHeight = ntriangleHeight - npaddingLen;
            polyA.setPoints(3, ntoSide, ntoBottom, ntoSide + nusingHeight, ntriangleHeight,
                                ntoSide + 2 * nusingHeight, ntoBottom);
            polyB.setPoints(3, nwidth - ntoSide, ntoBottom, nwidth - (ntoSide + nusingHeight), ntriangleHeight,
                                nwidth - (ntoSide + 2 * nusingHeight), ntoBottom);
            break;
         }
         default:
            break;
    }

    imagePainter.setPen(this->palette().buttonText().color());
    imagePainter.setBrush(this->palette().buttonText());
    imagePainter.setRenderHint(QPainter::Antialiasing);

    imagePainter.drawPolygon(polyA);
    imagePainter.drawPolygon(polyB);
    {
        QPolygon lines;
        lines.push_back(polyA.boundingRect().center());
        lines.push_back(polyB.boundingRect().center());

        int nwidth = painter.pen().width();
        Qt::PenStyle style = Qt::PenStyle(Qt::DotLine);
        Qt::PenCapStyle cap = painter.pen().capStyle();
        Qt::PenJoinStyle join = painter.pen().joinStyle();

        imagePainter.setPen(QPen(Qt::black, nwidth, style, cap, join));
        imagePainter.drawPolyline(lines);
    }
    imagePainter.end();
    pixmap = QPixmap::fromImage(image);
    painter.drawPixmap(0, 0, pixmap);
}
