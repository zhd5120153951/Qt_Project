/********************************************************************************
** Form generated from reading UI file 'frmmain.ui'
**
** Created by: Qt User Interface Compiler version 5.1.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMMAIN_H
#define UI_FRMMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_frmMain
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget_title;
    QHBoxLayout *horizontalLayout_2;
    QLabel *lab_Ico;
    QLabel *lab_Title;
    QWidget *widget_menu;
    QHBoxLayout *horizontalLayout;
    QLabel *labFull;
    QLabel *labStart;
    QLabel *labNVR;
    QLabel *labIPC;
    QLabel *labPollConfig;
    QLabel *labVideoPlayBack;
    QLabel *labConfig;
    QLabel *labExit;
    QLabel *labStyle;
    QPushButton *btnMenu_Min;
    QPushButton *btnMenu_Close;
    QWidget *widget_main;
    QHBoxLayout *horizontalLayout_3;
    QTreeWidget *treeMain;
    QGroupBox *gboxMain;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *lay1;
    QLabel *labVideo1;
    QLabel *labVideo2;
    QLabel *labVideo3;
    QLabel *labVideo4;
    QHBoxLayout *lay2;
    QLabel *labVideo5;
    QLabel *labVideo6;
    QLabel *labVideo7;
    QLabel *labVideo8;
    QHBoxLayout *lay3;
    QLabel *labVideo9;
    QLabel *labVideo10;
    QLabel *labVideo11;
    QLabel *labVideo12;
    QHBoxLayout *lay4;
    QLabel *labVideo13;
    QLabel *labVideo14;
    QLabel *labVideo15;
    QLabel *labVideo16;

    void setupUi(QDialog *frmMain)
    {
        if (frmMain->objectName().isEmpty())
            frmMain->setObjectName(QStringLiteral("frmMain"));
        frmMain->resize(1000, 656);
        frmMain->setMinimumSize(QSize(0, 0));
        frmMain->setSizeGripEnabled(false);
        verticalLayout = new QVBoxLayout(frmMain);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        widget_title = new QWidget(frmMain);
        widget_title->setObjectName(QStringLiteral("widget_title"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_title->sizePolicy().hasHeightForWidth());
        widget_title->setSizePolicy(sizePolicy);
        widget_title->setMinimumSize(QSize(100, 28));
        horizontalLayout_2 = new QHBoxLayout(widget_title);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        lab_Ico = new QLabel(widget_title);
        lab_Ico->setObjectName(QStringLiteral("lab_Ico"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lab_Ico->sizePolicy().hasHeightForWidth());
        lab_Ico->setSizePolicy(sizePolicy1);
        lab_Ico->setMinimumSize(QSize(30, 0));
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
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        labFull = new QLabel(widget_menu);
        labFull->setObjectName(QStringLiteral("labFull"));
        labFull->setCursor(QCursor(Qt::PointingHandCursor));
        labFull->setStyleSheet(QString::fromUtf8("font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));

        horizontalLayout->addWidget(labFull);

        labStart = new QLabel(widget_menu);
        labStart->setObjectName(QStringLiteral("labStart"));
        labStart->setCursor(QCursor(Qt::PointingHandCursor));
        labStart->setStyleSheet(QString::fromUtf8("font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));

        horizontalLayout->addWidget(labStart);

        labNVR = new QLabel(widget_menu);
        labNVR->setObjectName(QStringLiteral("labNVR"));
        labNVR->setCursor(QCursor(Qt::PointingHandCursor));
        labNVR->setStyleSheet(QString::fromUtf8("font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));

        horizontalLayout->addWidget(labNVR);

        labIPC = new QLabel(widget_menu);
        labIPC->setObjectName(QStringLiteral("labIPC"));
        labIPC->setCursor(QCursor(Qt::PointingHandCursor));
        labIPC->setStyleSheet(QString::fromUtf8("font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));

        horizontalLayout->addWidget(labIPC);

        labPollConfig = new QLabel(widget_menu);
        labPollConfig->setObjectName(QStringLiteral("labPollConfig"));
        labPollConfig->setCursor(QCursor(Qt::PointingHandCursor));
        labPollConfig->setStyleSheet(QString::fromUtf8("font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));

        horizontalLayout->addWidget(labPollConfig);

        labVideoPlayBack = new QLabel(widget_menu);
        labVideoPlayBack->setObjectName(QStringLiteral("labVideoPlayBack"));
        labVideoPlayBack->setCursor(QCursor(Qt::PointingHandCursor));
        labVideoPlayBack->setStyleSheet(QString::fromUtf8("font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));

        horizontalLayout->addWidget(labVideoPlayBack);

        labConfig = new QLabel(widget_menu);
        labConfig->setObjectName(QStringLiteral("labConfig"));
        labConfig->setCursor(QCursor(Qt::PointingHandCursor));
        labConfig->setStyleSheet(QString::fromUtf8("font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));

        horizontalLayout->addWidget(labConfig);

        labExit = new QLabel(widget_menu);
        labExit->setObjectName(QStringLiteral("labExit"));
        labExit->setCursor(QCursor(Qt::PointingHandCursor));
        labExit->setStyleSheet(QString::fromUtf8("font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));

        horizontalLayout->addWidget(labExit);

        labStyle = new QLabel(widget_menu);
        labStyle->setObjectName(QStringLiteral("labStyle"));
        sizePolicy1.setHeightForWidth(labStyle->sizePolicy().hasHeightForWidth());
        labStyle->setSizePolicy(sizePolicy1);
        labStyle->setMinimumSize(QSize(40, 0));
        labStyle->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout->addWidget(labStyle);

        btnMenu_Min = new QPushButton(widget_menu);
        btnMenu_Min->setObjectName(QStringLiteral("btnMenu_Min"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(btnMenu_Min->sizePolicy().hasHeightForWidth());
        btnMenu_Min->setSizePolicy(sizePolicy3);
        btnMenu_Min->setMinimumSize(QSize(40, 0));
        btnMenu_Min->setCursor(QCursor(Qt::ArrowCursor));
        btnMenu_Min->setFocusPolicy(Qt::NoFocus);
        btnMenu_Min->setFlat(true);

        horizontalLayout->addWidget(btnMenu_Min);

        btnMenu_Close = new QPushButton(widget_menu);
        btnMenu_Close->setObjectName(QStringLiteral("btnMenu_Close"));
        sizePolicy3.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
        btnMenu_Close->setSizePolicy(sizePolicy3);
        btnMenu_Close->setMinimumSize(QSize(40, 0));
        btnMenu_Close->setCursor(QCursor(Qt::ArrowCursor));
        btnMenu_Close->setFocusPolicy(Qt::NoFocus);
        btnMenu_Close->setFlat(true);

        horizontalLayout->addWidget(btnMenu_Close);


        horizontalLayout_2->addWidget(widget_menu);


        verticalLayout->addWidget(widget_title);

        widget_main = new QWidget(frmMain);
        widget_main->setObjectName(QStringLiteral("widget_main"));
        widget_main->setStyleSheet(QString::fromUtf8("font: 9pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));
        horizontalLayout_3 = new QHBoxLayout(widget_main);
        horizontalLayout_3->setSpacing(5);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(5, 5, 5, 5);
        treeMain = new QTreeWidget(widget_main);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeMain->setHeaderItem(__qtreewidgetitem);
        treeMain->setObjectName(QStringLiteral("treeMain"));
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(treeMain->sizePolicy().hasHeightForWidth());
        treeMain->setSizePolicy(sizePolicy4);
        treeMain->setMinimumSize(QSize(0, 0));

        horizontalLayout_3->addWidget(treeMain);

        gboxMain = new QGroupBox(widget_main);
        gboxMain->setObjectName(QStringLiteral("gboxMain"));
        QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(gboxMain->sizePolicy().hasHeightForWidth());
        gboxMain->setSizePolicy(sizePolicy5);
        verticalLayout_2 = new QVBoxLayout(gboxMain);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        lay1 = new QHBoxLayout();
        lay1->setSpacing(0);
        lay1->setObjectName(QStringLiteral("lay1"));
        labVideo1 = new QLabel(gboxMain);
        labVideo1->setObjectName(QStringLiteral("labVideo1"));
        QSizePolicy sizePolicy6(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(labVideo1->sizePolicy().hasHeightForWidth());
        labVideo1->setSizePolicy(sizePolicy6);
        labVideo1->setFocusPolicy(Qt::StrongFocus);
        labVideo1->setFrameShape(QFrame::Panel);
        labVideo1->setFrameShadow(QFrame::Plain);
        labVideo1->setLineWidth(1);
        labVideo1->setMidLineWidth(0);
        labVideo1->setAlignment(Qt::AlignCenter);

        lay1->addWidget(labVideo1);

        labVideo2 = new QLabel(gboxMain);
        labVideo2->setObjectName(QStringLiteral("labVideo2"));
        sizePolicy6.setHeightForWidth(labVideo2->sizePolicy().hasHeightForWidth());
        labVideo2->setSizePolicy(sizePolicy6);
        labVideo2->setFocusPolicy(Qt::StrongFocus);
        labVideo2->setFrameShape(QFrame::Panel);
        labVideo2->setFrameShadow(QFrame::Plain);
        labVideo2->setLineWidth(1);
        labVideo2->setMidLineWidth(0);
        labVideo2->setAlignment(Qt::AlignCenter);

        lay1->addWidget(labVideo2);

        labVideo3 = new QLabel(gboxMain);
        labVideo3->setObjectName(QStringLiteral("labVideo3"));
        sizePolicy6.setHeightForWidth(labVideo3->sizePolicy().hasHeightForWidth());
        labVideo3->setSizePolicy(sizePolicy6);
        labVideo3->setFocusPolicy(Qt::StrongFocus);
        labVideo3->setFrameShape(QFrame::Panel);
        labVideo3->setFrameShadow(QFrame::Plain);
        labVideo3->setLineWidth(1);
        labVideo3->setMidLineWidth(0);
        labVideo3->setAlignment(Qt::AlignCenter);

        lay1->addWidget(labVideo3);

        labVideo4 = new QLabel(gboxMain);
        labVideo4->setObjectName(QStringLiteral("labVideo4"));
        sizePolicy6.setHeightForWidth(labVideo4->sizePolicy().hasHeightForWidth());
        labVideo4->setSizePolicy(sizePolicy6);
        labVideo4->setFocusPolicy(Qt::StrongFocus);
        labVideo4->setFrameShape(QFrame::Panel);
        labVideo4->setFrameShadow(QFrame::Plain);
        labVideo4->setLineWidth(1);
        labVideo4->setMidLineWidth(0);
        labVideo4->setAlignment(Qt::AlignCenter);

        lay1->addWidget(labVideo4);


        verticalLayout_2->addLayout(lay1);

        lay2 = new QHBoxLayout();
        lay2->setSpacing(0);
        lay2->setObjectName(QStringLiteral("lay2"));
        labVideo5 = new QLabel(gboxMain);
        labVideo5->setObjectName(QStringLiteral("labVideo5"));
        sizePolicy6.setHeightForWidth(labVideo5->sizePolicy().hasHeightForWidth());
        labVideo5->setSizePolicy(sizePolicy6);
        labVideo5->setFocusPolicy(Qt::StrongFocus);
        labVideo5->setFrameShape(QFrame::Panel);
        labVideo5->setFrameShadow(QFrame::Plain);
        labVideo5->setLineWidth(1);
        labVideo5->setMidLineWidth(0);
        labVideo5->setAlignment(Qt::AlignCenter);

        lay2->addWidget(labVideo5);

        labVideo6 = new QLabel(gboxMain);
        labVideo6->setObjectName(QStringLiteral("labVideo6"));
        sizePolicy6.setHeightForWidth(labVideo6->sizePolicy().hasHeightForWidth());
        labVideo6->setSizePolicy(sizePolicy6);
        labVideo6->setFocusPolicy(Qt::StrongFocus);
        labVideo6->setFrameShape(QFrame::Panel);
        labVideo6->setFrameShadow(QFrame::Plain);
        labVideo6->setLineWidth(1);
        labVideo6->setMidLineWidth(0);
        labVideo6->setAlignment(Qt::AlignCenter);

        lay2->addWidget(labVideo6);

        labVideo7 = new QLabel(gboxMain);
        labVideo7->setObjectName(QStringLiteral("labVideo7"));
        sizePolicy6.setHeightForWidth(labVideo7->sizePolicy().hasHeightForWidth());
        labVideo7->setSizePolicy(sizePolicy6);
        labVideo7->setFocusPolicy(Qt::StrongFocus);
        labVideo7->setFrameShape(QFrame::Panel);
        labVideo7->setFrameShadow(QFrame::Plain);
        labVideo7->setLineWidth(1);
        labVideo7->setMidLineWidth(0);
        labVideo7->setAlignment(Qt::AlignCenter);

        lay2->addWidget(labVideo7);

        labVideo8 = new QLabel(gboxMain);
        labVideo8->setObjectName(QStringLiteral("labVideo8"));
        sizePolicy6.setHeightForWidth(labVideo8->sizePolicy().hasHeightForWidth());
        labVideo8->setSizePolicy(sizePolicy6);
        labVideo8->setFocusPolicy(Qt::StrongFocus);
        labVideo8->setFrameShape(QFrame::Panel);
        labVideo8->setFrameShadow(QFrame::Plain);
        labVideo8->setLineWidth(1);
        labVideo8->setMidLineWidth(0);
        labVideo8->setAlignment(Qt::AlignCenter);

        lay2->addWidget(labVideo8);


        verticalLayout_2->addLayout(lay2);

        lay3 = new QHBoxLayout();
        lay3->setSpacing(0);
        lay3->setObjectName(QStringLiteral("lay3"));
        labVideo9 = new QLabel(gboxMain);
        labVideo9->setObjectName(QStringLiteral("labVideo9"));
        sizePolicy6.setHeightForWidth(labVideo9->sizePolicy().hasHeightForWidth());
        labVideo9->setSizePolicy(sizePolicy6);
        labVideo9->setFocusPolicy(Qt::StrongFocus);
        labVideo9->setFrameShape(QFrame::Panel);
        labVideo9->setFrameShadow(QFrame::Plain);
        labVideo9->setLineWidth(1);
        labVideo9->setMidLineWidth(0);
        labVideo9->setAlignment(Qt::AlignCenter);

        lay3->addWidget(labVideo9);

        labVideo10 = new QLabel(gboxMain);
        labVideo10->setObjectName(QStringLiteral("labVideo10"));
        sizePolicy6.setHeightForWidth(labVideo10->sizePolicy().hasHeightForWidth());
        labVideo10->setSizePolicy(sizePolicy6);
        labVideo10->setFocusPolicy(Qt::StrongFocus);
        labVideo10->setFrameShape(QFrame::Panel);
        labVideo10->setFrameShadow(QFrame::Plain);
        labVideo10->setLineWidth(1);
        labVideo10->setMidLineWidth(0);
        labVideo10->setAlignment(Qt::AlignCenter);

        lay3->addWidget(labVideo10);

        labVideo11 = new QLabel(gboxMain);
        labVideo11->setObjectName(QStringLiteral("labVideo11"));
        sizePolicy6.setHeightForWidth(labVideo11->sizePolicy().hasHeightForWidth());
        labVideo11->setSizePolicy(sizePolicy6);
        labVideo11->setFocusPolicy(Qt::StrongFocus);
        labVideo11->setFrameShape(QFrame::Panel);
        labVideo11->setFrameShadow(QFrame::Plain);
        labVideo11->setLineWidth(1);
        labVideo11->setMidLineWidth(0);
        labVideo11->setAlignment(Qt::AlignCenter);

        lay3->addWidget(labVideo11);

        labVideo12 = new QLabel(gboxMain);
        labVideo12->setObjectName(QStringLiteral("labVideo12"));
        sizePolicy6.setHeightForWidth(labVideo12->sizePolicy().hasHeightForWidth());
        labVideo12->setSizePolicy(sizePolicy6);
        labVideo12->setFocusPolicy(Qt::StrongFocus);
        labVideo12->setFrameShape(QFrame::Panel);
        labVideo12->setFrameShadow(QFrame::Plain);
        labVideo12->setLineWidth(1);
        labVideo12->setMidLineWidth(0);
        labVideo12->setAlignment(Qt::AlignCenter);

        lay3->addWidget(labVideo12);


        verticalLayout_2->addLayout(lay3);

        lay4 = new QHBoxLayout();
        lay4->setSpacing(0);
        lay4->setObjectName(QStringLiteral("lay4"));
        labVideo13 = new QLabel(gboxMain);
        labVideo13->setObjectName(QStringLiteral("labVideo13"));
        sizePolicy6.setHeightForWidth(labVideo13->sizePolicy().hasHeightForWidth());
        labVideo13->setSizePolicy(sizePolicy6);
        labVideo13->setFocusPolicy(Qt::StrongFocus);
        labVideo13->setFrameShape(QFrame::Panel);
        labVideo13->setFrameShadow(QFrame::Plain);
        labVideo13->setLineWidth(1);
        labVideo13->setMidLineWidth(0);
        labVideo13->setAlignment(Qt::AlignCenter);

        lay4->addWidget(labVideo13);

        labVideo14 = new QLabel(gboxMain);
        labVideo14->setObjectName(QStringLiteral("labVideo14"));
        sizePolicy6.setHeightForWidth(labVideo14->sizePolicy().hasHeightForWidth());
        labVideo14->setSizePolicy(sizePolicy6);
        labVideo14->setFocusPolicy(Qt::StrongFocus);
        labVideo14->setFrameShape(QFrame::Panel);
        labVideo14->setFrameShadow(QFrame::Plain);
        labVideo14->setLineWidth(1);
        labVideo14->setMidLineWidth(0);
        labVideo14->setAlignment(Qt::AlignCenter);

        lay4->addWidget(labVideo14);

        labVideo15 = new QLabel(gboxMain);
        labVideo15->setObjectName(QStringLiteral("labVideo15"));
        sizePolicy6.setHeightForWidth(labVideo15->sizePolicy().hasHeightForWidth());
        labVideo15->setSizePolicy(sizePolicy6);
        labVideo15->setFocusPolicy(Qt::StrongFocus);
        labVideo15->setFrameShape(QFrame::Panel);
        labVideo15->setFrameShadow(QFrame::Plain);
        labVideo15->setLineWidth(1);
        labVideo15->setMidLineWidth(0);
        labVideo15->setAlignment(Qt::AlignCenter);

        lay4->addWidget(labVideo15);

        labVideo16 = new QLabel(gboxMain);
        labVideo16->setObjectName(QStringLiteral("labVideo16"));
        sizePolicy6.setHeightForWidth(labVideo16->sizePolicy().hasHeightForWidth());
        labVideo16->setSizePolicy(sizePolicy6);
        labVideo16->setFocusPolicy(Qt::StrongFocus);
        labVideo16->setFrameShape(QFrame::Panel);
        labVideo16->setFrameShadow(QFrame::Plain);
        labVideo16->setLineWidth(1);
        labVideo16->setMidLineWidth(0);
        labVideo16->setAlignment(Qt::AlignCenter);

        lay4->addWidget(labVideo16);


        verticalLayout_2->addLayout(lay4);


        horizontalLayout_3->addWidget(gboxMain);


        verticalLayout->addWidget(widget_main);


        retranslateUi(frmMain);

        QMetaObject::connectSlotsByName(frmMain);
    } // setupUi

    void retranslateUi(QDialog *frmMain)
    {
        frmMain->setWindowTitle(QApplication::translate("frmMain", "\350\247\206\351\242\221\347\233\221\346\216\247\347\263\273\347\273\237V201412", 0));
        lab_Ico->setText(QString());
        lab_Title->setText(QApplication::translate("frmMain", "\350\247\206\351\242\221\347\233\221\346\216\247\347\263\273\347\273\237", 0));
        labFull->setText(QApplication::translate("frmMain", "\350\277\233\345\205\245\345\205\250\345\261\217", 0));
        labStart->setText(QApplication::translate("frmMain", "\345\220\257\345\212\250\350\275\256\350\257\242", 0));
        labNVR->setText(QApplication::translate("frmMain", "NVR\347\256\241\347\220\206", 0));
        labIPC->setText(QApplication::translate("frmMain", "IPC\347\256\241\347\220\206", 0));
        labPollConfig->setText(QApplication::translate("frmMain", "\350\275\256\350\257\242\350\256\276\347\275\256", 0));
        labVideoPlayBack->setText(QApplication::translate("frmMain", "\350\247\206\351\242\221\345\233\236\346\224\276", 0));
        labConfig->setText(QApplication::translate("frmMain", "\347\263\273\347\273\237\350\256\276\347\275\256", 0));
        labExit->setText(QApplication::translate("frmMain", "\351\200\200\345\207\272\347\263\273\347\273\237", 0));
#ifndef QT_NO_TOOLTIP
        labStyle->setToolTip(QApplication::translate("frmMain", "\346\240\267\345\274\217", 0));
#endif // QT_NO_TOOLTIP
        labStyle->setText(QString());
#ifndef QT_NO_TOOLTIP
        btnMenu_Min->setToolTip(QApplication::translate("frmMain", "\346\234\200\345\260\217\345\214\226", 0));
#endif // QT_NO_TOOLTIP
        btnMenu_Min->setText(QString());
#ifndef QT_NO_TOOLTIP
        btnMenu_Close->setToolTip(QApplication::translate("frmMain", "\345\205\263\351\227\255", 0));
#endif // QT_NO_TOOLTIP
        btnMenu_Close->setText(QString());
        gboxMain->setTitle(QString());
        labVideo1->setText(QString());
        labVideo2->setText(QString());
        labVideo3->setText(QString());
        labVideo4->setText(QString());
        labVideo5->setText(QString());
        labVideo6->setText(QString());
        labVideo7->setText(QString());
        labVideo8->setText(QString());
        labVideo9->setText(QString());
        labVideo10->setText(QString());
        labVideo11->setText(QString());
        labVideo12->setText(QString());
        labVideo13->setText(QString());
        labVideo14->setText(QString());
        labVideo15->setText(QString());
        labVideo16->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class frmMain: public Ui_frmMain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMMAIN_H
