#ifndef LOGOINWIDGET_H
#define LOGOINWIDGET_H

#include <QDebug>
#include <QLabel>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class Dialog;
class GLDMaskBox;

class TestButton : public QPushButton
{
public:
    explicit TestButton(QWidget * parent = nullptr)
        : QPushButton(parent)
    {

    }
protected:
    void mousePressEvent(QMouseEvent *e)
    {
        qDebug() << "testbutton mousePressEvent";
        QPushButton::mousePressEvent(e);
    }
};

class LogoinWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogoinWidget(QWidget *parent = 0);
    ~LogoinWidget();

signals:

public slots:
    void showDialog();

protected:
    //void mousePressEvent(QMouseEvent *event);

private:
    QLabel*      m_name;
    QLabel*      m_password;

    QLineEdit*   m_edtName;
    QLineEdit*   m_edtPwd;

    TestButton* m_logoin;
    QPushButton* m_cancel;

    Dialog*      m_dialog;

    GLDMaskBox*  pMask;
};

#endif // LOGOINWIDGET_H
