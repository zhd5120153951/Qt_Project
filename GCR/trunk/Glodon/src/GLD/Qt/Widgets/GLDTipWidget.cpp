#include "GLDTipWidget.h"
#include "GLDFileUtils.h"

#include <QFile>
#include <QDomAttr>
#include <QSettings>
#include <QMouseEvent>
#include <QPushButton>
#include <QDomElement>
#include <QDomDocument>
#include <QDesktopWidget>

namespace GlodonMask
{

    GLDTipWidget::GLDTipWidget(QWidget *parent)
        : QWidget(parent)
        , m_step(0)
        , m_pMaskTitle(nullptr)
        , m_pNextButton(nullptr)
        , m_pCloseButton(nullptr)
    {
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TranslucentBackground);
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

        initMaskTitle();

        initNextButton();

        initCloseButton();
    }

    GLDTipWidget::GLDTipWidget(const GLDGuideInfo & guideInfo, NEXTCALLBACK goCallBack, QWidget * parent)
        : QWidget(parent)
        , m_pMaskTitle(nullptr)
        , m_pNextButton(nullptr)
        , m_pCloseButton(nullptr)
        , m_goCallBack(goCallBack)

    {
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TranslucentBackground);
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

        initMaskTitle();

        initNextButton();

        initCloseButton();

        // 蒙版
        setMaskWidgetStyle(guideInfo);

        // 关闭按钮
        setCloseButtonStyle(guideInfo);

        // 下一个按钮
        setNextButtonStyle(guideInfo);
    }

    GLDTipWidget::~GLDTipWidget()
    {

    }

    void GLDTipWidget::initMaskTitle()
    {
        m_pMaskTitle = new GLDMaskTitle(this);
        m_pMaskTitle->setObjectName("m_pMaskTitle");
        m_pMaskTitle->setGeometry(-1, -1, -1, -1);
        m_pMaskTitle->setAutoFillBackground(true);
    }

    void GLDTipWidget::initNextButton()
    {
        m_pNextButton = new QPushButton(this);
        m_pNextButton->setObjectName("m_pGuideNextButton");
        m_pNextButton->setGeometry(-1, -1, -1, -1);
        m_pNextButton->setAutoFillBackground(true);

        connect(m_pNextButton, &QPushButton::clicked, this, &GLDTipWidget::nextButtonClicked);
    }

    void GLDTipWidget::initCloseButton()
    {
        m_pCloseButton = new QPushButton(this);
        m_pCloseButton->setObjectName("m_pCloseButton");
        m_pCloseButton->setGeometry(-1, -1, -1, -1);
        m_pCloseButton->setAutoFillBackground(true);

        connect(m_pCloseButton, &QPushButton::clicked, this, &GLDTipWidget::tipWidgetClicked);
    }

    QPushButton *GLDTipWidget::nextBtn()
    {
        return m_pNextButton;
    }

    void GLDTipWidget::setMaskWidgetStyle(const GLDGuideInfo &guideInfo)
    {
        if (guideInfo.m_maskWidgetItem.m_normalImage != "")
        {
            const GString strMaskStyle = maskStyleSheet(guideInfo);
            m_pMaskTitle->setStyleSheet(strMaskStyle);
        }
        else
        {
            hide();
        }

        if (guideInfo.m_maskWidgetItem.m_leftXPos != -1 && guideInfo.m_maskWidgetItem.m_leftYPos != -1)
        {
            m_pMaskTitle->move(0, 0);
        }

        if (guideInfo.m_maskWidgetItem.m_width != -1 && guideInfo.m_maskWidgetItem.m_height != -1)
        {
            this->setFixedSize(guideInfo.m_maskWidgetItem.m_width, guideInfo.m_maskWidgetItem.m_height);
            m_pMaskTitle->setFixedSize(guideInfo.m_maskWidgetItem.m_width, guideInfo.m_maskWidgetItem.m_height);
        }
    }

    void GLDTipWidget::setCloseButtonStyle(const GLDGuideInfo &guideInfo)
    {
        if (guideInfo.m_closeButtonItem.m_normalImage != "")
        {
            const GString strCloseStyle = closeStyleSheet(guideInfo);
            m_pCloseButton->setStyleSheet(strCloseStyle);
        }

        if (guideInfo.m_closeButtonItem.m_leftXPos != -1 && guideInfo.m_closeButtonItem.m_leftYPos != -1)
        {
            m_pCloseButton->move(guideInfo.m_closeButtonItem.m_leftXPos,
                guideInfo.m_closeButtonItem.m_leftYPos);
        }

        if (guideInfo.m_closeButtonItem.m_width != -1 && guideInfo.m_closeButtonItem.m_height != -1)
        {
            m_pCloseButton->setFixedSize(guideInfo.m_closeButtonItem.m_width,
                guideInfo.m_closeButtonItem.m_height);
        }
    }

    void GLDTipWidget::setNextButtonStyle(const GLDGuideInfo &guideInfo)
    {
        if (guideInfo.m_nextButtonItem.m_normalImage != "")
        {
            const GString strNextStyle = nextStyleSheet(guideInfo);
            m_pNextButton->setStyleSheet(strNextStyle);
        }

        if (guideInfo.m_nextButtonItem.m_leftXPos != -1 && guideInfo.m_nextButtonItem.m_leftYPos != -1)
        {
            m_pNextButton->move(guideInfo.m_nextButtonItem.m_leftXPos,
                guideInfo.m_nextButtonItem.m_leftYPos);
        }

        if (guideInfo.m_nextButtonItem.m_width != -1 && guideInfo.m_nextButtonItem.m_height != -1)
        {
            m_pNextButton->setFixedSize(guideInfo.m_nextButtonItem.m_width,
                guideInfo.m_nextButtonItem.m_height);
        }
    }

    GString GLDTipWidget::maskStyleSheet(const GLDGuideInfo &guideInfo)
    {
        return GString("QLabel#m_pMaskTitle"
            "{"
            "border-image: url(" + guideInfo.m_maskWidgetItem.m_normalImage + ");"
            "}"
            "QLabel#m_pMaskTitle:hover"
            "{"
            "border-image: url(" + guideInfo.m_maskWidgetItem.m_hoverImage + ");"
            "}"
            "QLabel#m_pMaskTitle:pressed"
            "{"
            "border-image: url(" + guideInfo.m_maskWidgetItem.m_pressedImage + ");"
            "}");
    }

    GString GLDTipWidget::closeStyleSheet(const GLDGuideInfo &guideInfo)
    {
        return GString("QPushButton#m_pCloseButton"
            "{"
            "border-image: url(" + guideInfo.m_closeButtonItem.m_normalImage + ");"
            "}"
            "QPushButton#m_pCloseButton:hover"
            "{"
            "border-image: url(" + guideInfo.m_closeButtonItem.m_hoverImage + ");"
            "}"
            "QPushButton#m_pCloseButton:pressed"
            "{"
            "border-image: url(" + guideInfo.m_closeButtonItem.m_pressedImage + ");"
            "}");
    }

    GString GLDTipWidget::nextStyleSheet(const GLDGuideInfo &guideInfo)
    {
        return GString("QPushButton#m_pGuideNextButton"
            "{"
            "border-image: url(" + guideInfo.m_nextButtonItem.m_normalImage + ");"
            "}"
            "QPushButton#m_pGuideNextButton:hover"
            "{"
            "border-image: url(" + guideInfo.m_nextButtonItem.m_hoverImage + ");"
            "}"
            "QPushButton#m_pGuideNextButton:pressed"
            "{"
            "border-image: url(" + guideInfo.m_nextButtonItem.m_pressedImage + ");"
            "}");
    }

    void GLDTipWidget::nextButtonClicked()
    {
        if (nullptr != m_goCallBack)
        {
            m_goCallBack();
        }
    }

    void GLDTipWidget::closeMaskWidget()
    {
        QSettings oInis("E:/repository/qt demo/GCR/trunk/Glodon/demos/GLDMaskDemo/bin/config/NewGuide.ini", QSettings::IniFormat);
        oInis.setValue("GLDMaskWidget", 1);
        emit tipWidgetClicked();
    }

    GLDMaskTitle::GLDMaskTitle(QWidget *parent)
        : QLabel(parent)
    {

    }

    GLDMaskTitle::~GLDMaskTitle()
    {

    }
}
