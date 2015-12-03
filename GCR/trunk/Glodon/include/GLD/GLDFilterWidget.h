/****************************************************************************************************

  补充数据过滤控件

  作者: jiangjb 2014-03-13
  备注:
  审核:

  Copyright (c) 1994-2013 Glodon Corporation

****************************************************************************************************/
#ifndef GLDFILTERWIDGET_H
#define GLDFILTERWIDGET_H

#include <QWidget>
#include <QToolButton>
#include <QLineEdit>
#include "GLDString.h"
#include "GLDWidget_Global.h"

#define ICONBUTTON_SIZE 16

class GLDWIDGETSHARED_EXPORT GLDHintLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit GLDHintLineEdit(QWidget *parent = 0);

    bool refuseFocus() const;
    void setRefuseFocus(bool v);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void focusInEvent(QFocusEvent *e);

private:
    const Qt::FocusPolicy m_eDefaultFocusPolicy;
    bool m_bRefuseFocus;
};


// IconButton: This is a simple helper class that represents clickable icons
class GLDWIDGETSHARED_EXPORT GLDIconButton: public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(float fader READ fader WRITE setFader)
public:
    GLDIconButton(QWidget *parent);

    float fader();
    void setFader(float value);
    void animateShow(bool visible);

protected:
    void paintEvent(QPaintEvent *event);

private:
    float m_fFader;
};


class GLDWIDGETSHARED_EXPORT GLDFilterWidget : public QWidget
{
    Q_OBJECT
public:
    enum GLDLayoutMode {
        // For use in toolbars: Expand to the right
        lmAlignRight,
        // No special alignment
        lmAlignNone
    };

    explicit GLDFilterWidget(QWidget *parent = 0, GLDLayoutMode mode = lmAlignRight);
    GLDFilterWidget(QIcon icon, QWidget *parent = 0, GLDLayoutMode mode = lmAlignRight);

    GString text() const;
    void setText(const GString str);
    void resizeEvent(QResizeEvent *);
    bool refuseFocus() const; // see HintLineEdit
    void setRefuseFocus(bool v);
    void setAutoResize(bool autoResize);
    void setInputPlaceholderText(const GString &text);
signals:
    void filterChanged(const GString &);

public slots:
    void reset();

private slots:
    void checkButton(const GString &text);
private:
    void init(GLDLayoutMode mode, QIcon value);

private:
    GLDHintLineEdit *m_pLedtEditor;
    GLDIconButton *m_pBtnIcon;
    int m_nButtonWidth;
    GString m_sOldText;
    bool m_autoResize;//false
    GString m_inputPlaceholderText;
};

#endif // GLDFILTERWIDGET_H
