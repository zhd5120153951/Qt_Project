#include "CustomDialog.h"
#include "GLDFileUtils.h"

#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

#include <QLineEdit>
#include <QTextEdit>


CustomDialog::CustomDialog(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
{
    initUI();
    setObjectName("Dialog");
    resize(1400, 800);
    setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
}

CustomDialog::~CustomDialog()
{

}

void CustomDialog::initUI()
{
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 4);
    gridLayout->setColumnStretch(2, 1);
    gridLayout->setColumnStretch(3, 1);
    gridLayout->setColumnStretch(4, 4);

    gridLayout->setMargin(15);
    gridLayout->setColumnMinimumWidth(2, 15);

    m_btn1 = new QPushButton(QWidget::tr("First Name:"), this);
    QLineEdit *edit1 = new QLineEdit(this);
    QLabel *lbl2 = new QLabel(QWidget::tr("Last Name:"), this);
    QLineEdit *edit2 = new QLineEdit(this);
    QLabel *lbl3 = new QLabel(QWidget::tr("Sex:"), this);
    QLineEdit *edit3 = new QLineEdit(this);
    QLabel *lbl4 = new QLabel(QWidget::tr("Birthday:"), this);
    QLineEdit *edit4 = new QLineEdit(this);
    QLabel *lbl5 = new QLabel(QWidget::tr("Address:"), this);
    m_textEdt = new QTextEdit(this);

    gridLayout->addWidget(m_btn1, 0, 0);
    gridLayout->addWidget(edit1, 0, 1);
    gridLayout->addWidget(lbl2, 0, 3);
    gridLayout->addWidget(edit2, 0, 4);
    gridLayout->addWidget(lbl3, 1, 0);
    gridLayout->addWidget(edit3, 1, 1);
    gridLayout->addWidget(lbl4, 1, 3);
    gridLayout->addWidget(edit4, 1, 4);
    gridLayout->addWidget(lbl5, 2, 0);
    gridLayout->addWidget(m_textEdt, 3, 0, 2, 5);

    setLayout(gridLayout);

    QPushButton *btn = new QPushButton();
    btn->setObjectName("next");

    QFont font = btn->font();
    font.setPointSize(15);
    font.setBold(true);
    font.setFamily(QStringLiteral("华文宋体"));
    btn->setFont(font);

    btn->setText(QStringLiteral("下一步"));
    btn->setFixedSize(128, 40);

    btn->setAutoFillBackground(true);
    QPalette palette = btn->palette();
    palette.setColor(QPalette::ButtonText, QColor(Qt::white));
    palette.setColor(QPalette::Button, QColor(1, 169, 240));
    btn->setPalette(palette);
    btn->setFlat(true);

    m_pMask = GLDMaskBox::createMaskFor(m_btn1, btn, exePath() + "/images/Msg/login.png", exePath() + "/images/Msg/know.png", exePath() + "/config/config.ini");

    connect(btn, &QPushButton::clicked, this, &CustomDialog::showMask);
}

void CustomDialog::showPosition()
{
    GLDMaskBox::createMaskFor(m_pNextButton, nullptr, exePath() + "/images/Msg/login.png", exePath() + "/images/Msg/know.png", exePath() + "/config/config.ini");
}

void CustomDialog::showMask()
{
    GLDMaskBox::createMaskFor(m_textEdt, nullptr, exePath() + "/images/Msg/login.png", exePath() + "/images/Msg/know.png", exePath() + "/config/config.ini");
}
