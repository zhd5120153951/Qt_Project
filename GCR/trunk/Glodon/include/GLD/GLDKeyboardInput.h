#ifndef GLDKEYBOARDINPUT_H
#define GLDKEYBOARDINPUT_H

#include <QWidget>
#include <QMouseEvent>
#include <QSignalMapper>
#include <QGridLayout>

#include "GLDString.h"
#include "GLDWidget_Global.h"

class QPushButton;

class GLDWIDGETSHARED_EXPORT GLDKeyboardInput : public QWidget
{
    Q_OBJECT
public:
    explicit GLDKeyboardInput(QWidget *parent = 0);

public slots:
    void onClickedCapsLock(bool checked);
    void onClickedShift(bool checked);

public:
    void initUI();
    void show();
    void addButton(QGridLayout *pgridlayout, const GString numberTagList[],
                   const GString numberTagListBlock[], int lineNumber,
                   int startKeySpace, int length);
    void onClickCapsAndShift(bool checked);
    void addButton(QGridLayout *pgridlayout, QMap<GString, GString> hashMap, int lineNumber, int startKeySpace);

private:
    void initConnection();
    void setFormsMask();
    void mouseMoveEvent(QMouseEvent *event);
    void focusOutEvent(QFocusEvent *);

private:
    QPushButton *m_pclosebtn;
    QPushButton *m_pdeletebtn;
    QPushButton *m_pshiftbtn;
    QPushButton *m_pcapslockbtn;

    QSignalMapper *m_signalMapperLowercase;
    QSignalMapper *m_signalMapperBlockletter;
};
#endif // GLDKEYBOARDINPUT_H
