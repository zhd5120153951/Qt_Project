#ifndef GLDDockTABWIDGET_H
#define GLDDockTABWIDGET_H

#include <QTabWidget>
#include "GLDWidget_Global.h"

class GLDWIDGETSHARED_EXPORT GLDDockTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    GLDDockTabWidget(QWidget *parent);
    ~GLDDockTabWidget();

signals:
    void setBasePanelTitle(const QString &);
private slots:
    void onDragTabOut(int index);
    void onCurrenChanged(int index);
protected:
    virtual void tabRemoved(int);
};

#endif // GLDDockTABWIDGET_H
