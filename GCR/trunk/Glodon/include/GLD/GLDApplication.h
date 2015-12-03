#ifndef GLDAPPLICATION_H
#define GLDAPPLICATION_H

#include <QApplication>
#include <QSet>
#include "GLDWidget_Global.h"

class GLDWIDGETSHARED_EXPORT GLDApplication : public QApplication
{
    Q_OBJECT
public:
    GLDApplication(int &argc, char **argv, int _internal = ApplicationFlags);
    ~GLDApplication();
    bool notify(QObject *, QEvent *);

signals:
    //[NOTE] time-cost things should NOT be done in this function,
    //otherwise, it will block main event loop
    void idle();

    //static void initEventDispatcher();
protected:

    void onIdle();
    void timerEvent( QTimerEvent *event );
    //child can re-assign other types to this variable to filter out
    QSet<int> m_nonBusyEventTypes;
    QSet<int> m_busyEventTypes;
    bool useFilterInBusy;

private:
    int m_idleTimerID;

};

#endif // GLDAPPLICATION_H
