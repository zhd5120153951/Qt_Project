/********************************************************************************
** Form generated from reading UI file 'frmpollconfig.ui'
**
** Created by: Qt User Interface Compiler version 5.1.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMPOLLCONFIG_H
#define UI_FRMPOLLCONFIG_H

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
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_frmPollConfig
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
    QTreeWidget *treeMain;
    QGroupBox *gboxMain;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer;
    QPushButton *btnAddOne;
    QPushButton *btnAddAll;
    QSpacerItem *verticalSpacer_3;
    QPushButton *btnRemoveOne;
    QPushButton *btnRemoveAll;
    QSpacerItem *verticalSpacer_2;
    QPushButton *btnExcel;
    QSpacerItem *verticalSpacer_4;
    QTableView *tableMain;

    void setupUi(QDialog *frmPollConfig)
    {
        if (frmPollConfig->objectName().isEmpty())
            frmPollConfig->setObjectName(QStringLiteral("frmPollConfig"));
        frmPollConfig->resize(839, 578);
        verticalLayout = new QVBoxLayout(frmPollConfig);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        widget_title = new QWidget(frmPollConfig);
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

        widget_main = new QWidget(frmPollConfig);
        widget_main->setObjectName(QStringLiteral("widget_main"));
        widget_main->setStyleSheet(QString::fromUtf8("font: 11pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));
        horizontalLayout_3 = new QHBoxLayout(widget_main);
        horizontalLayout_3->setSpacing(5);
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
        treeMain->setStyleSheet(QString::fromUtf8("font: 9pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));

        horizontalLayout_3->addWidget(treeMain);

        gboxMain = new QGroupBox(widget_main);
        gboxMain->setObjectName(QStringLiteral("gboxMain"));
        sizePolicy1.setHeightForWidth(gboxMain->sizePolicy().hasHeightForWidth());
        gboxMain->setSizePolicy(sizePolicy1);
        gboxMain->setMinimumSize(QSize(130, 0));
        verticalLayout_2 = new QVBoxLayout(gboxMain);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        btnAddOne = new QPushButton(gboxMain);
        btnAddOne->setObjectName(QStringLiteral("btnAddOne"));
        btnAddOne->setCursor(QCursor(Qt::PointingHandCursor));

        verticalLayout_2->addWidget(btnAddOne);

        btnAddAll = new QPushButton(gboxMain);
        btnAddAll->setObjectName(QStringLiteral("btnAddAll"));
        btnAddAll->setCursor(QCursor(Qt::PointingHandCursor));

        verticalLayout_2->addWidget(btnAddAll);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_3);

        btnRemoveOne = new QPushButton(gboxMain);
        btnRemoveOne->setObjectName(QStringLiteral("btnRemoveOne"));
        btnRemoveOne->setCursor(QCursor(Qt::PointingHandCursor));

        verticalLayout_2->addWidget(btnRemoveOne);

        btnRemoveAll = new QPushButton(gboxMain);
        btnRemoveAll->setObjectName(QStringLiteral("btnRemoveAll"));
        btnRemoveAll->setCursor(QCursor(Qt::PointingHandCursor));

        verticalLayout_2->addWidget(btnRemoveAll);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);

        btnExcel = new QPushButton(gboxMain);
        btnExcel->setObjectName(QStringLiteral("btnExcel"));
        btnExcel->setCursor(QCursor(Qt::PointingHandCursor));

        verticalLayout_2->addWidget(btnExcel);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_4);


        horizontalLayout_3->addWidget(gboxMain);

        tableMain = new QTableView(widget_main);
        tableMain->setObjectName(QStringLiteral("tableMain"));

        horizontalLayout_3->addWidget(tableMain);


        verticalLayout->addWidget(widget_main);


        retranslateUi(frmPollConfig);

        QMetaObject::connectSlotsByName(frmPollConfig);
    } // setupUi

    void retranslateUi(QDialog *frmPollConfig)
    {
        frmPollConfig->setWindowTitle(QApplication::translate("frmPollConfig", "\350\275\256\350\257\242\350\256\276\347\275\256", 0));
        lab_Ico->setText(QString());
        lab_Title->setText(QApplication::translate("frmPollConfig", "\350\275\256\350\257\242\350\256\276\347\275\256", 0));
#ifndef QT_NO_TOOLTIP
        btnMenu_Close->setToolTip(QApplication::translate("frmPollConfig", "\345\205\263\351\227\255", 0));
#endif // QT_NO_TOOLTIP
        btnMenu_Close->setText(QString());
        gboxMain->setTitle(QString());
        btnAddOne->setText(QApplication::translate("frmPollConfig", "\346\267\273\345\212\240\351\200\211\344\270\255 \342\206\222", 0));
        btnAddAll->setText(QApplication::translate("frmPollConfig", "\346\267\273\345\212\240\346\211\200\346\234\211 \342\206\222", 0));
        btnRemoveOne->setText(QApplication::translate("frmPollConfig", "\342\206\220 \347\247\273\351\231\244\351\200\211\344\270\255", 0));
        btnRemoveAll->setText(QApplication::translate("frmPollConfig", "\342\206\220 \347\247\273\351\231\244\346\211\200\346\234\211", 0));
        btnExcel->setText(QApplication::translate("frmPollConfig", "\345\257\274\345\207\272\346\225\260\346\215\256", 0));
    } // retranslateUi

};

namespace Ui {
    class frmPollConfig: public Ui_frmPollConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMPOLLCONFIG_H
