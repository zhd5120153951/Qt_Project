#ifndef GLDDATETIMEEDIT_H
#define GLDDATETIMEEDIT_H

#include <QDateTimeEdit>
#include "qabstractspinbox.h"
#include "GLDWidget_Global.h"

class GLDWIDGETSHARED_EXPORT GLDDateTimeEdit : public QDateTimeEdit
{
    Q_OBJECT
    Q_PROPERTY(bool hasSelectedText READ hasSelectedText)
public:
    explicit GLDDateTimeEdit(QWidget *parent = 0);
    explicit GLDDateTimeEdit(const QDateTime &dt, QWidget *parent = 0);

    bool hasSelectedText();

Q_SIGNALS:
    void selectionChanged();
    void cursorPositionChanged();

public slots:
    void cut();
    void paste();
    void copy();
    void deleteSelectedText();

private slots:
    void doSelectionChanged();
    void doCursorPositionChanged(int, int);
};

#endif // GLDDATETIMEEDIT_H
