#include <QFile>
#include <QBitmap>
#include <QPainter>
#include <QPushButton>
#include <QDesktopServices>

#include "GLDStrings.h"
#include "GLDWebLogin.h"
#include "GLDFileUtils.h"
#include "GLDKeyboardInput.h"

const int c_nEditWidth                      = 180;
const int c_nSpaces                         = 6;
const int c_nMinLabelHeight                 = 23;
const int c_nKeyBoardWidth                  = 16;
const int c_nKeyBoardHeight                 = 16;
const int c_nLoginButtonWidth               = 90;
const int c_nLoginButtonHeight              = 28;
const int c_nButtonTextSize                 = 12;
const int c_nKeyBoardInputeWidth            = 359;
const int c_nKeyBoardInputeHeight           = 107;
const int c_nMaxMainSize                    = 310;

const QSize c_mainSize                      = QSize(370, 290);
const QSize c_loginTitleSize                = QSize(370, 129);
const QSize c_closeButtonSize               = QSize(40, 19);

const QMargins c_userNameContentsMargins    = QMargins(32, 15, 32, 0);
const QMargins c_rememberContentsMargins    = QMargins(94, 15, 94, 0);
const QMargins c_textMargins                = QMargins(5, 0, 5, 0);
const QMargins c_centerMargins              = QMargins(0, 15, 0, 0);
const QMargins c_errorInfoMargins           = QMargins(0, 10, 0, 5);
const QMargins c_loginButtonMargins         = QMargins(0, 0, 0, 13);
const QMargins c_mainMargins                = QMargins(0, 0, 0, 0);

const GString c_sLoginQssFile = ":/qsses/GBTPurpleStyle.qss";

GLDWebLogin::GLDWebLogin(QWidget *parent)
    : QDialog(parent),
      m_sRegisterAccountURL(GString()),
      m_sRetrievePasswordURL(GString()),
      m_loginState(seUnknow)
{
    m_edtPassword = new QLineEdit(this);
    m_edtAccount = new QLineEdit(this);
    m_btnKeyboard = new QPushButton(m_edtPassword);
    m_btnInfo = new QPushButton(this);
    m_btnLogin = new QPushButton(getGLDi18nStr(g_rsLogin), this);

    this->setStyleSheet(loadQssFile(c_sLoginQssFile));
    setUpUI();
}

GLDWebLogin::~GLDWebLogin()
{

}

void GLDWebLogin::setRegisterAccountURL(const GString &registerAccountURL)
{
    m_sRegisterAccountURL = registerAccountURL;
}

GString GLDWebLogin::registerAccountURL()
{
    return m_sRegisterAccountURL;
}

void GLDWebLogin::setRetrievePasswordURL(const GString &retrievePasswordURL)
{
    m_sRetrievePasswordURL = retrievePasswordURL;
}

GString GLDWebLogin::retrievePasswordURL()
{
    return m_sRetrievePasswordURL;
}

void GLDWebLogin::setLoginState(GLDLoginState loginState)
{
    m_loginState = loginState;
}

GLDLoginState GLDWebLogin::loginState()
{
    return m_loginState;
}

void GLDWebLogin::addTitle(QVBoxLayout *pMainlayout)
{
    QHBoxLayout *pTitlelayout = new QHBoxLayout;
    GLDLoginTitle *pLblTitle = new GLDLoginTitle(this);
    pLblTitle->setObjectName("GLDLoginTitle");
    pTitlelayout->addWidget(pLblTitle, 0, Qt::AlignTop);
    pMainlayout->addLayout(pTitlelayout);
    connect(pLblTitle, SIGNAL(sigCloseClicked()), this, SLOT(close()));
}

void GLDWebLogin::addUserName(QVBoxLayout *pCenterlayout)
{
    // 用户名区域布局
    QHBoxLayout *pAccountLayout = new QHBoxLayout;
    pAccountLayout->setContentsMargins(c_userNameContentsMargins);
    pAccountLayout->setSpacing(c_nSpaces);

    QLabel *pLblAccount = new QLabel(this);
    pLblAccount->setText(getGLDi18nStr(g_rsAccount));
    pLblAccount->setMinimumHeight(c_nMinLabelHeight);
    pAccountLayout->addWidget(pLblAccount);

    // 用户名输入框
    m_edtAccount->setObjectName("UserNameAndPassWord");
    m_edtAccount->setFixedWidth(c_nEditWidth);
    m_edtAccount->setTextMargins(c_textMargins);
    m_edtAccount->setMinimumHeight(c_nMinLabelHeight);
    m_edtAccount->setFocus();
    pAccountLayout->addWidget(m_edtAccount);

    //注册用户按钮
    QPushButton *pBtnRegister = new QPushButton(NULL, this);
    pBtnRegister->setObjectName("RegisterUser");
    pBtnRegister->setMinimumHeight(c_nMinLabelHeight);
    pBtnRegister->setCursor(Qt::PointingHandCursor);
    pBtnRegister->setText(getGLDi18nStr(g_rsRegisterAccount));
    pBtnRegister->setFlat(true);
    pAccountLayout->addWidget(pBtnRegister);
    pCenterlayout->addLayout(pAccountLayout);
    connect(pBtnRegister, SIGNAL(clicked()), this, SLOT(registerAcc()));
}

void GLDWebLogin::addPassword(QVBoxLayout *pCenterlayout)
{
    // 密码区域布局
    QHBoxLayout *pPassWordLayout = new QHBoxLayout;
    pPassWordLayout->setContentsMargins(c_userNameContentsMargins);
    pPassWordLayout->setSpacing(c_nSpaces);

    QLabel *pLblPassword = new QLabel(this);
    pLblPassword->setText(getGLDi18nStr(g_rsPassword));
    pLblPassword->setMinimumHeight(c_nMinLabelHeight);
    pPassWordLayout->addWidget(pLblPassword);

    // 密码输入框
    m_edtPassword->setObjectName("UserNameAndPassWord");
    m_edtPassword->setFixedWidth(c_nEditWidth);
    m_edtPassword->setMinimumHeight(c_nMinLabelHeight);
    m_edtPassword->setTextMargins(c_textMargins);
    m_edtPassword->setEchoMode(QLineEdit::Password);
    pPassWordLayout->addWidget(m_edtPassword);

    // 虚拟键盘,用于输入密码
    m_btnKeyboard->setFixedSize(c_nKeyBoardWidth, c_nKeyBoardHeight);
    m_btnKeyboard->setMinimumHeight(c_nKeyBoardHeight);
    m_btnKeyboard->setObjectName("VirtualKeyboard");
    m_btnKeyboard->setToolTip(getGLDi18nStr(g_rsOpenVirtualKeyboard));
    m_btnKeyboard->setFocusPolicy(Qt::NoFocus);
    m_btnKeyboard->setCursor(Qt::PointingHandCursor);
    connect(m_btnKeyboard, SIGNAL(clicked()), this, SLOT(openKeyboard()));

    // 找回密码按钮
    QPushButton *pBtnRetrievepassword = new QPushButton(NULL, this);
    pBtnRetrievepassword->setMinimumHeight(c_nMinLabelHeight);
    pBtnRetrievepassword->setObjectName("RetrivePassword");
    pBtnRetrievepassword->setCursor(Qt::PointingHandCursor);
    pPassWordLayout->addWidget(pBtnRetrievepassword);
    pBtnRetrievepassword->setText(getGLDi18nStr(g_rsGetBackPassword));
    pBtnRetrievepassword->setFlat(true);
    pCenterlayout->addLayout(pPassWordLayout);
    connect(pBtnRetrievepassword, SIGNAL(clicked()), this, SLOT(retrievePass()));
}

void GLDWebLogin::addCheckBoxes(QVBoxLayout *pCenterlayout)
{
    QHBoxLayout *rememberAndloginLayout = new QHBoxLayout;
    pCenterlayout->addLayout(rememberAndloginLayout);
    rememberAndloginLayout->setContentsMargins(c_rememberContentsMargins);

    QCheckBox *pRemeberPass = new QCheckBox(this);
    pRemeberPass->setObjectName("RememberPassWord");
    pRemeberPass->setMinimumHeight(c_nMinLabelHeight);
    rememberAndloginLayout->addWidget(pRemeberPass, 0, Qt::AlignCenter);
    pRemeberPass->setText(getGLDi18nStr(g_rsRemeberPassword));
    rememberAndloginLayout->addStretch(1);

    QCheckBox *pAutoLogin = new QCheckBox(this);
    if(pAutoLogin->checkState() == Qt::Checked)
    {
        pRemeberPass->setChecked(true);
    }
    pAutoLogin->setObjectName("RememberPassWord");
    pAutoLogin->setMinimumHeight(c_nMinLabelHeight);
    rememberAndloginLayout->addWidget(pAutoLogin, 0, Qt::AlignCenter);
    pAutoLogin->setText(getGLDi18nStr(g_rsAutoLogin));
}

void GLDWebLogin::addCenter(QVBoxLayout *pMainlayout)
{
    QVBoxLayout *pCenterlayout = new QVBoxLayout();
    pCenterlayout->setContentsMargins(c_centerMargins);
    pMainlayout->addLayout(pCenterlayout);

    // "用户名"区域水平布局
    addUserName(pCenterlayout);
    // "密码"区域水平布局
    addPassword(pCenterlayout);
    // 记住密码和自动登录
    addCheckBoxes(pCenterlayout);
}

void GLDWebLogin::addErrorInfo(QVBoxLayout *pMainlayout)
{
    QHBoxLayout *pHlyInfo = new QHBoxLayout;
    pMainlayout->addLayout(pHlyInfo);
    pHlyInfo->setContentsMargins(c_errorInfoMargins);

    m_btnInfo->setFixedHeight(c_nMinLabelHeight);
    m_btnInfo->setAttribute(Qt::WA_TranslucentBackground);
    m_btnInfo->setObjectName("ErrorInformation");
    m_btnInfo->setFlat(true);
    m_btnInfo->hide();

    pHlyInfo->addStretch();
    pHlyInfo->addWidget(m_btnInfo);
    pHlyInfo->addStretch();
}

void GLDWebLogin::addLoginButton(QVBoxLayout *pMainlayout)
{
    QHBoxLayout *pHlyButtonGroup = new QHBoxLayout;
    pHlyButtonGroup->addStretch();
    pHlyButtonGroup->setContentsMargins(c_loginButtonMargins);
    pMainlayout->addLayout(pHlyButtonGroup);

    m_btnLogin->setObjectName("Login");
    m_btnLogin->setFixedSize(c_nLoginButtonWidth, c_nLoginButtonHeight);
    m_btnLogin->setFlat(true);
    connect(m_btnLogin, SIGNAL(clicked()), this, SLOT(login()));

    //设置"登录"字体大小
    QFont font = m_btnLogin->font();
    font.setPointSize(c_nButtonTextSize);
    m_btnLogin->setFont(font);

    pHlyButtonGroup->addWidget(m_btnLogin);
    pHlyButtonGroup->addStretch();

    pMainlayout->addLayout(pHlyButtonGroup);
    setLayout(pMainlayout);
}

void GLDWebLogin::setUpUI()
{
    //FramelessWindowHint去掉标题栏;WindowStaysOnTopHint设置窗体置顶
    setFixedSize(c_mainSize);
    setObjectName("GLDWebLogin");
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    // 登录窗口主布局
    QVBoxLayout *pMainlayout = new QVBoxLayout();
    pMainlayout->setContentsMargins(c_mainMargins);
    // 登录窗口标题布局
    addTitle(pMainlayout);
    // 登录窗口中央布局
    addCenter(pMainlayout);
    // 错误信息提示
    addErrorInfo(pMainlayout);
    // 登录按钮
    addLoginButton(pMainlayout);
}

void GLDWebLogin::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        move(event->globalPos() - m_dragPoint);
        event->accept();
    }
    else
    {
        QDialog::mouseMoveEvent(event);
    }
}

void GLDWebLogin::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);

    if((NULL != m_edtPassword) && (NULL != m_btnKeyboard))
    {
        m_btnKeyboard->move(m_edtPassword->width()-m_btnKeyboard->width()-2,
                          (m_edtPassword->height()-m_btnKeyboard->height())/2);
    }
}

void GLDWebLogin::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragPoint = event->globalPos() - frameGeometry().topLeft();

        event->accept();
    }
    else
    {
        QDialog::mousePressEvent(event);
    }
}

void GLDWebLogin::login()
{
    GString strAccount = m_edtAccount->text();
    GString strPassword = m_edtPassword->text();

    if (NULL == strAccount || "" == strAccount)
    {
        showErrorNotifiy(seAccountError, getGLDi18nStr(g_rsAccountError));
        return;
    }

    if (NULL == strPassword || "" == strPassword)
    {
        showErrorNotifiy(sePasswordError, getGLDi18nStr(g_rsAccountError));
        return;
    }

    switch(loginState())
    {
    case seSuccess:
    {
        emit loginSuccess(strAccount);
        break;
    }

    case seAccountError:
    {
        showErrorNotifiy(seAccountError, getGLDi18nStr(g_rsAccountError));
        break;
    }

    case sePasswordError:
    {
        showErrorNotifiy(sePasswordError, getGLDi18nStr(g_rsAccountError));
        break;
    }

    case seNetworkError:
    {
        showErrorNotifiy(seNetworkError, getGLDi18nStr(g_rsAccountError));
        break;
    }
    }
    setUIEnable(false);
}

void GLDWebLogin::registerAcc()
{
    QDesktopServices::openUrl(QUrl(registerAccountURL()));
}

void GLDWebLogin::retrievePass()
{
    QDesktopServices::openUrl(QUrl(retrievePasswordURL()));
}

void GLDWebLogin::showErrorNotifiy(GLDLoginState error, const GString &msg)
{
    if (seSuccess == error)
    {
        accept();
    }
    else if (seNetworkError == error)
    {
        setFixedHeight(c_nMaxMainSize);
        m_btnInfo->show();
        m_btnInfo->setIcon(QIcon(":/icons/webLog/ew.png"));
        m_btnInfo->setText(msg);
    }
    else
    {
        setFixedHeight(c_nMaxMainSize);
        m_btnInfo->show();
        m_btnInfo->setIcon(QIcon(":/icons/webLog/eroo.png"));
        m_btnInfo->setText(msg);
    }

    setUIEnable(true);
}

void GLDWebLogin::setUIEnable(bool enable)
{
    m_btnLogin->setEnabled(enable);
    m_edtAccount->setEnabled(enable);
    m_edtPassword->setEnabled(enable);
}


void GLDWebLogin::openKeyboard()
{
    QPoint GlobalPoint(m_btnKeyboard->mapToGlobal(QPoint(0, 0)));
    GLDKeyboardInput *pkeyboardinput = new GLDKeyboardInput(this);
    pkeyboardinput->setGeometry(GlobalPoint.x() - c_nEditWidth, GlobalPoint.y() + c_nMinLabelHeight,
                                c_nKeyBoardInputeWidth, c_nKeyBoardInputeHeight);
    pkeyboardinput->show();
    m_edtPassword->setFocus();
}

void GLDWebLogin::onClickedBtn(const QString &string)
{
    if (NULL != string)
    {
        m_password = m_edtPassword->text();
        m_password.append(string.simplified());
        m_edtPassword->setText(m_password);
    }
}

void GLDWebLogin::onClickedDeletebtn()
{
    m_edtPassword->backspace();
}

GLDLoginTitle::GLDLoginTitle(QWidget *parent) : QLabel(parent)
{
    buildUI();
}

void GLDLoginTitle::buildUI()
{
    setFixedSize(c_loginTitleSize);
    QPushButton *pBtnClose = new QPushButton(this);
    pBtnClose->setObjectName("LoginTitleCloseButton");
    pBtnClose->setFixedSize(c_closeButtonSize);
    pBtnClose->setCursor(Qt::ArrowCursor);
    pBtnClose->setGeometry(width() - pBtnClose->width(), 0, pBtnClose->width(), pBtnClose->height());
    connect(pBtnClose, SIGNAL(clicked()), this, SIGNAL(sigCloseClicked()));
}

