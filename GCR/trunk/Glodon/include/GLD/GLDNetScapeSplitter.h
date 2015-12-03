#ifndef GLDNETSCAPESPLITTER_H
#define GLDNETSCAPESPLITTER_H

#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QStyleOption>
#include <QPixmapCache>

#include "GLDSplitter.h"
#include "GLDGlobal.h"
#include "GLDNetScapeSplitterHandle.h"

enum ArrowDirection
{
    leftToRight,
    rightToleft,
    TopToBottom,
    bottomToTop,
    inValidDirection
};

class GLDWIDGETSHARED_EXPORT GLDNestScapSplitterTripleHelper
{
public:
    GLDNestScapSplitterTripleHelper(QPoint &mid, int radius, int direction) :
        m_midPoint(mid), m_radius(radius), m_direction(direction) {}

public:
    QPoint vertexA() const;//左手顶点
    QPoint vertexB() const;//直角点
    QPoint vertexC() const;//右手顶点

private:
    QPoint m_midPoint;
    double m_radius;
    int m_direction;
};

/*!
 * \brief The GLDNetScapeSplitterButton class 用来绘制三角的button，并发送点击消息给splitter
 */
class GLDWIDGETSHARED_EXPORT GLDNetScapeSplitterButton : public QPushButton
{
    Q_OBJECT
public:
    explicit GLDNetScapeSplitterButton(Qt::Orientation o, QWidget *parent) :
        QPushButton(parent),
        m_arrowDirection(o == Qt::Horizontal ? leftToRight : TopToBottom),
        m_qssFileName(),
        m_orientation(o) {}

public:
    void setArrowDirection(int direction)
    {
        m_arrowDirection = direction;
        setButtonNameByArrowDirection();
        loadStyleSheet(m_qssFileName);
    }
    void setQssFileName(const QString &fileName)
    {
        m_qssFileName = fileName;
    }
    void setButtonNameByArrowDirection();
    void setButtonObjectName()
    {
        setButtonNameByArrowDirection();
    }

    virtual void loadStyleSheet(const QString &fileName)
    {
        G_UNUSED(fileName);
    }

protected:
    bool hitButton(const QPoint &pos) const;
    void mousePressEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
protected:
    /*!
     * \brief m_arrowDirection 0:left,1:right,2:top,3:bottom
     */
    int m_arrowDirection;

private:
    QString m_qssFileName;
    Qt::Orientation m_orientation;
private:
    friend class GLDNetScapeSplitterHandle;
};

/*!
 * \brief The GLDNetScapeSplitter 通过基类的setHandleWidth设置分隔条宽度
 * \note 现仅支持两个child,也就是说多个child时的不相关的index行为是由QSplitter定义或者是未定义的
 */
class GLDWIDGETSHARED_EXPORT GLDNetScapeSplitter : public GLDSplitter
{
    Q_OBJECT
public:
    enum SplitterAlign
    {
        alLeftBottom,       //向左/下收起(根据splitter的orientation组合出一个特定的方向)
        alRightTop          //向右/上收起(根据splitter的orientation组合出一个特定的方向)
    };

public:
    explicit GLDNetScapeSplitter(QWidget *parent = 0);
    explicit GLDNetScapeSplitter(Qt::Orientation original, QWidget *parent = 0);
    explicit GLDNetScapeSplitter(bool initCollapse, QWidget *parent = 0);
    explicit GLDNetScapeSplitter(Qt::Orientation original, bool initCollapse, QWidget *parent = 0);

public:
    inline SplitterAlign alignment() const
    {
        return m_align;
    }

    inline void setAlignment(SplitterAlign align)
    {
        m_align = align;
        //TODO
    }

    int indexOffsetForCollapse()
    {
        return m_align == alLeftBottom ? 0 : 1;
    }

    /*!
     * \brief hiddenLength 收起时的宽度
     */
    inline int hiddenLength() const
    {
        return m_minHideLength;
    }

    /*!
     * \brief hiddenLength 收起时的宽度
     * \note 界面显示后调用的即设即用效果暂时是TODO状态
     */
    inline void setHiddenLength(int value)
    {
        m_minHideLength = value;
    }

    /*!
     * \brief setCompletlyCollpased 设置通过Qt::Orientation与SplitterAlign确定方向的那一边在初始化时是被完全隐藏
     * 并且，通过点击button也是在给定的展开宽度与完全收起间进行切换。(忽略收起宽度的设置)
     * \param collpased
     * \warning 不要与setHiddenLength同时调用，（特别是以非0的参数），结果是未定义的
     * \note 这是一个窗口未显示前调用才有效的方法
     * \deprecated
     */
    void setCompletlyCollpased()
    {
        //    if (!m_bInitShow) return;
        //    m_bInitCollapse = true;
    }

    /*!
     * \brief setCompletlyCollpasedEnabled 设置确定的那一边的完全隐藏。分为当前是展开状态，以及当前是带有收起宽度的收起状态。
     * 均会切换为收起宽度为0的收起状态，并会将收起宽度置为0。对于已处于宽度为0的收起状态，则不会有任何动作。
     * \note 设置是否可以完全收起。若为false，则收起时的宽度将设为之前设置的收起时宽度(无设置则是默认)。若为true，则收起时的宽度将变为0
     */
    void setCollpaseCompletable(bool enabled = true);

    /*!
     * \brief setCollpaseSideStretchFactor 设置通过Qt::Orientation与SplitterAlign确定方向的那一边的初始的 展开尺寸，可以通过
     *拖动handle后，点击button进行隐藏的手法改变 展开尺寸，也就是说此方法只在初始时有效，并不保证move handle后的 展开尺寸
     * \param rate 可隐藏的那一边的 展开尺寸 相对于spliter的宽度的比率，必须是在(0, 1)之间，若与实际的尺寸计算后不足1像素，将会采用
     *默认值
     * \retval true:设置成功 false:设置失败
     * \warning 请不要试图调用基类的 setStretchFactor()方法 结果是未定义的
     * \note 这是一个窗口未显示前调用才有效的方法
     */
    bool setCollpaseSideStretchFactor(double rate);

    /*!
     * \brief setExpandSize
     * \param size
     * \see setCollpaseSideStretchFactor()
     * \warning size应是有意义的，若超出或为负值，结果是未定义的。
     * \note 这是一个窗口未显示前调用才有效的方法
     */
    void setExpandSize(int size)
    {
        //    if (!m_bInitShow) return false;
        m_nWantedExpandSize = size;
        m_dWantedExpandFactor = -1;
    }

    /*!
     * \brief setLastExpandedSizeWantedEnable
     * \param enabled true:最后一次展开的宽度为隐藏时点击按钮后的展开宽度
     *                false:通过setExpandSize、setCollpaseSideStretchFactor等设置的展开宽度是点击按钮的展开宽度
     */
    void setLastExpandedSizeWantedEnable(bool enabled = true)
    {
        m_bUsingLastExpandSize = enabled;
    }

    /*!
     * \brief 用于控制是否展开
     * \param show 展开
     */
    void doTrigger(bool show);

Q_SIGNALS:
    /*!
     * \brief resizeSignal 用以在resize事件中发出些信号，使得handle中的button得到通知，进而自适应调整button的位置
     * \param e
     */
    void resizeSignal(QResizeEvent *e);
    void resizeSignal(int length);

protected:
    void showEvent(QShowEvent *e)
    {
        GLDSplitter::showEvent(e);

        onShowEvent();
    }
    void onShowEvent();
    void resizeEvent(QResizeEvent *e)
    {
        emit resizeSignal(e);
        GLDSplitter::resizeEvent(e);
    }
    virtual GLDSplitterHandle *createHandle();
    void initSignalSlot(GLDNetScapeSplitterHandle *handle);
    inline bool isUsingLastExpandSize() const
    {
        return m_bUsingLastExpandSize;
    }

private Q_SLOTS:
    void changeHandleLength(GLDNetScapeSplitterHandle *handle, int handleLengthe);
    void emitHandlerMoved(int length, int index);

private:
    void init();
    int getHandleIndexOffset();
    void showSplitterHandle(GLDNetScapeSplitterHandle *splitterHandle);
    void updateSplitterExpendLength(GLDNetScapeSplitterHandle *splitterHandle);
    void updateSplitterSizes();
    void onFirstTimeShowEvent();

protected:
    int m_minHideLength;
    GLDNetScapeSplitterHandle *m_handle;

private:
    SplitterAlign m_align;
    bool m_bFirstTimeShow;
    bool m_bInitCollapse;
    bool m_bCollapseCompletable;
    bool m_bUsingLastExpandSize;
    int m_nWantedExpandSize;
    double m_dWantedExpandFactor;
private:
    friend class GLDNetScapeSplitterHandle;
    Q_DISABLE_COPY(GLDNetScapeSplitter)
};

#endif // GLDNETSCAPESPLITTER_H
