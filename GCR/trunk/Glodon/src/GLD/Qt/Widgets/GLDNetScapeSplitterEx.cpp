#include "GLDNetScapeSplitterEx.h"

GLDNetScapeSplitterButtonEx::GLDNetScapeSplitterButtonEx(Qt::Orientation o, QWidget *parent)
    : GLDNetScapeSplitterButton(o, parent)
{
    setButtonNameByArrowDirection();
    loadStyleSheet(QString("://qsses/splitter.qss"));
}

void GLDNetScapeSplitterButtonEx::paintEvent(QPaintEvent *e)
{
    QPushButton::paintEvent(e);
}

void GLDNetScapeSplitterButtonEx::loadStyleSheet(const QString &fileName)
{
    QFile styleSheet(fileName);

    if (!styleSheet.open(QIODevice::ReadOnly))
    {
        qWarning(QString("Can't open style sheet file: %1").arg(fileName).toLatin1().data());
        return;
    }

    this->setQssFileName(fileName);
    this->setStyleSheet(styleSheet.readAll());
}

//---GLDNetScapeSplitterHandle
GLDNetScapeSplitterHandleEx::GLDNetScapeSplitterHandleEx(Qt::Orientation o, GLDNetScapeSplitter *parent)
    : GLDNetScapeSplitterHandle(o, parent)
{

}

void GLDNetScapeSplitterHandleEx::initButton()
{
    m_pButton = new GLDNetScapeSplitterButtonEx(this->orientation(), this);

    m_pButton->setCursor(Qt::ArrowCursor);

    connect(m_pButton, SIGNAL(clicked()), this, SLOT(handlerButtonClick()));
}

//---GLDNetScapeSplitter
void GLDNetScapeSplitterEx::init()
{
    setHandleWidth(c_InitialHandleWidthEx);
}

GLDNetScapeSplitterEx::GLDNetScapeSplitterEx(QWidget *parent):
    GLDNetScapeSplitter(parent)
{
    init();
}

GLDNetScapeSplitterEx::GLDNetScapeSplitterEx(Qt::Orientation original,
        QWidget *parent):
    GLDNetScapeSplitter(original, parent)
{
    init();
}

GLDNetScapeSplitterEx::GLDNetScapeSplitterEx(bool initCollapsed,
        QWidget *parent):
    GLDNetScapeSplitter(initCollapsed, parent)
{
    init();
}

GLDNetScapeSplitterEx::GLDNetScapeSplitterEx(Qt::Orientation original,
        bool initCollapsed, QWidget *parent):
    GLDNetScapeSplitter(original, initCollapsed, parent)
{
    init();
}

void GLDNetScapeSplitterEx::loadStyleSheet(const QString &fileName)
{
    m_handle->getButton()->loadStyleSheet(fileName);
}

GLDSplitterHandle *GLDNetScapeSplitterEx::createHandle()
{
    GLDNetScapeSplitterHandleEx *handle = new GLDNetScapeSplitterHandleEx(this->orientation(), this);

    m_handle = handle;

    if (NULL != handle)
    {
        initSignalSlot(handle);
    }

    return handle;
}
