#include "GLDSearchEdit.h"
#include <QCompleter>
#include <QAbstractItemView>
#include <QPainter>
#include <QEvent>
#include <QBitmap>
#include "GLDFileUtils.h"

const QSize c_nSearchIconSize(34, 25);

GLDSearchEdit::GLDSearchEdit(QWidget *parent)
    : QLineEdit(parent), m_pSearchAndClearButton(NULL)
{
//    setAttribute(Qt::WA_WindowPropagation);
    initButton();
    initTipsView();
    setObjectName("GLDSearchEdit");
    m_sQss = loadQssFile(":/qsses/GLDSearchEdit.qss");
    setStyleSheet(m_sQss);
    setFixedHeight(25);
}

GLDSearchEdit::~GLDSearchEdit()
{
}

void GLDSearchEdit::setRecentSearchs(QStringList &recentSearchs)
{
    m_oRecentSearchs = recentSearchs;
}

QStringList &GLDSearchEdit::recentSearchs()
{
    return m_oRecentSearchs;
}

bool GLDSearchEdit::hasFocus()
{
    return m_pSearchAndClearButton->hasFocus();
}

bool GLDSearchEdit::event(QEvent *e)
{
    if ((e->type() == QEvent::HoverEnter || e->type() == QEvent::FocusIn)
            && m_pSearchAndClearButton != NULL)
    {
        m_pSearchAndClearButton->setHasFocus(true);
        setStyleSheet(m_sQss);
    }
    else if ((e->type() == QEvent::HoverLeave || e->type() == QEvent::FocusOut)
             && m_pSearchAndClearButton != NULL)
    {
        m_pSearchAndClearButton->setHasFocus(false);
        setStyleSheet(m_sQss);
    }
    return QLineEdit::event(e);
}

void GLDSearchEdit::resizeEvent(QResizeEvent *e)
{
    QLineEdit::resizeEvent(e);
    m_pSearchAndClearButton->move(rect().right() - c_nSearchIconSize.width(),
                                  (rect().height() - c_nSearchIconSize.height()) / 2);
    moveRecentTipsView();
}

void GLDSearchEdit::mousePressEvent(QMouseEvent *e)
{
    QLineEdit::mousePressEvent(e);
    if (text().isEmpty())
    {
        moveRecentTipsView();
        m_pRecentCompleter->popup()->show();
    }
}

void GLDSearchEdit::initButton()
{
    m_pSearchAndClearButton = new GLDSearchButton(this);
    m_pSearchAndClearButton->setObjectName("m_pSearchAndClearButton");

    QMargins oOldMargins = contentsMargins();
    oOldMargins.setRight(c_nSearchIconSize.width());
    setContentsMargins(oOldMargins);

    connect(this, &QLineEdit::textChanged, this, &GLDSearchEdit::onTextChange);
    connect(m_pSearchAndClearButton, &QPushButton::clicked, this, &GLDSearchEdit::onButtonClick);
    connect(this, &QLineEdit::editingFinished, this, &GLDSearchEdit::onEditingFinished);
}

void GLDSearchEdit::initTipsView()
{
   m_pRecentCompleter = new QCompleter(m_oRecentSearchs, this);
   m_pRecentCompleter->setMaxVisibleItems(3);
   m_pRecentCompleter->popup()->resize(rect().width(), 56);
   setCompleter(m_pRecentCompleter);
}

void GLDSearchEdit::moveRecentTipsView()
{
    QPoint oEditBottomLeft = mapToGlobal(rect().bottomLeft());
    m_pRecentCompleter->popup()->resize(rect().width(), 56);
    m_pRecentCompleter->popup()->move(oEditBottomLeft);
}

void GLDSearchEdit::onTextChange(const QString &)
{
    if (!text().isEmpty() && !m_pSearchAndClearButton->searching())
    {
        m_pSearchAndClearButton->setProperty("searching", true);
        setStyleSheet(m_sQss);
    }
    else if (text().isEmpty() && m_pSearchAndClearButton->searching())
    {
        m_pSearchAndClearButton->setProperty("searching", false);
        setStyleSheet(m_sQss);
    }
}

void GLDSearchEdit::onButtonClick()
{
    if (m_pSearchAndClearButton->searching())
    {
        clear();
    }
}

void GLDSearchEdit::onEditingFinished()
{
    if (!text().isEmpty() && !m_oRecentSearchs.contains(text()))
    {
        m_pRecentCompleter->model()->insertRow(0);
        QModelIndex oRecentIndex = m_pRecentCompleter->model()->index(0, 0);
        m_pRecentCompleter->model()->setData(oRecentIndex, text());
        m_oRecentSearchs.append(text());
    }
}

void GLDSearchEdit::onRecentTipsSelected(const QModelIndex &index)
{
    setText(index.data().toString());
}

GLDSearchButton::GLDSearchButton(QWidget *parent)
    : QPushButton(parent), m_bSearching(false)
{
}

GLDSearchButton::~GLDSearchButton()
{

}

void GLDSearchButton::setSearching(bool searching)
{
    m_bSearching = searching;
}

bool GLDSearchButton::searching()
{
    return m_bSearching;
}

bool GLDSearchButton::hasFocus()
{
    return m_bHasFocus;
}

void GLDSearchButton::setHasFocus(bool value)
{
    m_bHasFocus = value;
}

bool GLDSearchButton::event(QEvent *e)
{
    if (e->type() == QEvent::HoverEnter)
    {
        setCursor(QCursor(Qt::ArrowCursor));
    }

    return QPushButton::event(e);
}
