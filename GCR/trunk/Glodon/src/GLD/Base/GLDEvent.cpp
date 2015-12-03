#include <QObject>
#include <QEvent>
#include "GLDEvent.h"
#include "GLDSystem.h"
#include "GLDGlobal.h"

GLDEvent::GLDEvent(GLDEventType type, GLDWParam wParam, GLDLParam lParam) :
    QEvent((QEvent::Type)type), m_wParam(wParam), m_lParam(lParam), m_result(0)
{

}

GLDWParam GLDEvent::wParam() const
{
    return m_wParam;
}

GLDWParam GLDEvent::lParam() const
{
    return m_lParam;
}

GLDEventType GLDEvent::type() const
{
    return static_cast<GLDEventType>(t);
}

GLDEventResult GLDEvent::result() const
{
    return m_result;
}

void GLDEvent::setResult(GLDEventResult value)
{
    m_result = value;
}
