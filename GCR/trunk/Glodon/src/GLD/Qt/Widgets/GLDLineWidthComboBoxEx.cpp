/****
 * @file   : GLDLineWidthComboBoxEx.cpp
 * @brief  :
 *
 * @date   : 2014-08-18
 * @author : lijl-c
 * @remarks:
 ****/
#include <QStandardItem>
#include <QListWidget>
#include <QDateTime>
#include <QTableView>
#include <QPainter>
#include <QPen>
#include <QHeaderView>
#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QAction>
#include <QtCore>
#include <QResizeEvent>
#include <QLineEdit>

#include "GLDShadow.h"
#include "GLDStrings.h"
#include "GLDFileUtils.h"
#include "GLDScrollStyle.h"
#include "GLDLineWidthComboBoxEx.h"

const float c_pxToPt                         = (float)96 / 72; /*像素转换为磅*/
const float c_lineWidthChangeFactor          = 0.25;
const int c_LineWidthComboBoxWidth           = 219; /*O设置宽度 */
const int c_LineWidthComboBoxHight           = 25; /*高度*/
//const int c_LineWidthButtonWidth             = 147;   /*宽度*/
const int c_LinePaddingButton                = 5;  /*线条距离button的边距*/
const int c_ButtonPaddingTop                 = 12; /*button上部内边框像素*/
const int c_LabelPtWidth                     = 85;/*磅数label的宽度*/
const int c_buttonFontSize                   = 12;/*字体大小*/

const GString c_sLineWidthComboBoxQssFile = ":/qsses/GLDLineWidthComboBoxEx.qss";

/**
 * @brief
 *
 * @fn GLDLineWidthComboBoxEx::GLDLineWidthComboBoxEx
 * @param parent
 */
GLDLineWidthComboBoxEx::GLDLineWidthComboBoxEx(QWidget *parent) :
    QComboBox(parent),
    m_minLineCount(1),
    m_maxLineCount(25),
    m_lineWidthPopupHight(212),
    m_hasBorder(true),
    m_curIndex(0),
    m_curLineWidth(0)
{
    initPopUp();

    setModel(m_popupListWidgets->model());
    setView(m_popupListWidgets);

    QWidget *parentContainer = m_popupListWidgets->parentWidget();
    if (NULL != parentContainer)
    {
        parentContainer->setFixedWidth(m_popupListWidgets->width());
    }

    setLineEdit(new QLineEdit(this));
    lineEdit()->setReadOnly(true);

    setObjectName("GLDLineWidthComboBoxEx");
    setHasBorder(true);

    GLDShadow *pGLDShadow = new GLDShadow(this);
    pGLDShadow->removeShadow();

    setStyle(new GLDScrollStyle(this));
    this->setStyleSheet(loadQssFile(c_sLineWidthComboBoxQssFile));

    setEditText(getGLDi18nStr(g_rsNoLine));
}

/**
 * @brief
 *
 * @fn GLDLineWidthComboBoxEx::~GLDLineWidthComboBoxEx
 */
GLDLineWidthComboBoxEx::~GLDLineWidthComboBoxEx()
{
}

void GLDLineWidthComboBoxEx::setHasBorder(bool hasBorder)
{
    if (m_hasBorder != hasBorder)
    {
        m_hasBorder = hasBorder;
    }

    if (!m_hasBorder)
    {
        setProperty("GLDBorderStyle", "noBorder");
    }
    this->setStyleSheet(loadQssFile(c_sLineWidthComboBoxQssFile));
}

/**
 * @brief
 *
 * @fn GLDLineWidthComboBoxEx::initModel
 */
void GLDLineWidthComboBoxEx::initPopUp()
{
    m_popupListWidgets = new QListWidget(this);
    m_popupListWidgets->setProperty("GLDPopupListWidgets", true);
    m_popupListWidgets->setFixedWidth(c_LineWidthComboBoxWidth * 2);

    initLineWidth();
    initMoreLinesButton();
}

void GLDLineWidthComboBoxEx::initLineWidth()
{
    QListWidgetItem *pLineWidthItem = new QListWidgetItem(m_popupListWidgets);
    pLineWidthItem->setSizeHint(QSize(c_LineWidthComboBoxWidth, m_lineWidthPopupHight));

    QListWidget *pLineWidthWidget = new QListWidget(this);
    pLineWidthWidget->setProperty("GLDLineWidgets", true);
    createLineWidthRowItem(pLineWidthWidget);

    m_popupListWidgets->setItemWidget(pLineWidthItem, pLineWidthWidget);
    m_popupListWidgets->addItem(pLineWidthItem);
}

void GLDLineWidthComboBoxEx::initMoreLinesButton()
{
    QListWidgetItem *pItemMore = new QListWidgetItem(m_popupListWidgets);

    QListWidget *pMoreWidget = new QListWidget(this);
    pMoreWidget->setProperty("GLDMoreWidget", true);

    createMoreLinesItem(pMoreWidget);

    m_popupListWidgets->setItemWidget(pItemMore, pMoreWidget);
    m_popupListWidgets->addItem(pItemMore);
}

/**
 * @brief
 *
 * @fn GLDLineWidthComboBoxEx::createLineWidthRow
 * @param index
 * @return QWidget
 */
QWidget* GLDLineWidthComboBoxEx::createLineWidthRow(int index)
{
    GLDLineWidthRow *pLineWidthRow = new GLDLineWidthRow(this, index);

    connect(pLineWidthRow, SIGNAL(mapped(int)), this, SLOT(onClicked(int)));

    return pLineWidthRow;
}

/**
 * @brief
 *
 * @fn GLDLineWidthComboBoxEx::createLineWidthRowItem
 * @param pLineWidthItem
 */
void GLDLineWidthComboBoxEx::createLineWidthRowItem(QListWidget *pLineWidthItem)
{
    for (int i = 0; i< m_maxLineCount - 1; ++i)
    {
        QWidget *pLineWidthRow = createLineWidthRow(i);
        QListWidgetItem *pLineWidgetItem = new QListWidgetItem(pLineWidthItem);
        pLineWidthItem->setItemWidget(pLineWidgetItem, pLineWidthRow);
        pLineWidthItem->addItem(pLineWidgetItem);
    }
}

void GLDLineWidthComboBoxEx::createMoreLinesItem(QListWidget *pMoreItem)
{
    QWidget *pMoreWidget = new QWidget(this);
    QHBoxLayout *pLayout = new QHBoxLayout(this);
    // 设置外边距
    pLayout->setContentsMargins(0, 0, 0, 0);
    pMoreWidget->setLayout(pLayout);

    QPushButton *pMoreButton = new QPushButton(getGLDi18nStr(g_rsOtherLine));
    pMoreButton->setProperty("GLDMoreButton", true);
    pLayout->addWidget(pMoreButton);
    connect(pMoreButton, SIGNAL(clicked()), this, SLOT(onClicked()));

    QListWidgetItem *pMoreItemChild = new QListWidgetItem(pMoreItem);
    pMoreItem->setItemWidget(pMoreItemChild, pMoreWidget);
    pMoreItem->addItem(pMoreItemChild);
}

/**
 * @brief
 *
 * @fn GLDLineWidthComboBoxEx::onClicked
 * @param index
 */
void GLDLineWidthComboBoxEx::onClicked(int index)
{
    m_curIndex = index;
    if (m_curIndex == 0)
    {
        setCurLineWidth(0);
        setEditText(getGLDi18nStr(g_rsNoLine));
    }
    else
    {
        setCurLineWidth(m_curIndex * c_lineWidthChangeFactor);
        setEditText(QString("%0 ").arg(m_curIndex * c_lineWidthChangeFactor) + getGLDi18nStr(g_rsLineWidth));
    }
    hidePopup();

    emit itemClicked(m_curIndex);
}

void GLDLineWidthComboBoxEx::onClicked()
{
    QMessageBox::warning(this, "MoreLines", QString("more lines"));
}

/**
  * GLDLineWidthButton
*/
GLDLineWidthButton::GLDLineWidthButton(qreal lineWeight, QWidget *parent):
    QPushButton(parent),
    m_lineWeigth(lineWeight)
{
    setProperty("GLDLineWidthButton", true);
}

void GLDLineWidthButton::resizeEvent(QResizeEvent *e)
{
    drawLineWidth(m_lineWeigth, e->size().width());
}

void GLDLineWidthButton::drawLineWidth(qreal lineWeight, int lineLength)
{
    QPixmap pix(lineLength, c_LineWidthComboBoxHight);
    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    QPen pen = painter.pen();

    QIcon icon;

    if (lineWeight != 0)
    {
        pen.setWidthF(lineWeight);
        pen.setColor(Qt::black);
        painter.setPen(pen);
        //padding-top为12px 高度要改变否则线条不在button中间
        //笔变粗之后会自动扩展，每次画的时候起点要向右移动笔半径的距离
        int nY = (geometry().height() - c_ButtonPaddingTop) / 2;
        QLineF lineF(c_LinePaddingButton + lineWeight / 2, nY,
                     lineLength - c_LinePaddingButton - lineWeight / 2, nY);
        painter.drawLine(lineF);

        icon.addPixmap(pix);
        setIcon(icon);
        setIconSize(QSize(lineLength , geometry().height()));
    }
    else
    {
        pen.setWidth(c_buttonFontSize);
        pen.setColor("#44515a");
        painter.setPen(pen);
        painter.drawText(QPointF(c_LinePaddingButton, c_ButtonPaddingTop - 2), getGLDi18nStr(g_rsNoLine));

        icon.addPixmap(pix);
        setIcon(icon);
        setIconSize(QSize(lineLength , geometry().height()));
    }
}

GLDLineWidthRow::GLDLineWidthRow(QWidget *parent, int rowIndex) :
    QWidget(parent),
    m_label(new QLabel(this)),
    m_rowLayout(new QHBoxLayout(this)),
    m_rowIndex(rowIndex)
{
    m_lineWidthBtn = new GLDLineWidthButton(c_pxToPt * m_rowIndex * c_lineWidthChangeFactor, this);
    m_rowLayout->setSpacing(4);
    m_rowLayout->setContentsMargins(0, 0, 0, 0);
    initRowLabel(m_rowIndex);
    initRowButton(m_rowIndex);
}

void GLDLineWidthRow::initRowLabel(int rowIndex)
{
    if (rowIndex == 0)
    {
        m_label->setPixmap(QPixmap(":/icons/GLDLineWidthEx-noline.png"));
        m_label->setProperty("GLDNoLineLabel", true);
        m_label->setFixedSize(c_LabelPtWidth, c_LineWidthComboBoxHight);
        m_label->setAlignment(Qt::AlignRight);

        m_rowLayout->addWidget(m_label);
    }
    else
    {
        m_label->setText(QString("%0 ").arg(rowIndex * c_lineWidthChangeFactor) + getGLDi18nStr(g_rsLineWidth));
        m_label->setProperty("GLDPixlLable", true);
        m_label->setFixedSize(c_LabelPtWidth, c_LineWidthComboBoxHight);
        m_label->setAlignment(Qt::AlignRight);

        m_rowLayout->addWidget(m_label);
    }
}

void GLDLineWidthRow::initRowButton(int rowIndex)
{
    QSignalMapper *pLineMapper = new QSignalMapper(this);

    if (rowIndex == 0)
    {
        m_lineWidthBtn->setProperty("GLDNoLineButton", true);

        m_rowLayout->addWidget(m_lineWidthBtn);

        pLineMapper->setMapping(m_lineWidthBtn, rowIndex);
        connect(m_lineWidthBtn, SIGNAL(clicked()), pLineMapper, SLOT(map()));
    }
    else
    {
        m_rowLayout->addWidget(m_lineWidthBtn);

        pLineMapper->setMapping(m_lineWidthBtn, rowIndex);
        connect(m_lineWidthBtn, SIGNAL(clicked()), pLineMapper, SLOT(map()));
    }

    connect(pLineMapper, SIGNAL(mapped(int)), this, SIGNAL(mapped(int)));
}
