#include <QtCore>
#include <QAbstractItemView>
#include <QLayout>
#include <QPoint>
#include <QPainter>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "GLDShadow.h"

const int c_nShadowWidth = 2;
const int c_nShadowRGB = 240;
const int c_nShadowRGBGrowth = 2;
//const int c_nShadowMaxAlpha = 255;
//const int c_nShadowAlphaGrowth = 10;
const int c_nShadowAlpha[8] = {255, 100, 90, 60, 30, 30, 15, 15};

const char c_strHasShadow[] = "hasShadow";

GLDShadow::GLDShadowWidget::GLDShadowWidget(QWidget *parent) :
    QWidget(parent, Qt::Popup)
{
}

bool GLDShadow::GLDShadowWidget::event(QEvent *e)
{
#ifdef Q_OS_WIN
    if (e->type() == QEvent::WinIdChange)
    {
        HWND hwnd = reinterpret_cast<HWND>(winId());
        DWORD DWStyle = ::GetClassLong(hwnd, GCL_STYLE);
        DWStyle &= ~CS_DROPSHADOW;
        ::SetClassLong(hwnd, GCL_STYLE, DWStyle);
    }
#endif
    return QWidget::event(e);
}

GLDShadow::GLDShadow(QWidget *parent):
    QObject(parent),
    m_bHandleResize(true),
    m_parent(parent)
{
}

void GLDShadow::doRemoveShadow()
{
    static bool s_bFirstRun = true;

    if (s_bFirstRun)
    {
        GLDShadowWidget Widget;
        Widget.show();
        s_bFirstRun = false;
    }
}

void GLDShadow::removeShadow()
{
    if (m_parent)
    {
        if (addShadow(m_parent))
        {
            doRemoveShadow();
        }
    }
}

bool GLDShadow::addShadow(QWidget *popup)
{
    if (NULL == popup)
    {
        return false;
    }
    else if (addShadow(dynamic_cast<QComboBox *>(popup)))
    {
        return true;
    }
    else
    {
        popup->installEventFilter(this);
        return true;
    }
}

bool GLDShadow::addShadow(QComboBox *popup)
{
    if (popup == NULL)
    {
        return false;
    }

    QWidget *pContainer = popup->view()->parentWidget();

    pContainer->setWindowFlags(pContainer->windowFlags() | Qt::FramelessWindowHint);
    pContainer->installEventFilter(this);
    pContainer->setProperty(c_strHasShadow, true);

    return true;
}

bool GLDShadow::eventFilter(QObject *object, QEvent *event)
{

    if (object->property(c_strHasShadow).isValid() && object->property(c_strHasShadow).toBool())
    {
        QWidget *pContainer = static_cast<QWidget *>(object);

        switch (event->type())
        {
            case QEvent::Paint:
                paintShadow(pContainer);
                break;

            case QEvent::Show:
                extendWidth(pContainer);
                break;

            case QEvent::Hide:
                doViewHide(pContainer);
                break;

            default:
                break;
        }
    }

    return QObject::eventFilter(object, event);
}

void GLDShadow::extendWidth(QWidget *container)
{
    if (!m_bHandleResize)
    {
        return;
    }

    m_bHandleResize = false;

    QWidget *pParent = container->parentWidget();
    container->resize(container->width() + c_nShadowWidth * 2, container->height() + c_nShadowWidth);
    container->layout()->setContentsMargins(c_nShadowWidth, 0, c_nShadowWidth, c_nShadowWidth);

    QPoint pointNow = container->pos();
    int nOffsetX = pParent->mapFromGlobal(pointNow).x() - pointNow.x();

    container->move(-nOffsetX - 2, pointNow.y());
    m_bHandleResize = true;
}

void GLDShadow::doViewHide(QWidget *container)
{
    container->resize(container->width() - c_nShadowWidth * 2, container->height() - c_nShadowWidth);

	if (NULL != container->layout())
	{
		container->layout()->setContentsMargins(0, 0, 0, 0);
	}
}

void GLDShadow::paintShadow(QWidget *container)
{
    QPainter painter(container);

    for (int i = 0; i < c_nShadowWidth; i++)
    {
        int nRGB = c_nShadowRGB + i * c_nShadowRGBGrowth;
        QPen pen;
        // –Ë“™œ»ª≠2œÒÀÿµƒ“ı”∞
        const QString c_sShadows[2] = {"#c0cce5", "#e1e5ed"};
        if (i < 2)
        {
            pen.setColor(QColor(c_sShadows[i]));
            painter.setPen(pen);
        }
        else
        {
            pen.setColor(QColor(nRGB, nRGB, nRGB, c_nShadowAlpha[i]));
        }
        pen.setWidth(1);
        painter.setPen(pen);

        QPoint pointStart;
        QPoint pointEnd;
        QRect rect = container->rect();

        // ª≠◊Û≤‡“ı”∞
        pointStart.setX(c_nShadowWidth - i - 1);
        pointStart.setY(rect.bottomLeft().y() - c_nShadowWidth + i + 1);
        pointEnd.setX(pointStart.x());
        pointEnd.setY(0);
        painter.drawLine(pointStart, pointEnd);

        // ª≠œ¬±ﬂ“ı”∞
        pointStart.setX(c_nShadowWidth - i - 1);
        pointStart.setY(rect.bottomLeft().y() - c_nShadowWidth + i + 1);
        pointEnd.setX(rect.bottomRight().x() - c_nShadowWidth + i + 1);
        pointEnd.setY(pointStart.y());
        painter.drawLine(pointStart, pointEnd);

        // ª≠”“≤‡“ı”∞
        pointStart.setX(rect.bottomRight().x() - c_nShadowWidth + i + 1);
        pointStart.setY(rect.bottomRight().y() - c_nShadowWidth + i + 1);
        pointEnd.setX(pointStart.x());
        pointEnd.setY(0);
        painter.drawLine(pointStart, pointEnd);
    }
}
