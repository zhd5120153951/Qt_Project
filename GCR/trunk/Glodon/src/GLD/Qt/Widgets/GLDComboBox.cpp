#include "GLDComboBox.h"

#include <QComboBox>
#include <QAbstractItemView>
#include <QTreeView>
#include <QPainter>
#include <QKeyEvent>
#include <QListView>
#include <QCompleter>
#include <QCoreApplication>
#include <QClipboard>

#include "GLDShadow.h"
#include "GLDFileUtils.h"
#include "GLDScrollStyle.h"

const int c_maxVisibleItems = 6;

const GString c_sCustomComBoxQssFile = ":/qsses/GLDCustomComboBoxEx.qss";

/* GLDCustomComboBox */
GLDCustomComboBox::GLDCustomComboBox(QWidget *parent) : QComboBox(parent), m_isReadOnly(false)
{
    GLDComboBoxLineEdit *lineEdit = new GLDComboBoxLineEdit(this);
    setLineEdit(lineEdit);

    connect(lineEdit, SIGNAL(cursorPositionChanged(int, int)),
            this, SIGNAL(cursorPositionChanged()));
    connect(lineEdit, SIGNAL(selectionChanged()),
            this, SIGNAL(selectionChanged()));
}

void GLDCustomComboBox::showPopup()
{
    resizeContent();
    QComboBox::showPopup();
    QTreeView *treeView = dynamic_cast<QTreeView *>(view());

    if (NULL != treeView)
    {
        connect(treeView, SIGNAL(expanded(QModelIndex)), this, SLOT(onExpandOrCollapsed(QModelIndex)));
        connect(treeView, SIGNAL(collapsed(QModelIndex)), this, SLOT(onExpandOrCollapsed(QModelIndex)));
    }
}

void GLDCustomComboBox::hidePopup()
{
    QComboBox::hidePopup();
    QTreeView *treeView = dynamic_cast<QTreeView *>(view());

    if (NULL != treeView)
    {
        treeView->sizeHint();
        disconnect(treeView, SIGNAL(expanded(QModelIndex)), this, SLOT(onExpandOrCollapsed(QModelIndex)));
        disconnect(treeView, SIGNAL(collapsed(QModelIndex)), this, SLOT(onExpandOrCollapsed(QModelIndex)));
    }
}

void GLDCustomComboBox::updateEditFieldGeometry()
{
    QStyleOptionComboBox optCombo;
    optCombo.init(this);
    optCombo.editable = true;
    optCombo.subControls = QStyle::SC_ComboBoxFrame | QStyle::SC_ComboBoxEditField;
    {
        optCombo.subControls |= QStyle::SC_ComboBoxArrow;
        QRect rect = style()->subControlRect(QStyle::CC_ComboBox, &optCombo, QStyle::SC_ComboBoxEditField, this);
#ifdef __APPLE__
#else
        rect = rect.adjusted(-2, -2, 2, 2);
#endif

        if (lineEdit() != NULL)
        {
            lineEdit()->setGeometry(rect);
        }
    }
}

bool GLDCustomComboBox::isReadOnly()
{
    if (NULL != lineEdit())
    {
        return lineEdit()->isReadOnly();
    }
    else
    {
        return false;
    }
}

void GLDCustomComboBox::setReadOnly(bool value)
{
    if (value != m_isReadOnly && lineEdit() != NULL)
    {
        m_isReadOnly = value;
        GLDComboBoxLineEdit *gldLineEdit = dynamic_cast<GLDComboBoxLineEdit *>(lineEdit());

        if (NULL != gldLineEdit)
        {
            gldLineEdit->setReadOnly(value);
        }
    }
}

bool GLDCustomComboBox::hasSelectedText()
{
    return lineEdit()->hasSelectedText();
}

bool GLDCustomComboBox::canDelete()
{
    if (m_isReadOnly)
    {
        GLDComboBoxLineEdit *gldLineEdit = dynamic_cast<GLDComboBoxLineEdit *>(lineEdit());

        if (NULL != gldLineEdit)
        {
            return gldLineEdit->canDelete();
        }
    }

    return true;
}

void GLDCustomComboBox::setCanDelete(bool value)
{
    if (m_isReadOnly)
    {
        GLDComboBoxLineEdit *gldLineEdit = dynamic_cast<GLDComboBoxLineEdit *>(lineEdit());

        if (NULL != gldLineEdit)
        {
            gldLineEdit->setCanDelete(value);
        }
    }
}

void GLDCustomComboBox::setCurrentIndexAndEmitSignal(int index)
{
    emit onSetCurrentIndex(index);
    QComboBox::setCurrentIndex(index);
}

void GLDCustomComboBox::resizeContent()
{
    QTreeView *treeView = dynamic_cast<QTreeView *>(view());
    int nFitWidth = this->sizeHint().width();

    if (NULL != treeView)
    {
        nFitWidth = qMax(nFitWidth, treeView->sizeHint().width());
    }

    int nWidth = (nFitWidth > width()) ? nFitWidth : width();

    //修正字符为全角时，字符显示不全
    nWidth += 4;

    //如果为无边框，加上左右边框大小
    if (property("borderStyle") == "noBorder")
    {
        nWidth += 2;
    }

    if (QWidget *popup = this->view()->window())
    {
        popup->setMinimumWidth(nWidth);
        // max width = 1000
        popup->setMaximumWidth(1000);
    }
}

void GLDCustomComboBox::setAlignment(Qt::Alignment align)
{
    lineEdit()->setAlignment(align);
}

void GLDCustomComboBox::onExpandOrCollapsed(const QModelIndex &index)
{
    resizeContent();
    Q_UNUSED(index);
}

void GLDCustomComboBox::doCursorPositionChanged(int, int)
{
    emit cursorPositionChanged();
}

void GLDCustomComboBox::doSelectionChanged()
{
    emit selectionChanged();
}

void GLDCustomComboBox::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    updateEditFieldGeometry();
}

GLDCustomComboBoxEx::GLDCustomComboBoxEx(QWidget *parent) :
    GLDCustomComboBox(parent),
    m_hasBorder(true),
    m_isShow(false)
{
    setView(new QListView(this));

    // Qss整体美化
    setMaxVisibleItems(c_maxVisibleItems);
    setHasBorder(true);
    this->setStyleSheet(loadQssFile(c_sCustomComBoxQssFile));

    // 移除阴影
    GLDShadow *pShadow = new GLDShadow(this);
    pShadow->removeShadow();

    // 滚动条美化
    setStyle(new GLDScrollStyle(this));

    this->lineEdit()->installEventFilter(this);
}

void GLDCustomComboBoxEx::setFilterPopupStyleSheet()
{
    if (NULL != completer() && NULL != completer()->popup())
    {
        completer()->popup()->setStyle(new GLDScrollStyle(this));
        // 加载QSS设置过滤弹出框的样式
        completer()->popup()->setStyleSheet("\
                                            QAbstractItemView\
                                            {\
                                                color: #6f6f6f;\
                                                background-color: #fff;\
                                                border: 1px solid #39a9d1;\
                                                outline: 0px;\
                                            }"
                                            );
        completer()->popup()->setItemDelegate(new FilterPopupItemDelegate(completer()->popup()));
        // 根据内容自适应长度
        QTreeView *treeViewPopup = dynamic_cast<QTreeView *>(completer()->popup());
        int nHintWidth = sizeHint().width();

        if (NULL != treeViewPopup)
        {
            nHintWidth = qMax(nHintWidth, treeViewPopup->sizeHint().width());
        }

        int nFitWidth = (nHintWidth > width()) ? nHintWidth : width();

        if (QWidget *popup = completer()->popup()->window())
        {
            popup->setFixedWidth(nFitWidth);
        }
    }
}

void GLDCustomComboBoxEx::resizeEvent(QResizeEvent *e)
{
    QStyleOptionComboBox optCombo;
    optCombo.init(this);
    optCombo.editable = true;
    optCombo.subControls = QStyle::SC_ComboBoxFrame | QStyle::SC_ComboBoxEditField;
    {
        optCombo.subControls |= QStyle::SC_ComboBoxArrow;
        QRect rect = style()->subControlRect(QStyle::CC_ComboBox, &optCombo, QStyle::SC_ComboBoxEditField, this);

        if (lineEdit() != NULL)
        {
            lineEdit()->setGeometry(rect);
        }
    }
    Q_UNUSED(e);
}

bool GLDCustomComboBoxEx::eventFilter(QObject *target, QEvent *e)
{
    if (this->lineEdit() == target && (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonDblClick))
    {
        QMouseEvent *event = static_cast<QMouseEvent*>(e);

        if (event->buttons() & Qt::LeftButton)
        {
            if (m_isShow)
            {
                hidePopup();
                m_isShow = false;
                return false;
            }
            else
            {
                showPopup();
                m_isShow = true;
                return true;
            }
        }
    }

    return GLDCustomComboBox::eventFilter(target, e);
}

void GLDCustomComboBoxEx::keyPressEvent(QKeyEvent *e)
{
    //过滤弹出框的美化
    setFilterPopupStyleSheet();

    QComboBox::keyPressEvent(e);
}

void GLDCustomComboBoxEx::setHasBorder(bool hasBorder)
{
    if (m_hasBorder != hasBorder)
    {
        m_hasBorder = hasBorder;
    }

    if (!m_hasBorder)
    {
        setProperty("borderStyle", "noBorder");
        this->setStyleSheet(loadQssFile(c_sCustomComBoxQssFile));
    }
}

GLDLineWidthComboBox::GLDLineWidthComboBox(QWidget *parent) : QComboBox(parent)
{
    m_parent = parent;
    m_parentSize = QSize();
    m_minLineWidth = 0;
    m_maxLineWidth = 3;
    m_currentLineWidth = m_minLineWidth;
    m_inAddItem = false;

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

void GLDLineWidthComboBox::setMinLineWidth(int minLineWidth)
{
    if (m_minLineWidth != minLineWidth)
    {
        m_minLineWidth = minLineWidth;
        initLineWidthIcon();
    }
}

void GLDLineWidthComboBox::setMaxLineWidth(int maxLineWidth)
{
    if (m_maxLineWidth != maxLineWidth)
    {
        m_maxLineWidth = maxLineWidth;
        initLineWidthIcon();
    }
}

void GLDLineWidthComboBox::setCurrentLineWidth(int lineWidth)
{
    setCurrentIndex(lineWidth - m_minLineWidth);
    m_currentLineWidth = lineWidth;
}

void GLDLineWidthComboBox::resizeEvent(QResizeEvent *)
{
    if (m_parent->size() != m_parentSize)
    {
        initLineWidthIcon();
        m_parentSize = m_parent->size();
    }
}

void GLDLineWidthComboBox::onCurrentIndexChanged(int index)
{
    if (!m_inAddItem && index != -1)
    {
        m_currentLineWidth = m_minLineWidth + index;
    }
}

void GLDLineWidthComboBox::initLineWidthIcon()
{
    setIconSize(QSize(width() - 70, 24));
    clear();
    m_inAddItem = true;

    for (int i = m_minLineWidth; i <= m_maxLineWidth; i++)
    {
        QPixmap pix(QSize(width() - 70, 24));
        pix.fill(Qt::transparent);

        QPainter painter(&pix);

        QPen pen = painter.pen();
        pen.setColor(Qt::black);
        pen.setWidth(i);
        painter.setPen(pen);

        if (i != 0)
        {
            painter.drawLine(0, pix.height() / 2, pix.width() , pix.height() / 2);
        }

        addItem(QIcon(pix), tr("pixel:%1").arg(i)); //TRANS_STRING("像素:%1" ).arg(i));
        setItemData(i - m_minLineWidth, i);
    }

    m_inAddItem = false;
    setCurrentLineWidth(m_currentLineWidth);
}

GLDComboBoxLineEdit::GLDComboBoxLineEdit(QWidget *parent) :
    QLineEdit(parent), m_canDelete(false)
{
}

bool GLDComboBoxLineEdit::canDelete()
{
    if (isReadOnly())
    {
        return m_canDelete;
    }
    else
    {
        return true;
    }
}

void GLDComboBoxLineEdit::setCanDelete(bool value)
{
    if (isReadOnly())
    {
        m_canDelete = value;
    }
}

void GLDComboBoxLineEdit::deleteSelectedText()
{
    if (hasSelectedText())
    {
        this->del();
    }
}

void GLDComboBoxLineEdit::keyPressEvent(QKeyEvent *keyEvent)
{
    if (isReadOnly() && m_canDelete && (keyEvent->key() == Qt::Key_Backspace || keyEvent->key() == Qt::Key_Delete))
    {
        clear();
        keyEvent->accept();
    }

    QLineEdit::keyPressEvent(keyEvent);
}

void GLDCustomComboBox::cut()
{
    if(false == isReadOnly())
    {
        lineEdit()->cut();
    }
}

void GLDCustomComboBox::paste()
{
    if(false == isReadOnly())
    {
        lineEdit()->paste();
    }

}

void GLDCustomComboBox::copy()
{
    lineEdit()->copy();
}

void GLDCustomComboBox::deleteSelectedText()
{
    if (hasSelectedText())
    {
        lineEdit()->del();
    }
}

FilterPopupItemDelegate::FilterPopupItemDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{
    Q_UNUSED(parent)
}

FilterPopupItemDelegate::~FilterPopupItemDelegate()
{

}

void FilterPopupItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem  view_option(option);

    //添加鼠标滑过的背景色
    if (option.state & QStyle::State_MouseOver)
    {
        painter->fillRect(view_option.rect, QColor(219, 244, 252));
        view_option.palette.setColor(QPalette::HighlightedText, QColor(255, 0, 0));
    }
    QStyledItemDelegate::paint(painter, option, index);
}

QSize FilterPopupItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(25);
    return size;
}
