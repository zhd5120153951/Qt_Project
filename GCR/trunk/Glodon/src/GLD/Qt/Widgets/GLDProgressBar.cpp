#include <QThread>
#include <QMutex>
#include <QApplication>
#include "GLDProgressBar.h"
#include "GLDProgressBarEvent.h"

GLDProgressBar::GLDProgressBar(QWidget *parent)
{
    m_progressDlg = new QProgressDialog(parent);
    m_progressDlg->setCancelButton(NULL);
    m_progressDlg->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    m_progressDlg->setAutoFillBackground(true);
    m_progressDlg->setMaximum(0);
    m_progressDlg->setModal(true);
    m_progressDlg->setAutoClose(true);
    m_progressDlg->setAutoReset(false);

    m_barThreadId = this->thread()->currentThreadId();
}

GLDProgressBar::~GLDProgressBar()
{
    if (!isCreateThread())
    {
        QApplication::postEvent(this, new GDisposeEvent());
        QApplication::sendPostedEvents(this, Dispose);
        QMutex mutex;
        QMutexLocker locker(&mutex);
        QThread::msleep(100);
    }
    else
    {
        delete m_progressDlg;
    }
}

void GLDProgressBar::setRange(int LRange, int RRange)
{
    m_progressDlg->setRange(LRange, RRange);
}

void GLDProgressBar::updateProgress(int pos)
{
    if (!isCreateThread())
    {
        GUpdateEvent *evt = new GUpdateEvent();
        evt->setValue(pos);
        QApplication::postEvent(this, evt, Qt::HighEventPriority);
        QApplication::flush();
        m_cachePos = pos;
        QMutex mutex;
        QMutexLocker locker(&mutex);
        m_waitobj.wait(&mutex, 10);
    }
    else
    {
        m_progressDlg->setValue(pos);
    }
}

int GLDProgressBar::getProgressPos()
{
    return m_progressDlg->value();
}

void GLDProgressBar::setValue(const int pos)
{
    qApp->processEvents();
    m_progressDlg->setValue(pos);
    m_waitobj.wakeAll();
}

bool GLDProgressBar::event(QEvent *event)
{
    if (event->type() == SetValue)
    {
        m_progressDlg->setValue(m_cachePos);
        QApplication::flush();
        m_waitobj.wakeAll();
    }
    else if (event->type() == SetVisible)
    {
        GVisibleEvent *pEvt = dynamic_cast<GVisibleEvent *>(event);
        m_progressDlg->setVisible(pEvt->visible());
        m_waitobj.wakeAll();
    }
    else if (event->type() == Resize)
    {
        GResizeEvent *pEvt = dynamic_cast<GResizeEvent *>(event);
        m_progressDlg->resize(pEvt->width(), pEvt->height());
    }
    else if (event->type() == SetPosition)
    {
        GSetPosEvent *pEvt = dynamic_cast<GSetPosEvent *>(event);
        m_progressDlg->move(pEvt->x(), pEvt->y());
    }
    else if (event->type() == Dispose)
    {
        delete m_progressDlg;
    }

    return true;
}

void GLDProgressBar::hide()
{
    if (!isCreateThread())
    {
        GVisibleEvent *evt = new GVisibleEvent();
        evt->setIsVisible(false);
        QApplication::postEvent(this, evt, Qt::HighEventPriority);
    }
    else
    {
        m_progressDlg->hide();
    }
}

void GLDProgressBar::show()
{
    if (!isCreateThread())
    {
        GVisibleEvent *evt = new GVisibleEvent();
        evt->setIsVisible(true);
        QApplication::postEvent(this, evt, Qt::HighEventPriority);
        QApplication::sendPostedEvents(this, SetVisible);
        QMutex mutex;
        QMutexLocker locker(&mutex);
        m_waitobj.wait(&mutex, 10);;
    }
    else
    {
        m_progressDlg->show();
    }
}

bool GLDProgressBar::isCreateThread()
{
    return m_barThreadId == this->thread()->currentThreadId();
}

void GLDProgressBar::setSize(int width, int height)
{
    if (!isCreateThread())
    {
        GResizeEvent *evt = new GResizeEvent();
        evt->setWidth(width);
        evt->setHeight(height);
        QApplication::postEvent(this, evt, Qt::HighEventPriority);
    }
    else
    {
        m_progressDlg->resize(width, height);
    }
}

void GLDProgressBar::setPos(int x, int y)
{
    if (!isCreateThread())
    {
        GSetPosEvent *evt = new GSetPosEvent();
        evt->setX(x);
        evt->setY(y);
        QApplication::postEvent(this, evt, Qt::HighEventPriority);
    }
    else
    {
        m_progressDlg->move(x, y);
    }
}

void GLDProgressBar::setText(const QString &str)
{
    qApp->processEvents();
    m_progressDlg->setLabelText(str);
}



