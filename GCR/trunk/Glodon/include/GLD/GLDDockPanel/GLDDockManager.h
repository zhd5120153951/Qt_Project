#ifndef GLDDockMANAGER_H
#define GLDDockMANAGER_H

#include <QWidget>
#include <map>
#include "GLDDockCommon.h"
#include "GLDDockFrame.h"
#include "GLDWidget_Global.h"

class GLDDockNode;

class GLDWIDGETSHARED_EXPORT GLDDockManager : public QObject
{
    friend class GLDDockPanel;
    friend class GLDDockFrame;

    Q_OBJECT

public:
    GLDDockManager(QWidget *parent);
    ~GLDDockManager();

    GLDDockFrame *getDockFrame()
    {
        return dockFrmae_;
    }
    GLDDockPanel *addPanel(int id, const QString &title, QPoint pos, QSize size, QWidget *contensWidget = NULL);
    GLDDockPanel *getDockPanelByID(int id);

public slots:
    bool dockPanelToFrame(GLDDockPanel *panel, DockArea area);
    bool dockPanelToPanel(GLDDockPanel *from, GLDDockPanel *target, DockArea area);

public:
    bool isRootNode(GLDDockNode *node);

    void undockPanel(GLDDockPanel *panel);

private:
    void onDragEnterPanel();
    void onDragLeavePanel();
    void onEndDragAtPanel();
    bool dockPanelToFloatPanel(GLDDockPanel *from, GLDDockPanel *target, DockArea area);
    bool dockPanelToDockedPanel(GLDDockPanel *from, GLDDockPanel *target, DockArea area);
private:
    GLDDockFrame *dockFrmae_;
    std::map<int, GLDDockPanel *> dockPanels_;
};

#endif // GLDDockMANAGER_H
