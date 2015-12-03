#include "GLDStrUtils.h"
#include "GLDNetScapeSplitter.h"
#include "GLDNetScapeSplitterHandle.h"

/* GLDNetScapeSplitterHandle */


GLDNetScapeSplitterHandle::GLDNetScapeSplitterHandle(Qt::Orientation o, GLDSplitter *parent):
    GLDSplitterHandle(o, parent),
    m_buttonHeight(c_InitialButtonLength),
    m_collapseLength(c_InitialHidenLength),
    m_expandedLength(100),
    m_pSplitter(dynamic_cast<GLDNetScapeSplitter *>(parent)),
    m_dragMoveState(initial),
    m_bExpanded(true)
{

}

void GLDNetScapeSplitterHandle::initButton()
{
    m_pButton = new GLDNetScapeSplitterButton(this->orientation(), this);
    m_pButton->setStyleSheet("border-width:1px;");//border:none;
    m_pButton->setVisible(true);
    m_pButton->setCursor(Qt::ArrowCursor);

    connect(m_pButton, SIGNAL(clicked()), this, SLOT(handlerButtonClick()));
}

int GLDNetScapeSplitterHandle::newArrowDirection() const
{
    int nDirection = -1;

    const int nlocalOffset = m_pSplitter->indexOffsetForCollapse(); //注意此处的方向与定义处略有不同，因此时的expanded值在对应状态时是现在的值的非
    const bool bHorizontal = this->orientation() == Qt::Horizontal;

    if (m_bExpanded)
    {
        nDirection = bHorizontal ? (1 - nlocalOffset) : (2 + nlocalOffset);
    }
    else
    {
        nDirection = bHorizontal ? nlocalOffset : (3 - nlocalOffset);
    }

    return nDirection;
}

void GLDNetScapeSplitterHandle::handlerButtonClick()
{
    m_bExpanded = !m_bExpanded;

    Q_ASSERT(m_pSplitter != NULL);

    emit splitterStateSwitched(this, m_bExpanded ? m_expandedLength : m_collapseLength );

    if (m_dragMoveState >= moved)
    {
        m_dragMoveState = initial;
    }
    else
    {
        m_pButton->update();

        const int nDirection = newArrowDirection();

        m_pButton->setArrowDirection(nDirection);
    }
}

void GLDNetScapeSplitterHandle::adjustButtonPos(int splitterLength)
{
    QRect thisRect = this->rect();

    if (this->orientation() == Qt::Horizontal)
    {
        m_pButton->setGeometry(0, (splitterLength - m_buttonHeight) / 2.0,
                               thisRect.width(), m_buttonHeight);
    }
    else
    {
        m_pButton->setGeometry((splitterLength - m_buttonHeight) / 2.0, 0,
                               m_buttonHeight, thisRect.height());
    }
}

void GLDNetScapeSplitterHandle::onSplitterResized(int nSplitterLength)
{
    if (!m_pButton)
    {
        return;
    }

    if(nSplitterLength <= 20)
    {
        return;
    }

    adjustButtonPos(nSplitterLength);
}

void GLDNetScapeSplitterHandle::onSplitterResized(QResizeEvent *e)
{
    const int nSplitterLength = (orientation() == Qt::Horizontal)? e->size().height() : e->size().width();

    onSplitterResized(nSplitterLength);

    changeSplitterExpendLength(e);
}

void GLDNetScapeSplitterHandle::changeSplitterExpendLength(QResizeEvent *e)
{
    // 重新计算
    if(!m_pSplitter)
    {
        return;
    }

    if(m_pSplitter->isUsingLastExpandSize())
    {
        return;
    }

    if((orientation() == Qt::Horizontal
        ? e->size().width() == e->oldSize().width()
        : e->size().height() == e->oldSize().height()))
    {
        return;
    }

    if(m_expandedLength <= 1)
    {
        return;
    }

    if (m_pSplitter->isUsingLastExpandSize() && isInInterval(m_pSplitter->m_dWantedExpandFactor, 0.0, 1.0, 0))
    {
        m_expandedLength = m_pSplitter->m_dWantedExpandFactor
                           * (orientation() == Qt::Horizontal ? e->size().width() : e->size().height());
    }
}

void GLDNetScapeSplitterHandle::mousePressEvent(QMouseEvent *e)
{
    if (Qt::LeftButton == (e->buttons() & Qt::LeftButton))
    {
        m_dragMoveState = pressed;
    }

    return GLDSplitterHandle::mousePressEvent(e);
}

void GLDNetScapeSplitterHandle::mouseMoveEvent(QMouseEvent *e)
{
    if (Qt::LeftButton == (e->buttons() & Qt::LeftButton))
    {
        m_dragMoveState = moved;
    }

    return GLDSplitterHandle::mouseMoveEvent(e);
}

void GLDNetScapeSplitterHandle::mouseReleaseEvent(QMouseEvent *e)
{
    if (Qt::LeftButton == (e->button() & Qt::LeftButton))
    {
        m_dragMoveState = dragging;
    }

    return GLDSplitterHandle::mouseReleaseEvent(e);
}

int GLDNetScapeSplitterHandle::curArrowDirection() const
{
    if (this->orientation() == Qt::Horizontal)
    {
        switch (m_pSplitter->m_align)
        {
            case GLDNetScapeSplitter::alLeftBottom:
                return m_bExpanded ? rightToleft : leftToRight;

            case GLDNetScapeSplitter::alRightTop:
                return m_bExpanded ? leftToRight :rightToleft;
        }
    }
    else
    {
        switch (m_pSplitter->m_align)
        {
            case GLDNetScapeSplitter::alLeftBottom:
                return m_bExpanded ?  TopToBottom : bottomToTop;

            case GLDNetScapeSplitter::alRightTop:
                return m_bExpanded ?  bottomToTop : TopToBottom;
        }
    }

    return inValidDirection;
}

void GLDNetScapeSplitterHandle::saveExpandLength(int expandLength) const
{
    if (m_pSplitter->isUsingLastExpandSize())
    {
        m_expandedLength = expandLength;
    }
}

void GLDNetScapeSplitterHandle::adjustArrowDirection()
{
    if (m_pSplitter)
    {
        int nDirection = curArrowDirection();
        m_pButton->setArrowDirection(nDirection);
    }
}



