#include <QStyleOption>

#include "GLDSplitter.h"
#include "GLDSplitter_p.h"
#include "GLDSplitterHandle.h"

GLDSplitterHandle::GLDSplitterHandle(Qt::Orientation orientation, GLDSplitter *parent)
    : QWidget(*new GLDSplitterHandlePrivate, parent, 0)
{
    Q_D(GLDSplitterHandle);
    d->s = parent;
    setOrientation(orientation);
}

/*!
    Destructor.
*/
GLDSplitterHandle::~GLDSplitterHandle()
{
}

/*!
    Sets the orientation of the splitter handle to \a orientation.
    This is usually propagated from the GLDSplitter.

    \sa GLDSplitter::setOrientation()
*/
void GLDSplitterHandle::setOrientation(Qt::Orientation orientation)
{
    Q_D(GLDSplitterHandle);
    d->orient = orientation;
#ifndef QT_NO_CURSOR
    setCursor(orientation == Qt::Horizontal ? Qt::SplitHCursor : Qt::SplitVCursor);
#endif
}

/*!
   Returns the handle's orientation. This is usually propagated from the GLDSplitter.

   \sa GLDSplitter::orientation()
*/
Qt::Orientation GLDSplitterHandle::orientation() const
{
    Q_D(const GLDSplitterHandle);
    return d->orient;
}


/*!
    Returns true if widgets are resized dynamically (opaquely), otherwise
    returns false. This value is controlled by the GLDSplitter.

    \sa GLDSplitter::opaqueResize()

*/
bool GLDSplitterHandle::opaqueResize() const
{
    Q_D(const GLDSplitterHandle);
    return d->s->opaqueResize();
}


/*!
    Returns the splitter associated with this splitter handle.

    \sa GLDSplitter::handle()
*/
GLDSplitter *GLDSplitterHandle::splitter() const
{
    return d_func()->s;
}

/*!
    Tells the splitter to move this handle to position \a pos, which is
    the distance from the left or top edge of the widget.

    Note that \a pos is also measured from the left (or top) for
    right-to-left languages. This function will map \a pos to the
    appropriate position before calling GLDSplitter::moveSplitter().

    \sa GLDSplitter::moveSplitter(), closestLegalPosition()
*/
void GLDSplitterHandle::moveSplitter(int pos)
{
    Q_D(GLDSplitterHandle);
    if (d->s->isRightToLeft() && d->orient == Qt::Horizontal)
        pos = d->s->contentsRect().width() - pos;
    d->s->moveSplitter(pos, d->s->indexOf(this));
}

/*!
   Returns the closest legal position to \a pos of the splitter
   handle. The positions are measured from the left or top edge of
   the splitter, even for right-to-left languages.

   \sa GLDSplitter::closestLegalPosition(), moveSplitter()
*/

int GLDSplitterHandle::closestLegalPosition(int pos)
{
    Q_D(GLDSplitterHandle);
    GLDSplitter *s = d->s;
    if (s->isRightToLeft() && d->orient == Qt::Horizontal) {
        int w = s->contentsRect().width();
        return w - s->closestLegalPosition(w - pos, s->indexOf(this));
    }
    return s->closestLegalPosition(pos, s->indexOf(this));
}

/*!
    \reimp
*/
QSize GLDSplitterHandle::sizeHint() const
{
    Q_D(const GLDSplitterHandle);
    int hw = d->s->handleWidth();
    QStyleOption opt(0);
    opt.init(d->s);
    opt.state = QStyle::State_None;
    return parentWidget()->style()->sizeFromContents(QStyle::CT_Splitter, &opt, QSize(hw, hw), d->s)
        .expandedTo(QApplication::globalStrut());
}

/*!
    \reimp
*/
void GLDSplitterHandle::resizeEvent(QResizeEvent *event)
{
    Q_D(const GLDSplitterHandle);

    // When splitters are only 1 or 0 pixel large we increase the
    // actual grab area to five pixels

    // Note that GLDSplitter uses contentsRect for layouting
    // and ensures that handles are drawn on top of widgets
    // We simply use the contents margins for draggin and only
    // paint the mask area
    bool useTinyMode = (d->s->handleWidth() <= 1);
    setAttribute(Qt::WA_MouseNoMask, useTinyMode);
    if (useTinyMode) {
        if (orientation() == Qt::Horizontal)
            setContentsMargins(2, 0, 2, 0);
        else
            setContentsMargins(0, 2, 0, 2);
        setMask(QRegion(contentsRect()));
    }

    QWidget::resizeEvent(event);
}

/*!
    \reimp
*/
bool GLDSplitterHandle::event(QEvent *event)
{
    Q_D(GLDSplitterHandle);
    switch(event->type()) {
    case QEvent::HoverEnter:
        d->hover = true;
        update();
        break;
    case QEvent::HoverLeave:
        d->hover = false;
        update();
        break;
    default:
        break;
    }
    return QWidget::event(event);
}

/*!
    \reimp
*/
void GLDSplitterHandle::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(GLDSplitterHandle);
    if (!(e->buttons() & Qt::LeftButton))
        return;
    int pos = d->pick(parentWidget()->mapFromGlobal(e->globalPos()))
                 - d->mouseOffset;
    if (opaqueResize()) {
        moveSplitter(pos);
    } else {
        d->s->setRubberBand(closestLegalPosition(pos));
    }
}

/*!
   \reimp
*/
void GLDSplitterHandle::mousePressEvent(QMouseEvent *e)
{
    Q_D(GLDSplitterHandle);
    if (e->button() == Qt::LeftButton) {
        d->mouseOffset = d->pick(e->pos());
        d->pressed = true;
        update();
    }
}

/*!
   \reimp
*/
void GLDSplitterHandle::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(GLDSplitterHandle);
    if (!opaqueResize() && e->button() == Qt::LeftButton) {
        int pos = d->pick(parentWidget()->mapFromGlobal(e->globalPos()))
                     - d->mouseOffset;
        d->s->setRubberBand(-1);
        moveSplitter(pos);
    }
    if (e->button() == Qt::LeftButton) {
        d->pressed = false;
        update();
    }
}

/*!
   \reimp
*/
void GLDSplitterHandle::paintEvent(QPaintEvent *)
{
    Q_D(GLDSplitterHandle);
    QPainter p(this);
    QStyleOption opt(0);
    opt.rect = contentsRect();
    opt.palette = palette();
    if (orientation() == Qt::Horizontal)
        opt.state = QStyle::State_Horizontal;
    else
        opt.state = QStyle::State_None;
    if (d->hover)
        opt.state |= QStyle::State_MouseOver;
    if (d->pressed)
        opt.state |= QStyle::State_Sunken;
    if (isEnabled())
        opt.state |= QStyle::State_Enabled;
    parentWidget()->style()->drawControl(QStyle::CE_Splitter, &opt, &p, d->s);
}

/*!
    Returns a new splitter handle as a child widget of this splitter.
    This function can be reimplemented in subclasses to provide support
    for custom handles.

    \sa handle(), indexOf()
*/
GLDSplitterHandle *GLDSplitter::createHandle()
{
    Q_D(GLDSplitter);
    return new GLDSplitterHandle(d->orient, this);
}

/*!
    Returns the handle to the left (or above) for the item in the
    splitter's layout at the given \a index. The handle at index 0 is
    always hidden.

    For right-to-left languages such as Arabic and Hebrew, the layout
    of horizontal splitters is reversed. The handle will be to the
    right of the widget at \a index.

    \sa count(), widget(), indexOf(), createHandle(), setHandleWidth()
*/
GLDSplitterHandle *GLDSplitter::handle(int index) const
{
    Q_D(const GLDSplitter);
    if (index < 0 || index >= d->list.size())
        return 0;
    return d->list.at(index)->handle;
}

