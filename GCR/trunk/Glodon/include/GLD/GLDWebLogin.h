#ifndef GLDWEBLOGIN_H
#define GLDWEBLOGIN_H

#include <QMap>
#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QCheckBox>
#include <QMouseEvent>
#include <QVBoxLayout>

#include "GLDString.h"
#include "GLDWidget_Global.h"

class QPushButton;
class GLDKeyboardInput;

enum GLDLoginState
{
    seSuccess = 0,
    seAccountError = 1,
    sePasswordError = 2,
    seUnknow = 3,
    seNetworkError =4
};

class GLDWIDGETSHARED_EXPORT GLDWebLogin : public QDialog
{
    Q_OBJECT
public:
    explicit GLDWebLogin(QWidget *parent = 0);
    ~GLDWebLogin();

public:
    void setRegisterAccountURL(const GString &registerAccountURL);
    GString registerAccountURL();
    void setRetrievePasswordURL(const GString &retrievePasswordURL);
    GString retrievePasswordURL();
    void setLoginState(GLDLoginState loginState);
    GLDLoginState loginState();

protected: 
    void setUpUI();
    void setUIEnable(bool enable);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void showErrorNotifiy(GLDLoginState error, const GString &msg);

private:
    void addTitle(QVBoxLayout *pMainlayout);
    void addCenter(QVBoxLayout *pMainlayout);
    void addErrorInfo(QVBoxLayout *pMainlayout);
    void addLoginButton(QVBoxLayout *pMainlayout);
    void addUserName(QVBoxLayout *pCenterlayout);
    void addPassword(QVBoxLayout *pCenterlayout);
    void addCheckBoxes(QVBoxLayout *pCenterlayout);

signals:
    void loginSuccess(const GString& sAccountName);

public slots:
    void login();
    void registerAcc();
    void retrievePass();
    void openKeyboard();
    void onClickedDeletebtn();
    void onClickedBtn(const QString &string);

private:
    QLineEdit *m_edtAccount;          //用户名
    QLineEdit *m_edtPassword;         //密码
    QPushButton *m_btnInfo;           //用户名或密码错误时,提示信息
    QPushButton *m_btnLogin;          //登陆按钮
    QPushButton *m_btnKeyboard;       //弹出虚拟键盘

    QPoint m_dragPoint;

    GString m_password;
    GString m_sRegisterAccountURL;
    GString m_sRetrievePasswordURL;
    GLDLoginState m_loginState;
};


class GLDWIDGETSHARED_EXPORT GLDLoginTitle : public QLabel
{
    Q_OBJECT
public:
    explicit GLDLoginTitle(QWidget *parent = 0);
    ~GLDLoginTitle() {}

signals:
    void sigCloseClicked();

private:
    void buildUI();
};
#endif // GLDWEBLOGIN_H
