#include "GLDProgressBarEvent.h"

bool GVisibleEvent::visible()
{
    return m_visible;
}

void GVisibleEvent::setIsVisible( bool visible )
{
    m_visible = visible;
}

int GResizeEvent::width()
{
   return m_width;
}

int GResizeEvent::height()
{
   return m_height;
}

void GResizeEvent::setWidth( int width )
{
    m_width = width;
}

void GResizeEvent::setHeight( int height )
{
    m_height = height;
}

int GSetPosEvent::x()
{
    return m_x;
}

int GSetPosEvent::y()
{
    return m_y;
}

void GSetPosEvent::setX( int x )
{
   m_x = x;
}

void GSetPosEvent::setY( int y )
{
   m_y = y;
}

int GUpdateEvent::value()
{
    return m_value;
}

void GUpdateEvent::setValue( int value )
{
    m_value = value;
}
