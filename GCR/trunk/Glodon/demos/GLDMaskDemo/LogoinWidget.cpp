#include <QDebug>
#include <QPalette>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "GLDFileUtils.h"
#include "GLDMaskBox.h"
#include "LogoinWidget.h"
#include "CustomDialog.h"

LogoinWidget::LogoinWidget(QWidget *parent)
    : QWidget(parent)
    , m_name(nullptr)
    , m_password(nullptr)
    , m_edtName(nullptr)
    , m_edtPwd(nullptr)
    , m_logoin(nullptr)
    , m_cancel(nullptr)
    , m_dialog(nullptr)
{
    QHBoxLayout *nameLayout = new QHBoxLayout;
    m_name = new QLabel(tr("name:"), this);
    m_edtName = new QLineEdit(this);
    nameLayout->addWidget(m_name);
    nameLayout->addWidget(m_edtName);

    QHBoxLayout *pwdLayout = new QHBoxLayout;
    m_password = new QLabel(tr("password:"), this);
    m_edtPwd = new QLineEdit(this);
    pwdLayout->addWidget(m_password);
    pwdLayout->addWidget(m_edtPwd);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    m_logoin = new QPushButton(tr("login"), this);
    m_cancel = new QPushButton(tr("cancel"), this);
    btnLayout->addWidget(m_logoin);
    btnLayout->addWidget(m_cancel);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(nameLayout);
    layout->addLayout(pwdLayout);
    layout->addLayout(btnLayout);

    setLayout(layout);

    resize(900, 600);

    QPushButton *pIKnow = new QPushButton();

    QFont font = pIKnow->font();
    font.setPointSize(15);
    font.setBold(true);
    font.setFamily(QStringLiteral("华文宋体"));
    pIKnow->setFont(font);

    pIKnow->setText(QStringLiteral("我知道了"));
    pIKnow->setFixedSize(128, 40);

    pIKnow->setAutoFillBackground(true);
    QPalette palette = pIKnow->palette();
    palette.setColor(QPalette::ButtonText, QColor(Qt::white));
    palette.setColor(QPalette::Button, QColor(1, 169, 240));
    pIKnow->setPalette(palette);
    pIKnow->setFlat(true);

    pMask = GLDMaskBox::createMaskFor(m_logoin, pIKnow, exePath() + "/images/Msg/login.png", "", exePath() + "/config/config.ini");
    //connect(btn, &QPushButton::clicked, this, &LogoinWidget::showDialog);

    pMask->setArrowColor(QColor(Qt::red));
    pMask->setArrowLineWidth(2);
    connect(m_logoin, &QPushButton::clicked, this, &LogoinWidget::showDialog);
    //connect(pMask, &GLDMaskBox::customClicked, this, &LogoinWidget::showDialog);
}

LogoinWidget::~LogoinWidget()
{

}

void LogoinWidget::showDialog()
{
    //disconnect(pMask, &GLDMaskBox::customClicked, this, &LogoinWidget::showDialog);
    //pMask->close();
    m_dialog = new Dialog(this);
    m_dialog->show();
}
