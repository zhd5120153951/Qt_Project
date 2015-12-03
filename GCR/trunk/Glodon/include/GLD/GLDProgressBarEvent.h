/*!
*@file
*@brief 定义设置值的各种事件
*@author wangdz
*@date 2014年12月13日
*@remarks
*
*Copyright (c) 1998-25 Glodon Corporation
*/
#ifndef GLDPROGRESSBAREVENT_H
#define GLDPROGRESSBAREVENT_H


#include <QEvent>
#include "GLDWidget_Global.h"

static const QEvent::Type SetValue    = (QEvent::Type) QEvent::registerEventType(QEvent::User + 100);
static const QEvent::Type SetVisible  = (QEvent::Type) QEvent::registerEventType(QEvent::User + 101);
static const QEvent::Type Resize      = (QEvent::Type) QEvent::registerEventType(QEvent::User + 102);
static const QEvent::Type SetPosition = (QEvent::Type) QEvent::registerEventType(QEvent::User + 103);
static const QEvent::Type Dispose     = (QEvent::Type) QEvent::registerEventType(QEvent::User + 104);

//刷新滚动条值事件
class GLDWIDGETSHARED_EXPORT GUpdateEvent: public QEvent
{
public:
    GUpdateEvent(): QEvent(SetValue) {}
    int value();
    void setValue(int value);

private:
    int m_value;
};

//设置显隐事件
class GLDWIDGETSHARED_EXPORT GVisibleEvent: public QEvent
{
public:
    GVisibleEvent(): QEvent(SetVisible)
    {
        m_visible = false;
    }
    bool visible();
    void setIsVisible(bool visible);

private:
    bool m_visible;
};

//设置大小事件
class GLDWIDGETSHARED_EXPORT GResizeEvent: public QEvent
{
public:
    GResizeEvent(): QEvent(Resize)
    {
        m_width = 0;
        m_height = 0;
    }
    int width();
    int height();
    void setWidth(int width);
    void setHeight(int height);

private:
    int m_width;
    int m_height;
};

class GLDWIDGETSHARED_EXPORT GSetPosEvent: public QEvent
{
public:
    GSetPosEvent(): QEvent(SetPosition)
    {
        m_x = 0;
        m_y = 0;
    }
    int x();
    int y();
    void setX(int x);
    void setY(int y);

private:
    int m_x;
    int m_y;
};

class GLDWIDGETSHARED_EXPORT GDisposeEvent: public QEvent
{
public:
    GDisposeEvent(): QEvent(Dispose) {}
};

#endif
