#ifndef GLDDockFRAME_H
#define GLDDockFRAME_H

#include <QtWidgets>
#include "GLDDockCommon.h"
#include <map>

class GLDDockPanel;
class GLDDockNode;
class GLDDockMaskWidget;
class GLDDockManager;
class GLDDockArrows;

class GLDDockFrame : public QWidget
{
    Q_OBJECT

public:
    explicit GLDDockFrame(GLDDockManager *manager, QWidget *parent);
    virtual ~GLDDockFrame();
    void showArrow();

    void relayout();
protected:
    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void dragMoveEvent(QDragMoveEvent *);
    virtual void dragLeaveEvent(QDragLeaveEvent *);
    virtual void dropEvent(QDropEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    GLDDockArrows *m_pArrows;
    GLDDockNode *rootNode_;
    GLDDockMaskWidget *maskWidget_;
    std::map<int, GLDDockPanel *> dockPanels_;
    GLDDockManager *manager_;

    DockArea lastMaskArea_;
private:
    void onDragEnterPanel();
    void onDragLeavePanel();
    void onEndDragAtPanel();
    friend class GLDDockManager;
};

#endif // GLDDockFRAME_H
