#ifndef GLDABSTRACTBTNEDIT_H
#define GLDABSTRACTBTNEDIT_H

#include <QWidget>
#include <QVector>
#include "GLDWidget_Global.h"

class QLineEdit;
class QTextEdit;
class QPlainTextEdit;
class QButtonGroup;
class QAbstractButton;

class GLDWIDGETSHARED_EXPORT GLDAbstractBtnEdit : public QWidget
{
    Q_OBJECT
public:
    explicit GLDAbstractBtnEdit(QWidget *parent = 0);
    virtual ~GLDAbstractBtnEdit() = 0;
    bool setButtonsCount(int nBtnCount);
    inline int buttonsCount() const;
    inline int maximumButtonsCount() const;
    virtual QString text() const = 0;
    virtual void setText(QString text) = 0;
    bool eventFilter(QObject *obj, QEvent *event);
//    bool connectBtnClicked(BtnClickedSlot btnSlot, int btnIndex);
    bool setBtnText(QString &text, int btnIndex);
    bool setButtonIcon(QIcon &icon, int btnIndex);

signals:
    void ellipsisButtonClicked(int clickedBtnIndex);
    void ellipsisButtonClicked(QAbstractButton *clickedBtn);

public slots:
    virtual void onEllipsisButtonClicked(int clickedBtnIndex);
    virtual void onEllipsisButtonClicked(QAbstractButton *clickedBtn);
    virtual void selectAll() = 0;
    virtual void cursorPosInsertANewLine() = 0;
    virtual void cursorToEnd() = 0;
protected:
    void init();
protected:
    QVector<QSize> m_btnIconSize;
    QVector<int> m_btnUsingIcon;
    QButtonGroup *m_btns;
    QVariant *m_associateData;
    static const int m_maxBtnCount = 5;
private:
    Q_DISABLE_COPY(GLDAbstractBtnEdit)
};

class GLDWIDGETSHARED_EXPORT GLDBtnsLineEdit : public GLDAbstractBtnEdit
{
    Q_OBJECT
public:
    explicit GLDBtnsLineEdit(int btnCount = 2, QWidget *parent = 0);
    ~GLDBtnsLineEdit();
    QString text() const;
    void setText(QString text);
    void paintEvent(QPaintEvent *);
public slots:
    void selectAll();
    void cursorPosInsertANewLine();
    void cursorToEnd();
protected:
    void init(int btnCount);
protected:
    QLineEdit *m_edit;
private:
    Q_DISABLE_COPY(GLDBtnsLineEdit)
};

#endif // GLDABSTRACTBTNEDIT_H
