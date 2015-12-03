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

#include "GLDSplitter.h"
#include "GLDSplitter_p.h"
#ifndef QT_NO_SPLITTER

#include "qapplication.h"
#include "qcursor.h"
#include "qdrawutil.h"
#include "qevent.h"
#include "qlayout.h"
//#include "qlist.h"
#include "qpainter.h"
#include "qrubberband.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qtextstream.h"
#include "qvarlengtharray.h"
#include "qvector.h"
#include "private/qlayoutengine_p.h"

#include "qtimer.h"
#ifdef GLDSPLITTER_DEBUG
#include "qdebug.h"
#endif
#include <qnamespace.h>

#include <ctype.h>

//G_GLODON_BEGIN_NAMESPACE

//#define GLDSPLITTER_DEBUG


int GLDSplitterLayoutStruct::getWidgetSize(Qt::Orientation orient)
{
    if (sizer == -1)
    {
        QSize s = widget->sizeHint();
        const int presizer = pick(s, orient);
        const int realsize = pick(widget->size(), orient);

        if (!s.isValid() || (widget->testAttribute(Qt::WA_Resized) && (realsize > presizer)))
        {
            sizer = pick(widget->size(), orient);
        }
        else
        {
            sizer = presizer;
        }

        QSizePolicy p = widget->sizePolicy();
        int sf = (orient == Qt::Horizontal) ? p.horizontalStretch() : p.verticalStretch();

        if (sf > 1)
        {
            sizer *= sf;
        }
    }

    return sizer;
}

int GLDSplitterLayoutStruct::getHandleSize(Qt::Orientation orient)
{
    return pick(handle->sizeHint(), orient);
}

void GLDSplitterPrivate::init()
{
    Q_Q(GLDSplitter);
    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Preferred);

    if (orient == Qt::Vertical)
    {
        sp.transpose();
    }

    q->setSizePolicy(sp);
    q->setAttribute(Qt::WA_WState_OwnSizePolicy, false);
}

void GLDSplitterPrivate::recalc(bool update)
{
    Q_Q(GLDSplitter);
    int n = list.count();
    /*
      Splitter handles before the first visible widget or right
      before a hidden widget must be hidden.
    */
    bool first = true;
    bool allInvisible = n != 0;

    for (int i = 0; i < n ; ++i)
    {
        GLDSplitterLayoutStruct *s = list.at(i);
        bool widgetHidden = s->widget->isHidden();

        if (allInvisible && !widgetHidden && !s->collapsed)
        {
            allInvisible = false;
        }

        s->handle->setHidden(first || widgetHidden);

        if (!widgetHidden)
        {
            first = false;
        }
    }

    if (allInvisible)
        for (int i = 0; i < n ; ++i)
        {
            GLDSplitterLayoutStruct *s = list.at(i);

            if (!s->widget->isHidden())
            {
                s->collapsed = false;
                break;
            }
        }

    int fi = 2 * q->frameWidth();
    int maxl = fi;
    int minl = fi;
    int maxt = QWIDGETSIZE_MAX;
    int mint = fi;
    /*
      calculate min/max sizes for the whole splitter
    */
    bool empty = true;

    for (int j = 0; j < n; j++)
    {
        GLDSplitterLayoutStruct *s = list.at(j);

        if (!s->widget->isHidden())
        {
            empty = false;

            if (!s->handle->isHidden())
            {
                minl += s->getHandleSize(orient);
                maxl += s->getHandleSize(orient);
            }

            QSize minS = qSmartMinSize(s->widget);
            minl += pick(minS);
            maxl += pick(s->widget->maximumSize());
            mint = qMax(mint, trans(minS));
            int tm = trans(s->widget->maximumSize());

            if (tm > 0)
            {
                maxt = qMin(maxt, tm);
            }
        }
    }

    if (empty)
    {
        if (dynamic_cast<GLDSplitter *>(parent))
        {
            // nested splitters; be nice
            maxl = maxt = 0;
        }
        else
        {
            // GLDSplitter with no children yet
            maxl = QWIDGETSIZE_MAX;
        }
    }
    else
    {
        maxl = qMin<int>(maxl, QWIDGETSIZE_MAX);
    }

    if (maxt < mint)
    {
        maxt = mint;
    }

    if (update)
    {
        if (orient == Qt::Horizontal)
        {
            q->setMaximumSize(maxl, maxt);

            if (q->isWindow())
            {
                q->setMinimumSize(minl, mint);
            }
        }
        else
        {
            q->setMaximumSize(maxt, maxl);

            if (q->isWindow())
            {
                q->setMinimumSize(mint, minl);
            }
        }

        doResize();
        q->updateGeometry();
    }
    else
    {
        firstShow = true;
    }
}

void GLDSplitterPrivate::doResize(const QList<int> *pSizes)
{
    Q_Q(GLDSplitter);
    QRect r = q->contentsRect();
    int n = list.count();
    QVector<QLayoutStruct> a(n * 2);
    int i;

    bool noStretchFactorsSet = true;

    for (i = 0; i < n; ++i)
    {
        QSizePolicy p = list.at(i)->widget->sizePolicy();
        int sf = orient == Qt::Horizontal ? p.horizontalStretch() : p.verticalStretch();

        if (sf != 0)
        {
            noStretchFactorsSet = false;
            break;
        }
    }

    int j = 0;

    for (i = 0; i < n; ++i)
    {
        GLDSplitterLayoutStruct *s = list.at(i);
        int nSizeWanted = -1;

        if (pSizes)
        {
            nSizeWanted = pSizes->at(i);
        }

#ifdef GLDSPLITTER_DEBUG
        qDebug("widget %d hidden: %d collapsed: %d handle hidden: %d", i, s->widget->isHidden(),
               s->collapsed, s->handle->isHidden());
#endif

        a[j].init();

        if (s->handle->isHidden())
        {
            a[j].maximumSize = 0;
        }
        else
        {
            a[j].sizeHint = a[j].minimumSize = a[j].maximumSize = s->getHandleSize(orient);
            a[j].empty = false;
        }

        ++j;

        a[j].init();

        if (s->widget->isHidden() || s->collapsed)
        {
            a[j].maximumSize = 0;
        }
        else
        {
            a[j].minimumSize = pick(qSmartMinSize(s->widget));
            a[j].maximumSize = pick(s->widget->maximumSize());
            a[j].empty = false;

            bool stretch = noStretchFactorsSet;

            if (!stretch)
            {
                QSizePolicy p = s->widget->sizePolicy();
                int sf = orient == Qt::Horizontal ? p.horizontalStretch() : p.verticalStretch();
                stretch = (sf != 0);
            }

            if (stretch)
            {
                a[j].stretch = s->getWidgetSize(orient);
                a[j].sizeHint = a[j].minimumSize;
                a[j].expansive = true;
            }
            else
            {
                a[j].sizeHint = qMax(s->getWidgetSize(orient), a[j].minimumSize);
            }

            if (nSizeWanted > 0 && nSizeWanted != a[j].sizeHint)
            {
                a[j].sizeHint = nSizeWanted;

                if (nSizeWanted > a[j].minimumSize)
                {
                    a[j].minimumSize = nSizeWanted;
                }
                else if (nSizeWanted < a[j].minimumSize)
                {
                    a[j].minimumSize = nSizeWanted;
                }
            }
        }

        ++j;
    }

    qGeomCalc(a, 0, n * 2, pick(r.topLeft()), pick(r.size()), 0);

#ifdef GLDSPLITTER_DEBUG

    for (i = 0; i < n * 2; ++i)
    {
        qDebug("%*s%d: stretch %d, sh %d, minS %d, maxS %d, exp %d, emp %d -> %d, %d",
               i, "", i,
               a[i].stretch,
               a[i].sizeHint,
               a[i].minimumSize,
               a[i].maximumSize,
               a[i].expansive,
               a[i].empty,
               a[i].pos,
               a[i].size);
    }

#endif

    for (i = 0; i < n; ++i)
    {
        GLDSplitterLayoutStruct *s = list.at(i);
        setGeo(s, a[i * 2 + 1].pos, a[i * 2 + 1].size, false);
    }
}

void GLDSplitterPrivate::storeSizes()
{
    for (int i = 0; i < list.size(); ++i)
    {
        GLDSplitterLayoutStruct *sls = list.at(i);
        sls->sizer = pick(sls->rect.size());
    }
}

void GLDSplitterPrivate::addContribution(int index, int *min, int *max, bool mayCollapse) const
{
    GLDSplitterLayoutStruct *s = list.at(index);

    if (!s->widget->isHidden())
    {
        if (!s->handle->isHidden())
        {
            *min += s->getHandleSize(orient);
            *max += s->getHandleSize(orient);
        }

        if (mayCollapse || !s->collapsed)
        {
            *min += pick(qSmartMinSize(s->widget));
        }

        *max += pick(s->widget->maximumSize());
    }
}

int GLDSplitterPrivate::findWidgetJustBeforeOrJustAfter(int index, int delta, int &collapsibleSize) const
{
    if (delta < 0)
    {
        index += delta;
    }

    do
    {
        QWidget *w = list.at(index)->widget;

        if (!w->isHidden())
        {
            if (collapsible(list.at(index)))
            {
                collapsibleSize = pick(qSmartMinSize(w));
            }

            return index;
        }

        index += delta;
    }
    while (index >= 0 && index < list.count());

    return -1;
}

/*
  For the splitter handle with index \a index, \a min and \a max give the range without collapsing any widgets,
  and \a farMin and farMax give the range with collapsing included.
*/
void GLDSplitterPrivate::getRange(int index, int *farMin, int *min, int *max, int *farMax) const
{
    Q_Q(const GLDSplitter);
    int n = list.count();

    if (index <= 0 || index >= n)
    {
        return;
    }

    int collapsibleSizeBefore = 0;
    int idJustBefore = findWidgetJustBeforeOrJustAfter(index, -1, collapsibleSizeBefore);

    int collapsibleSizeAfter = 0;
    int idJustAfter = findWidgetJustBeforeOrJustAfter(index, +1, collapsibleSizeAfter);

    int minBefore = 0;
    int minAfter = 0;
    int maxBefore = 0;
    int maxAfter = 0;
    int i;

    for (i = 0; i < index; ++i)
    {
        addContribution(i, &minBefore, &maxBefore, i == idJustBefore);
    }

    for (i = index; i < n; ++i)
    {
        addContribution(i, &minAfter, &maxAfter, i == idJustAfter);
    }

    QRect r = q->contentsRect();
    int farMinVal;
    int minVal;
    int maxVal;
    int farMaxVal;

    int smartMinBefore = qMax(minBefore, pick(r.size()) - maxAfter);
    int smartMaxBefore = qMin(maxBefore, pick(r.size()) - minAfter);

    minVal = pick(r.topLeft()) + smartMinBefore;
    maxVal = pick(r.topLeft()) + smartMaxBefore;

    farMinVal = minVal;

    if (minBefore - collapsibleSizeBefore >= pick(r.size()) - maxAfter)
    {
        farMinVal -= collapsibleSizeBefore;
    }

    farMaxVal = maxVal;

    if (pick(r.size()) - (minAfter - collapsibleSizeAfter) <= maxBefore)
    {
        farMaxVal += collapsibleSizeAfter;
    }

    if (farMin)
    {
        *farMin = farMinVal;
    }

    if (min)
    {
        *min = minVal;
    }

    if (max)
    {
        *max = maxVal;
    }

    if (farMax)
    {
        *farMax = farMaxVal;
    }
}

int GLDSplitterPrivate::adjustPos(int pos, int index, int *farMin, int *min, int *max, int *farMax) const
{
    const int Threshold = 40;

    getRange(index, farMin, min, max, farMax);

    if (pos >= *min)
    {
        if (pos <= *max)
        {
            return pos;
        }
        else
        {
            int delta = pos - *max;
            int width = *farMax - *max;

            if (delta > width / 2 && delta >= qMin(Threshold, width))
            {
                return *farMax;
            }
            else
            {
                return *max;
            }
        }
    }
    else
    {
        int delta = *min - pos;
        int width = *min - *farMin;

        if (delta > width / 2 && delta >= qMin(Threshold, width))
        {
            return *farMin;
        }
        else
        {
            return *min;
        }
    }
}

bool GLDSplitterPrivate::collapsible(GLDSplitterLayoutStruct *s) const
{
    if (s->collapsible != Default)
    {
        return (bool)s->collapsible;
    }
    else
    {
        return childrenCollapsible;
    }
}

void GLDSplitterPrivate::updateHandles()
{
    Q_Q(GLDSplitter);
    recalc(q->isVisible());
}

void GLDSplitterPrivate::setSizes_helper(const QList<int> &sizes, bool clampNegativeSize)
{
    int j = 0;

    for (int i = 0; i < list.size(); ++i)
    {
        GLDSplitterLayoutStruct *s = list.at(i);

        s->collapsed = false;
        s->sizer = sizes.value(j++);

        if (clampNegativeSize && s->sizer < 0)
        {
            s->sizer = 0;
        }

        int smartMinSize = pick(qSmartMinSize(s->widget));

        // Make sure that we reset the collapsed state.
        if (s->sizer == 0)
        {
            if (collapsible(s) && smartMinSize > 0)
            {
                s->collapsed = true;
            }
            else
            {
                s->sizer = smartMinSize;
            }
        }
        else
        {
            if (s->sizer < smartMinSize)
            {
                s->sizer = smartMinSize;
            }
        }
    }

    doResize(&sizes);
}

void GLDSplitterPrivate::setGeo(GLDSplitterLayoutStruct *sls, int p, int s, bool allowCollapse)
{
    Q_Q(GLDSplitter);
    QWidget *w = sls->widget;
    QRect r;
    QRect contents = q->contentsRect();

    //TODO
    if (p < 0)
    {
        p = 0;
    }

    if (s > (q->orientation() == Qt::Horizontal ? q->rect().width() : q->rect().height()))
    {
        s = (q->orientation() == Qt::Horizontal ? q->rect().width() : q->rect().height());
    }

    if (orient == Qt::Horizontal)
    {
        r.setRect(p, contents.y(), s, contents.height());
    }
    else
    {
        r.setRect(contents.x(), p, contents.width(), s);
    }

    sls->rect = r;

    int minSize = pick(qSmartMinSize(w));

    if (orient == Qt::Horizontal && q->isRightToLeft())
    {
        r.moveRight(contents.width() - r.left());
    }

    if (allowCollapse)
    {
        sls->collapsed = s <= 0 && minSize > 0 && !w->isHidden();
    }

    //   Hide the child widget, but without calling hide() so that
    //   the splitter handle is still shown.
    if (sls->collapsed)
    {
        r.moveTopLeft(QPoint(-r.width() - 1, -r.height() - 1));
    }

    w->setGeometry(r);

    if (!sls->handle->isHidden())
    {
        GLDSplitterHandle *h = sls->handle;
        QSize hs = h->sizeHint();
        int left, top, right, bottom;
        h->getContentsMargins(&left, &top, &right, &bottom);

        if (orient == Qt::Horizontal)
        {
            if (q->isRightToLeft())
            {
                p = contents.width() - p + hs.width();
            }

            h->setGeometry(p - hs.width() - left, contents.y(), hs.width() + left + right, contents.height());
        }
        else
        {
            h->setGeometry(contents.x(), p - hs.height() - top, contents.width(), hs.height() + top + bottom);
        }
    }
}

void GLDSplitterPrivate::doMove(bool backwards, int hPos, int index, int delta, bool mayCollapse,
                                int *positions, int *widths)
{
    if (index < 0 || index >= list.count())
    {
        return;
    }

#ifdef GLDSPLITTER_DEBUG
    qDebug() << "GLDSplitterPrivate::doMove" << backwards << hPos << index << delta << mayCollapse;
#endif

    GLDSplitterLayoutStruct *s = list.at(index);
    QWidget *w = s->widget;

    int nextId = backwards ? index - delta : index + delta;

    if (w->isHidden())
    {
        doMove(backwards, hPos, nextId, delta, collapsible(nextId), positions, widths);
    }
    else
    {
        int hs = s->handle->isHidden() ? 0 : s->getHandleSize(orient);

        int  ws = backwards ? hPos - pick(s->rect.topLeft())
                  : pick(s->rect.bottomRight()) - hPos - hs + 1;

        if (ws > 0 || (!s->collapsed && !mayCollapse))
        {
            ws = qMin(ws, pick(w->maximumSize()));
            ws = qMax(ws, pick(qSmartMinSize(w)));
        }
        else
        {
            ws = 0;
        }

        positions[index] = backwards ? hPos - ws : hPos + hs;
        widths[index] = ws;
        doMove(backwards, backwards ? hPos - ws - hs : hPos + hs + ws, nextId, delta,
               collapsible(nextId), positions, widths);
    }

}

GLDSplitterLayoutStruct *GLDSplitterPrivate::findWidget(QWidget *w) const
{
    for (int i = 0; i < list.size(); ++i)
    {
        if (list.at(i)->widget == w)
        {
            return list.at(i);
        }
    }

    return 0;
}


/*!
    \internal
*/
void GLDSplitterPrivate::insertWidget_helper(int index, QWidget *widget, bool show)
{
    Q_Q(GLDSplitter);
    QBoolBlocker b(blockChildAdd);
    bool needShow = show && q->isVisible() &&
                    !(widget->isHidden() && widget->testAttribute(Qt::WA_WState_ExplicitShowHide));

    if (widget->parentWidget() != q)
    {
        widget->setParent(q);
    }

    if (needShow)
    {
        widget->show();
    }

    insertWidget(index, widget);
    recalc(q->isVisible());
}

/*
    Inserts the widget \a w at position \a index in the splitter's list of widgets.

    If \a w is already in the splitter, it will be moved to the new position.
*/

GLDSplitterLayoutStruct *GLDSplitterPrivate::insertWidget(int index, QWidget *w)
{
    Q_Q(GLDSplitter);
    GLDSplitterLayoutStruct *sls = 0;
    int i;
    int last = list.count();

    for (i = 0; i < list.size(); ++i)
    {
        GLDSplitterLayoutStruct *s = list.at(i);

        if (s->widget == w)
        {
            sls = s;
            --last;
            break;
        }
    }

    if (index < 0 || index > last)
    {
        index = last;
    }

    if (sls)
    {
        list.move(i, index);
    }
    else
    {
        GLDSplitterHandle *newHandle = 0;
        sls = new GLDSplitterLayoutStruct;
        QString tmp = QLatin1String("qt_splithandle_");
        tmp += w->objectName();
        newHandle = q->createHandle();
        newHandle->setObjectName(tmp);
        sls->handle = newHandle;
        sls->widget = w;
        w->lower();
        list.insert(index, sls);

        if (newHandle && q->isVisible())
        {
            newHandle->show();    // will trigger sending of post events
        }

    }

    return sls;
}

/*!
    \class GLDSplitter
    \brief The GLDSplitter class implements a splitter widget.

    \ingroup organizers
    \inmodule QtWidgets


    A splitter lets the user control the size of child widgets by dragging the
    boundary between them. Any number of widgets may be controlled by a
    single splitter. The typical use of a GLDSplitter is to create several
    widgets and add them using insertWidget() or addWidget().

    The following example will show a QListView, QTreeView, and
    QTextEdit side by side, with two splitter handles:

    \snippet splitter/splitter.cpp 0

    If a widget is already inside a GLDSplitter when insertWidget() or
    addWidget() is called, it will move to the new position. This can be used
    to reorder widgets in the splitter later. You can use indexOf(),
    widget(), and count() to get access to the widgets inside the splitter.

    A default GLDSplitter lays out its children horizontally (side by side); you
    can use setOrientation(Qt::Vertical) to lay its
    children out vertically.

    By default, all widgets can be as large or as small as the user
    wishes, between the \l minimumSizeHint() (or \l minimumSize())
    and \l maximumSize() of the widgets.

    GLDSplitter resizes its children dynamically by default. If you
    would rather have GLDSplitter resize the children only at the end of
    a resize operation, call setOpaqueResize(false).

    The initial distribution of size between the widgets is determined by
    multiplying the initial size with the stretch factor.
    You can also use setSizes() to set the sizes
    of all the widgets. The function sizes() returns the sizes set by the user.
    Alternatively, you can save and restore the sizes of the widgets from a
    QByteArray using saveState() and restoreState() respectively.

    When you hide() a child, its space will be distributed among the
    other children. It will be reinstated when you show() it again.

    \note Adding a QLayout to a GLDSplitter is not supported (either through
    setLayout() or making the GLDSplitter a parent of the QLayout); use addWidget()
    instead (see example above).

    \sa GLDSplitterHandle, QHBoxLayout, QVBoxLayout, QTabWidget
*/


/*!
    Constructs a horizontal splitter with the \a parent
    argument passed on to the QFrame constructor.

    \sa setOrientation()
*/
GLDSplitter::GLDSplitter(QWidget *parent)
    : QFrame(*new GLDSplitterPrivate, parent)
{
    Q_D(GLDSplitter);
    d->orient = Qt::Horizontal;
    d->init();
}


/*!
    Constructs a splitter with the given \a orientation and \a parent.

    \sa setOrientation()
*/
GLDSplitter::GLDSplitter(Qt::Orientation orientation, QWidget *parent)
    : QFrame(*new GLDSplitterPrivate, parent)
{
    Q_D(GLDSplitter);
    d->orient = orientation;
    d->init();
}


/*!
    Destroys the splitter. All children are deleted.
*/

GLDSplitter::~GLDSplitter()
{
    Q_D(GLDSplitter);
    delete d->rubberBand;

    while (!d->list.isEmpty())
    {
        delete d->list.takeFirst();
    }
}

/*!
    Updates the splitter's state. You should not need to call this
    function.
*/
void GLDSplitter::refresh()
{
    Q_D(GLDSplitter);
    d->recalc(true);
}

/*!
    \property GLDSplitter::orientation
    \brief the orientation of the splitter

    By default, the orientation is horizontal (i.e., the widgets are
    laid out side by side). The possible orientations are
    Qt::Horizontal and Qt::Vertical.

    \sa GLDSplitterHandle::orientation()
*/

void GLDSplitter::setOrientation(Qt::Orientation orientation)
{
    Q_D(GLDSplitter);

    if (d->orient == orientation)
    {
        return;
    }

    if (!testAttribute(Qt::WA_WState_OwnSizePolicy))
    {
        QSizePolicy sp = sizePolicy();
        sp.transpose();
        setSizePolicy(sp);
        setAttribute(Qt::WA_WState_OwnSizePolicy, false);
    }

    d->orient = orientation;

    for (int i = 0; i < d->list.size(); ++i)
    {
        GLDSplitterLayoutStruct *s = d->list.at(i);
        s->handle->setOrientation(orientation);
    }

    d->recalc(isVisible());
}

Qt::Orientation GLDSplitter::orientation() const
{
    Q_D(const GLDSplitter);
    return d->orient;
}

/*!
    \property GLDSplitter::childrenCollapsible
    \brief whether child widgets can be resized down to size 0 by the user

    By default, children are collapsible. It is possible to enable
    and disable the collapsing of individual children using
    setCollapsible().

    \sa setCollapsible()
*/

void GLDSplitter::setChildrenCollapsible(bool collapse)
{
    Q_D(GLDSplitter);
    d->childrenCollapsible = collapse;
}

bool GLDSplitter::childrenCollapsible() const
{
    Q_D(const GLDSplitter);
    return d->childrenCollapsible;
}

/*!
    Sets whether the child widget at \a index is collapsible to \a collapse.

    By default, children are collapsible, meaning that the user can
    resize them down to size 0, even if they have a non-zero
    minimumSize() or minimumSizeHint(). This behavior can be changed
    on a per-widget basis by calling this function, or globally for
    all the widgets in the splitter by setting the \l
    childrenCollapsible property.

    \sa childrenCollapsible
*/

void GLDSplitter::setCollapsible(int index, bool collapse)
{
    Q_D(GLDSplitter);

    if (index < 0 || index >= d->list.size())
    {
        qWarning("GLDSplitter::setCollapsible: Index %d out of range", index);
        return;
    }

    d->list.at(index)->collapsible = collapse ? 1 : 0;
}

/*!
    Returns true if the widget at \a index is collapsible, otherwise returns false.
*/
bool GLDSplitter::isCollapsible(int index) const
{
    Q_D(const GLDSplitter);

    if (index < 0 || index >= d->list.size())
    {
        qWarning("GLDSplitter::isCollapsible: Index %d out of range", index);
        return false;
    }

    return d->list.at(index)->collapsible;
}

/*!
    \reimp
*/
void GLDSplitter::resizeEvent(QResizeEvent *)
{
    Q_D(GLDSplitter);
    d->doResize();
}

/*!
    Adds the given \a widget to the splitter's layout after all the other
    items.

    If \a widget is already in the splitter, it will be moved to the new position.

    \sa insertWidget(), widget(), indexOf()
*/
void GLDSplitter::addWidget(QWidget *widget)
{
    Q_D(GLDSplitter);
    insertWidget(d->list.count(), widget);
}

/*!
    Inserts the \a widget specified into the splitter's layout at the
    given \a index.

    If \a widget is already in the splitter, it will be moved to the new position.

    if \a index is an invalid index, then the widget will be inserted at the end.

    \sa addWidget(), indexOf(), widget()
*/
void GLDSplitter::insertWidget(int index, QWidget *widget)
{
    Q_D(GLDSplitter);
    d->insertWidget_helper(index, widget, true);
}

/*!
    \fn int GLDSplitter::indexOf(QWidget *widget) const

    Returns the index in the splitter's layout of the specified \a widget. This
    also works for handles.

    Handles are numbered from 0. There are as many handles as there
    are child widgets, but the handle at position 0 is always hidden.


    \sa count(), widget()
*/
int GLDSplitter::indexOf(QWidget *w) const
{
    Q_D(const GLDSplitter);

    for (int i = 0; i < d->list.size(); ++i)
    {
        GLDSplitterLayoutStruct *s = d->list.at(i);

        if (s->widget == w || s->handle == w)
        {
            return i;
        }
    }

    return -1;
}

/*!
    Returns the widget at the given \a index in the splitter's layout.

    \sa count(), handle(), indexOf(), insertWidget()
*/
QWidget *GLDSplitter::widget(int index) const
{
    Q_D(const GLDSplitter);

    if (index < 0 || index >= d->list.size())
    {
        return 0;
    }

    return d->list.at(index)->widget;
}

/*!
    Returns the number of widgets contained in the splitter's layout.

    \sa widget(), handle()
*/
int GLDSplitter::count() const
{
    Q_D(const GLDSplitter);
    return d->list.count();
}

/*!
    \reimp

    Tells the splitter that the child widget described by \a c has been
    inserted or removed.

    This method is also used to handle the situation where a widget is created
    with the splitter as a parent but not explicitly added with insertWidget()
    or addWidget(). This is for compatibility and not the recommended way of
    putting widgets into a splitter in new code. Please use insertWidget() or
    addWidget() in new code.

    \sa addWidget(), insertWidget()
*/

void GLDSplitter::childEvent(QChildEvent *c)
{
    Q_D(GLDSplitter);

    if (!c->child()->isWidgetType())
    {
        if (c->type() == QEvent::ChildAdded && dynamic_cast<QLayout *>(c->child()))
        {
            qWarning("Adding a QLayout to a GLDSplitter is not supported.");
        }

        return;
    }

    QWidget *w = static_cast<QWidget *>(c->child());

    if (c->added() && !d->blockChildAdd && !w->isWindow() && !d->findWidget(w))
    {
        d->insertWidget_helper(d->list.count(), w, false);
    }
    else if (c->polished() && !d->blockChildAdd)
    {
        if (isVisible() && !(w->isHidden() && w->testAttribute(Qt::WA_WState_ExplicitShowHide)))
        {
            w->show();
        }
    }
    else if (c->type() == QEvent::ChildRemoved)
    {
        for (int i = 0; i < d->list.size(); ++i)
        {
            GLDSplitterLayoutStruct *s = d->list.at(i);

            if (s->widget == w)
            {
                d->list.removeAt(i);
                delete s;
                d->recalc(isVisible());
                return;
            }
        }
    }
}


/*!
    Displays a rubber band at position \a pos. If \a pos is negative, the
    rubber band is removed.
*/

void GLDSplitter::setRubberBand(int pos)
{
    Q_D(GLDSplitter);

    if (pos < 0)
    {
        if (d->rubberBand)
        {
            d->rubberBand->deleteLater();
        }

        return;
    }

    QRect r = contentsRect();
    const int rBord = 3; // customizable?
    int hw = handleWidth();

    if (!d->rubberBand)
    {
        QBoolBlocker b(d->blockChildAdd);
        d->rubberBand = new QRubberBand(QRubberBand::Line, this);
        // For accessibility to identify this special widget.
        d->rubberBand->setObjectName(QLatin1String("qt_rubberband"));
    }

    const QRect newGeom = d->orient == Qt::Horizontal ? QRect(QPoint(pos + hw / 2 - rBord, r.y()), QSize(2 * rBord, r.height()))
                          : QRect(QPoint(r.x(), pos + hw / 2 - rBord), QSize(r.width(), 2 * rBord));
    d->rubberBand->setGeometry(newGeom);
    d->rubberBand->show();
}

/*!
    \reimp
*/

bool GLDSplitter::event(QEvent *e)
{
    Q_D(GLDSplitter);

    switch (e->type())
    {
        case QEvent::Hide:

            // Reset firstShow to false here since things can be done to the splitter in between
            if (!d->firstShow)
            {
                d->firstShow = true;
            }

            break;

        case QEvent::Show:
            if (!d->firstShow)
            {
                break;
            }

            d->firstShow = false;

            // fall through
        case QEvent::HideToParent:
        case QEvent::ShowToParent:
        case QEvent::LayoutRequest:
            d->recalc(isVisible());
            break;

        default:
            ;
    }

    return QWidget::event(e);
}

/*!
    \fn GLDSplitter::splitterMoved(int pos, int index)

    This signal is emitted when the splitter handle at a particular \a
    index has been moved to position \a pos.

    For right-to-left languages such as Arabic and Hebrew, the layout
    of horizontal splitters is reversed. \a pos is then the
    distance from the right edge of the widget.

    \sa moveSplitter()
*/

/*!
    Moves the left or top edge of the splitter handle at \a index as
    close as possible to position \a pos, which is the distance from the
    left or top edge of the widget.

    For right-to-left languages such as Arabic and Hebrew, the layout
    of horizontal splitters is reversed. \a pos is then the distance
    from the right edge of the widget.

    \sa splitterMoved(), closestLegalPosition(), getRange()
*/
void GLDSplitter::moveSplitter(int pos, int index)
{
    Q_D(GLDSplitter);
    GLDSplitterLayoutStruct *s = d->list.at(index);
    int farMin;
    int min;
    int max;
    int farMax;
    int adjustedPos;

#ifdef GLDSPLITTER_DEBUG
    int debugp = pos;
#endif

    pos = adjustedPos = d->adjustPos(pos, index, &farMin, &min, &max, &farMax);

#ifdef GLDSPLITTER_DEBUG
    qDebug() << "GLDSplitter::moveSplitter"
             << "<farMin =" << farMin << ">"
             << "<farMax =" << farMax << ">"
             << "<min =" << min << ">"
             << "<max =" << max << ">";
#endif

    int oldP = d->pick(s->rect.topLeft());

#ifdef GLDSPLITTER_DEBUG
    qDebug() << "GLDSplitter::moveSplitter" << debugp << index << "adjusted" << pos << "oldP" << oldP;
#endif

    QVarLengthArray<int, 32> poss(d->list.count());
    QVarLengthArray<int, 32> ws(d->list.count());
    bool upLeft;

    if(d->collapsedLength != -1)
    {
        if(d->collapsedLength > adjustedPos)
        {
            adjustedPos = farMin;
        }

        if(d->collapsedLength > farMax - adjustedPos)
        {
            adjustedPos = farMax;
        }
    }

    d->doMove(false, adjustedPos, index, +1, (d->collapsible(s) && (adjustedPos > max)), poss.data(), ws.data());
    d->doMove(true, adjustedPos, index - 1, +1, (d->collapsible(index - 1) && (adjustedPos < min)), poss.data(), ws.data());
    upLeft = (adjustedPos < oldP);

    int wid, delta, count = d->list.count();

    if (upLeft)
    {
        wid = 0;
        delta = 1;
    }
    else
    {
        wid = count - 1;
        delta = -1;
    }

#ifdef GLDSPLITTER_DEBUG
    qDebug() << "GLDSplitter::moveSplitter" << pos << index << endl;
#endif

    for (; wid >= 0 && wid < count; wid += delta)
    {
        GLDSplitterLayoutStruct *sls = d->list.at(wid);

        if (!sls->widget->isHidden())
        {
            d->setGeo(sls, poss[wid], ws[wid], true);
#ifdef GLDSPLITTER_DEBUG
            qDebug() << "GLDSplitter::moveSplitter...setGeo" << (int)poss[wid] << (int)ws[wid];
#endif
        }
    }

    d->storeSizes();

    emit splitterMoved(pos, index);
}


/*!
    Returns the valid range of the splitter at \a index in
    *\a{min} and *\a{max} if \a min and \a max are not 0.
*/

void GLDSplitter::getRange(int index, int *min, int *max) const
{
    Q_D(const GLDSplitter);
    d->getRange(index, min, 0, 0, max);
}


/*!
    Returns the closest legal position to \a pos of the widget at \a index.

    For right-to-left languages such as Arabic and Hebrew, the layout
    of horizontal splitters is reversed. Positions are then measured
    from the right edge of the widget.

    \sa getRange()
*/

int GLDSplitter::closestLegalPosition(int pos, int index)
{
    Q_D(GLDSplitter);
    int x, i, n, u;
    return d->adjustPos(pos, index, &u, &n, &i, &x);
}

/*!
    \property GLDSplitter::opaqueResize
    \brief whether resizing is opaque

    Opaque resizing is on by default.
*/

bool GLDSplitter::opaqueResize() const
{
    Q_D(const GLDSplitter);
    return d->opaque;
}


void GLDSplitter::setOpaqueResize(bool on)
{
    Q_D(GLDSplitter);
    d->opaque = on;
}


/*!
    \reimp
*/
QSize GLDSplitter::sizeHint() const
{
    Q_D(const GLDSplitter);
    ensurePolished();
    int l = 0;
    int t = 0;

    for (int i = 0; i < d->list.size(); ++i)
    {
        QWidget *w = d->list.at(i)->widget;

        if (w->isHidden())
        {
            continue;
        }

        QSize s = w->sizeHint();

        if (s.isValid())
        {
            l += d->pick(s);
            t = qMax(t, d->trans(s));
        }
    }

    return orientation() == Qt::Horizontal ? QSize(l, t) : QSize(t, l);
}


/*!
    \reimp
*/

QSize GLDSplitter::minimumSizeHint() const
{
    Q_D(const GLDSplitter);
    ensurePolished();
    int l = 0;
    int t = 0;

    for (int i = 0; i < d->list.size(); ++i)
    {
        GLDSplitterLayoutStruct *s = d->list.at(i);

        if (!s || !s->widget)
        {
            continue;
        }

        if (s->widget->isHidden())
        {
            continue;
        }

        QSize widgetSize = qSmartMinSize(s->widget);

        if (widgetSize.isValid())
        {
            l += d->pick(widgetSize);
            t = qMax(t, d->trans(widgetSize));
        }

        if (!s->handle || s->handle->isHidden())
        {
            continue;
        }

        QSize splitterSize = s->handle->sizeHint();

        if (splitterSize.isValid())
        {
            l += d->pick(splitterSize);
            t = qMax(t, d->trans(splitterSize));
        }
    }

    return orientation() == Qt::Horizontal ? QSize(l, t) : QSize(t, l);
}


/*!
    Returns a list of the size parameters of all the widgets in this splitter.

    If the splitter's orientation is horizontal, the list contains the
    widgets width in pixels, from left to right; if the orientation is
    vertical, the list contains the widgets' heights in pixels,
    from top to bottom.

    Giving the values to another splitter's setSizes() function will
    produce a splitter with the same layout as this one.

    Note that invisible widgets have a size of 0.

    \sa setSizes()
*/

QList<int> GLDSplitter::sizes() const
{
    Q_D(const GLDSplitter);
    ensurePolished();

    QList<int> list;

    for (int i = 0; i < d->list.size(); ++i)
    {
        GLDSplitterLayoutStruct *s = d->list.at(i);
        list.append(d->pick(s->rect.size()));
    }

    return list;
}

/*!
    Sets the child widgets' respective sizes to the values given in the \a list.

    If the splitter is horizontal, the values set the width of each
    widget in pixels, from left to right. If the splitter is vertical, the
    height of each widget is set, from top to bottom.

    Extra values in the \a list are ignored. If \a list contains too few
    values, the result is undefined, but the program will still be well-behaved.

    The overall size of the splitter widget is not affected.
    Instead, any additional/missing space is distributed amongst the
    widgets according to the relative weight of the sizes.

    If you specify a size of 0, the widget will be invisible. The size policies
    of the widgets are preserved. That is, a value smaller than the minimal size
    hint of the respective widget will be replaced by the value of the hint.

    \sa sizes()
*/

void GLDSplitter::setSizes(const QList<int> &list)
{
    Q_D(GLDSplitter);
    d->setSizes_helper(list, true);
}

/*!
    \property GLDSplitter::handleWidth
    \brief the width of the splitter handles

    By default, this property contains a value that depends on the user's platform
    and style preferences.

    If you set handleWidth to 1 or 0, the actual grab area will grow to overlap a
    few pixels of its respective widgets.
*/

int GLDSplitter::handleWidth() const
{
    Q_D(const GLDSplitter);

    if (d->handleWidth >= 0)
    {
        return d->handleWidth;
    }
    else
    {
        return style()->pixelMetric(QStyle::PM_SplitterWidth, 0, this);
    }
}

void GLDSplitter::setHandleWidth(int width)
{
    Q_D(GLDSplitter);

    d->handleWidth = width;
    d->updateHandles();
}

int GLDSplitter::collapsedLength() const
{
    Q_D(const GLDSplitter);

    if (d->collapsedLength >= 0)
    {
        return d->collapsedLength;
    }
    else
    {
        return 0;
    }
}

void GLDSplitter::setCollapsedLength(int length)
{
    Q_D(GLDSplitter);

    d->collapsedLength = length;
}

/*!
    \reimp
*/
void GLDSplitter::changeEvent(QEvent *ev)
{
    Q_D(GLDSplitter);

    if (ev->type() == QEvent::StyleChange)
    {
        d->updateHandles();
    }

    QFrame::changeEvent(ev);
}

static const qint32 SplitterMagic = 0xff;

/*!
    Saves the state of the splitter's layout.

    Typically this is used in conjunction with QSettings to remember the size
    for a future session. A version number is stored as part of the data.
    Here is an example:

    \snippet splitter/splitter.cpp 1

    \sa restoreState()
*/
QByteArray GLDSplitter::saveState() const
{
    Q_D(const GLDSplitter);
    int version = 0;
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << qint32(SplitterMagic);
    stream << qint32(version);
    QList<int> list;

    for (int i = 0; i < d->list.size(); ++i)
    {
        GLDSplitterLayoutStruct *s = d->list.at(i);
        list.append(s->sizer);
    }

    stream << list;
    stream << childrenCollapsible();
    stream << qint32(handleWidth());
    stream << opaqueResize();
    stream << qint32(orientation());
    return data;
}

/*!
    Restores the splitter's layout to the \a state specified.
    Returns true if the state is restored; otherwise returns false.

    Typically this is used in conjunction with QSettings to restore the size
    from a past session. Here is an example:

    Restore the splitter's state:

    \snippet splitter/splitter.cpp 2

    A failure to restore the splitter's layout may result from either
    invalid or out-of-date data in the supplied byte array.

    \sa saveState()
*/
bool GLDSplitter::restoreState(const QByteArray &state)
{
    Q_D(GLDSplitter);
    int version = 0;
    QByteArray sd = state;
    QDataStream stream(&sd, QIODevice::ReadOnly);
    QList<int> list;
    bool b;
    qint32 i;
    qint32 marker;
    qint32 v;

    stream >> marker;
    stream >> v;

    if (marker != SplitterMagic || v != version)
    {
        return false;
    }

    stream >> list;
    d->setSizes_helper(list, false);

    stream >> b;
    setChildrenCollapsible(b);

    stream >> i;
    setHandleWidth(i);

    stream >> b;
    setOpaqueResize(b);

    stream >> i;
    setOrientation(Qt::Orientation(i));
    d->doResize();

    return true;
}

/*!
    Updates the size policy of the widget at position \a index to
    have a stretch factor of \a stretch.

    \a stretch is not the effective stretch factor; the effective
    stretch factor is calculated by taking the initial size of the
    widget and multiplying it with \a stretch.

    This function is provided for convenience. It is equivalent to

    \snippet code/src_gui_widgets_qsplitter.cpp 0

    \sa setSizes(), widget()
*/
void GLDSplitter::setStretchFactor(int index, int stretch)
{
    Q_D(GLDSplitter);

    if (index <= -1 || index >= d->list.count())
    {
        return;
    }

    QWidget *widget = d->list.at(index)->widget;
    QSizePolicy sp = widget->sizePolicy();
    sp.setHorizontalStretch(stretch);
    sp.setVerticalStretch(stretch);
    widget->setSizePolicy(sp);
}

/*!
    \relates GLDSplitter
    \obsolete

    Use \a ts << \a{splitter}.saveState() instead.
*/

QTextStream &operator<<(QTextStream &ts, const GLDSplitter &splitter)
{
    ts << splitter.saveState() << endl;
    return ts;
}

/*!
    \relates GLDSplitter
    \obsolete

    Use \a ts >> \a{splitter}.restoreState() instead.
*/

QTextStream &operator>>(QTextStream &ts, GLDSplitter &splitter)
{
    QString line = ts.readLine();
    line = line.simplified();
    line.replace(QLatin1Char(' '), QString());
    line = line.toUpper();

    splitter.restoreState(line.toLatin1());
    return ts;
}

//G_GLODON_END_NAMESPACE

#endif // QT_NO_SPLITTER
