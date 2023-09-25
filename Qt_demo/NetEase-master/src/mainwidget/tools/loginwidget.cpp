#include "loginwidget.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

LogInWidget::LogInWidget(QWidget *parent)
    : QDialog(parent)
{
    setFixedSize(250, 100);

    QLabel *userNameLabel = new QLabel(QStringLiteral("�û���: "));
    m_nameLineEdit = new QLineEdit(this);
    m_nameLineEdit->setFixedWidth(150);

    QLabel *passwdLabel = new QLabel(QStringLiteral("��  ��: "));
    m_passwdLineEdit = new QLineEdit(this);
    m_passwdLineEdit->setFixedWidth(150);
    m_passwdLineEdit->setEchoMode(QLineEdit::Password);

    m_okButton = new QPushButton(QStringLiteral("ȷ��"));
    m_okButton->setFixedWidth(100);
    m_cancelButton = new QPushButton(QStringLiteral("ȡ��"));
    m_cancelButton->setFixedWidth(100);

    connect(m_okButton, SIGNAL(clicked()), this, SLOT(on_logIn_clicked()));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *userLayout = new QHBoxLayout;
    userLayout->addWidget(userNameLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    userLayout->addSpacing(5);
    userLayout->addWidget(m_nameLineEdit, 0, Qt::AlignRight | Qt::AlignVCenter);
    userLayout->setContentsMargins(5, 0, 5, 0);

    QHBoxLayout *passwdLayout = new QHBoxLayout;
    passwdLayout->addWidget(passwdLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    passwdLayout->addSpacing(5);
    passwdLayout->addWidget(m_passwdLineEdit, 0, Qt::AlignRight | Qt::AlignVCenter);
    passwdLayout->setContentsMargins(5, 0, 5, 0);

    QHBoxLayout *ubttonLayout = new QHBoxLayout;
    ubttonLayout->addWidget(m_okButton, 0, Qt::AlignLeft | Qt::AlignVCenter);
    ubttonLayout->addSpacing(5);
    ubttonLayout->addWidget(m_cancelButton, 0, Qt::AlignRight | Qt::AlignVCenter);
    ubttonLayout->setContentsMargins(5, 0, 5, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(userLayout);
    mainLayout->addLayout(passwdLayout);
    mainLayout->addLayout(ubttonLayout);
    mainLayout->setContentsMargins(5, 5, 5, 5);
}

LogInWidget::~LogInWidget()
{

}

void LogInWidget::on_logIn_clicked()
{
    QString name = m_nameLineEdit->text();
    QString passwd = m_passwdLineEdit->text();

    if (name == "" || passwd == "")
    {
        QMessageBox::warning(this, QStringLiteral("��Ϣ����"),
                             QStringLiteral("�û��������벻��Ϊ��..."),
                             QMessageBox::Ok | QMessageBox::Cancel);
        return;
    }

    emit logInClicked(name, passwd);
}

