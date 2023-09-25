#include "addmusiclist.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>

AddMusicList::AddMusicList(QWidget *parent)
    : QDialog(parent)
{
    m_okButton = new QPushButton(QStringLiteral("ȷ��"));
    m_cancelButton = new QPushButton(QStringLiteral("ȡ��"));
    m_nameLineEdit = new QLineEdit;
    m_nameLineEdit->setFixedWidth(180);
    m_pathLineEdit = new QLineEdit;
    m_pathLineEdit->setFixedWidth(130);
    m_checkPathButton = new QPushButton(QStringLiteral("���"));
    m_checkPathButton->setFixedWidth(40);

    connect(m_okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_checkPathButton, SIGNAL(clicked()), this, SLOT(checkButtonClicked()));

    QLabel *nameLabel = new QLabel(QStringLiteral("�赥����"));
    QLabel *pathLabel = new QLabel(QStringLiteral("����·��"));

    QHBoxLayout *name = new QHBoxLayout;
    name->addWidget(nameLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    name->addSpacing(5);
    name->addWidget(m_nameLineEdit, 0, Qt::AlignVCenter);
    name->setContentsMargins(5, 5, 5, 5);

    QHBoxLayout *path = new QHBoxLayout;
    path->addWidget(pathLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    path->addSpacing(5);
    path->addWidget(m_pathLineEdit, Qt::AlignVCenter);
    path->addWidget(m_checkPathButton, 0, Qt::AlignRight | Qt::AlignVCenter);
    path->setContentsMargins(5, 5, 5, 5);

    QHBoxLayout *button = new QHBoxLayout;
    button->addWidget(m_okButton, 0, Qt::AlignVCenter | Qt::AlignHCenter);
    button->addSpacing(10);
    button->addWidget(m_cancelButton, 0, Qt::AlignVCenter | Qt::AlignHCenter);
    button->setContentsMargins(5, 5, 5, 5);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(name);
    mainLayout->addLayout(path);
    mainLayout->addLayout(button);
    mainLayout->setContentsMargins(5, 5, 5, 5);
}

AddMusicList::~AddMusicList()
{

}

void AddMusicList::okButtonClicked()
{
    if (m_nameLineEdit->text() == "" || m_pathLineEdit->text() == "")
    {
        QMessageBox::warning(this, QStringLiteral("����"),
                             QStringLiteral("�赥���͸���·������Ϊ��..."),
                             QMessageBox::Ok | QMessageBox::Cancel);
        return;
    }

    m_okButton->setEnabled(false);
    emit okButtonClicked(m_nameLineEdit->text(), m_files);
    this->accept();
}

void AddMusicList::checkButtonClicked()
{
    QString path = QDir::currentPath();
    m_files = QFileDialog::getOpenFileNames(this, QStringLiteral("��������ļ�"),
                                            path, QStringLiteral("MP3 �ļ� (*.mp3);;ȫ���ļ� (*.*)"));

    if (!m_files.isEmpty())
    {
        QString str;
        for (int i = 0; i < m_files.count(); i++)
            str.append(m_files.at(i)).append(";");
        m_pathLineEdit->setText(str);
    }
    else
    {
        m_pathLineEdit->setText("");
    }
}
