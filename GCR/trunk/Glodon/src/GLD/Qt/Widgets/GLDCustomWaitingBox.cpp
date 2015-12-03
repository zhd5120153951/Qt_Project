#include <QPoint>
#include <QPushButton>
#include <QPainter>
#include <QResizeEvent>
#include <QString>

#include "GLDCustomWaitingBox.h"

GLDCustomWaitingBox::GLDCustomWaitingBox(QWidget *parent)
    : QWidget(parent)
    , m_mapWait("://icons/CLDCustomWaitingBox_Gray.png")
    , m_nimageIndex(0)
    , m_nTimerId(0)
    , m_bkColor(0, 0, 0, 0)
{
    connect(this, SIGNAL(start()), this, SLOT(show()));
    connect(this, SIGNAL(end()), this, SLOT(hide()));

    hide();
}

GLDCustomWaitingBox::~GLDCustomWaitingBox()
{
    if (m_nTimerId != 0)
    {
        QObject::killTimer(m_nTimerId);
        m_nTimerId = 0;
    }
}

bool GLDCustomWaitingBox::loadWaitMap(const QString &fileName)
{
    return m_mapWait.load(fileName);
}

void GLDCustomWaitingBox::hide()
{
    QWidget *pParent = parentWidget();

    if (pParent != NULL)
    {
        pParent->removeEventFilter(this);
    }

    QWidget::hide();
}

void GLDCustomWaitingBox::show()
{
    QWidget *pParent = parentWidget();

    if (pParent != NULL)
    {
        move(0, 0);
        resize(pParent->size());
    }

    QWidget::show();

    raise();

    if (pParent != NULL)
    {
        pParent->installEventFilter(this);
    }

    if (m_nTimerId != 0)
    {
        QObject::killTimer(m_nTimerId);
        m_nTimerId = 0;
    }

    m_nTimerId = QObject::startTimer(60);

    this->activateWindow();
}

void GLDCustomWaitingBox::setBackGroundColor(const QColor &color)
{
    m_bkColor = color;
    update();
}

QColor GLDCustomWaitingBox::backGroundColor()
{
    return m_bkColor;
}

bool GLDCustomWaitingBox::eventFilter(QObject *target, QEvent *event)
{
    QWidget *pParent = parentWidget();

    if (target == pParent)
    {
        if (event->type() == QEvent::Resize)
        {
            QResizeEvent *presizeEvent = (QResizeEvent *)event;
            move(0, 0);
            resize(presizeEvent->size());
        }
    }

    if (event->type() == QEvent::HoverMove
            && event->type() == QEvent::MouseButtonPress
            && event->type() == QEvent::MouseButtonDblClick
            && event->type() == QEvent::MouseButtonPress
            && event->type() == QEvent::MouseMove
            && event->type() == QEvent::MouseMove
            && event->type() == QEvent::Wheel
            && event->type() == QEvent::KeyPress
            && event->type() == QEvent::KeyRelease)
    {
        return true;
    }

    return QWidget::eventFilter(target, event);
}

void GLDCustomWaitingBox::paintEvent(QPaintEvent *)
{
    QPainter painterMem(this);
    painterMem.setPen(QPen(m_bkColor));
    painterMem.setBrush(QBrush(m_bkColor));
    painterMem.drawRect(0, 0, width(), height());

    int nxIndex = m_nimageIndex % 6;
    int nyIndex = m_nimageIndex / 6;

    int nwidth = m_mapWait.width() / 6;
    int nheight = m_mapWait.height() / 2;

    painterMem.drawPixmap((width() - nwidth) / 2
                          , (height() - nheight) / 2
                          , nwidth
                          , nheight
                          , m_mapWait
                          , nxIndex * nwidth
                          , nyIndex * nheight
                          , nwidth
                          , nheight);
}

void GLDCustomWaitingBox::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_nTimerId)
    {
        m_nimageIndex = m_nimageIndex % 11;
        update();
        ++m_nimageIndex;
    }
}
