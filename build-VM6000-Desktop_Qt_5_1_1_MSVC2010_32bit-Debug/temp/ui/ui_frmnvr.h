/********************************************************************************
** Form generated from reading UI file 'frmnvr.ui'
**
** Created by: Qt User Interface Compiler version 5.1.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMNVR_H
#define UI_FRMNVR_H

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

class Ui_frmNVR
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
    QLabel *labNVRID;
    QComboBox *cboxNVRID;
    QLabel *labNVRName;
    QLineEdit *txtNVRName;
    QLabel *labNVRAddr;
    QLineEdit *txtNVRAddr;
    QLabel *labNVRIP;
    QLineEdit *txtNVRIP;
    QLabel *labNVRType;
    QComboBox *cboxNVRType;
    QLabel *labNVRUserName;
    QLineEdit *txtNVRUserName;
    QLabel *labNVRUserPwd;
    QLineEdit *txtNVRUserPwd;
    QLabel *labNVRUse;
    QComboBox *cboxNVRUse;
    QPushButton *btnAdd;
    QPushButton *btnDelete;
    QPushButton *btnUpdate;
    QPushButton *btnExcel;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *frmNVR)
    {
        if (frmNVR->objectName().isEmpty())
            frmNVR->setObjectName(QStringLiteral("frmNVR"));
        frmNVR->resize(991, 674);
        verticalLayout = new QVBoxLayout(frmNVR);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        widget_title = new QWidget(frmNVR);
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

        widget_main = new QWidget(frmNVR);
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
        labNVRID = new QLabel(gboxRight);
        labNVRID->setObjectName(QStringLiteral("labNVRID"));

        verticalLayout_2->addWidget(labNVRID);

        cboxNVRID = new QComboBox(gboxRight);
        cboxNVRID->setObjectName(QStringLiteral("cboxNVRID"));

        verticalLayout_2->addWidget(cboxNVRID);

        labNVRName = new QLabel(gboxRight);
        labNVRName->setObjectName(QStringLiteral("labNVRName"));

        verticalLayout_2->addWidget(labNVRName);

        txtNVRName = new QLineEdit(gboxRight);
        txtNVRName->setObjectName(QStringLiteral("txtNVRName"));
        QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(txtNVRName->sizePolicy().hasHeightForWidth());
        txtNVRName->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(txtNVRName);

        labNVRAddr = new QLabel(gboxRight);
        labNVRAddr->setObjectName(QStringLiteral("labNVRAddr"));

        verticalLayout_2->addWidget(labNVRAddr);

        txtNVRAddr = new QLineEdit(gboxRight);
        txtNVRAddr->setObjectName(QStringLiteral("txtNVRAddr"));
        sizePolicy4.setHeightForWidth(txtNVRAddr->sizePolicy().hasHeightForWidth());
        txtNVRAddr->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(txtNVRAddr);

        labNVRIP = new QLabel(gboxRight);
        labNVRIP->setObjectName(QStringLiteral("labNVRIP"));

        verticalLayout_2->addWidget(labNVRIP);

        txtNVRIP = new QLineEdit(gboxRight);
        txtNVRIP->setObjectName(QStringLiteral("txtNVRIP"));
        sizePolicy4.setHeightForWidth(txtNVRIP->sizePolicy().hasHeightForWidth());
        txtNVRIP->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(txtNVRIP);

        labNVRType = new QLabel(gboxRight);
        labNVRType->setObjectName(QStringLiteral("labNVRType"));

        verticalLayout_2->addWidget(labNVRType);

        cboxNVRType = new QComboBox(gboxRight);
        cboxNVRType->setObjectName(QStringLiteral("cboxNVRType"));
        sizePolicy4.setHeightForWidth(cboxNVRType->sizePolicy().hasHeightForWidth());
        cboxNVRType->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(cboxNVRType);

        labNVRUserName = new QLabel(gboxRight);
        labNVRUserName->setObjectName(QStringLiteral("labNVRUserName"));

        verticalLayout_2->addWidget(labNVRUserName);

        txtNVRUserName = new QLineEdit(gboxRight);
        txtNVRUserName->setObjectName(QStringLiteral("txtNVRUserName"));
        sizePolicy4.setHeightForWidth(txtNVRUserName->sizePolicy().hasHeightForWidth());
        txtNVRUserName->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(txtNVRUserName);

        labNVRUserPwd = new QLabel(gboxRight);
        labNVRUserPwd->setObjectName(QStringLiteral("labNVRUserPwd"));

        verticalLayout_2->addWidget(labNVRUserPwd);

        txtNVRUserPwd = new QLineEdit(gboxRight);
        txtNVRUserPwd->setObjectName(QStringLiteral("txtNVRUserPwd"));
        sizePolicy4.setHeightForWidth(txtNVRUserPwd->sizePolicy().hasHeightForWidth());
        txtNVRUserPwd->setSizePolicy(sizePolicy4);
        txtNVRUserPwd->setEchoMode(QLineEdit::Password);

        verticalLayout_2->addWidget(txtNVRUserPwd);

        labNVRUse = new QLabel(gboxRight);
        labNVRUse->setObjectName(QStringLiteral("labNVRUse"));

        verticalLayout_2->addWidget(labNVRUse);

        cboxNVRUse = new QComboBox(gboxRight);
        cboxNVRUse->setObjectName(QStringLiteral("cboxNVRUse"));
        sizePolicy4.setHeightForWidth(cboxNVRUse->sizePolicy().hasHeightForWidth());
        cboxNVRUse->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(cboxNVRUse);

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


        retranslateUi(frmNVR);

        QMetaObject::connectSlotsByName(frmNVR);
    } // setupUi

    void retranslateUi(QDialog *frmNVR)
    {
        frmNVR->setWindowTitle(QApplication::translate("frmNVR", "NVR\347\256\241\347\220\206", 0));
        lab_Ico->setText(QString());
        lab_Title->setText(QApplication::translate("frmNVR", "NVR\347\256\241\347\220\206", 0));
#ifndef QT_NO_TOOLTIP
        btnMenu_Close->setToolTip(QApplication::translate("frmNVR", "\345\205\263\351\227\255", 0));
#endif // QT_NO_TOOLTIP
        btnMenu_Close->setText(QString());
        gboxRight->setTitle(QString());
        labNVRID->setText(QApplication::translate("frmNVR", "\347\274\226\345\217\267", 0));
        labNVRName->setText(QApplication::translate("frmNVR", "\345\220\215\347\247\260", 0));
        txtNVRName->setText(QString());
        labNVRAddr->setText(QApplication::translate("frmNVR", "\345\234\260\345\235\200", 0));
        txtNVRAddr->setText(QString());
        labNVRIP->setText(QApplication::translate("frmNVR", "IP\345\234\260\345\235\200", 0));
        txtNVRIP->setText(QString());
        labNVRType->setText(QApplication::translate("frmNVR", "\347\261\273\345\236\213", 0));
        labNVRUserName->setText(QApplication::translate("frmNVR", "\347\224\250\346\210\267\345\220\215", 0));
        txtNVRUserName->setText(QApplication::translate("frmNVR", "admin", 0));
        labNVRUserPwd->setText(QApplication::translate("frmNVR", "\345\257\206\347\240\201", 0));
        txtNVRUserPwd->setText(QApplication::translate("frmNVR", "admin", 0));
        labNVRUse->setText(QApplication::translate("frmNVR", "\347\212\266\346\200\201", 0));
        cboxNVRUse->clear();
        cboxNVRUse->insertItems(0, QStringList()
         << QApplication::translate("frmNVR", "\345\220\257\347\224\250", 0)
         << QApplication::translate("frmNVR", "\347\246\201\347\224\250", 0)
        );
        btnAdd->setText(QApplication::translate("frmNVR", "\346\267\273\345\212\240(&A)", 0));
        btnDelete->setText(QApplication::translate("frmNVR", "\345\210\240\351\231\244(&D)", 0));
        btnUpdate->setText(QApplication::translate("frmNVR", "\344\277\256\346\224\271(&U)", 0));
        btnExcel->setText(QApplication::translate("frmNVR", "\345\257\274\345\207\272(&E)", 0));
    } // retranslateUi

};

namespace Ui {
    class frmNVR: public Ui_frmNVR {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMNVR_H
