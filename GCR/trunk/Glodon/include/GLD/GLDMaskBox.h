#ifndef GLDMASK_H
#define GLDMASK_H

#include "GLDMask_Global.h"
#include "GLDIrregularForm.h"
#include <windows.h>

#include <QSettings>

class QPushButton;

namespace GlodonMask
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

        QString  m_strTipPath;      // 提示信息路径
        QString  m_strBtnPath;      // 按钮路径
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
        static GLDMaskBox* createMaskFor(QWidget* widget,
            QPushButton *btn = nullptr,
            const QString & tipInfoPath = "",
            const QString & btnInfoPath = "",
            const QString & iniPath = "");

        /**
         * @brief 设置蒙版背景色
         * @param maskColor
         */
        void setMaskColor(MASKCOLOR maskColor);

        /**
         * @brief 设置箭头颜色
         * @param color
         */
        void setArrowColor(const QColor& color);

        /**
         * @brief 设置箭头线条粗细
         * @param lineWidth
         */
        void setArrowLineWidth(const int lineWidth);

        /**
         * @brief 读取ini文件
         * @param filePath
         */
        void openIniFile(const QString& filePath);

        /**
        * @brief 读取ini文件用来判断是否显示蒙版
        * @return
        */
        bool canShow();

    private:
        void setMaskShow();
        bool getMaskShow(const QString& prefix, const QString& key);

        QString getValue(const QString& prefix, const QString& key);
        void setValue(const QString& prefix, const QString& key);

        /**
         * @brief 计算提示信息位置
         * @return
         */
        CoordinateParam calcPosOfTipInfo();

    private:
        GLDMaskBox(QWidget *parent = nullptr);
        GLDMaskBox(GLDMaskBoxParam& param, QWidget * parent = nullptr);
        GLDMaskBox(GLDMaskBoxParam& param, const QString & iniPath,
            QPushButton *btn = nullptr, QWidget * parent = nullptr);
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
         * @brief 绘制指向左上角的箭头
         * @param startPoint    起点
         * @param endPoint      终点
         * @param painter
         */
        void drawLeftTopArrow(QPoint &startPoint, QPoint &endPoint, QPainter &painter);

        /**
         * @brief 绘制指向左下角的箭头
         * @param startPoint    起点
         * @param endPoint      终点
         * @param painter
         */
        void drawLeftBottomArrow(QPoint &startPoint, QPoint &endPoint, QPainter &painter);

        /**
         * @brief 绘制指向右上角的箭头
         * @param startPoint    起点
         * @param endPoint      终点
         * @param painter
         */
        void drawTopRightArrow(QPoint &startPoint, QPoint &endPoint, QPainter &painter);

        /**
         * @brief 绘制指向右下角的箭头
         * @param startPoint    起点
         * @param endPoint      终点
         * @param painter
         */
        void drawRightBottomArrow(QPoint &startPoint, QPoint &endPoint, QPainter &painter);

        /**
         * @brief 获取widget的顶级父窗口的句柄
         * @param pWidget
         */
        HWND getHandle(QWidget *pWidget);

    private:
        static GLDMaskBox*    m_pMaskBox;       // 蒙版widget

        GLDMaskBox::MASKCOLOR m_maskColor;      // 蒙版背景色

        GLDMaskBoxParam       m_oMaskBoxParam;  // 蒙版参数

        QWidget*              m_pClippedWgt;    // 需要显示蒙版的widget
        QSettings*            m_pSettings;      // ini文件设置
        GLDIrregularForm*     m_pTipBox;        // 提示信息

        bool                  m_bShowMask;      // 是否显示蒙版

        QColor                m_arrowColor;     // 箭头颜色
        int                   m_arrowLineWidth; // 箭头线条粗细
        QString               m_iniPath;        // ini文件路径
        QString               m_btnObjectName;  // 自定义按钮对象名
    };
}
#endif // GLDMASK_H
