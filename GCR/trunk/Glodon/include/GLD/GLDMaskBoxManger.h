#ifndef GLDMASKBOXMANGER_H
#define GLDMASKBOXMANGER_H

#include "GLDMaskBox.h"

namespace GlodonMask
{

    class GLDMASKSHARED_EXPORT GLDMaskBoxManger : public QWidget
    {
    public:
        explicit GLDMaskBoxManger(QList<QWidget *> wgtList, const QString & iniPath, QWidget* parent = nullptr);

        virtual ~GLDMaskBoxManger();

        void onNextBtnClicked();

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
