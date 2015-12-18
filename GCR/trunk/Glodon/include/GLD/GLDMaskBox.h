#ifndef GLDMASKBOX_H
#define GLDMASKBOX_H

#include "GLDTipWidget.h"
#include "GLDMask_Global.h"
#include "GLDIrregularForm.h"

#include <QList>
#include <QSettings>
#include <QtXml/QDomElement>

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

    struct GLDMaskBoxParam
    {
    public:
        GLDMaskBoxParam()
            : m_maskWidget(nullptr)
            , m_pTipWgt(nullptr)
        {

        }

        GLDMaskBoxParam& operator=(GLDMaskBoxParam& param)
        {
            m_maskWidget = param.m_maskWidget;
            m_pTipWgt = param.m_pTipWgt;
            return *this;
        }

        QWidget*      m_maskWidget;      // 需要显示蒙版的widget
        GLDTipWidget* m_pTipWgt;         // 提示信息窗体
    };

    class GLDMASKBOXSHARED_EXPORT GLDMaskBox : public QWidget
    {
        Q_OBJECT

    public:
        enum MASKCOLOR
        {
            GrayColor,      // 128, 128, 128
            GlassColor,     // 201, 120, 12
            CalaeattaColor, // 252, 239, 232
            CreamColor      // 20, 20, 20
        };

    public:
        explicit GLDMaskBox(GLDMaskBoxParam& param, QWidget * parent = nullptr);
        explicit GLDMaskBox(QList<QWidget *> wgtList, const QString & iniPath, QWidget * parent = nullptr);

        virtual ~GLDMaskBox();

        /**
        * @brief 初始化蒙版列表
        * @param widget
        */
        void initMaskList();

        /**
        * @brief 初始化蒙版参数列表
        * @param wgtList
        */
        void initMaskParamList(QList<QWidget*> wgtList);

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

        /**
         * @brief 获取蒙版参数
         * @return
         */
        GLDMaskBoxParam getMaskBoxParam();

    private:
        /**
         * @brief 计算提示信息位置
         * @return
         */
        CoordinateParam calcPosOfTipInfo();

    Q_SIGNALS:
        void nextBtnClicked();

    public slots:
        /**
         * @brief 关闭蒙版
         */
        void slotClose();

    protected:
        virtual void paintEvent(QPaintEvent *event);
        virtual bool eventFilter(QObject *watched, QEvent *event);

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

    private:
        MASKCOLOR               m_maskColor;      // 蒙版背景色

        GLDMaskBoxParam         m_oMaskBoxParam;  // 蒙版参数

        QWidget*                m_pClippedWgt;    // 需要显示蒙版的widget
        GLDTipWidget*           m_pTipWidget;     // 提示信息窗体

        QColor                  m_arrowColor;     // 箭头颜色
        int                     m_arrowLineWidth; // 箭头线条粗细
        QString                 m_iniPath;        // ini文件路径
        QString                 m_btnObjectName;  // 自定义按钮对象名

    };
}
#endif // GLDMASKBOX_H
