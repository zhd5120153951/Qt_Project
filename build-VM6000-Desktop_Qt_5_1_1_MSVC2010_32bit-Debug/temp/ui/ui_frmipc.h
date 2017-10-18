/********************************************************************************
** Form generated from reading UI file 'frmipc.ui'
**
** Created by: Qt User Interface Compiler version 5.1.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMIPC_H
#define UI_FRMIPC_H

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
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_frmIPC
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
    QHBoxLayout *horizontalLayout_3;
    QTableView *tableMain;
    QGroupBox *gboxRight;
    QVBoxLayout *verticalLayout_2;
    QLabel *labIPCID;
    QComboBox *cboxIPCID;
    QLabel *labIPCName;
    QLineEdit *txtIPCName;
    QLabel *labNVRID;
    QComboBox *cboxNVRID;
    QLabel *labNVRName;
    QComboBox *cboxNVRName;
    QLabel *labIPCType;
    QComboBox *cboxIPCType;
    QLabel *labIPCRtspAddrMain;
    QLineEdit *txtIPCRtspAddrMain;
    QLabel *labIPCRtspAddrSub;
    QLineEdit *txtIPCRtspAddrSub;
    QLabel *labIPCUserName;
    QLineEdit *txtIPCUserName;
    QLabel *labIPCUserPwd;
    QLineEdit *txtIPCUserPwd;
    QLabel *labIPCUse;
    QComboBox *cboxIPCUse;
    QPushButton *btnAdd;
    QPushButton *btnDelete;
    QPushButton *btnUpdate;
    QPushButton *btnExcel;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *frmIPC)
    {
        if (frmIPC->objectName().isEmpty())
            frmIPC->setObjectName(QStringLiteral("frmIPC"));
        frmIPC->resize(925, 794);
        verticalLayout = new QVBoxLayout(frmIPC);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        widget_title = new QWidget(frmIPC);
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

        widget_main = new QWidget(frmIPC);
        widget_main->setObjectName(QStringLiteral("widget_main"));
        widget_main->setStyleSheet(QString::fromUtf8("font: 11pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));
        horizontalLayout_3 = new QHBoxLayout(widget_main);
        horizontalLayout_3->setSpacing(5);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(5, 5, 5, 5);
        tableMain = new QTableView(widget_main);
        tableMain->setObjectName(QStringLiteral("tableMain"));

        horizontalLayout_3->addWidget(tableMain);

        gboxRight = new QGroupBox(widget_main);
        gboxRight->setObjectName(QStringLiteral("gboxRight"));
        sizePolicy1.setHeightForWidth(gboxRight->sizePolicy().hasHeightForWidth());
        gboxRight->setSizePolicy(sizePolicy1);
        gboxRight->setMinimumSize(QSize(220, 0));
        verticalLayout_2 = new QVBoxLayout(gboxRight);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        labIPCID = new QLabel(gboxRight);
        labIPCID->setObjectName(QStringLiteral("labIPCID"));

        verticalLayout_2->addWidget(labIPCID);

        cboxIPCID = new QComboBox(gboxRight);
        cboxIPCID->setObjectName(QStringLiteral("cboxIPCID"));

        verticalLayout_2->addWidget(cboxIPCID);

        labIPCName = new QLabel(gboxRight);
        labIPCName->setObjectName(QStringLiteral("labIPCName"));

        verticalLayout_2->addWidget(labIPCName);

        txtIPCName = new QLineEdit(gboxRight);
        txtIPCName->setObjectName(QStringLiteral("txtIPCName"));
        QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(txtIPCName->sizePolicy().hasHeightForWidth());
        txtIPCName->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(txtIPCName);

        labNVRID = new QLabel(gboxRight);
        labNVRID->setObjectName(QStringLiteral("labNVRID"));

        verticalLayout_2->addWidget(labNVRID);

        cboxNVRID = new QComboBox(gboxRight);
        cboxNVRID->setObjectName(QStringLiteral("cboxNVRID"));

        verticalLayout_2->addWidget(cboxNVRID);

        labNVRName = new QLabel(gboxRight);
        labNVRName->setObjectName(QStringLiteral("labNVRName"));

        verticalLayout_2->addWidget(labNVRName);

        cboxNVRName = new QComboBox(gboxRight);
        cboxNVRName->setObjectName(QStringLiteral("cboxNVRName"));

        verticalLayout_2->addWidget(cboxNVRName);

        labIPCType = new QLabel(gboxRight);
        labIPCType->setObjectName(QStringLiteral("labIPCType"));

        verticalLayout_2->addWidget(labIPCType);

        cboxIPCType = new QComboBox(gboxRight);
        cboxIPCType->setObjectName(QStringLiteral("cboxIPCType"));
        sizePolicy4.setHeightForWidth(cboxIPCType->sizePolicy().hasHeightForWidth());
        cboxIPCType->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(cboxIPCType);

        labIPCRtspAddrMain = new QLabel(gboxRight);
        labIPCRtspAddrMain->setObjectName(QStringLiteral("labIPCRtspAddrMain"));

        verticalLayout_2->addWidget(labIPCRtspAddrMain);

        txtIPCRtspAddrMain = new QLineEdit(gboxRight);
        txtIPCRtspAddrMain->setObjectName(QStringLiteral("txtIPCRtspAddrMain"));
        sizePolicy4.setHeightForWidth(txtIPCRtspAddrMain->sizePolicy().hasHeightForWidth());
        txtIPCRtspAddrMain->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(txtIPCRtspAddrMain);

        labIPCRtspAddrSub = new QLabel(gboxRight);
        labIPCRtspAddrSub->setObjectName(QStringLiteral("labIPCRtspAddrSub"));

        verticalLayout_2->addWidget(labIPCRtspAddrSub);

        txtIPCRtspAddrSub = new QLineEdit(gboxRight);
        txtIPCRtspAddrSub->setObjectName(QStringLiteral("txtIPCRtspAddrSub"));
        sizePolicy4.setHeightForWidth(txtIPCRtspAddrSub->sizePolicy().hasHeightForWidth());
        txtIPCRtspAddrSub->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(txtIPCRtspAddrSub);

        labIPCUserName = new QLabel(gboxRight);
        labIPCUserName->setObjectName(QStringLiteral("labIPCUserName"));

        verticalLayout_2->addWidget(labIPCUserName);

        txtIPCUserName = new QLineEdit(gboxRight);
        txtIPCUserName->setObjectName(QStringLiteral("txtIPCUserName"));
        sizePolicy4.setHeightForWidth(txtIPCUserName->sizePolicy().hasHeightForWidth());
        txtIPCUserName->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(txtIPCUserName);

        labIPCUserPwd = new QLabel(gboxRight);
        labIPCUserPwd->setObjectName(QStringLiteral("labIPCUserPwd"));

        verticalLayout_2->addWidget(labIPCUserPwd);

        txtIPCUserPwd = new QLineEdit(gboxRight);
        txtIPCUserPwd->setObjectName(QStringLiteral("txtIPCUserPwd"));
        sizePolicy4.setHeightForWidth(txtIPCUserPwd->sizePolicy().hasHeightForWidth());
        txtIPCUserPwd->setSizePolicy(sizePolicy4);
        txtIPCUserPwd->setEchoMode(QLineEdit::Password);

        verticalLayout_2->addWidget(txtIPCUserPwd);

        labIPCUse = new QLabel(gboxRight);
        labIPCUse->setObjectName(QStringLiteral("labIPCUse"));

        verticalLayout_2->addWidget(labIPCUse);

        cboxIPCUse = new QComboBox(gboxRight);
        cboxIPCUse->setObjectName(QStringLiteral("cboxIPCUse"));
        sizePolicy4.setHeightForWidth(cboxIPCUse->sizePolicy().hasHeightForWidth());
        cboxIPCUse->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(cboxIPCUse);

        btnAdd = new QPushButton(gboxRight);
        btnAdd->setObjectName(QStringLiteral("btnAdd"));
        btnAdd->setCursor(QCursor(Qt::PointingHandCursor));
        QIcon icon;
        icon.addFile(QStringLiteral(":/image/add.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnAdd->setIcon(icon);
        btnAdd->setIconSize(QSize(20, 20));

        verticalLayout_2->addWidget(btnAdd);

        btnDelete = new QPushButton(gboxRight);
        btnDelete->setObjectName(QStringLiteral("btnDelete"));
        btnDelete->setCursor(QCursor(Qt::PointingHandCursor));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/image/delete.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnDelete->setIcon(icon1);
        btnDelete->setIconSize(QSize(20, 20));

        verticalLayout_2->addWidget(btnDelete);

        btnUpdate = new QPushButton(gboxRight);
        btnUpdate->setObjectName(QStringLiteral("btnUpdate"));
        btnUpdate->setCursor(QCursor(Qt::PointingHandCursor));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/image/update.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnUpdate->setIcon(icon2);
        btnUpdate->setIconSize(QSize(20, 20));

        verticalLayout_2->addWidget(btnUpdate);

        btnExcel = new QPushButton(gboxRight);
        btnExcel->setObjectName(QStringLiteral("btnExcel"));
        btnExcel->setCursor(QCursor(Qt::PointingHandCursor));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/image/excel.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnExcel->setIcon(icon3);
        btnExcel->setIconSize(QSize(20, 20));

        verticalLayout_2->addWidget(btnExcel);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        horizontalLayout_3->addWidget(gboxRight);


        verticalLayout->addWidget(widget_main);

        QWidget::setTabOrder(tableMain, cboxIPCID);
        QWidget::setTabOrder(cboxIPCID, txtIPCName);
        QWidget::setTabOrder(txtIPCName, cboxNVRID);
        QWidget::setTabOrder(cboxNVRID, cboxIPCType);
        QWidget::setTabOrder(cboxIPCType, txtIPCRtspAddrMain);
        QWidget::setTabOrder(txtIPCRtspAddrMain, txtIPCRtspAddrSub);
        QWidget::setTabOrder(txtIPCRtspAddrSub, txtIPCUserName);
        QWidget::setTabOrder(txtIPCUserName, txtIPCUserPwd);
        QWidget::setTabOrder(txtIPCUserPwd, cboxIPCUse);
        QWidget::setTabOrder(cboxIPCUse, btnAdd);
        QWidget::setTabOrder(btnAdd, btnDelete);
        QWidget::setTabOrder(btnDelete, btnUpdate);

        retranslateUi(frmIPC);

        QMetaObject::connectSlotsByName(frmIPC);
    } // setupUi

    void retranslateUi(QDialog *frmIPC)
    {
        frmIPC->setWindowTitle(QApplication::translate("frmIPC", "IPC\347\256\241\347\220\206", 0));
        lab_Ico->setText(QString());
        lab_Title->setText(QApplication::translate("frmIPC", "IPC\347\256\241\347\220\206", 0));
#ifndef QT_NO_TOOLTIP
        btnMenu_Close->setToolTip(QApplication::translate("frmIPC", "\345\205\263\351\227\255", 0));
#endif // QT_NO_TOOLTIP
        btnMenu_Close->setText(QString());
        gboxRight->setTitle(QString());
        labIPCID->setText(QApplication::translate("frmIPC", "\347\274\226\345\217\267", 0));
        labIPCName->setText(QApplication::translate("frmIPC", "\345\220\215\347\247\260", 0));
        txtIPCName->setText(QString());
        labNVRID->setText(QApplication::translate("frmIPC", "NVR\347\274\226\345\217\267", 0));
        labNVRName->setText(QApplication::translate("frmIPC", "NVR\345\220\215\347\247\260", 0));
        labIPCType->setText(QApplication::translate("frmIPC", "\347\261\273\345\236\213", 0));
        labIPCRtspAddrMain->setText(QApplication::translate("frmIPC", "\344\270\273\347\240\201\346\265\201\345\234\260\345\235\200", 0));
        txtIPCRtspAddrMain->setText(QApplication::translate("frmIPC", "rtsp://192.168.1.200:554/0", 0));
        labIPCRtspAddrSub->setText(QApplication::translate("frmIPC", "\345\255\220\347\240\201\346\265\201\345\234\260\345\235\200", 0));
        txtIPCRtspAddrSub->setText(QApplication::translate("frmIPC", "rtsp://192.168.1.200:554/1", 0));
        labIPCUserName->setText(QApplication::translate("frmIPC", "\347\224\250\346\210\267\345\220\215", 0));
        txtIPCUserName->setText(QApplication::translate("frmIPC", "admin", 0));
        labIPCUserPwd->setText(QApplication::translate("frmIPC", "\345\257\206\347\240\201", 0));
        txtIPCUserPwd->setText(QApplication::translate("frmIPC", "admin", 0));
        labIPCUse->setText(QApplication::translate("frmIPC", "\347\212\266\346\200\201", 0));
        cboxIPCUse->clear();
        cboxIPCUse->insertItems(0, QStringList()
         << QApplication::translate("frmIPC", "\345\220\257\347\224\250", 0)
         << QApplication::translate("frmIPC", "\347\246\201\347\224\250", 0)
        );
        btnAdd->setText(QApplication::translate("frmIPC", "\346\267\273\345\212\240(&A)", 0));
        btnDelete->setText(QApplication::translate("frmIPC", "\345\210\240\351\231\244(&D)", 0));
        btnUpdate->setText(QApplication::translate("frmIPC", "\344\277\256\346\224\271(&U)", 0));
        btnExcel->setText(QApplication::translate("frmIPC", "\345\257\274\345\207\272(&E)", 0));
    } // retranslateUi

};

namespace Ui {
    class frmIPC: public Ui_frmIPC {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMIPC_H
