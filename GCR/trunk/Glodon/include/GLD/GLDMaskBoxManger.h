#ifndef GLDMASKBOXMANGER_H
#define GLDMASKBOXMANGER_H

#include "GLDMaskBox.h"

namespace GlodonMask
{
    class GLDMASKBOXSHARED_EXPORT GLDMaskBoxManger : public QWidget
    {
    public:
        explicit GLDMaskBoxManger(QList<QWidget *> wgtList, const QString & iniPath, QWidget* parent = nullptr);

        virtual ~GLDMaskBoxManger();

        /**
         * @brief 获取第index个蒙版
         * @param index
         * @return
         */
        GLDMaskBox* get(const int index);

        /**
         * @brief 返回蒙版个数
         * @return
         */
        size_t size();

    private:

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
        * @brief 初始化提示信息列表
        * @return
        */
        void initTipInfo(const QString & iniPath);

        /**
        * @brief 解析xml文件
        * @return
        */
        void parseXML(const QString & iniPath);

        /**
        * @brief 解析xml文件中的一个节点项
        * @return
        */
        GLDGuideInfoItem parseNodeItem(QDomElement &element);

        /**
        * @brief TipWidget上下一个按钮被点击触发的操作
        * @return
        */
        void onNextBtnClicked();

        /**
         * @brief canShow
         * @return
         */
        bool canShow();

    private:
        QList<QWidget *>        m_wgtList;        // 需要设置蒙版的widget列表
        QString                 m_iniPath;        // 提示信息配置文件路径
        static int              m_step;           // 第几个蒙版
        QList<GLDTipWidget*>    m_tipInfoList;    // 提示信息列表
        QList<GLDGuideInfo>     m_guideInfoList;  // 向导信息列表
        QList<GLDMaskBox*>      m_maskBoxList;    // 蒙版列表
        QList<GLDMaskBoxParam>  m_maskParamList;  // 蒙版参数列表
    };
}

#endif // GLDMASKBOXMANGER_H
