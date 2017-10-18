/********************************************************************************
** Form generated from reading UI file 'frmconfig.ui'
**
** Created by: Qt User Interface Compiler version 5.1.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMCONFIG_H
#define UI_FRMCONFIG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <switchbutton.h>

QT_BEGIN_NAMESPACE

class Ui_frmConfig
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget_title;
    QHBoxLayout *horizontalLayout_2;
    QLabel *lab_Ico;
    QLabel *lab_Title;
    QWidget *widget_menu;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnMenu_Close;
    QWidget *widget_main;
    QGroupBox *groupBox;
    QLineEdit *txtAppTitle;
    QLabel *labAppTitle;
    QLabel *labNVRType;
    QLineEdit *txtNVRType;
    QLineEdit *txtIPCType;
    QLabel *labIPCType;
    QComboBox *cboxRtspType;
    QLabel *labRtspType;
    SwitchButton *btnAutoPoll;
    QLabel *labAutoPoll;
    QLabel *labPollType;
    QComboBox *cboxPollType;
    QLabel *labAutoRun;
    SwitchButton *btnAutoRun;
    QComboBox *cboxPollInterval;
    QLabel *labPollInterval;
    QLabel *labMsec;
    QLabel *labUseVideoOffLineCheck;
    QLabel *labVideoOffLineCheckInterval;
    SwitchButton *btnUseVideoOffLineCheck;
    QLabel *labSec;
    QComboBox *cboxDelayTime;
    QLabel *labDelayTime;
    QComboBox *cboxVideoOffLineCheckInterval;
    QLabel *labSec2;
    QLabel *labSec1;
    QComboBox *cboxPollSleep;
    QLabel *labPollSleep;
    QPushButton *btnClearNVR;
    QPushButton *btnClearIPC;
    QPushButton *btnClearPoll;
    QPushButton *btnClearAll;
    QPushButton *btnOk;
    QPushButton *btnCancel;

    void setupUi(QDialog *frmConfig)
    {
        if (frmConfig->objectName().isEmpty())
            frmConfig->setObjectName(QStringLiteral("frmConfig"));
        frmConfig->resize(654, 404);
        verticalLayout = new QVBoxLayout(frmConfig);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        widget_title = new QWidget(frmConfig);
        widget_title->setObjectName(QStringLiteral("widget_title"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_title->sizePolicy().hasHeightForWidth());
        widget_title->setSizePolicy(sizePolicy);
        widget_title->setMinimumSize(QSize(100, 28));
        horizontalLayout_2 = new QHBoxLayout(widget_title);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        lab_Ico = new QLabel(widget_title);
        lab_Ico->setObjectName(QStringLiteral("lab_Ico"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lab_Ico->sizePolicy().hasHeightForWidth());
        lab_Ico->setSizePolicy(sizePolicy1);
        lab_Ico->setMinimumSize(QSize(31, 0));
        lab_Ico->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(lab_Ico);

        lab_Title = new QLabel(widget_title);
        lab_Title->setObjectName(QStringLiteral("lab_Title"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lab_Title->sizePolicy().hasHeightForWidth());
        lab_Title->setSizePolicy(sizePolicy2);
        lab_Title->setStyleSheet(QString::fromUtf8("font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));
        lab_Title->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(lab_Title);

        widget_menu = new QWidget(widget_title);
        widget_menu->setObjectName(QStringLiteral("widget_menu"));
        sizePolicy1.setHeightForWidth(widget_menu->sizePolicy().hasHeightForWidth());
        widget_menu->setSizePolicy(sizePolicy1);
        horizontalLayout = new QHBoxLayout(widget_menu);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        btnMenu_Close = new QPushButton(widget_menu);
        btnMenu_Close->setObjectName(QStringLiteral("btnMenu_Close"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
        btnMenu_Close->setSizePolicy(sizePolicy3);
        btnMenu_Close->setMinimumSize(QSize(40, 0));
        btnMenu_Close->setCursor(QCursor(Qt::ArrowCursor));
        btnMenu_Close->setFocusPolicy(Qt::NoFocus);
        btnMenu_Close->setFlat(true);

        horizontalLayout->addWidget(btnMenu_Close);


        horizontalLayout_2->addWidget(widget_menu);


        verticalLayout->addWidget(widget_title);

        widget_main = new QWidget(frmConfig);
        widget_main->setObjectName(QStringLiteral("widget_main"));
        widget_main->setStyleSheet(QString::fromUtf8("font: 11pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));
        groupBox = new QGroupBox(widget_main);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 631, 311));
        txtAppTitle = new QLineEdit(groupBox);
        txtAppTitle->setObjectName(QStringLiteral("txtAppTitle"));
        txtAppTitle->setGeometry(QRect(100, 20, 511, 28));
        labAppTitle = new QLabel(groupBox);
        labAppTitle->setObjectName(QStringLiteral("labAppTitle"));
        labAppTitle->setGeometry(QRect(20, 20, 71, 31));
        labNVRType = new QLabel(groupBox);
        labNVRType->setObjectName(QStringLiteral("labNVRType"));
        labNVRType->setGeometry(QRect(20, 60, 71, 31));
        txtNVRType = new QLineEdit(groupBox);
        txtNVRType->setObjectName(QStringLiteral("txtNVRType"));
        txtNVRType->setGeometry(QRect(100, 60, 511, 28));
        txtIPCType = new QLineEdit(groupBox);
        txtIPCType->setObjectName(QStringLiteral("txtIPCType"));
        txtIPCType->setGeometry(QRect(100, 100, 511, 28));
        labIPCType = new QLabel(groupBox);
        labIPCType->setObjectName(QStringLiteral("labIPCType"));
        labIPCType->setGeometry(QRect(20, 100, 71, 31));
        cboxRtspType = new QComboBox(groupBox);
        cboxRtspType->setObjectName(QStringLiteral("cboxRtspType"));
        cboxRtspType->setGeometry(QRect(290, 140, 101, 28));
        labRtspType = new QLabel(groupBox);
        labRtspType->setObjectName(QStringLiteral("labRtspType"));
        labRtspType->setGeometry(QRect(210, 140, 71, 31));
        btnAutoPoll = new SwitchButton(groupBox);
        btnAutoPoll->setObjectName(QStringLiteral("btnAutoPoll"));
        btnAutoPoll->setGeometry(QRect(100, 180, 87, 28));
        labAutoPoll = new QLabel(groupBox);
        labAutoPoll->setObjectName(QStringLiteral("labAutoPoll"));
        labAutoPoll->setGeometry(QRect(20, 180, 71, 31));
        labPollType = new QLabel(groupBox);
        labPollType->setObjectName(QStringLiteral("labPollType"));
        labPollType->setGeometry(QRect(210, 180, 71, 31));
        cboxPollType = new QComboBox(groupBox);
        cboxPollType->setObjectName(QStringLiteral("cboxPollType"));
        cboxPollType->setGeometry(QRect(290, 180, 101, 28));
        labAutoRun = new QLabel(groupBox);
        labAutoRun->setObjectName(QStringLiteral("labAutoRun"));
        labAutoRun->setGeometry(QRect(20, 140, 71, 31));
        btnAutoRun = new SwitchButton(groupBox);
        btnAutoRun->setObjectName(QStringLiteral("btnAutoRun"));
        btnAutoRun->setGeometry(QRect(100, 140, 87, 28));
        cboxPollInterval = new QComboBox(groupBox);
        cboxPollInterval->setObjectName(QStringLiteral("cboxPollInterval"));
        cboxPollInterval->setGeometry(QRect(490, 180, 71, 28));
        labPollInterval = new QLabel(groupBox);
        labPollInterval->setObjectName(QStringLiteral("labPollInterval"));
        labPollInterval->setGeometry(QRect(410, 180, 71, 31));
        labMsec = new QLabel(groupBox);
        labMsec->setObjectName(QStringLiteral("labMsec"));
        labMsec->setGeometry(QRect(570, 220, 31, 31));
        labUseVideoOffLineCheck = new QLabel(groupBox);
        labUseVideoOffLineCheck->setObjectName(QStringLiteral("labUseVideoOffLineCheck"));
        labUseVideoOffLineCheck->setGeometry(QRect(20, 220, 71, 31));
        labVideoOffLineCheckInterval = new QLabel(groupBox);
        labVideoOffLineCheckInterval->setObjectName(QStringLiteral("labVideoOffLineCheckInterval"));
        labVideoOffLineCheckInterval->setGeometry(QRect(210, 220, 71, 31));
        btnUseVideoOffLineCheck = new SwitchButton(groupBox);
        btnUseVideoOffLineCheck->setObjectName(QStringLiteral("btnUseVideoOffLineCheck"));
        btnUseVideoOffLineCheck->setGeometry(QRect(100, 220, 87, 28));
        labSec = new QLabel(groupBox);
        labSec->setObjectName(QStringLiteral("labSec"));
        labSec->setGeometry(QRect(370, 220, 21, 31));
        cboxDelayTime = new QComboBox(groupBox);
        cboxDelayTime->setObjectName(QStringLiteral("cboxDelayTime"));
        cboxDelayTime->setGeometry(QRect(490, 220, 71, 28));
        labDelayTime = new QLabel(groupBox);
        labDelayTime->setObjectName(QStringLiteral("labDelayTime"));
        labDelayTime->setGeometry(QRect(410, 220, 71, 31));
        cboxVideoOffLineCheckInterval = new QComboBox(groupBox);
        cboxVideoOffLineCheckInterval->setObjectName(QStringLiteral("cboxVideoOffLineCheckInterval"));
        cboxVideoOffLineCheckInterval->setGeometry(QRect(290, 220, 71, 28));
        labSec2 = new QLabel(groupBox);
        labSec2->setObjectName(QStringLiteral("labSec2"));
        labSec2->setGeometry(QRect(570, 180, 21, 31));
        labSec1 = new QLabel(groupBox);
        labSec1->setObjectName(QStringLiteral("labSec1"));
        labSec1->setGeometry(QRect(570, 140, 31, 31));
        cboxPollSleep = new QComboBox(groupBox);
        cboxPollSleep->setObjectName(QStringLiteral("cboxPollSleep"));
        cboxPollSleep->setGeometry(QRect(490, 140, 71, 28));
        labPollSleep = new QLabel(groupBox);
        labPollSleep->setObjectName(QStringLiteral("labPollSleep"));
        labPollSleep->setGeometry(QRect(410, 140, 71, 31));
        btnClearNVR = new QPushButton(groupBox);
        btnClearNVR->setObjectName(QStringLiteral("btnClearNVR"));
        btnClearNVR->setGeometry(QRect(20, 260, 141, 32));
        btnClearNVR->setCursor(QCursor(Qt::PointingHandCursor));
        btnClearIPC = new QPushButton(groupBox);
        btnClearIPC->setObjectName(QStringLiteral("btnClearIPC"));
        btnClearIPC->setGeometry(QRect(170, 260, 141, 32));
        btnClearIPC->setCursor(QCursor(Qt::PointingHandCursor));
        btnClearPoll = new QPushButton(groupBox);
        btnClearPoll->setObjectName(QStringLiteral("btnClearPoll"));
        btnClearPoll->setGeometry(QRect(320, 260, 141, 32));
        btnClearPoll->setCursor(QCursor(Qt::PointingHandCursor));
        btnClearAll = new QPushButton(groupBox);
        btnClearAll->setObjectName(QStringLiteral("btnClearAll"));
        btnClearAll->setGeometry(QRect(470, 260, 141, 32));
        btnClearAll->setCursor(QCursor(Qt::PointingHandCursor));
        btnOk = new QPushButton(widget_main);
        btnOk->setObjectName(QStringLiteral("btnOk"));
        btnOk->setGeometry(QRect(450, 330, 91, 32));
        btnOk->setMinimumSize(QSize(0, 28));
        btnOk->setCursor(QCursor(Qt::PointingHandCursor));
        btnOk->setStyleSheet(QStringLiteral(""));
        QIcon icon;
        icon.addFile(QStringLiteral(":/image/update.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnOk->setIcon(icon);
        btnOk->setIconSize(QSize(20, 20));
        btnCancel = new QPushButton(widget_main);
        btnCancel->setObjectName(QStringLiteral("btnCancel"));
        btnCancel->setGeometry(QRect(550, 330, 91, 32));
        btnCancel->setMinimumSize(QSize(0, 28));
        btnCancel->setCursor(QCursor(Qt::PointingHandCursor));
        btnCancel->setStyleSheet(QStringLiteral(""));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/image/close.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnCancel->setIcon(icon1);
        btnCancel->setIconSize(QSize(20, 20));

        verticalLayout->addWidget(widget_main);


        retranslateUi(frmConfig);

        QMetaObject::connectSlotsByName(frmConfig);
    } // setupUi

    void retranslateUi(QDialog *frmConfig)
    {
        frmConfig->setWindowTitle(QApplication::translate("frmConfig", "\347\263\273\347\273\237\350\256\276\347\275\256", 0));
        lab_Ico->setText(QString());
        lab_Title->setText(QApplication::translate("frmConfig", "\347\263\273\347\273\237\350\256\276\347\275\256", 0));
#ifndef QT_NO_TOOLTIP
        btnMenu_Close->setToolTip(QApplication::translate("frmConfig", "\345\205\263\351\227\255", 0));
#endif // QT_NO_TOOLTIP
        btnMenu_Close->setText(QString());
        groupBox->setTitle(QString());
        labAppTitle->setText(QApplication::translate("frmConfig", "\350\275\257\344\273\266\346\240\207\351\242\230", 0));
        labNVRType->setText(QApplication::translate("frmConfig", "NVR\345\216\202\345\225\206", 0));
        labIPCType->setText(QApplication::translate("frmConfig", "IPC\345\216\202\345\225\206", 0));
        cboxRtspType->clear();
        cboxRtspType->insertItems(0, QStringList()
         << QApplication::translate("frmConfig", "\344\270\273\347\240\201\346\265\201", 0)
         << QApplication::translate("frmConfig", "\345\255\220\347\240\201\346\265\201", 0)
        );
        labRtspType->setText(QApplication::translate("frmConfig", "\347\240\201\346\265\201\347\261\273\345\236\213", 0));
        btnAutoPoll->setText(QString());
        labAutoPoll->setText(QApplication::translate("frmConfig", "\350\207\252\345\212\250\350\275\256\350\257\242", 0));
        labPollType->setText(QApplication::translate("frmConfig", "\350\275\256\350\257\242\347\224\273\351\235\242", 0));
        cboxPollType->clear();
        cboxPollType->insertItems(0, QStringList()
         << QApplication::translate("frmConfig", "1\347\224\273\351\235\242", 0)
         << QApplication::translate("frmConfig", "4\347\224\273\351\235\242", 0)
         << QApplication::translate("frmConfig", "9\347\224\273\351\235\242", 0)
         << QApplication::translate("frmConfig", "16\347\224\273\351\235\242", 0)
        );
        labAutoRun->setText(QApplication::translate("frmConfig", "\345\274\200\346\234\272\345\220\257\345\212\250", 0));
        btnAutoRun->setText(QString());
        labPollInterval->setText(QApplication::translate("frmConfig", "\350\275\256\350\257\242\351\227\264\351\232\224", 0));
        labMsec->setText(QApplication::translate("frmConfig", "\346\257\253\347\247\222", 0));
        labUseVideoOffLineCheck->setText(QApplication::translate("frmConfig", "\347\246\273\347\272\277\346\243\200\346\265\213", 0));
        labVideoOffLineCheckInterval->setText(QApplication::translate("frmConfig", "\346\243\200\346\265\213\351\227\264\351\232\224", 0));
        btnUseVideoOffLineCheck->setText(QString());
        labSec->setText(QApplication::translate("frmConfig", "\347\247\222", 0));
        labDelayTime->setText(QApplication::translate("frmConfig", "\347\274\223\345\255\230\346\227\266\351\227\264", 0));
        labSec2->setText(QApplication::translate("frmConfig", "\347\247\222", 0));
        labSec1->setText(QApplication::translate("frmConfig", "\346\257\253\347\247\222", 0));
        labPollSleep->setText(QApplication::translate("frmConfig", "\350\275\256\350\257\242\345\273\266\346\227\266", 0));
        btnClearNVR->setText(QApplication::translate("frmConfig", "\346\270\205\347\251\272NVR\344\277\241\346\201\257", 0));
        btnClearIPC->setText(QApplication::translate("frmConfig", "\346\270\205\347\251\272IPC\344\277\241\346\201\257", 0));
        btnClearPoll->setText(QApplication::translate("frmConfig", "\346\270\205\347\251\272\350\275\256\350\257\242\344\277\241\346\201\257", 0));
        btnClearAll->setText(QApplication::translate("frmConfig", "\346\201\242\345\244\215\345\207\272\345\216\202\345\200\274", 0));
        btnOk->setText(QApplication::translate("frmConfig", "\347\241\256\345\256\232(&O)", 0));
        btnCancel->setText(QApplication::translate("frmConfig", "\345\217\226\346\266\210(&C)", 0));
    } // retranslateUi

};

namespace Ui {
    class frmConfig: public Ui_frmConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMCONFIG_H
