#include "GLDMaskBoxManger.h"
#include <assert.h>

#include <QFile>

namespace GlodonMask
{
    int GLDMaskBoxManger::m_step = 0;

    GLDMaskBoxManger::GLDMaskBoxManger(QList<QWidget *> wgtList, const QString &iniPath, QWidget *parent)
        : QWidget(parent)
        , m_wgtList(wgtList)
        , m_iniPath(iniPath)
    {
        initTipInfo(m_iniPath);
        initMaskParamList(m_wgtList);
        initMaskList();

        m_maskBoxList[m_step]->getMaskBoxParam().m_pTipWgt->setParent(m_maskBoxList[m_step]);
        m_maskBoxList[m_step]->setParent(topParentWidget(m_maskBoxList[m_step]->getMaskBoxParam().m_maskWidget));

    }

    GLDMaskBoxManger::~GLDMaskBoxManger()
    {

    }

    GLDMaskBox* GLDMaskBoxManger::get(const int index)
    {
        return m_maskBoxList[index];
    }

    size_t GLDMaskBoxManger::size()
    {
        return m_maskBoxList.size();
    }

    void GLDMaskBoxManger::initMaskList()
    {
        GLDMaskBox* pTip = nullptr;

        for (int index = 0; index < m_maskParamList.size(); ++index)
        {
            GLDMaskBoxParam param = m_maskParamList.at(index);

            QWidget* pWidget = topParentWidget(param.m_maskWidget);
            pTip = new GLDMaskBox(param, pWidget);
            m_maskBoxList.append(pTip);
        }
    }

    void GLDMaskBoxManger::initMaskParamList(QList<QWidget *> wgtList)
    {
        assert(wgtList.size() == m_tipInfoList.size());

        for (int index = 0; index < wgtList.size(); ++index)
        {
            QWidget* widget = wgtList.at(index);

            if (!widget)
            {
                break;
            }

            GLDMaskBoxParam param;
            param.m_maskWidget = widget;
            param.m_pTipWgt = m_tipInfoList.at(index);

            m_maskParamList.append(param);
        }
    }

    void GLDMaskBoxManger::initTipInfo(const QString &iniPath)
    {
        parseXML(iniPath);

        GLDTipWidget* pTipWgt = nullptr;

        foreach(GLDGuideInfo guideInfo, m_guideInfoList)
        {
            NEXTCALLBACK next = std::bind(&GLDMaskBoxManger::onNextBtnClicked, this);
            pTipWgt = new GLDTipWidget(guideInfo, next);
            m_tipInfoList.append(pTipWgt);
        }
    }

    void GLDMaskBoxManger::parseXML(const QString &iniPath)
    {
        if (nullptr == iniPath)
        {
            return;
        }

        QFile file(iniPath);
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
            return;
        }

        QString strError;
        QDomDocument document;
        int errLin = 0, errCol = 0;

        if (!document.setContent(&file, false, &strError, &errLin, &errCol))
        {
            return;
        }

        if (document.isNull())
        {
            return;
        }

        QDomElement root = document.documentElement();
        QString elementTagName = root.firstChildElement().tagName();
        QDomNodeList nodeList = root.elementsByTagName(elementTagName);

        for (int i = 0; i < nodeList.size(); ++i)
        {
            GLDGuideInfo guideInfo;

            QDomElement element = nodeList.item(i).firstChildElement();

            while (!element.isNull())
            {
                GLDGuideInfoItem guideInfoItem = parseNodeItem(element);

                if (element.tagName() == "mask")
                {
                    guideInfo.m_maskWidgetItem = guideInfoItem;
                }
                else if (element.tagName() == "close")
                {
                    guideInfo.m_closeButtonItem = guideInfoItem;
                }
                else if (element.tagName() == "next")
                {
                    guideInfo.m_nextButtonItem = guideInfoItem;
                }

                element = element.nextSiblingElement();
            }

            m_guideInfoList.append(guideInfo);
        }
    }

    GLDGuideInfoItem GLDMaskBoxManger::parseNodeItem(QDomElement &element)
    {
        int width, height, leftXpos, leftYpos;
        QString normalImage, hoverImage, pressedImage;

        if (element.hasAttribute("width"))
        {
            width = element.attributeNode("width").value().toInt();
        }

        if (element.hasAttribute("height"))
        {
            height = element.attributeNode("height").value().toInt();
        }

        if (element.hasAttribute("normalimage"))
        {
            normalImage = element.attributeNode("normalimage").value();
        }

        if (element.hasAttribute("hoverimage"))
        {
            hoverImage = element.attributeNode("hoverimage").value();
        }

        if (element.hasAttribute("pressedimage"))
        {
            pressedImage = element.attributeNode("pressedimage").value();
        }

        if (element.hasAttribute("leftXpos"))
        {
            leftXpos = element.attributeNode("leftXpos").value().toInt();
        }

        if (element.hasAttribute("leftYpos"))
        {
            leftYpos = element.attributeNode("leftYpos").value().toInt();
        }

        return GLDGuideInfoItem(width, height, leftXpos, leftYpos, normalImage, hoverImage, pressedImage);
    }

    void GLDMaskBoxManger::onNextBtnClicked()
    {
        m_maskBoxList[m_step]->close();

        m_step++;

        if (m_step >= m_maskBoxList.size())
        {
            return;
        }

        m_maskBoxList[m_step]->setParent(topParentWidget(m_maskBoxList[m_step]->getMaskBoxParam().m_maskWidget));
        m_maskBoxList[m_step]->getMaskBoxParam().m_pTipWgt->setParent(m_maskBoxList[m_step]);
        m_maskBoxList[m_step]->show();
    }

}
