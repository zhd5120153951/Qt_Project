#ifndef GLDSEARCHEDIT_H
#define GLDSEARCHEDIT_H

#include <QSize>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>
#include "GLDWidget_Global.h"

class QCompleter;
class QResizeEvent;
class QMouseEvent;
class GLDSearchButton;

class GLDWIDGETSHARED_EXPORT GLDSearchEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(bool hasFocus READ hasFocus)
public:
    GLDSearchEdit(QWidget *parent = 0);
    ~GLDSearchEdit();

public:
    void setRecentSearchs(QStringList &recentSearchs);
    QStringList& recentSearchs();

    bool hasFocus();

protected:
    bool event(QEvent *e);
    void resizeEvent(QResizeEvent *e);
    void mousePressEvent(QMouseEvent *e);

private:
    void initButton();
    void initTipsView();
    void moveRecentTipsView();

private slots:
    void onTextChange(const QString &);
    void onButtonClick();
    void onEditingFinished();
    void onRecentTipsSelected(const QModelIndex &index);

private:
    GLDSearchButton *m_pSearchAndClearButton;
    QStringList m_oRecentSearchs;
    QCompleter *m_pRecentCompleter;
    QString m_sQss;
};

class GLDWIDGETSHARED_EXPORT GLDSearchButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(bool searching READ searching WRITE setSearching)
    Q_PROPERTY(bool hasFocus READ hasFocus WRITE setHasFocus)
public:
    GLDSearchButton(QWidget *parent = 0);
    ~GLDSearchButton();

public:
    void setSearching(bool searching);
    bool searching();

    void setHasFocus(bool value);
    bool hasFocus();

protected:
    bool event(QEvent *e);

private:
    bool m_bSearching;
    bool m_bHasFocus; // EditøÚ «∑Ò”–focus
};
#endif // GLDSEARCHEDIT_H
