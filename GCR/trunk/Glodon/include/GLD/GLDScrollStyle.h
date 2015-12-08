#ifndef GLDSCROLLSTYLE_H
#define GLDSCROLLSTYLE_H

#include <QProxyStyle>
#include "GLDWidget_Global.h"

class GLDWIDGETSHARED_EXPORT GLDScrollStyle: public QProxyStyle
{
public:
    explicit GLDScrollStyle(QObject *parent, bool isSubControl = true);
    ~GLDScrollStyle();

public:
    inline void setIsShowArrow(bool isShow = false) { m_isShowArrow = isShow; }
    inline bool isShowArrow() { return m_isShowArrow; }
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                            QPainter *painter, const QWidget *widget) const;
    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const;
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt,
                         SubControl sc, const QWidget *widget) const;

    /**
     * @brief setHandleColor 设置滚柄默认颜色
     * @param handleColor
     */
    inline void setHandleColor(const QColor &handleColor) { m_scrollBarHandleColor = handleColor; }

    /**
     * @brief handleColor 滚柄默认颜色
     * @return
     */
    inline QColor handleColor() const { return m_scrollBarHandleColor; }

    /**
     * @brief setHandleHoverColor 设置鼠标划过时滚柄的颜色
     * @param handleHoverColor
     */
    inline void setHandleHoverColor(const QColor &handleHoverColor) { m_scrollBarHandleHoverColor = handleHoverColor; }

    /**
     * @brief handHoverColor 鼠标划过时滚柄的颜色
     * @return
     */
    inline QColor handHoverColor() const { return m_scrollBarHandleHoverColor; }

    /**
     * @brief setTrackColor 设置轨道默认颜色
     * @param trackColor
     */
    inline void setTrackColor(const QColor &trackColor) { m_scrollBarTrackColor = trackColor; }

    /**
     * @brief trackColor 轨道默认颜色
     * @return
     */
    inline QColor trackColor() const { return m_scrollBarTrackColor; }

    /**
     * @brief setTrackHoverColor 设置鼠标划过时轨道的颜色
     * @param trackHoverColor
     */
    inline void setTrackHoverColor(const QColor &trackHoverColor) { m_scrollBarTrackHoverColor = trackHoverColor; }

    /**
     * @brief trackHoverColor 鼠标划过时轨道的颜色
     * @return
     */
    inline QColor trackHoverColor() const { return m_scrollBarTrackHoverColor; }

protected:
    virtual void loadStyleIniFile(const QString &path = ":/inis/scrollstyle.ini");

private:
    void initScrollBar(
            const QStyleOptionComplex * option,
            const QWidget             * widget,
            QPainter                  * painter,
            bool                        bMouseOver,
            bool                        bHorizontal,
            bool                        bIsMaxOut
        ) const;
    void getScrollBarColors(QColor &startColor, QColor &middleColor,
                            QColor &endColor, bool bMouseOver) const;

    void drawBackground(QRect drawRect, QPainter *painter, bool bMouseOver, bool bHorizontal) const;
    void drawScrollbar(ComplexControl control, const QStyleOptionComplex *option,
                       QPainter *painter, const QWidget *widget) const;
    void drawScrollBarAddLine(const QStyleOptionComplex *option,
                              QPainter *painter, const QWidget *widget,
                              bool bMouseOver, bool bHorizontal) const;
    void drawScrollBarSubLine(const QStyleOptionComplex *option,
                              QPainter *painter, const QWidget *widget,
                              bool bMouseOver, bool bHorizontal) const;
    void drawScrollBarGroove(const QStyleOptionComplex *option,
                             QPainter *painter, const QWidget *widget,
                             bool bMouseOver, bool bHorizontal) const;
    void drawArrows(QPainter *painter, QRect drawRectAdd, QRect drawRectSub,
                    Qt::Orientation Direct, bool bMouseOver) const;
    void drawSlider(QStyle::SubControls sc, QPainter *painter,
                    const QStyleOptionComplex *option, const QWidget *widget,
                    bool bHorizontal) const;

private:
    bool m_isShowArrow;
    int m_nSliderMinLength;
    int m_nSliderSize;
    int m_nArrowWidth;
    int m_nArrowHeight;
    int m_nSliderX;
    int m_nSliderY;
    int m_nArrowX;
    int m_nArrowY;
    int m_nSubSliderMinLength;
    int m_nSliderAddOffset;
    int m_nSliderSubOffset;

    QColor m_scrollBarHandleColor; // 滚动条滚柄的默认颜色
    QColor m_scrollBarHandleHoverColor; // 鼠标划过滚柄时的颜色
    QColor m_scrollBarTrackColor; // 滚动条轨道的默认颜色
    QColor m_scrollBarTrackHoverColor; // 鼠标划过轨道时的颜色

    int m_scrollBarHandleRadiusX;  // 滚动条滚柄圆角x
    int m_scrollBarHandleRadiusY;  // 滚动条滚柄圆角y

    QString m_strLeftArrowIconPath;
    QString m_strRightArrowIconPath;
    QString m_strUpArrowIconPath;
    QString m_strDownArrowIconPath;
};
#endif // GLDSCROLLSTYLE_H
