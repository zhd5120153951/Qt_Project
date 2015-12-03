#include "GLDFileUtils.h"
#include "GLDWidget_Global.h"
#include "GLDScrollStyle.h"

#include <QColor>
#include <QLinearGradient>
#include <QPainter>
#include <QSettings>
#include <QStyleOptionSlider>

GLDScrollStyle::GLDScrollStyle(QObject *parent, bool isSubControl):
    m_isShowArrow(false),
    m_bFirstLoad(true),
    m_nSliderMinLength(60),
    m_nSliderSize(8),
    m_nArrowWidth(8),
    m_nArrowHeight(4),
    m_nSliderX(3),
    m_nSliderY(3),
    m_nArrowX(3),
    m_nArrowY(6),
    m_nSubSliderMinLength(45),
    m_nSliderAddOffset(8),
    m_nSliderSubOffset(4),
    m_scrollBarHandleColor("#648dd8"),
    m_scrollBarHandleHoverColor("#7da6e4"),
    m_scrollBarTrackColor("#ffffff"),
    m_scrollBarTrackHoverColor("#f4f4f4")
{
    setParent(parent); // 防止内存泄漏
    setIsShowArrow();
    loadStyleIniFile();

    if (isSubControl)
    {
        m_nSliderMinLength = m_nSubSliderMinLength;
    }
    else
    {
        m_nSliderMinLength = 60;
    }
}

GLDScrollStyle::~GLDScrollStyle()
{
}

void GLDScrollStyle::loadStyleIniFile(const QString &path)
{
    if (m_bFirstLoad)
    {
		if (!fileExists(path))
		{
			return;
		}
		QSettings settings(path, QSettings::IniFormat);

        m_nSliderSize = settings.value("sliderSize").toInt();
        m_nSliderMinLength = settings.value("sliderMinLength").toInt();
        m_nSubSliderMinLength = settings.value("subSliderMinLength").toInt();
        m_nArrowWidth = settings.value("arrowWidth").toInt();
        m_nArrowHeight = settings.value("arrowHeight").toInt();
        m_nSliderX = settings.value("sliderX").toInt();
        m_nSliderY = settings.value("sliderY").toInt();
        m_nArrowX = settings.value("arrowX").toInt();
        m_nArrowY = settings.value("arrowY").toInt();
        m_nSliderAddOffset = settings.value("sliderAddOffset").toInt();
        m_nSliderSubOffset = settings.value("sliderSubOffset").toInt();

        m_scrollBarHandleColor = QColor(settings.value("scrollBarHandleColor").toString());
        m_scrollBarHandleHoverColor = QColor(settings.value("scrollBarHandleHoverColor").toString());
        m_scrollBarTrackColor = QColor(settings.value("scrollBarTrackColor").toString());
        m_scrollBarTrackHoverColor = QColor(settings.value("scrollBarTrackHoverColor").toString());
        m_bFirstLoad = false;
    }
}

void GLDScrollStyle::drawComplexControl(QStyle::ComplexControl control,
                                        const QStyleOptionComplex *option,
                                        QPainter *painter, const QWidget *widget) const
{
    if (control == CC_ScrollBar)
    {
        drawScrollbar(control, option, painter, widget);
        return;
    }
    else
    {
        QProxyStyle::drawComplexControl(control, option, painter, widget);
    }
}

int GLDScrollStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option,
                                const QWidget *widget) const
{
    if (metric == QStyle::PM_ScrollBarSliderMin)
    {
        return m_nSliderMinLength;
    }
    else
    {
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
}

QRect GLDScrollStyle::subControlRect(QStyle::ComplexControl cc,
                                     const QStyleOptionComplex *opt,
                                     QStyle::SubControl sc, const QWidget *widget) const
{
    bool bIsHorz = opt->state & State_Horizontal;

    if (cc == CC_ScrollBar
            && (sc == QStyle::SC_ScrollBarGroove
                || sc == QStyle::SC_ScrollBarSlider))
    {
        QRect rectRes = QProxyStyle::subControlRect(cc, opt, sc, widget);

        if (bIsHorz)
        {
            rectRes.setLeft(rectRes.left() - m_nSliderAddOffset);
            rectRes.setRight(rectRes.right() + m_nSliderSubOffset);
        }
        else
        {
            rectRes.setBottom(rectRes.bottom() + m_nSliderSubOffset);
            rectRes.setTop(rectRes.top() - m_nSliderAddOffset);
        }

        return rectRes;
    }

    return QProxyStyle::subControlRect(cc, opt, sc, widget);
}

void GLDScrollStyle::drawArrows(
        QPainter      * painter,
        QRect           drawRectAdd,
        QRect           drawRectSub,
        Qt::Orientation Direct,
        bool            bMouseOver
        ) const
{
    QPixmap pixmapAdd;
    QPixmap pixmapSub;
    QPixmap pixmap;

    // 如果支持鼠标滑过才显示箭头的效果，并且鼠标没有滑过，显示无图片
    // 否则全部贴图
    if (!m_isShowArrow && !bMouseOver)
    {
        pixmap.load("");
    }
    else
    {
        drawRectSub.adjust(m_nArrowX, m_nArrowX, m_nArrowX, m_nArrowX);
        if (Direct == Qt::Horizontal)
        {
            drawRectAdd.adjust(m_nArrowY, m_nArrowX, m_nArrowY, m_nArrowX);

            pixmapAdd.load(":/icons/scrollrightarrow.png");
            pixmapSub.load(":/icons/scrollleftarrow.png");
        }
        else
        {
            drawRectAdd.adjust(m_nArrowX, m_nArrowY, m_nArrowX, m_nArrowY);

            pixmapAdd.load(":/icons/scrolldownarrow.png");
            pixmapSub.load(":/icons/scrolluparrow.png");
        }
    }

    painter->drawPixmap(drawRectAdd, pixmapAdd);
    painter->drawPixmap(drawRectSub, pixmapSub);
}

void GLDScrollStyle::drawSlider(QStyle::SubControls sc,
                                QPainter *painter,
                                const QStyleOptionComplex *option,
                                const QWidget *widget,
                                bool bHorizontal) const
{
    QRect drawRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarSlider, widget);
    if (bHorizontal)
    {
        drawRect.adjust(0, m_nSliderY, 0, m_nSliderY);
        drawRect.setHeight(m_nSliderSize);
    }
    else
    {
        drawRect.adjust(m_nSliderX, 0, m_nSliderX, 0);
        drawRect.setWidth(m_nSliderSize);
    }

    QPainterPath painterPath;
    painterPath.addRect(drawRect);
    if (sc & SC_ScrollBarSlider)
    {
        painter->fillPath(painterPath, m_scrollBarHandleHoverColor); // 鼠标在滚动柄上时的颜色
    }
    else
    {
        painter->fillPath(painterPath, m_scrollBarHandleColor); // 鼠标不在滚动柄上时的颜色
    }
}

 // 画渐变背景（包括：AddLine、SubLine、ScrollBarGroove）
void GLDScrollStyle::drawBackground(QRect drawRect, QPainter *painter,
                                    bool bMouseOver, bool bHorizontal) const
{
    QColor startColor;
    QColor middleColor;
    QColor endColor;
    getScrollBarColors(startColor, middleColor, endColor, bMouseOver);

    QLinearGradient linearGradient;
    if (bHorizontal)
    {
        linearGradient = QLinearGradient(drawRect.left(),
                                         drawRect.top(),
                                         drawRect.left(),
                                         drawRect.bottom());
    }
    else
    {
        linearGradient = QLinearGradient(drawRect.left(),
                                         drawRect.top(),
                                         drawRect.right(),
                                         drawRect.top());
    }

    linearGradient.setColorAt(0.0, startColor);
    linearGradient.setColorAt(0.3, middleColor);
    linearGradient.setColorAt(0.8, middleColor);
    linearGradient.setColorAt(1.0, endColor);
    painter->setBrush(QBrush(linearGradient));
    painter->setPen(QPen(Qt::transparent));
    painter->drawRect(drawRect);
}

void GLDScrollStyle::getScrollBarColors(QColor &startColor, QColor &middleColor,
                                        QColor &endColor,
                                        bool bMouseOver) const
{
    if (!bMouseOver)
    {
        if (m_isShowArrow)
        {
            // 水平滚动条轨道的默认渐变
            startColor = m_scrollBarTrackColor;
            middleColor = m_scrollBarTrackColor;
            endColor = m_scrollBarTrackColor;
        }
        else
        {
            startColor = m_scrollBarTrackColor;
            middleColor = m_scrollBarTrackColor;
            endColor = m_scrollBarTrackColor;
        }
    }
    else
    {
        // 鼠标滑过水平滚动条时的背景渐变
        startColor = m_scrollBarTrackHoverColor;
        middleColor = m_scrollBarTrackHoverColor;
        endColor = m_scrollBarTrackHoverColor;
    }
}

void GLDScrollStyle::drawScrollBarAddLine(const QStyleOptionComplex *option,
                                          QPainter *painter, const QWidget *widget,
                                          bool bMouseOver, bool bHorizontal) const
{
    // 画水平、垂直AddLine的渐变背景
    QRect drawRectAdd = subControlRect(CC_ScrollBar, option, SC_ScrollBarAddLine, widget);
    drawBackground(drawRectAdd, painter, bMouseOver, bHorizontal);
}

void GLDScrollStyle::drawScrollBarSubLine(const QStyleOptionComplex *option,
                                          QPainter *painter, const QWidget *widget,
                                          bool bMouseOver, bool bHorizontal) const
{
    // 画水平、垂直SubLine的渐变背景
    QRect drawRectSub = subControlRect(CC_ScrollBar, option, SC_ScrollBarSubLine, widget);
    drawBackground(drawRectSub, painter, bMouseOver, bHorizontal);
}

void GLDScrollStyle::drawScrollBarGroove(const QStyleOptionComplex *option,
                                         QPainter *painter, const QWidget *widget,
                                         bool bMouseOver, bool bHorizontal) const
{
    // 画水平、垂直ScrollBarGroove的渐变背景
    QRect drawRectGroove = subControlRect(CC_ScrollBar, option, SC_ScrollBarGroove, widget);
    drawBackground(drawRectGroove, painter, bMouseOver, bHorizontal);
}

void GLDScrollStyle::initScrollBar(
        const QStyleOptionComplex * option,
        const QWidget             * widget,
        QPainter                  * painter,
        bool                        bMouseOver,
        bool                        bHorizontal,
        bool                        bIsMaxOut
        ) const
{
    if (bIsMaxOut)
    {
        bMouseOver = true;
    }
    drawScrollBarAddLine(option, painter, widget, bMouseOver, bHorizontal);
    drawScrollBarSubLine(option, painter, widget, bMouseOver, bHorizontal);
    drawScrollBarGroove(option, painter, widget, bMouseOver, bHorizontal);
}

void GLDScrollStyle::drawScrollbar(
        QStyle::ComplexControl      control,
        const QStyleOptionComplex * option,
        QPainter                  * painter,
        const QWidget             * widget
        ) const
{
    const QStyleOptionSlider *pScrollbar = qstyleoption_cast<const QStyleOptionSlider *>(option);
    if (!pScrollbar)
    {
        return;
    }

    bool bIsMaxOut = (pScrollbar->maximum == pScrollbar->minimum);
    State flags = option->state;
    if (widget && widget->testAttribute(Qt::WA_UnderMouse) && widget->isActiveWindow())
    {
        flags |= State_MouseOver;
    }
    if (bIsMaxOut)
    {
        flags &= ~State_Enabled;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    bool bMouseOver = flags & State_MouseOver;
    bool bHorizontal = flags & State_Horizontal;
    initScrollBar(option, widget, painter, bMouseOver, bHorizontal, bIsMaxOut); // 初始化滚动条颜色

    // 画滚动柄
    SubControls sub = option->subControls;
    QStyle::SubControls sc = pScrollbar->activeSubControls;
    if ((sub & SC_ScrollBarSlider) && !bIsMaxOut)
    {
        drawSlider(sc, painter, option, widget, bHorizontal);
    }

    // 鼠标滑过滚动条时显示上下箭头
    if ((sub & SC_ScrollBarGroove) && !bIsMaxOut)
    {
        QRect drawRectAdd = subControlRect(CC_ScrollBar, option, SC_ScrollBarAddLine, widget);
        QRect drawRectSub = subControlRect(CC_ScrollBar, option, SC_ScrollBarSubLine, widget);
        if (bHorizontal)
        {
            drawRectAdd.setWidth(m_nArrowHeight);
            drawRectAdd.setHeight(m_nArrowWidth);
            drawRectSub.setWidth(m_nArrowHeight);
            drawRectSub.setHeight(m_nArrowWidth);
            drawArrows(painter, drawRectAdd, drawRectSub, Qt::Horizontal, bMouseOver);
        }
        else
        {
            drawRectAdd.setWidth(m_nArrowWidth);
            drawRectAdd.setHeight(m_nArrowHeight);
            drawRectSub.setWidth(m_nArrowWidth);
            drawRectSub.setHeight(m_nArrowHeight);
            drawArrows(painter, drawRectAdd, drawRectSub, Qt::Vertical, bMouseOver);
        }
    }

    painter->restore();
    G_UNUSED(control);
}
