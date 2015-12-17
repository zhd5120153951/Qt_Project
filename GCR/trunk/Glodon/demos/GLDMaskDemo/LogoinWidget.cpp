#include <QDebug>
#include <QPalette>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "GLDFileUtils.h"
#include "LogoinWidget.h"
#include "CustomDialog.h"
#include "GLDMaskBoxManger.h"

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

    QList<QWidget *> widgetLst;
    widgetLst.append(m_logoin);
    widgetLst.append(m_cancel);

    GLDMaskBoxManger * pManger = new GLDMaskBoxManger(widgetLst, exePath() + "/config/GuideInfo.xml", this);

    connect(m_logoin, &QPushButton::clicked, this, &LogoinWidget::showDialog);
}

LogoinWidget::~LogoinWidget()
{

}

void LogoinWidget::showDialog()
{
    m_dialog = new CustomDialog(this);
    m_dialog->show();
}

