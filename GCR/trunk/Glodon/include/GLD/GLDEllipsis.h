#ifndef GLDELLIPSIS_H
#define GLDELLIPSIS_H

#include <QString>
#include <QLineEdit>
#include <QSizePolicy>
#include <QPaintEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QModelIndex>
#include <QPlainTextEdit>
#include "GLDWidget_Global.h"

const int c_MinButtonEditBtnWidth = 18;

class GLDWIDGETSHARED_EXPORT GLDAbstractButtonEdit: public QWidget
{
    Q_OBJECT

public:
    GLDAbstractButtonEdit(QWidget *parent);
    virtual ~GLDAbstractButtonEdit();

    virtual QString text() const = 0;
    virtual void setText(QString text) = 0;

    QString buttonCaption() const;
    void setButtonCaption(QString caption);

    QModelIndex modelIndex() const;
    void setModelIndex(QModelIndex newIndex);

    virtual void setEditable(bool canEdit) = 0;
    virtual bool selectAll() = 0;
    virtual bool cursorPosInsertANewLine(bool addANewLine = true) = 0;
    virtual bool cursorMoveTextEnd() = 0;
    virtual QMargins contentsMargins() const = 0;
    virtual void setContentsMargins(const QMargins &rhs) const = 0;
    virtual void setHasBorder(bool) {}

public Q_SLOTS:
    void onEllipsisButtonClicked();

//protected Q_SLOTS:
//    bool event(QEvent *e);

Q_SIGNALS:
    void ellipsisButtonClicked();
    void ellipsisButtonClicked(const QModelIndex &index);

protected:
    QPushButton *m_button;
    QModelIndex index;
};

class GLDWIDGETSHARED_EXPORT GLDPlainButtonEdit : public GLDAbstractButtonEdit
{
    Q_OBJECT
    Q_PROPERTY(bool readOnly READ isReadOnly)
    Q_PROPERTY(bool hasSelectedText READ hasSelectedText)
public:
    GLDPlainButtonEdit(QWidget *parent);
    virtual ~GLDPlainButtonEdit();

    void paintEvent(QPaintEvent *);

    QString text() const;
    void setText(QString text);
    void setEditable(bool canEdit);
    bool eventFilter(QObject *, QEvent *);
    bool selectAll();
    bool cursorPosInsertANewLine(bool addANewLine = true);
    bool cursorMoveTextEnd();
    QMargins contentsMargins() const;
    void setContentsMargins(const QMargins &rhs) const;
    void setFont(const QFont &font);

    inline bool hasSelectedText(){ return m_hasSelectedText; }
    inline bool isReadOnly(){ return m_plainTextEdit->isReadOnly(); }

public slots:
    void cut();
    void paste();
    void copy();
    void deleteSelectedText();

signals:
    void copyAvailable(bool yes);

private slots:
    void onCopyAvailable(bool yes);

protected:
    QPlainTextEdit *m_plainTextEdit;

private:
    bool m_hasSelectedText;
};

class GLDWIDGETSHARED_EXPORT GLDEllipsisLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit GLDEllipsisLineEdit(QWidget *parent = 0);

    void closeCopy();
    void openCopy();
    void closePaste();
    void openPaste();
    void keyPressEvent(QKeyEvent *e);

private:
    bool m_closePaste;
    bool m_closeCopy;
};

class GLDWIDGETSHARED_EXPORT GLDLineButtonEdit : public GLDAbstractButtonEdit
{
    Q_OBJECT
    Q_PROPERTY(bool readOnly READ isReadOnly)
    Q_PROPERTY(bool hasSelectedText READ hasSelectedText)
public:
    GLDLineButtonEdit(QWidget *parent, QLineEdit *editor = NULL);
    virtual ~GLDLineButtonEdit();

    void paintEvent(QPaintEvent *);

    QString text() const;
    void setText(QString text);
    void setEditable(bool canEdit);
    bool eventFilter(QObject *, QEvent *);
    bool selectAll();
    bool cursorPosInsertANewLine(bool addANewLine = true);
    bool cursorMoveTextEnd();
    virtual QMargins contentsMargins() const;
    void setContentsMargins(const QMargins &rhs) const;
    void setFont(const QFont &font);

    inline bool hasSelectedText(){ return m_lineEdit->hasSelectedText(); }

    inline bool isReadOnly(){ return m_lineEdit->isReadOnly(); }
    inline QLineEdit* lineEdit() { return m_lineEdit; }

    void closeCopy();
    void openCopy();
    void closePaste();
    void openPaste();
public slots:
    void cut();
    void paste();
    void copy();
    void deleteSelectedText();

signals:
    void selectionChanged();
    void cursorPositionChanged();

private slots:
    void doSelectionChanged();
    void doCursorPositionChanged(int, int);

protected:
    QLineEdit *m_lineEdit;
};

class GLDWIDGETSHARED_EXPORT GLDLineButtonEditEx: public GLDLineButtonEdit
{
public:
    GLDLineButtonEditEx(QWidget *parent = 0, QLineEdit *editor = NULL);
    ~GLDLineButtonEditEx();

public:
    void paintEvent(QPaintEvent *e);
    virtual void setHasBorder(bool enable = true);
};

class GLDWIDGETSHARED_EXPORT GLDPlainButtonEditEx : public GLDPlainButtonEdit
{
public:
    GLDPlainButtonEditEx(QWidget *parent = 0);
    ~GLDPlainButtonEditEx();

public:
    void paintEvent(QPaintEvent *e);
    virtual void setHasBorder(bool bHasBorder = true);
};

#endif // GLDELLIPSIS_H
