#include "GLDAbstractBtnEdit.h"
#include "GLDGlobal.h"
#include <QCoreApplication>
#include <QPlainTextEdit>
#include <QButtonGroup>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>

const int c_MinButtonEditBtnWidth = 18;

GLDAbstractBtnEdit::GLDAbstractBtnEdit(QWidget *parent) :
    QWidget(parent), m_btnIconSize(m_maxBtnCount,QSize(0,0)),
    m_btnUsingIcon(m_maxBtnCount, 0), m_btns(NULL), m_associateData(NULL)
{
    init();
}

GLDAbstractBtnEdit::~GLDAbstractBtnEdit()
{
    if (!m_btns->parent())
    {
        freeAndNil(m_btns);
    }
}

bool GLDAbstractBtnEdit::setButtonsCount(int nBtnCount)
{
    if (nBtnCount <= m_maxBtnCount && m_btns)
    {
        for (int i = 0; i<nBtnCount; ++i)
        {
            m_btns->addButton(new QPushButton(QString(" ..."), this), i);
        }
        QObject::connect(m_btns, SIGNAL(buttonClicked(int)), this, SLOT(onEllipsisButtonClicked(int)));
        return true;
    }
    else
        return false;
}

int GLDAbstractBtnEdit::buttonsCount() const
{
    int ncount = 0;
    if (m_btns)
        ncount = m_btns->buttons().count();
    return ncount;
}

int GLDAbstractBtnEdit::maximumButtonsCount() const
{
    return m_maxBtnCount;
}

bool GLDAbstractBtnEdit::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *editor = dynamic_cast<QWidget*>(obj);
    QAbstractButton *btn = dynamic_cast<QAbstractButton *>(obj);
    if (!editor)
        return false;
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
        {
            if (keyEvent->modifiers() == Qt::CTRL)
            {
                ellipsisButtonClicked(btn);
                return true;
            }
            else if (keyEvent->modifiers() == Qt::ALT)
            {
                cursorPosInsertANewLine();
                return true;
            }
            else
            {
                QCoreApplication::sendEvent(this, event);
                return true;
            }
        }
        else if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down)
        {
            if (keyEvent->modifiers() == Qt::ALT)
            {
                ellipsisButtonClicked(btn);
                return true;
            }
            else
            {
                QCoreApplication::sendEvent(this, event);
                return true;
            }
        }
        else if (keyEvent->key() == Qt::Key_Tab)
        {
            QCoreApplication::sendEvent(this, event);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

bool GLDAbstractBtnEdit::setBtnText(QString &text, int btnIndex)
{
    QAbstractButton *btn = m_btns->button(btnIndex);
    if (btn)
    {
        btn->setText(text);
        return true;
    }
    return false;
}

bool GLDAbstractBtnEdit::setButtonIcon(QIcon &icon, int btnIndex)
{
    if (btnIndex >= 0 && btnIndex < buttonsCount())
    {
        m_btns->button(btnIndex)->setText("");
        m_btns->button(btnIndex)->setIcon(icon);
        m_btnIconSize[btnIndex] = m_btns->button(btnIndex)->iconSize();
        m_btnUsingIcon[btnIndex] = 1;
        return true;
    }
    else
        return false;
}

void GLDAbstractBtnEdit::onEllipsisButtonClicked(int clickedBtnIndex)
{
    emit ellipsisButtonClicked(clickedBtnIndex);
    QAbstractButton *btn = m_btns->button(clickedBtnIndex);
    if (btn)
        emit ellipsisButtonClicked(btn);
}

void GLDAbstractBtnEdit::onEllipsisButtonClicked(QAbstractButton *clickedBtn)
{
    if (m_btns && clickedBtn && buttonsCount() > 0)
    {
        int nbtnIndex = 0;
        QList<QAbstractButton *> btns = m_btns->buttons();
        for (QList<QAbstractButton *>::Iterator it = btns.begin(); it != btns.end(); ++it, ++nbtnIndex)
        {
            if (clickedBtn == *it)
            {
                emit ellipsisButtonClicked(nbtnIndex);
                emit ellipsisButtonClicked(clickedBtn);
            }
        }
    }
}

void GLDAbstractBtnEdit::init()
{
    m_btns = new QButtonGroup(this);
}

GLDBtnsLineEdit::GLDBtnsLineEdit(int btnCount, QWidget *parent):GLDAbstractBtnEdit(parent),
    m_edit(NULL)
{
    init(btnCount);
    m_edit->installEventFilter(this);

    if (btnCount > 0)
    {
        int ntargetBtnWidth = qMax(c_MinButtonEditBtnWidth, m_btns->button(0)->fontMetrics().width(" ..."));
        int nbtnWidth = btnCount * ntargetBtnWidth + 2;
        int nlineEditWidth = width() - nbtnWidth;

        m_edit->setGeometry(0, 0, nlineEditWidth, height());
    }
    else
    {
        m_edit->setGeometry(0, 0, width() - 1, height());
    }
    this->setFocusProxy(m_edit);
}

GLDBtnsLineEdit::~GLDBtnsLineEdit()
{

}

QString GLDBtnsLineEdit::text() const
{
    if (m_edit)
        return m_edit->text();
    else
        return QString("");
}

void GLDBtnsLineEdit::setText(QString text)
{
    m_edit->setText(text);
    m_edit->selectAll();
}

void GLDBtnsLineEdit::paintEvent(QPaintEvent *)
{
    int nk(0);
    int nl(0);
    for (nk = 0, nl = 1; nk < 100; ++nk)
    {
        nl = nk + 2;
    }
    int ntargetBtnWidth = 0;
    int nbtnNums = buttonsCount();
    if (nbtnNums > 0)
        ntargetBtnWidth = qMax(c_MinButtonEditBtnWidth, m_btns->button(0)->fontMetrics().width(" ..."));
//    m_edit->setGeometry(0 ,0, width() - btnNums * targetBtnWidth + 2 + btnNums - 1, height());
    int nbtnsWidth = 0;
    for (int i = nbtnNums - 1; i >= 0; --i)
    {
        QAbstractButton *m_button = m_btns->button(i);
        bool busingIcon = (0 != m_btnUsingIcon[i]);
        int nbtnOriginalHeight = -1;
        if (busingIcon)
        {
            m_button->resize(m_btnIconSize[i]);
            nbtnsWidth += m_btnIconSize[i].width();
            nbtnOriginalHeight += (height() - m_btnIconSize[i].height()) / 2.0;
            m_button->setStyleSheet("border-radius:16px;");
        }
        else
        {
            m_button->resize(ntargetBtnWidth + 1, height() + 2);
            nbtnsWidth += m_button->width();
            nbtnOriginalHeight += (height() - m_button->height()) / 2.0;
        }
        m_button->move(width() - nbtnsWidth + 1, nbtnOriginalHeight);
    }
    m_edit->setGeometry(0,0,width() - nbtnsWidth + 2,height());
}

void GLDBtnsLineEdit::selectAll()
{
    if (m_edit)
        m_edit->selectAll();
}

void GLDBtnsLineEdit::cursorPosInsertANewLine()
{
    return cursorToEnd();
}

void GLDBtnsLineEdit::cursorToEnd()
{
    if (m_edit && m_edit->isEnabled() && m_edit->hasFocus())
    {
        bool bselectText = false;
        m_edit->end(bselectText);
    }
}

void GLDBtnsLineEdit::init(int btnCount)
{
    freeAndNil(m_edit);
    m_edit = new QLineEdit(this);
    setButtonsCount(btnCount);
}
