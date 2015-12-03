#ifndef GLDDOUBLESPINBOX_H
#define GLDDOUBLESPINBOX_H

#include <QDoubleSpinBox>
#include <QDoubleValidator>
#include <QSpinBox>
#include "qabstractspinbox.h"
#include "GLDNameSpace.h"
#include "GLDWidget_Global.h"

G_GLODON_BEGIN_NAMESPACE

class GLDWIDGETSHARED_EXPORT GLDSpinBox : public QSpinBox
{
    Q_OBJECT
    Q_PROPERTY(bool hasSelectedText READ hasSelectedText)
public:
    explicit GLDSpinBox(QWidget *parent = 0);

    void setWheelEnable(bool enabled) { m_wheelEnable = enabled; }
    inline bool wheelEnabled() const { return m_wheelEnable; }

    void setUpAndDownEnable(bool enabled) { m_upAndDownEnable = enabled; }
    inline bool upAndDownEnabled() const {return m_upAndDownEnable;}

    bool hasSelectedText();

signals:
    void selectionChanged();
    void cursorPositionChanged();

public slots:
    void cut();
    void copy();
    void paste();
    void deleteSelectedText();

protected:
    void keyPressEvent(QKeyEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event);
#endif

protected:
    bool m_wheelEnable;
    bool m_upAndDownEnable;

private slots:
    void doSelectionChanged();
    void doCursorPositionChanged(int, int);

};

class GLDWIDGETSHARED_EXPORT GLDDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
    Q_PROPERTY(bool hasSelectedText READ hasSelectedText)
public:
    explicit GLDDoubleSpinBox(QWidget *parent = 0);
    ~GLDDoubleSpinBox();
public:
    virtual QValidator::State validate(QString &input, int &pos) const;
    virtual double valueFromText(const QString &text) const;
    virtual QString textFromValue(double val) const;

    inline bool decimalsEnable() const { return m_decimalsEnable; }
    void setDecimalsEnable(bool value);

    void setWheelEnable(bool enabled) { m_wheelEnable = enabled; }
    inline bool wheelEnabled() const { return m_wheelEnable; }

    void setUpAndDownEnable(bool enabled) { m_upAndDownEnable = enabled; }
    inline bool upAndDownEnabled() const {return m_upAndDownEnable;}

    bool hasSelectedText();
public slots:
    void cut();
    void copy();
    void paste();
    void deleteSelectedText();

signals:
    void selectionChanged();
    void cursorPositionChanged();

protected:
    void keyPressEvent(QKeyEvent *event);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event);
#endif
    //change public to protected
    void setDecimals(int prec);
    static const int normalDecimals = 15;
private:

protected:
    enum NumberMode { IntegerMode, DoubleStandardMode, DoubleScientificMode };

private:
    bool m_decimalsEnable;
    bool m_wheelEnable;
    bool m_upAndDownEnable;
    int m_orgDecimals;
    mutable QDoubleValidator *m_validator;

private slots:
    void doSelectionChanged();
    void doCursorPositionChanged(int, int);
};

class GLDWIDGETSHARED_EXPORT GLDSpinBoxEx : public GLDSpinBox
{
    Q_OBJECT
public:
    explicit GLDSpinBoxEx(QWidget *parent = 0);
    ~GLDSpinBoxEx(){}

public:
    void setHasBorder(bool bHasBorder);

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void resizeEvent(QResizeEvent *event);
    void updateEditFieldGeometry();
};

class GLDWIDGETSHARED_EXPORT GLDDoubleSpinBoxEx : public GLDDoubleSpinBox
{
    Q_OBJECT
public:
    explicit GLDDoubleSpinBoxEx(QWidget *parent = 0);
    ~GLDDoubleSpinBoxEx(){}

public:
    void setHasBorder(bool bHasBorder);

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void resizeEvent(QResizeEvent *event);
    void updateEditFieldGeometry();
};

G_GLODON_END_NAMESPACE

#endif // GLDDOUBLESPINBOX_H
