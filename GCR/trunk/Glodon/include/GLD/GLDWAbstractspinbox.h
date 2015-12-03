/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtWidgets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef GLDWABSTRACTSPINBOX_H
#define GLDWABSTRACTSPINBOX_H

#include <QtWidgets/qwidget.h>
#include <QtGui/qvalidator.h>
#include <QAbstractSpinBox>
//#include "GLDWAbstractspinbox_p.h"
#include "GLDWidget_Global.h"

#ifndef QT_NO_SPINBOX

class GLDPlainTextEdit;

class GLDWAbstractSpinBoxPrivate;
class QStyleOptionSpinBox;

class GLDWIDGETSHARED_EXPORT GLDWAbstractSpinBox : public QWidget
{
    Q_OBJECT

    Q_ENUMS(ButtonSymbols)
    Q_ENUMS(CorrectionMode)
    Q_PROPERTY(bool wrapping READ wrapping WRITE setWrapping)
    Q_PROPERTY(bool frame READ hasFrame WRITE setFrame)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
//    Q_PROPERTY(ButtonSymbols buttonSymbols READ buttonSymbols WRITE setButtonSymbols)
    Q_PROPERTY(QString specialValueText READ specialValueText WRITE setSpecialValueText)
    Q_PROPERTY(QString text READ text)
    Q_PROPERTY(bool accelerated READ isAccelerated WRITE setAccelerated)
//    Q_PROPERTY(CorrectionMode correctionMode READ correctionMode WRITE setCorrectionMode)
    Q_PROPERTY(bool acceptableInput READ hasAcceptableInput)
    Q_PROPERTY(bool keyboardTracking READ keyboardTracking WRITE setKeyboardTracking)
public:
    explicit GLDWAbstractSpinBox(QWidget *parent = 0);
    ~GLDWAbstractSpinBox();

//    Q_DECLARE_FLAGS(StepEnabled, StepEnabledFlag)

    QAbstractSpinBox::ButtonSymbols buttonSymbols() const;
    void setButtonSymbols(QAbstractSpinBox::ButtonSymbols bs);

    void setCorrectionMode(QAbstractSpinBox::CorrectionMode cm);
    QAbstractSpinBox::CorrectionMode correctionMode() const;

    bool hasAcceptableInput() const;
    QString text() const;

    QString specialValueText() const;
    void setSpecialValueText(const QString &txt);

    bool wrapping() const;
    void setWrapping(bool w);

    void setReadOnly(bool r);
    bool isReadOnly() const;

    void setKeyboardTracking(bool kt);
    bool keyboardTracking() const;

    void setAlignment(Qt::Alignment flag);
    Qt::Alignment alignment() const;

    void setFrame(bool);
    bool hasFrame() const;

    void setAccelerated(bool on);
    bool isAccelerated() const;

    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    void interpretText();
    bool event(QEvent *event);

    QVariant inputMethodQuery(Qt::InputMethodQuery) const;
public Q_SLOTS:
//    void stepUp();
//    void stepDown();
    void selectAll();
    virtual void clear();
protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event);
#endif
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);
    void hideEvent(QHideEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void timerEvent(QTimerEvent *event);
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent *event);
    void initStyleOption(QStyleOptionSpinBox *option) const;

    GLDPlainTextEdit *lineEdit() const;
    void setLineEdit(GLDPlainTextEdit *edit);

//    virtual StepEnabled stepEnabled() const;
Q_SIGNALS:
    void editingFinished();
protected:
    GLDWAbstractSpinBox(GLDWAbstractSpinBoxPrivate &dd, QWidget *parent = 0);

private:
//    Q_PRIVATE_SLOT(d_func(), void _q_editorTextChanged(const QString &))
//    Q_PRIVATE_SLOT(d_func(), void _q_editorCursorPositionChanged(int, int))

    Q_DECLARE_PRIVATE(GLDWAbstractSpinBox)
    Q_DISABLE_COPY(GLDWAbstractSpinBox)
};

//Q_DECLARE_OPERATORS_FOR_FLAGS(QAbstractSpinBox::StepEnabled)

#endif // QT_NO_SPINBOX

#endif // GLDWABSTRACTSPINBOX_H
