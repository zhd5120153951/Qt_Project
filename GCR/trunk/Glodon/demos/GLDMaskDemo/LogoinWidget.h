#ifndef LOGOINWIDGET_H
#define LOGOINWIDGET_H

#include <QDebug>
#include <QLabel>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

#include "GLDMaskBox.h"

using namespace GlodonMask;

class CustomDialog;

class LogoinWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogoinWidget(QWidget *parent = 0);
    ~LogoinWidget();

public slots:
    void showDialog();

private:
    QLabel*        m_name;
    QLabel*        m_password;

    QLineEdit*     m_edtName;
    QLineEdit*     m_edtPwd;

    QPushButton*   m_logoin;
    QPushButton*   m_cancel;

    CustomDialog*  m_dialog;
};

#endif // LOGOINWIDGET_H
