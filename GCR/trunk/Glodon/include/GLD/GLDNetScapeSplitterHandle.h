#ifndef GLDNETSCAPESPLITTERHANDLE_H
#define GLDNETSCAPESPLITTERHANDLE_H

#include "GLDSplitterHandle.h"
#include "GLDWidget_Global.h"

const int c_InitialHidenLength = 12;
const int c_InitialHandleWidth = 10;
const int c_InitialButtonLength = 120;

const int c_TriangleToEndRate = 20;
const int c_TriangleBottomToBorderPixel = 2;

class GLDNetScapeSplitter;
class GLDNetScapeSplitterButton;

/*!
 * \brief The GLDNetScapeSplitterHandle class 自写的handle类，主要功能是与GLDNetScapeSplitter搭配使用，成为
 *GLDNetScapeSplitterButton的容器
 */
class GLDWIDGETSHARED_EXPORT GLDNetScapeSplitterHandle : public GLDSplitterHandle
{
    Q_OBJECT
public:
    explicit GLDNetScapeSplitterHandle(Qt::Orientation o, GLDSplitter *parent);

public:
    /*!
     * \brief setButtonLength 设置包含的GLDNetScapeSplitterButton的长度值(所谓长度，是指普通的btn较长的那一边的尺寸)
     * \param val 像素为单位
     */
    void setButtonLength(int val)
    {
        m_buttonHeight = val;
        //TODO btn显示后的设置需要仍然有效
    }

    int buttonLenght() const
    {
        return m_buttonHeight;
    }

    /*!
     * \brief setCollapseLength
     * \param val 设置要可被收起的那一侧收起时的最小尺寸,一般地,会有一个不为零的默认值,若此处设置为0或负值,则表示可以完全收起(也就是隐藏)
     *
     */
    void setCollapseLength(int val)
    {
        int nPreVal = m_collapseLength;
        m_collapseLength = qMax(val, 0);
        Q_UNUSED(nPreVal);
        //TODO 考虑界面已经显示后调用此接口的情形
    }
    int collapseLength() const
    {
        return m_collapseLength;
    }

    int dragMoveState() const
    {
        return m_dragMoveState;
    }

    inline bool getSplitterState()
    {
        return m_bExpanded;
    }

    inline GLDNetScapeSplitterButton *getButton()
    {
        return m_pButton;
    }

    void adjustButtonPos(int buttonHeight);

    virtual void initButton();

signals:
    void splitterStateSwitched(GLDNetScapeSplitterHandle *, int);

public slots:
    virtual void handlerButtonClick();
    virtual void onSplitterResized(QResizeEvent *e);
    virtual void onSplitterResized(int nSplitterLength);

protected:
    enum EnDragMoveState
    {
        initial = -1,
        pressed,
        moved,
        dragging
    };

protected:
    void setDragMoveState(int state)
    {
        m_dragMoveState = state;
    }

    void changeSplitterExpendLength(QResizeEvent *e);

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    int curArrowDirection() const;
    int newArrowDirection() const;
    void adjustArrowDirection();


    void saveExpandLength(int expandLength) const;

protected:
    mutable int m_buttonHeight;
    mutable int m_collapseLength;
    mutable int m_expandedLength;

    GLDNetScapeSplitterButton *m_pButton;
    GLDNetScapeSplitter *m_pSplitter;

    int m_dragMoveState;
    bool m_bExpanded;

private:
    friend class GLDNetScapeSplitter;
    friend class GLDNetScapeSplitterButton;
    Q_DISABLE_COPY(GLDNetScapeSplitterHandle)
};

#endif // GLDNETSCAPESPLITTERHANDLE_H
