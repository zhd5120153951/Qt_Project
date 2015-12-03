#include "GLDApplication.h"

#include "GLDException.h"
#include "GLDUIUtils.h"

//#define DEBUG
#ifdef DEBUG
#include <QDebug>
#endif

/*
#include <private/qeventdispatcher_win_p.h>

class GLDEventDispatcherWin32 : public QEventDispatcherWin32
{
public:
    explicit GLDEventDispatcherWin32(QObject *parent = 0);
    bool processEvents(QEventLoop::ProcessEventsFlags flags);
};

GLDEventDispatcherWin32::GLDEventDispatcherWin32(QObject *parent) : QEventDispatcherWin32(parent)
{
}

bool GLDEventDispatcherWin32::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    try
    {
        return QEventDispatcherWin32::processEvents(flags);
    }
    catch (GLDException &e)
    {
        showPrompt(e.message());
        return false;
    }
}
*/

/* GLDApplication */

GLDApplication::GLDApplication(int &argc,
                               char **argv,
                               int _internal)
    : QApplication(argc, argv, _internal)
    , useFilterInBusy(true)
    , m_idleTimerID(0)
{
    // 初始化资源文件
    Q_INIT_RESOURCE(GLDIcons);
    Q_INIT_RESOURCE(GLDQsses);
    Q_INIT_RESOURCE(GLDInis);

    m_idleTimerID = this->startTimer(0);
    m_nonBusyEventTypes.insert(QEvent::ZeroTimerEvent);
    //if there is some bug, pls try to remove those code lines below
    m_nonBusyEventTypes.insert(QEvent::DynamicPropertyChange);
    //Leave
    m_nonBusyEventTypes.insert(QEvent::Enter);//10
    m_nonBusyEventTypes.insert(QEvent::Leave);//11
    m_nonBusyEventTypes.insert(QEvent::Paint);//12
    m_nonBusyEventTypes.insert(QEvent::Resize);//14
    m_nonBusyEventTypes.insert(QEvent::Create);//15
    m_nonBusyEventTypes.insert(QEvent::Destroy);//16
    m_nonBusyEventTypes.insert(QEvent::Show);//17
    m_nonBusyEventTypes.insert(QEvent::Hide);//18
    m_nonBusyEventTypes.insert(QEvent::ParentChange);//21
    m_nonBusyEventTypes.insert(QEvent::HideToParent);//27
    m_nonBusyEventTypes.insert(QEvent::PaletteChange);//39
    m_nonBusyEventTypes.insert(QEvent::DeferredDelete);//52
    m_nonBusyEventTypes.insert(QEvent::ChildAdded);//68
    m_nonBusyEventTypes.insert(QEvent::ChildPolished);//69
    m_nonBusyEventTypes.insert(QEvent::ChildRemoved);//71
    m_nonBusyEventTypes.insert(QEvent::ActionChanged);
    m_nonBusyEventTypes.insert(QEvent::PolishRequest);//74
    m_nonBusyEventTypes.insert(QEvent::Polish);//75
    m_nonBusyEventTypes.insert(QEvent::LayoutRequest);//76
    m_nonBusyEventTypes.insert(QEvent::UpdateRequest);//77
    m_nonBusyEventTypes.insert(QEvent::UpdateLater);//78

    m_nonBusyEventTypes.insert(QEvent::LanguageChange);//89
    m_nonBusyEventTypes.insert(QEvent::FontChange);//97
    m_nonBusyEventTypes.insert(QEvent::StyleChange);//100
    m_nonBusyEventTypes.insert(QEvent::ActionRemoved);//115

    m_nonBusyEventTypes.insert(QEvent::ToolTipChange);//184
    m_nonBusyEventTypes.insert(QEvent::LayoutRequest);

    // busy events (used in filtered in)
    m_busyEventTypes.insert(QEvent::MouseButtonPress);
    m_busyEventTypes.insert(QEvent::MouseButtonRelease);
    m_busyEventTypes.insert(QEvent::MouseButtonDblClick);
    m_busyEventTypes.insert(QEvent::MouseMove);
    m_busyEventTypes.insert(QEvent::KeyPress);
    m_busyEventTypes.insert(QEvent::KeyRelease);
    m_busyEventTypes.insert(QEvent::TabletPress);
    m_busyEventTypes.insert(QEvent::TabletRelease);
    m_busyEventTypes.insert(QEvent::Wheel);
    m_busyEventTypes.insert(QEvent::TabletEnterProximity);
    m_busyEventTypes.insert(QEvent::TabletLeaveProximity);
    m_busyEventTypes.insert(QEvent::CursorChange);
    m_busyEventTypes.insert(QEvent::TouchBegin);
    m_busyEventTypes.insert(QEvent::TouchUpdate);
    m_busyEventTypes.insert(QEvent::TouchEnd);
#ifndef QT_NO_GESTURES
    m_busyEventTypes.insert(QEvent::Gesture);
    m_busyEventTypes.insert(QEvent::NativeGesture);
#endif
    m_busyEventTypes.insert(QEvent::Scroll);
    m_busyEventTypes.insert(QEvent::InputMethodQuery);
    m_busyEventTypes.insert(QEvent::OrientationChange);
    m_busyEventTypes.insert(QEvent::ThemeChange);
    //m_busyEventTypes.insert(QEvent::Paint);//?????

}

GLDApplication::~GLDApplication()
{
    if (0!=m_idleTimerID) {
        this->killTimer(m_idleTimerID);
    }
}

void GLDApplication::timerEvent( QTimerEvent *event )
{
    if ( event->timerId() == m_idleTimerID) {
        // first time switching from busy into idle
        this->killTimer(m_idleTimerID);
        m_idleTimerID = 0;
        onIdle();
    }

}

bool GLDApplication::notify(QObject *receiver, QEvent *e)
{
    try
    {
        bool result = QApplication::notify(receiver, e);

        if (QEvent::Timer == e->type()) {
            QTimerEvent* qtimerEvent = dynamic_cast<QTimerEvent *>(e);
            if (NULL == qtimerEvent ||
                    qtimerEvent->timerId()!=m_idleTimerID ) {
                //m_busy = true;
            }
        } else {
            bool isBusyNow = false;
            if (useFilterInBusy &&
                    m_busyEventTypes.find(e->type())!=
                    m_busyEventTypes.end()) {
                isBusyNow = true;
#ifdef DEBUG
                qDebug( "1<GLDApplication::notify()>type=%d", e->type());
#endif
            }
            else if (!useFilterInBusy && m_nonBusyEventTypes.find(e->type())==
                   m_nonBusyEventTypes.end() &&
                   e->type()<QEvent::User) {
                isBusyNow = true;
#ifdef DEBUG
                qDebug( "2<GLDApplication::notify()>type=%d", e->type());
#endif
            }
            if (isBusyNow && 0 == m_idleTimerID) {
                m_idleTimerID = this->startTimer(0);
            }

        }
        return result;
    }
    catch (GLDException &e)
    {
        showPrompt(e.message());
        return false;
    }
}

/*
void GLDApplication::initEventDispatcher()
{
    QCoreApplication::setEventDispatcher(new GLDEventDispatcherWin32(0));
}
*/

void GLDApplication::onIdle()
{
#ifdef DEBUG
    qDebug( "<GLDApplication::onIdle>:emit idle()");
#endif
    emit idle();
}
