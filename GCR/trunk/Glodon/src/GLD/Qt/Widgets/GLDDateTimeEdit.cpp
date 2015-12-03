#include "GLDDateTimeEdit.h"
#include "qabstractspinbox.h"
#include <QLineEdit>

GLDDateTimeEdit::GLDDateTimeEdit(QWidget *parent):
    QDateTimeEdit(parent)
{
    connect(lineEdit(), SIGNAL(selectionChanged()),
            this, SLOT(doSelectionChanged()));
    connect(lineEdit(), SIGNAL(cursorPositionChanged(int,int)),
            this, SLOT(doCursorPositionChanged(int,int)));
}

GLDDateTimeEdit::GLDDateTimeEdit(const QDateTime &dt, QWidget *parent):
    QDateTimeEdit(dt, parent)
{
    connect(lineEdit(), SIGNAL(selectionChanged()),
            this, SLOT(doSelectionChanged()));
    connect(lineEdit(), SIGNAL(cursorPositionChanged(int,int)),
            this, SLOT(doCursorPositionChanged(int,int)));
}

bool GLDDateTimeEdit::hasSelectedText()
{
    return lineEdit()->hasSelectedText();
}

void GLDDateTimeEdit::cut()
{
    lineEdit()->cut();
}

void GLDDateTimeEdit::doSelectionChanged()
{
    emit selectionChanged();
}

void GLDDateTimeEdit::doCursorPositionChanged(int, int)
{
    emit cursorPositionChanged();
}

void GLDDateTimeEdit::copy()
{
    lineEdit()->copy();
}

void GLDDateTimeEdit::deleteSelectedText()
{
    if (hasSelectedText())
    {
        lineEdit()->del();
    }
}

void GLDDateTimeEdit::paste()
{
    lineEdit()->paste();
}
