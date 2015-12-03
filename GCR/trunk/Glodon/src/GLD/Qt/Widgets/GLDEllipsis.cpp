#include <QString>
#include <QApplication>

#include "GLDEllipsis.h"
#include "GLDFileUtils.h"

const QString c_sLineButtonEditQssFile = ":/qsses/GLDLineButtonEditEx.qss";
const QString c_sPlainButtonEditQssFile = ":/qsses/GLDPlainButtonEditEx.qss";

GLDAbstractButtonEdit::GLDAbstractButtonEdit(QWidget *parent) :
    QWidget(parent)
{
    m_button = new QPushButton(QString(" ..."), this);
//    m_button->installEventFilter(this);

    int nTargetBtnWidth = qMax(c_MinButtonEditBtnWidth, m_button->fontMetrics().width(" ..."));
    m_button->resize(nTargetBtnWidth + 1, height() + 2);
    m_button->move(width() - nTargetBtnWidth, -1);

    connect(m_button, SIGNAL(clicked()), this, SLOT(onEllipsisButtonClicked()));
}

GLDAbstractButtonEdit::~GLDAbstractButtonEdit()
{
    if (m_button != NULL)
    {
        freeAndNil(m_button);
    }
}

QString GLDAbstractButtonEdit::buttonCaption() const
{
    return m_button->text();
}

void GLDAbstractButtonEdit::setButtonCaption(QString caption)
{
    m_button->setText(caption);
}

QModelIndex GLDAbstractButtonEdit::modelIndex() const
{
    return index;
}

void GLDAbstractButtonEdit::setModelIndex(QModelIndex newIndex)
{
    index = newIndex;
}

void GLDAbstractButtonEdit::onEllipsisButtonClicked()
{
    emit ellipsisButtonClicked();
    emit ellipsisButtonClicked(index);

    // 在tableView开启失去焦点退出编辑状态时，如果外部在接收到以上两个信号后，弹框，会导致tableView立即closeEdit，
    // 当前对象就被释放了，再setFocus会引起崩溃。
    // litz-a之前修改的回车跳格问题，先暂时不处理。
//        setFocus();
}

//bool GLDAbstractButtonEdit::event(QEvent *e)
//{
//    if (e->type() == QEvent::Wheel)
//    {
//        //数据非法会出现两次弹窗
////        parentWidget()->setFocus();
//        return true;
//    }
//    return QWidget::event(e);
//}

QString GLDPlainButtonEdit::text() const
{
    QString strPlainText = m_plainTextEdit->toPlainText();
    return strPlainText;
}

void GLDPlainButtonEdit::setText(QString text)
{
#ifdef Q_OS_WIN
    text.replace("\r\n", "\n");
#endif
    m_plainTextEdit->setPlainText(text);
    m_plainTextEdit->selectAll();
}

void GLDPlainButtonEdit::setEditable(bool canEdit)
{
    m_plainTextEdit->setReadOnly(!canEdit);
}

bool GLDPlainButtonEdit::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *editor = dynamic_cast<QWidget*>(obj);
    if (!editor)
        return false;
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
        {
            if (keyEvent->modifiers() == Qt::CTRL)
            {
                onEllipsisButtonClicked();
                return true;
            }
            else if (keyEvent->modifiers() == Qt::ALT)
            {
                cursorPosInsertANewLine(true);
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
                onEllipsisButtonClicked();
                return true;
            }
            else
            {
                QCoreApplication::sendEvent(this, event);
                return true;
            }
        }
        else if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab)
        {
            QCoreApplication::sendEvent(this, event);
            return true;
        }
        else if (keyEvent->key() != Qt::Key_Left && keyEvent->key() != Qt::Key_Right
                 && keyEvent->key() != Qt::Key_Home && keyEvent->key() != Qt::Key_End)
        {
            event->ignore();
            return false;
        }
    }
    if(event->type() == QEvent::FocusOut)
    {
        if(QApplication::focusWidget() != m_button)
        {
            QCoreApplication::sendEvent(this, event);
            return false;
        }
    }
    return QWidget::eventFilter(obj, event);
}

bool GLDPlainButtonEdit::selectAll()
{
    bool bSelectSuccess = false;
    if (m_plainTextEdit && m_plainTextEdit->isEnabled() && m_plainTextEdit->hasFocus())
    {
        m_plainTextEdit->selectAll();
        bSelectSuccess = true;
    }
    return bSelectSuccess;
}

bool GLDPlainButtonEdit::cursorPosInsertANewLine(bool addANewLine)
{
    bool bSelectSuccess = false;
    if (m_plainTextEdit && m_plainTextEdit->isEnabled() && m_plainTextEdit->hasFocus())
    {
        if (addANewLine)
        {
            m_plainTextEdit->removeEventFilter(this);
            QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
            QCoreApplication::sendEvent(m_plainTextEdit, &keyEvent);
            m_plainTextEdit->installEventFilter(this);
        }
        else
            m_plainTextEdit->moveCursor(QTextCursor::End);
        bSelectSuccess = true;
    }
    return bSelectSuccess;
}

bool GLDPlainButtonEdit::cursorMoveTextEnd()
{
    return cursorPosInsertANewLine(false);
}

QMargins GLDPlainButtonEdit::contentsMargins() const
{
    return m_plainTextEdit->contentsMargins();
}

void GLDPlainButtonEdit::setContentsMargins(const QMargins &rhs) const
{
    m_plainTextEdit->setContentsMargins(rhs);
}

void GLDPlainButtonEdit::setFont(const QFont &font)
{
    m_plainTextEdit->setFont(font);
}

void GLDPlainButtonEdit::cut()
{
    m_plainTextEdit->cut();
}

void GLDPlainButtonEdit::paste()
{
    m_plainTextEdit->paste();
}

void GLDPlainButtonEdit::copy()
{
    m_plainTextEdit->copy();
}

void GLDPlainButtonEdit::deleteSelectedText()
{
    if (hasSelectedText())
    {
        m_plainTextEdit->textCursor().removeSelectedText();
    }
}

void GLDPlainButtonEdit::onCopyAvailable(bool yes)
{
    m_hasSelectedText = yes;
    emit copyAvailable(yes);
}

GLDPlainButtonEdit::GLDPlainButtonEdit(QWidget *parent) :
    GLDAbstractButtonEdit(parent), m_hasSelectedText(false)
{
    m_plainTextEdit = new QPlainTextEdit(this);
    m_plainTextEdit->installEventFilter(this);

    int nTargetBtnWidth = qMax(c_MinButtonEditBtnWidth, m_button->fontMetrics().width(" ..."));
    m_plainTextEdit->setGeometry(0, 0, width() - nTargetBtnWidth + 2, height());
    m_plainTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_plainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_plainTextEdit->document()->setDocumentMargin(0);
    this->setFocusProxy(m_plainTextEdit);

    connect(m_plainTextEdit, SIGNAL(copyAvailable(bool)),
            this, SLOT(onCopyAvailable(bool)));
}

GLDPlainButtonEdit::~GLDPlainButtonEdit()
{
    if (m_plainTextEdit != NULL)
    {
        delete m_plainTextEdit;
    }
}

void GLDPlainButtonEdit::paintEvent(QPaintEvent *)
{
    int nTargetBtnWidth = qMax(c_MinButtonEditBtnWidth, m_button->fontMetrics().width(" ..."));
    m_plainTextEdit->setGeometry(0 ,0, width() - nTargetBtnWidth + 2, height());
    m_button->resize(nTargetBtnWidth + 1, height() + 2);
    m_button->move(width() - nTargetBtnWidth, -1);
}

QString GLDLineButtonEdit::text() const
{
    return m_lineEdit->text();
}

void GLDLineButtonEdit::setText(QString text)
{
    m_lineEdit->setText(text);
}

void GLDLineButtonEdit::setEditable(bool canEdit)
{
    m_lineEdit->setReadOnly(!canEdit);
}

bool GLDLineButtonEdit::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *editor = dynamic_cast<QWidget*>(obj);
    if (!editor)
        return false;
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
        {
            if (keyEvent->modifiers() == Qt::CTRL)
            {
                onEllipsisButtonClicked();
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
                onEllipsisButtonClicked();
                return true;
            }
        }
        else if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab)
        {
            QCoreApplication::sendEvent(this, event);
            return true;
        }
        else if (keyEvent->key() != Qt::Key_Left && keyEvent->key() != Qt::Key_Right
                 && keyEvent->key() != Qt::Key_Home && keyEvent->key() != Qt::Key_End)
        {
            event->ignore();
            return false;
        }
    }
    return QWidget::eventFilter(obj, event);
}

bool GLDLineButtonEdit::selectAll()
{
    bool bSelectSuccess = false;
    if (m_lineEdit && m_lineEdit->isEnabled())
    {
        m_lineEdit->setFocus();
        m_lineEdit->selectAll();
        bSelectSuccess = true;
    }
    return bSelectSuccess;
}

bool GLDLineButtonEdit::cursorPosInsertANewLine(bool addANewLine)
{
    Q_UNUSED(addANewLine);
    bool bSelectSuccess = false;
    if (m_lineEdit && m_lineEdit->isEnabled() && m_lineEdit->hasFocus())
    {
        //官方QLineEdit不提供折行功能
//        m_lineEdit->cursorForward(false,m_lineEdit->text().length());
        bool bSelectText = false;
        m_lineEdit->end(bSelectText);
        bSelectSuccess = true;
    }
    return bSelectSuccess;
}

bool GLDLineButtonEdit::cursorMoveTextEnd()
{
    return cursorPosInsertANewLine(false);
}

QMargins GLDLineButtonEdit::contentsMargins() const
{
    return m_lineEdit->contentsMargins();
}

void GLDLineButtonEdit::setContentsMargins(const QMargins &rhs) const
{
    m_lineEdit->setContentsMargins(rhs);
}

void GLDLineButtonEdit::setFont(const QFont &font)
{
    m_lineEdit->setFont(font);
}

void GLDLineButtonEdit::cut()
{
    m_lineEdit->cut();
}

void GLDLineButtonEdit::paste()
{
    m_lineEdit->paste();
}

void GLDLineButtonEdit::copy()
{
    m_lineEdit->copy();
}

void GLDLineButtonEdit::deleteSelectedText()
{
    if (hasSelectedText())
    {
        m_lineEdit->del();
    }
}

GLDLineButtonEdit::GLDLineButtonEdit(QWidget *parent, QLineEdit *editor) :
    GLDAbstractButtonEdit(parent)
{
    if (editor == NULL)
    {
        m_lineEdit = new GLDEllipsisLineEdit(this);
    }
    else
    {
        m_lineEdit = editor;
        m_lineEdit->setParent(this);
    }
    m_lineEdit->installEventFilter(this);

    int nTargetBtnWidth = qMax(c_MinButtonEditBtnWidth, m_button->fontMetrics().width(" ..."));
    int nBtnWidth = nTargetBtnWidth + 2;
    int nLineEditWidth = width() - nBtnWidth;

    m_lineEdit->setGeometry(0, 0, nLineEditWidth, height());
    this->setFocusProxy(m_lineEdit);

    connect(m_lineEdit, SIGNAL(cursorPositionChanged(int,int)),
            this, SLOT(doCursorPositionChanged(int,int)));
    connect(m_lineEdit, SIGNAL(selectionChanged()),
            this, SLOT(doSelectionChanged()));
}

GLDLineButtonEdit::~GLDLineButtonEdit()
{
    if (m_lineEdit != NULL)
    {
        delete m_lineEdit;
    }
}

void GLDLineButtonEdit::paintEvent(QPaintEvent *)
{
    int nTargetBtnWidth = qMax(c_MinButtonEditBtnWidth, m_button->fontMetrics().width(" ..."));
    int nLineWidth = width() - nTargetBtnWidth + 2;
    m_lineEdit->setGeometry(0 ,0, nLineWidth, height());

    int nBtnWidth = nTargetBtnWidth + 1;
    m_button->resize(nBtnWidth, height() + 2);
    m_button->move(nLineWidth - 2, -1);
}

void GLDLineButtonEdit::doSelectionChanged()
{
    emit selectionChanged();
}

void GLDLineButtonEdit::doCursorPositionChanged(int, int)
{
    emit cursorPositionChanged();
}

void GLDLineButtonEdit::closeCopy()
{
    GLDEllipsisLineEdit *oLineEdit = dynamic_cast<GLDEllipsisLineEdit *>(m_lineEdit);
    if (NULL != oLineEdit)
    {
        oLineEdit->closeCopy();
    }
}

void GLDLineButtonEdit::openCopy()
{
    GLDEllipsisLineEdit *oLineEdit = dynamic_cast<GLDEllipsisLineEdit *>(m_lineEdit);
    if (NULL != oLineEdit)
    {
        oLineEdit->openCopy();
    }
}

void GLDLineButtonEdit::closePaste()
{
    GLDEllipsisLineEdit *oLineEdit = dynamic_cast<GLDEllipsisLineEdit *>(m_lineEdit);
    if (NULL != oLineEdit)
    {
        oLineEdit->closePaste();
    }
}

void GLDLineButtonEdit::openPaste()
{
    GLDEllipsisLineEdit *oLineEdit = dynamic_cast<GLDEllipsisLineEdit *>(m_lineEdit);
    if (NULL != oLineEdit)
    {
        oLineEdit->openPaste();
    }
}

GLDLineButtonEditEx::GLDLineButtonEditEx(QWidget *parent, QLineEdit *editor):
    GLDLineButtonEdit(parent)
{
    Q_UNUSED(editor);
    setProperty("EllipsisButtonWrap", "GLDLineButtonEditEx");
    setHasBorder(true);
    this->setStyleSheet(loadQssFile(c_sLineButtonEditQssFile));
}

GLDLineButtonEditEx::~GLDLineButtonEditEx()
{

}

void GLDLineButtonEditEx::paintEvent(QPaintEvent *e)
{
    GLDLineButtonEdit::paintEvent(e);
    m_button->setPalette(QPalette(Qt::white));
    m_button->setAutoFillBackground(true);
}

void GLDLineButtonEditEx::setHasBorder(bool bHasBorder)
{
    if (bHasBorder)
    {
       setProperty("EllipsisButtonWrap", "GLDLineButtonEditEx");
    }
    else
    {
        setProperty("EllipsisButtonWrap", "noborder");
    }
    this->setStyleSheet(loadQssFile(c_sLineButtonEditQssFile));
}

GLDPlainButtonEditEx::GLDPlainButtonEditEx(QWidget *parent):
    GLDPlainButtonEdit(parent)
{
    QRect buttonRect = m_button->rect();
    m_button->setGeometry(buttonRect);
    // Qss美化
    setProperty("EllipsisButtonWrap", "GLDPlainButtonEditEx");
    setHasBorder(true);
    this->setStyleSheet(loadQssFile(c_sPlainButtonEditQssFile));
}

GLDPlainButtonEditEx::~GLDPlainButtonEditEx()
{

}

void GLDPlainButtonEditEx::paintEvent(QPaintEvent *e)
{
    GLDPlainButtonEdit::paintEvent(e);
    m_button->setPalette(QPalette(Qt::white));
    m_button->setAutoFillBackground(true);
}

void GLDPlainButtonEditEx::setHasBorder(bool bHasBorder)
{
    if (bHasBorder)
    {
        setProperty("EllipsisButtonWrap", "GLDPlainButtonEditEx");
    }
    else
    {
        setProperty("EllipsisButtonWrap", "noborder");
    }
    setStyle(NULL);
    this->setStyleSheet(loadQssFile(c_sPlainButtonEditQssFile));
}

GLDEllipsisLineEdit::GLDEllipsisLineEdit(QWidget *parent) :
    QLineEdit(parent), m_closePaste(false), m_closeCopy(false)
{

}

void GLDEllipsisLineEdit::closeCopy()
{
    m_closeCopy = true;
}

void GLDEllipsisLineEdit::openCopy()
{
    m_closeCopy = false;
}

void GLDEllipsisLineEdit::closePaste()
{
    m_closePaste = true;
}

void GLDEllipsisLineEdit::openPaste()
{
    m_closePaste = false;
}

void GLDEllipsisLineEdit::keyPressEvent(QKeyEvent *e)
{
    if (m_closePaste && e->key() == Qt::Key_V &&
            e->modifiers() == Qt::ControlModifier)
    {
        e->accept();
        return ;
    }
    if (m_closeCopy && e->key() == Qt::Key_C &&
            e->modifiers() == Qt::ControlModifier)
    {
        e->accept();
        return ;
    }

    QLineEdit::keyPressEvent(e);
}
