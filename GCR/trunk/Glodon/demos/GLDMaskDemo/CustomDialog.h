#ifndef DIALOG_H
#define DIALOG_H

#include "GLDMaskBox.h"

#include <QDialog>

using namespace GlodonMask;

class QTextEdit;

class CustomDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CustomDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~CustomDialog();

protected:
    void initUI();

public slots:
    void showPosition();
    void showMask();

private:
    QPushButton* m_btn1;
    QPushButton* m_pNextButton;

    QTextEdit*   m_textEdt;
    GLDMaskBox*  m_pMask;
    QWidget *m_prevWidget;
    QWidget *m_nextWidget;
};

#endif // DIALOG_H
