#ifndef GLDSPLITTERHANDLE_H
#define GLDSPLITTERHANDLE_H

#include <QWidget>

class GLDSplitter;
class GLDSplitterHandlePrivate;
#include "GLDWidget_Global.h"

class GLDWIDGETSHARED_EXPORT GLDSplitterHandle : public QWidget
{
    Q_OBJECT
public:
    explicit GLDSplitterHandle(Qt::Orientation o, GLDSplitter *parent);
    ~GLDSplitterHandle();

    void setOrientation(Qt::Orientation o);
    Qt::Orientation orientation() const;

    bool opaqueResize() const;
    GLDSplitter *splitter() const;

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);
    void moveSplitter(int p);

    bool event(QEvent *);

    int closestLegalPosition(int p);

private:
    Q_DISABLE_COPY(GLDSplitterHandle)
    Q_DECLARE_PRIVATE(GLDSplitterHandle)
};

#endif // GLDSPLITTERHANDLE_H
