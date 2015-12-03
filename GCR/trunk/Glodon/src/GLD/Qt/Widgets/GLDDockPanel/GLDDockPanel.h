#ifndef GLDDockPANEL_H
#define GLDDockPANEL_H

#include <QWidget>
#include "GLDDockCommon.h"
#include "GLDDockPanelComponents.h"
#include "GLDDockArrows.h"

class GLDDockManager;
class GLDDockMaskWidget;
class GLDDockTabWidget;

class GLDDockPanel : public QWidget
{
    Q_OBJECT
public:
    explicit GLDDockPanel(GLDDockManager *manager, QWidget *frame);

    QWidget *getContensWidget()
    {
        return contensWidget_;
    }
    QWidget *resetContensWidget(QWidget *newWidget)
    {
        QWidget *old = contensWidget_;
        contensWidget_ = newWidget;
        resetContensWidgetPosAndSize();

        return old;
    }

    void setId(int id)
    {
        id_ = id;
    }
    int id()
    {
        return id_;
    }

    bool isDocked()
    {
        return isDocked_;
    }
    bool dockTo(QWidget *, DockArea);
    void undock();
    void startDrag();
private:
    void resizeWidget(int curX, int curY);
    void relayout();
    void setDockStatus();
    void setFloatStatus();
    void setTabbedStatus(bool isTabbed, GLDDockPanel *parentTabPanel);

private:
    int id_;    //panel ID
    int titleRectHeight_;    // panel窗口的标题栏的高度.
    QWidget *contensWidget_; // panel窗口内的widget.
    int edgeWidth_;         // 边框的宽度.
    bool isDocked_;
    QSize floatSize_;
    GLDDockManager *manager_;
    GLDDockArrows arrows_;
    DockArea lastMaskArea_;
    GLDDockMaskWidget *maskWidget_;

    bool isTabbed_;
    GLDDockPanel *parentTabPanel_;

    enum PanelType
    {
        DockPanel,
        TabContainer,
        SplitContainer,
    };

    PanelType panelType_;

    GLDDockPanelTitle *title_;
    GLDDockPanelEdgeLeft *leftEdge_;
    GLDDockPanelEdgeTop *topEdge_;
    GLDDockPanelEdgeRight *rightEdge_;
    GLDDockPanelEdgeBottom *bottomEdge_;
    GLDDockPanelEdgeLeftTop *leftTopEdge_;
    GLDDockPanelEdgeRightTop *rightTopEdge_;
    GLDDockPanelEdgeRightBottom *rightBottomEdge_;
    GLDDockPanelEdgeLeftBottom *leftBottomEdge_;

private:
    void showArrow();
protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *e);
    void resetContensWidgetPosAndSize();

    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void dragMoveEvent(QDragMoveEvent *);
    virtual void dragLeaveEvent(QDragLeaveEvent *);
    virtual void dropEvent(QDropEvent *);

    friend class GLDDockManager;
    friend class GLDDockPanelTitle;
    friend class GLDDockTabWidget;
};

#endif // GLDDockPANEL_H
