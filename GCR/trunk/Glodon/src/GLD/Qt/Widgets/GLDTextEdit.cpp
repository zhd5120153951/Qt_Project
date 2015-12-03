#include "GLDWindowComboBoxEx.h"

#include <QPainter>
#include <QPalette>
#include <QFile>

#include "GLDTextEdit.h"
#include "GLDFileUtils.h"

const GString c_sLineEditQssFile = ":/qsses/GLDCustomLineEdit.qss";
const GString c_sPlainTextEditQssFile = ":/qsses/GLDPlainTextEditEx.qss";

GLDPlainTextEdit::GLDPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    m_isFouseKeepInLineEdit = false;
    setContentsMargins(0, 0, 0, 0);
    document()->setDocumentMargin(0);
    connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
}

bool GLDPlainTextEdit::hasSelectedText()
{
    return textCursor().hasSelection();
}

QString GLDPlainTextEdit::text()
{
    return toPlainText();
}

void GLDPlainTextEdit::setText(QString text)
{
    setPlainText(text);
}

QString GLDPlainTextEdit::placeholderText() const
{
    return m_placeholderText;
}

void GLDPlainTextEdit::setPlaceholderText(const QString &placeholderText)
{
    if (m_placeholderText != placeholderText) {
        m_placeholderText = placeholderText;
        if (!hasFocus())
            update();
    }
}

void GLDPlainTextEdit::mousePressEvent(QMouseEvent *e)
{
    QPlainTextEdit::mousePressEvent(e);
    e->accept();
    GLDWindowComboBoxEx *combox = dynamic_cast<GLDWindowComboBoxEx *>(parentWidget());
    if (NULL == combox)
    {
        return;
    }

    else if (!combox->isHidePopupOnEdit())
    {
        if (e->buttons() & Qt::LeftButton)
        {
            combox->showPopup();
        }
    }
}

void GLDPlainTextEdit::paintEvent(QPaintEvent *e)
{
    QPlainTextEdit::paintEvent(e);
    QPainter p(viewport());
    QPalette pal = palette();

    QFontMetrics fm = fontMetrics();
    int nLeft, nRight, nBottom, nTop = 0;
    getContentsMargins(&nLeft, &nTop, &nRight, &nBottom);

    QRect r = rect();

    int minLB = qMax(0, - fm.minLeftBearing());

    QRect lineRect(cursorRect().left(), cursorRect().top(), r.width() - nLeft - nRight, cursorRect().height());

    if (document()->toPlainText().isEmpty()) {
        if (!m_placeholderText.isEmpty()) {
            QColor col = pal.text().color();
            col.setAlpha(128);
            QPen oldpen = p.pen();
            p.setPen(col);
            QRect ph = lineRect.adjusted(minLB, 0, 0, 0);
            QString elidedText = fm.elidedText(m_placeholderText, Qt::ElideRight, ph.width());
            p.drawText(ph, Qt::AlignVCenter | Qt::AlignLeft, elidedText);
            p.setPen(oldpen);
        }
    }
    if (m_isFouseKeepInLineEdit)
    {
        setFocus();
    }
}

void GLDPlainTextEdit::deleteSelectedText()
{
    textCursor().removeSelectedText();}

void GLDPlainTextEdit::onTextChanged()
{
    if (m_text == toPlainText())
    {
        return;
    }
    else
    {
        m_text = toPlainText();
        emit textChanged(m_text);
    }
}

GLDCustomLineEdit::GLDCustomLineEdit(QWidget *parent) :
    QLineEdit(parent),
    m_hasBorder(true)
{
    this->setStyleSheet(loadQssFile(c_sLineEditQssFile));
}

void GLDCustomLineEdit::setHasBorder(bool bValue)
{
    if (bValue != m_hasBorder)
    {
        m_hasBorder = bValue;
    }

    if (!m_hasBorder)
    {
        setProperty("border", "noBorder");
    }
    this->setStyleSheet(loadQssFile(c_sLineEditQssFile));
}

void GLDCustomLineEdit::deleteSelectedText()
{
    if (hasSelectedText())
        this->del();
}

void GLDPlainTextEdit::forceSelectColorToBlue()
{
   QPalette palette = this->palette();
   palette.setColor(QPalette::Highlight, QColor(70, 150, 255));
   palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
   setPalette(palette);
}

void GLDPlainTextEdit::setFouseIsKeepInLineEdit(bool isAddFouseSetting)
{
    m_isFouseKeepInLineEdit = isAddFouseSetting;
}

GLDPlainTextEditEx::GLDPlainTextEditEx(QWidget *parent) :
    GLDPlainTextEdit(parent)
{
    this->setStyleSheet(loadQssFile(c_sPlainTextEditQssFile));
}
