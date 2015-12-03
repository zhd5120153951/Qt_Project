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

#ifndef GLDSPLITTER_H
#define GLDSPLITTER_H

#include <QFrame>

#include "GLDNameSpace.h"
#include "GLDSplitterHandle.h"
#include "GLDWidget_Global.h"

#ifndef QT_NO_SPLITTER

class GLDSplitterPrivate;
class QTextStream;
class GLDSplitterHandle;

class GLDWIDGETSHARED_EXPORT GLDSplitter : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(bool opaqueResize READ opaqueResize WRITE setOpaqueResize)
    Q_PROPERTY(int handleWidth READ handleWidth WRITE setHandleWidth)
    Q_PROPERTY(bool childrenCollapsible READ childrenCollapsible WRITE setChildrenCollapsible)

public:
    explicit GLDSplitter(QWidget *parent = 0);
    explicit GLDSplitter(Qt::Orientation, QWidget *parent = 0);
    ~GLDSplitter();

public:
    void setOrientation(Qt::Orientation);
    Qt::Orientation orientation() const;

    void setChildrenCollapsible(bool);
    bool childrenCollapsible() const;

    void setCollapsible(int index, bool);
    bool isCollapsible(int index) const;

    void setOpaqueResize(bool opaque = true);
    bool opaqueResize() const;

public:
    void addWidget(QWidget *widget);
    void insertWidget(int index, QWidget *widget);
    void refresh();

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    QList<int> sizes() const;
    void setSizes(const QList<int> &list);

    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);

    int handleWidth() const;
    void setHandleWidth(int);

    int collapsedLength() const;
    void setCollapsedLength(int length);

    int indexOf(QWidget *w) const;
    QWidget *widget(int index) const;
    int count() const;

    void getRange(int index, int *, int *) const;
    GLDSplitterHandle *handle(int index) const;

    void setStretchFactor(int index, int stretch);

Q_SIGNALS:
    void splitterMoved(int pos, int index);

protected:
    virtual GLDSplitterHandle *createHandle();

    void childEvent(QChildEvent *);
    void resizeEvent(QResizeEvent *);
    void changeEvent(QEvent *);

    bool event(QEvent *);

    void moveSplitter(int pos, int index);
    void setRubberBand(int position);

    int closestLegalPosition(int, int);

private:
    Q_DISABLE_COPY(GLDSplitter)
    Q_DECLARE_PRIVATE(GLDSplitter)

private:
    friend class GLDSplitterHandle;
};

QTextStream &operator<<(QTextStream &, const GLDSplitter &);
QTextStream &operator>>(QTextStream &, GLDSplitter &);

#endif // QT_NO_SPLITTER

#endif // QSPLITTER_H
