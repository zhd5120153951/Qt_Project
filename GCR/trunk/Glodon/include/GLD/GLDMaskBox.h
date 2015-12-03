#ifndef GLDMASK_H
#define GLDMASK_H

#include "GLDMask_Global.h"
#include "GLDIrregularForm.h"

#include <QPoint>
#include <QSettings>

namespace GLDCBB
{
    static QWidget* topParentWidget(QWidget* pWgt)
    {
        if (!pWgt)
        {
            return nullptr;
        }

        QWidget* widget = nullptr;

        for (widget = pWgt; widget != nullptr; widget = widget->parentWidget())
        {
            if (widget->isWindow())
            {
                break;
            }
        }

        return widget;
    }
}

struct CoordinateParam
{
public:
    enum Quadrant
    {
        Zero,
        First,
        Second,
        Third,
        Fourth
    };

    CoordinateParam()
        : m_point(-1, -1)
        , m_quadrant(Zero)
    {

    }

    QPoint                     m_point;
    CoordinateParam::Quadrant  m_quadrant;
};

class GLDMaskBoxParam
{
public:
    GLDMaskBoxParam()
        : m_strTipPath("")
        , m_strBtnPath("")
        , m_maskWidget(nullptr)
    {

    }

    GLDMaskBoxParam& operator=(GLDMaskBoxParam& param)
    {
        m_strTipPath = param.m_strTipPath;
        m_strBtnPath = param.m_strBtnPath;
        m_maskWidget = param.m_maskWidget;
        return *this;
    }

    QString  m_strTipPath;      // 提示信息
    QString  m_strBtnPath;      // 按钮
    QWidget* m_maskWidget;      // 需要显示蒙版的widget
};

class GLDMASKSHARED_EXPORT GLDMaskBox : public QWidget
{
    Q_OBJECT

public:
    enum MASKCOLOR
    {
        GrayColor,      // 128, 128, 128
        GlassColor,     // 201, 120, 12
        CalaeattaColor, // 252, 239, 232
        CreamColor      // 233, 241, 246
    };

public:
    static GLDMaskBox* createMaskFor(QWidget* widget, const QString & tipInfoPath = "", const QString & btnInfoPath = "");

    /**
     * @brief 设置蒙版颜色
     * @param maskColor
     */
    void setMaskColor(MASKCOLOR maskColor);

    /**
     * @brief 设置箭头颜色
     * @param color
     */
    void setArrowColor(const QColor& color);

    /**
     * @brief 设置箭头曲线宽度
     * @param lineWidth
     */
    void setArrowLineWidth(const int lineWidth);

    /**
     * @brief 读取ini文件用于判断是否需要显示蒙版
     * @param filePath
     */
    void openIniFile(const QString& filePath);

private:
    void setMaskShow();
    bool getMaskShow(const QString& prefix, const QString& key);

    QString getValue(const QString& prefix, const QString& key);
    void setValue(const QString& prefix, const QString& key);

    QPoint calcPosOfOwner();

    /**
     * @brief 计算提示信息位置
     * @return
     */
    CoordinateParam calcPosOfTipInfo();

private:
    GLDMaskBox(QWidget *parent = nullptr);
    GLDMaskBox(GLDMaskBoxParam& param, QWidget * parent = nullptr);
    virtual ~GLDMaskBox();

Q_SIGNALS:
    void customClicked();

public slots:
    /**
     * @brief 关闭蒙版
     */
    void slotClose();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    /**
     * @brief 绘制蒙版
     * @param painter
     */
    void drawMask(QPainter & painter);

    /**
     * @brief 绘制指向左上角的箭头,即贝塞尔曲线
     * @param ownerPoint    起点
     * @param endPoint      终点
     * @param painter
     */
    void drawLeftTopArrow(QPoint &startPoint, QPoint &endPoint, QPainter &painter);

    /**
     * @brief 绘制指向左下角的箭头,即贝塞尔曲线
     * @param ownerPoint    起点
     * @param endPoint      终点
     * @param painter
     */
    void drawLeftBottomArrow(QPoint &startPoint, QPoint &endPoint, QPainter &painter);

private:
    static GLDMaskBox*    m_pMaskBox;

    GLDMaskBox::MASKCOLOR m_maskColor;

    GLDMaskBoxParam       m_oMaskBoxParam;

    QWidget*              m_pClippedWgt;
    QSettings*            m_pSettings;
    GLDIrregularForm*     m_pTipBox;

    bool                  m_bShowMask;

    QColor                m_arrowColor;
    int                   m_arrowLineWidth;
};

#endif // GLDMASK_H
