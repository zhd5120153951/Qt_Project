#ifndef GLDDockMASKWIDGET_H
#define GLDDockMASKWIDGET_H

#include <QWidget>
#include "GLDDockCommon.h"

class GLDDockMaskWidget : public QWidget
{
    Q_OBJECT

public:
    GLDDockMaskWidget(QWidget *parent);
    ~GLDDockMaskWidget();

    void showOnDockArea(DockArea area);
private:

};

#endif // GLDDockMASKWIDGET_H
