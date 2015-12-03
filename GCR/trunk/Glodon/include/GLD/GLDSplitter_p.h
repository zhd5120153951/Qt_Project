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

#ifndef QSPLITTER_P_H
#define QSPLITTER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
// It is copied from qt5.1.1/qsplitter_p.h by chenq-a for GLDNetScapeSpliter
// at 2014/6/19.
// We mean it.
//

#include <QList>
#include "private/qwidget_p.h"
#include "qrubberband.h"
#include "GLDNameSpace.h"

//G_GLODON_BEGIN_NAMESPACE

template <typename T> class QList;

#define GLD_DECLARE_PUBLIC(Class)                                    \
    inline Class* q_func() { return static_cast<Class *>(q_ptr); } \
    inline const Class* q_func() const { return static_cast<const Class *>(q_ptr); } \
    friend class Class;

static const uint Default = 2;

class QFramePrivate : public QWidgetPrivate
{
//    GLD_DECLARE_PUBLIC(QFrame)
public:
    QFramePrivate() : frect(QRect(0, 0, 0, 0)), frameStyle(QFrame::NoFrame | QFrame::Plain),
        lineWidth(1), midLineWidth(0), frameWidth(0), leftFrameWidth(0), rightFrameWidth(0),
        topFrameWidth(0), bottomFrameWidth(0){}

    void        updateFrameWidth(){}
    void        updateStyledFrameWidths(){}

    QRect       frect;
    int         frameStyle;
    short       lineWidth;
    short       midLineWidth;
    short       frameWidth;
    short       leftFrameWidth, rightFrameWidth;
    short       topFrameWidth, bottomFrameWidth;

    inline void init() { setLayoutItemMargins(QStyle::SE_FrameLayoutItem); }

};
class GLDSplitterHandle;
class GLDSplitterLayoutStruct
{
public:
    QRect rect;
    int sizer;
    uint collapsed : 1;
    uint collapsible : 2;
    QWidget *widget;
    GLDSplitterHandle *handle;

    GLDSplitterLayoutStruct() : sizer(-1), collapsed(false), collapsible(Default), widget(0), handle(0) {}
    ~GLDSplitterLayoutStruct() { delete handle; }
    int getWidgetSize(Qt::Orientation orient);
    int getHandleSize(Qt::Orientation orient);
    int pick(const QSize &size, Qt::Orientation orient)
    { return (orient == Qt::Horizontal) ? size.width() : size.height(); }
};

class GLDSplitter;
class GLDSplitterPrivate : public QFramePrivate
{
    GLD_DECLARE_PUBLIC(GLDSplitter)
public:
    GLDSplitterPrivate() : rubberBand(0), opaque(true), firstShow(true),
                         childrenCollapsible(true), compatMode(false), handleWidth(-1), collapsedLength(-1), blockChildAdd(false) {}

    QPointer<QRubberBand> rubberBand;
    mutable QList<GLDSplitterLayoutStruct *> list;
    Qt::Orientation orient;
    bool opaque : 8;
    bool firstShow : 8;
    bool childrenCollapsible : 8;
    bool compatMode : 8;
    int handleWidth;
    int collapsedLength;
    bool blockChildAdd;

    inline int pick(const QPoint &pos) const
    { return orient == Qt::Horizontal ? pos.x() : pos.y(); }
    inline int pick(const QSize &s) const
    { return orient == Qt::Horizontal ? s.width() : s.height(); }

    inline int trans(const QPoint &pos) const
    { return orient == Qt::Vertical ? pos.x() : pos.y(); }
    inline int trans(const QSize &s) const
    { return orient == Qt::Vertical ? s.width() : s.height(); }

    void init();
    void recalc(bool update = false);
    void doResize(const QList<int>* pSizes = 0);
    void storeSizes();
    void getRange(int index, int *, int *, int *, int *) const;
    void addContribution(int, int *, int *, bool) const;
    int adjustPos(int, int, int *, int *, int *, int *) const;
    bool collapsible(GLDSplitterLayoutStruct *) const;
    bool collapsible(int index) const
    { return (index < 0 || index >= list.size()) ? true : collapsible(list.at(index)); }
    GLDSplitterLayoutStruct *findWidget(QWidget *) const;
    void insertWidget_helper(int index, QWidget *widget, bool show);
    GLDSplitterLayoutStruct *insertWidget(int index, QWidget *);
    void doMove(bool backwards, int pos, int index, int delta,
                bool mayCollapse, int *positions, int *widths);
    void setGeo(GLDSplitterLayoutStruct *s, int pos, int size, bool allowCollapse);
    int findWidgetJustBeforeOrJustAfter(int index, int delta, int &collapsibleSize) const;
    void updateHandles();
    void setSizes_helper(const QList<int> &sizes, bool clampNegativeSize = false);

};

class GLDSplitterHandlePrivate : public QWidgetPrivate
{
    GLD_DECLARE_PUBLIC(GLDSplitterHandle)
public:
    GLDSplitterHandlePrivate() : s(0), orient(Qt::Horizontal), mouseOffset(0), opaq(false), hover(false), pressed(false)
    {}

    inline int pick(const QPoint &pos) const
    { return orient == Qt::Horizontal ? pos.x() : pos.y(); }

    GLDSplitter *s;
    Qt::Orientation orient;
    int mouseOffset;
    bool opaq    : 1;
    bool hover   : 1;
    bool pressed : 1;
};

//G_GLODON_END_NAMESPACE

#endif
