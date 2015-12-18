#ifndef GLDMASKWIDGET_H
#define GLDMASKWIDGET_H

#include "GLDMask_Global.h"

#include <QLabel>
#include <QDialog>
#include <functional>

namespace GlodonMask
{
    typedef std::function<void()> NEXTCALLBACK;

    struct GLDGuideInfoItem
    {
        GLDGuideInfoItem(int width = -1, int height = -1, int leftXPos = -1, int leftYPos = -1,
                         QString normalImage = "", QString hoverImage = "", QString pressedImage = "")
            : m_width(width)
            , m_height(height)
            , m_leftXPos(leftXPos)
            , m_leftYPos(leftYPos)
            , m_normalImage(normalImage)
            , m_hoverImage(hoverImage)
            , m_pressedImage(pressedImage)
        {

        }

        int m_width;               // 图片宽度
        int m_height;              // 图片高度
        int m_leftXPos;            // 左上角X坐标
        int m_leftYPos;            // 左上角Y坐标

        QString m_normalImage;     // 正常情况下图片
        QString m_hoverImage;      // 鼠标划过时效果
        QString m_pressedImage;    // 鼠标按下时效果
    };

    struct GLDGuideInfo
    {
        GLDGuideInfo()
        {

        }

        GLDGuideInfo(GLDGuideInfoItem maskWidgetItem, GLDGuideInfoItem nextButtonItem,
                     GLDGuideInfoItem closeButtonItem)
            : m_maskWidgetItem(maskWidgetItem)
            , m_nextButtonItem(nextButtonItem)
            , m_closeButtonItem(closeButtonItem)
        {

        }

        GLDGuideInfoItem m_maskWidgetItem;    // 蒙版
        GLDGuideInfoItem m_nextButtonItem;    // 下一步
        GLDGuideInfoItem m_closeButtonItem;   // 关闭
    };

    class GLDMaskTitle : public QLabel
    {
        Q_OBJECT

    public:
        explicit GLDMaskTitle(QWidget *parent = 0);
        ~GLDMaskTitle();
    };


    typedef QList<GLDGuideInfo> GLDGuideInfoList;

    class GLDMASKSHARED_EXPORT GLDTipWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit GLDTipWidget(const GLDGuideInfo & guideInfo, NEXTCALLBACK goCallBack = nullptr, QWidget * parent = 0);
        ~GLDTipWidget();

        /**
         * @brief 初始化蒙版主界面
         */
        void initMaskTitle();

        /**
         * @brief 初始化下一步按钮
         */
        void initNextButton();

        /**
         * @brief 初始化关闭按钮
         */
        void initCloseButton();

        /**
         * @brief 获取下一步按钮
         * @return
         */
        QPushButton* nextBtn();

    private:

        /**
         * @brief 设置蒙版样式
         * @param guideInfo    当前蒙版页信息
         */
        void setMaskWidgetStyle(const GLDGuideInfo &guideInfo);

        /**
         * @brief 设置关闭按钮样式
         * @param guideInfo    当前蒙版页信息
         */
        void setCloseButtonStyle(const GLDGuideInfo &guideInfo);

        /**
         * @brief 设置下一步按钮样式
         * @param guideInfo    当前蒙版页信息
         */
        void setNextButtonStyle(const GLDGuideInfo &guideInfo);

        /**
         * @brief 获取蒙版样式
         * @param guideInfo    当前蒙版页信息
         * @return
         */
        QString maskStyleSheet(const GLDGuideInfo &guideInfo);

        /**
         * @brief 获取关闭按钮样式
         * @param guideInfo    当前蒙版页信息
         * @return
         */
        QString closeStyleSheet(const GLDGuideInfo &guideInfo);

        /**
         * @brief 获取下一步按钮样式
         * @param guideInfo    当前蒙版页信息
         * @return
         */
        QString nextStyleSheet(const GLDGuideInfo &guideInfo);

    signals:
        void tipWidgetClicked();

    private slots:
        /**
         * @brief 关闭蒙版界面
         */
        void closeMaskWidget();

        /**
         * @brief 下一步按钮被点击
         */
        void nextButtonClicked();

    private:
        GLDMaskTitle*       m_pMaskTitle;          // 蒙版图片
        QPushButton*        m_pNextButton;         // 下一步按钮
        QPushButton*        m_pCloseButton;        // 关闭按钮
        NEXTCALLBACK        m_goCallBack;
    };
}

#endif // GLDMASKWIDGET_H
