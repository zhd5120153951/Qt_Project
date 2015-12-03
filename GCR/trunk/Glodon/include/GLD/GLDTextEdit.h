#ifndef GLDTEXTEDIT_H
#define GLDTEXTEDIT_H

#include <QPlainTextEdit>
#include <QLineEdit>
#include "GLDWidget_Global.h"

class GLDWIDGETSHARED_EXPORT GLDPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

    Q_PROPERTY(bool hasSelectedText READ hasSelectedText)
public:
    explicit GLDPlainTextEdit(QWidget *parent = 0);
    
    bool hasSelectedText();
    QString text();
    void setText(QString text);

    QString placeholderText() const;
    void setPlaceholderText(const QString &placeholderText);
    //强制选择颜色为蓝色（在右键快捷菜单的时候，仍然是蓝色）
    void forceSelectColorToBlue();
    void setFouseIsKeepInLineEdit(bool isAddFouseSetting);//设置是否在peintevent中添加设置焦点的函数，用于实现gldwindowcombobox的焦点问题

signals:
    void textChanged(const QString &str);

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
public slots:
    void deleteSelectedText();
    friend class GLDWAbstractSpinBox;

private slots:
    void onTextChanged();

private:
    QString m_text;
    QString m_placeholderText;
    bool m_isFouseKeepInLineEdit;
};

class GLDWIDGETSHARED_EXPORT GLDPlainTextEditEx : public GLDPlainTextEdit
{
    Q_OBJECT
public:
    explicit GLDPlainTextEditEx(QWidget *parent = 0);
};

class GLDWIDGETSHARED_EXPORT GLDCustomLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit GLDCustomLineEdit(QWidget *parent = 0);
    void setHasBorder(bool bValue);

public slots:
    void deleteSelectedText();

private:
    bool m_hasBorder;
};

#endif // GLDTEXTEDIT_H
