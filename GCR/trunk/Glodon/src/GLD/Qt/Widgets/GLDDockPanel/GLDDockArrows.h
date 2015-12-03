#ifndef GLDDockARROWS_H
#define GLDDockARROWS_H

#include <QObject>
#include <QWidget>
#include <QPixmap>

#include "GLDDockCommon.h"

class QArrowWidget;

class GLDDockArrows : public QObject
{
    Q_OBJECT
public:
    explicit GLDDockArrows(QWidget *parent = 0);

    void show(int dockArea);

    DockArea getDockAreaByPos(QPoint pos);
private:
    QArrowWidget *leftWid;
    QArrowWidget *topWid;
    QArrowWidget *rightWid;
    QArrowWidget *bottomWid;
    QArrowWidget *centerWid;
    QArrowWidget *centerLeftWid;
    QArrowWidget *centerRightWid;
    QArrowWidget *centerTopWid;
    QArrowWidget *centerBottomWid;

    QWidget *parentWid;
public slots:

};

#endif // GLDDockARROWS_H
