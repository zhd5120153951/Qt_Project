#include <QTimer>
#include <qmath.h>
#include <QPainter>
#include <QTreeView>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QApplication>
#include <QDateTimeEdit>
#include <QFontComboBox>
#include <QPlainTextEdit>
#include <QCalendarWidget>
#include <QItemEditorFactory>
#include <private/qobject_p.h>
#include <QCompleter>

#include "GLDGlobal.h"
#include "GLDStream.h"
#include "GLDUITypes.h"
#include "GLDSpinBox.h"
#include "GLDEllipsis.h"
#include "GLDStrUtils.h"
#include "GLDTextEdit.h"
#include "GLDFontListEx.h"
#include "GLDTableView.h"
#include "GLDColorList.h"
#include "GLDColorListEx.h"
#include "GLDTreeModel.h"
#include "GLDGroupModel.h"
#include "GLDImageEditor.h"
#include "qabstractitemview.h"
#include "GLDWindowComboBoxEx.h"
#include "GLDStylePaintUtils.h"
#include "GLDAbstractItemModel.h"
#include "TextDocumentRtfOutput.h"
#include "GLDDefaultItemDelegate.h"
#include "GLDDrawSymbol.h"
#include "GLDDateTimeEditEx.h"
#include "GLDLineWidthComboBoxEx.h"
#define QFIXED_MAX (INT_MAX/256)
#define CON_INDENT 10

const int minColorRGB = 0;             // 最小颜色值的十进制表示
const int maxColorRGB = 16777215;      // 最大颜色值的十进制表示

using namespace gld;

class GlodonDefaultItemDelegatePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(GlodonDefaultItemDelegate)

public:
    GlodonDefaultItemDelegatePrivate() : f(0), clipPainting(true) {}

    inline const QItemEditorFactory *editorFactory() const
    {
        return f ? f : QItemEditorFactory::defaultFactory();
    }

    inline QIcon::Mode iconMode(QStyle::State state) const
    {
        if (QStyle::State_Enabled != (state & QStyle::State_Enabled))
        {
            return QIcon::Disabled;
        }

        if (QStyle::State_Selected == (state & QStyle::State_Selected))
        {
            return QIcon::Selected;
        }

        return QIcon::Normal;
    }

    inline QIcon::State iconState(QStyle::State state) const
    {
        return (QStyle::State_Open == (state & QStyle::State_Open)) ? QIcon::On : QIcon::Off;
    }

    inline static QString replaceNewLine(QString text)
    {
        const QChar c_nl = QLatin1Char('\n');

        for (int i = 0; i < text.count(); ++i)
            if (text.at(i) == c_nl)
            {
                text[i] = QChar::LineSeparator;
            }

        return text;
    }

    QItemEditorFactory *f;
    bool clipPainting;
    QSizeF doTextLayout(int lineWidth) const;

    mutable QTextLayout textLayout;
    mutable QTextOption textOption;

    // ### temporary hack until we have QStandardItemDelegate
    mutable struct Icon
    {
        QIcon icon;
        QIcon::Mode mode;
        QIcon::State state;
    } tmp;

    static QString valueToText(const QVariant &value);

    const QWidget *widget(const QStyleOptionViewItem &option) const
    {
        return option.widget;
    }

    int textHMargin(const QVariant &marginData, const QStyleOptionViewItem &option) const
    {
        if (marginData.isValid())
        {
            return (marginData.toInt() & 0x00FF0000) >> 16;
        }
        else
        {
            const QWidget *widget = option.widget;
            QStyle *style = widget ? widget->style() : QApplication::style();
            return style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1;
        }
    }

    inline int textVMargin(const QVariant &marginData) const
    {
        return marginData.isValid() ? (marginData.toInt() & 0x000000FF) / 2 : 0;
    }
};

/* GlodonDefaultItemDelegate */

GlodonDefaultItemDelegate::GlodonDefaultItemDelegate(QObject *parent) :
    QItemDelegate(parent), m_curEditor(NULL), m_pTable(NULL), m_oComboBoxCurIndex(-1), m_factor(1),
    m_closeEditEvent(NULL), m_comboBoxConvenient(true), m_bCloseEditorOnFocusOut(false), m_inCommitData(false),
    m_inSetModelData(false), m_checkSetModelData(true), m_bUseBlendColor(false), m_includingLeading(false),
    m_enterJump(false), m_isTextEllipse(false), m_bRepeatCommit(false), m_bCanHideEidtOnExit(true),
    m_bIgnoreActiveWindowFocusReason(false), m_bUseComboBoxCompleter(true),
    m_wrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere), m_oSelectedCellBackgroundColor()
{
    setClipping(false);
}

void GlodonDefaultItemDelegate::paintDouble(QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    bool bReadOnly = false;
    GEditStyle editstyle = editStyle(dataIndex(index), bReadOnly);

    if (editstyle == esLineWidthList)
    {
        drawBackground(painter, option, index);

        QRect rect = option.rect.adjusted(8, 4, -10, -6);
        painter->save();
        QPen pen = painter->pen();
        pen.setColor(Qt::black);
        pen.setWidthF(m_oDisplayData.toDouble());
        painter->setPen(pen);

        if (m_oDisplayData.toDouble() != 0)
        {
            painter->drawLine(rect.left(), (rect.top() + rect.bottom()) / 2,
                              rect.right(), (rect.top() + rect.bottom()) / 2);
        }

        painter->restore();
        drawFocus(painter, option, option.rect);
    }
    else
    {

        QString strFloatview("");
        emit onQueryFloatOrDoubleViewFormat(index, strFloatview);

        if (strFloatview.length() == 0)
        {
            paintOther(painter, option, index);
            //            QItemDelegate::paint(painter, option, index);
        }
        else
        {
            QPixmap pixmap;
            QVariant ico = index.data(Qt::DecorationRole);
            QRect decorationRect;

            if (ico.isValid())
            {
                pixmap = decoration(option, ico);
                decorationRect = QRect(QPoint(0, 0), pixmap.size());
            }
            else
            {
                decorationRect = QRect();
            }

            QRect displayRect = textRectangle(textLayoutBounds(option), option.font, strFloatview);

            QRect checkRect;
            //Qt::CheckState checkState = Qt::Unchecked;
            m_oDisplayData = index.data(Qt::CheckStateRole);

            if (m_oDisplayData.isValid())
            {
                //checkState = static_cast<Qt::CheckState>(value.toInt());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                checkRect = doCheck(option, option.rect, m_oDisplayData);
#else
                checkRect = check(option, option.rect, value);
#endif
            }

            innerDoLayout(option, &checkRect, &decorationRect, &displayRect, false);

            drawBackground(painter, option, index);

            QImage img = pixmap.toImage();
            img.scaled(decorationRect.size());
            painter->drawImage(decorationRect, img);

            drawDisplay(painter, option, displayRect, strFloatview, index);
            drawFocus(painter, option, displayRect);
        }
    }
}

void GlodonDefaultItemDelegate::paintBool(QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    opt.rect = option.rect;
    opt.state = opt.state & ~QStyle::State_HasFocus;

    bool bReadOnly = false;
    GEditStyle editstyle = editStyle(dataIndex(index), bReadOnly);

    if (editstyle == esNone || bReadOnly)
    {
        opt.state = opt.state & ~QStyle::State_Enabled;
    }

    if (m_oDisplayData.isNull())
    {
        opt.state |= QStyle::State_NoChange;
    }
    else if (m_oDisplayData.toBool())
    {
        opt.state |= QStyle::State_On;
    }
    else
    {
        opt.state |= QStyle::State_Off;
    }

    drawBackground(painter, option, index);

    QPoint center = opt.rect.center();
    QRect checkBox(center.x() - 6, center.y() - 6, 13, 13);

    GlodonTableView *tableView = (GlodonTableView *)(this->parent());

    if (tableView && tableView->isTree() && index.column() == tableView->treeColumn())
    {
        const GTreeViewItem &viewItem = tableView->treeDrawInfo()->m_viewItems.at(index.row());

        if (!((viewItem.parentIndex == -1) && viewItem.hasChildren != 1))
        {
            QRect textRect(opt.rect.left() - CON_INDENT * (viewItem.treeLevel + 1) - 6, opt.rect.top(),
                           opt.rect.width() + CON_INDENT * (viewItem.treeLevel + 1) + 6, opt.rect.height());
            checkBox = QRect(textRect.center().rx() - 6, textRect.center().ry() - 6, 13, 13);
        }
    }

    opt.rect = checkBox;
    //设置“_q_no_animation”值是因为如果把CheckBox的State_Enable去掉时,可能会出现画得不正确
    bool b_q_no_animation = opt.styleObject->property("_q_no_animation").toBool();
    opt.styleObject->setProperty("_q_no_animation", true);

    // 去掉该效果，该效果在ribbon风格下，会出现刷新问题
    opt.state &= ~QStyle::State_MouseOver;

    const QWidget *widget = opt.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();

#ifndef __APPLE__
    style->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &opt, painter, widget);
#else
    style->drawPrimitive(QStyle::PE_IndicatorCheckBox, &opt, painter, widget);
#endif
    opt.styleObject->setProperty("_q_no_animation", b_q_no_animation);

    drawFocus(painter, option, option.rect);
}

void GlodonDefaultItemDelegate::paintImg(QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    QStyleOptionViewItem opt = setOptions(index, option);
    GAspectRatioMode gRatioMode = KeepAspectPixel;
    emit onQueryImageAspectRatioMode(index, gRatioMode);
    drawBackground(painter, opt, index);
    drawFocus(painter, opt, opt.rect);

    QPoint topLeft = opt.rect.topLeft();
    QImage img = m_oDisplayData.value<QImage>();
    topLeft.setY(topLeft.y() + opt.rect.height() / 2 - img.height() / 2);

    if (opt.displayAlignment.testFlag(Qt::AlignLeft))
    {
        topLeft.setX(topLeft.x() + 1);
    }
    else if (opt.displayAlignment.testFlag(Qt::AlignRight))
    {
        topLeft.setX(opt.rect.right() - img.width());
    }
    else
    {
        topLeft.setX(topLeft.x() + opt.rect.width() / 2 - img.width() / 2);
    }

    if (gRatioMode == KeepAspectRatio || gRatioMode == KeepAspectRatioByExpanding)
    {
        QImage result = img.scaled(opt.rect.width() - 2, opt.rect.height() - 2, (Qt::AspectRatioMode)gRatioMode);

        topLeft.setX(topLeft.x() + 1);
        painter->drawImage(topLeft, result);
    }
    else if (gRatioMode == IgnoreAspectRatio)
    {
        QRect source = img.rect();
        QRect target = QRect(opt.rect.left() - 1, opt.rect.top() - 1,
                             opt.rect.width() - 2, opt.rect.height() - 2);
        painter->drawImage(target, img, source);
    }
    else
    {
        painter->drawImage(topLeft, img);
    }
}

void GlodonDefaultItemDelegate::paintColor(QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    drawBackground(painter, option, index);

    QRect rect = option.rect.adjusted(8, 4, -10, -6);
    QPixmap pix(rect.size());
    pix.fill(m_oDisplayData.value<QColor>());
    painter->drawPixmap(rect, pix);
    painter->drawRect(rect);

    drawFocus(painter, option, option.rect);
}

void GlodonDefaultItemDelegate::paintGType(QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index,
        GlodonDefaultItemDelegate::GDataType type) const
{
    QStyleOptionViewItem opt = setOptions(index, option);
    opt.rect = option.rect;

    QPixmap pixmap;
    QVariant ico = index.data(Qt::DecorationRole);
    QRect decorationRect;

    if (ico.isValid())
    {
        pixmap = decoration(opt, ico);
        decorationRect = QRect(QPoint(0, 0), pixmap.size());
    }
    else
    {
        decorationRect = QRect();
    }

    QRect displayRect = textRectangle(textLayoutBounds(opt), opt.font, m_oDisplayData.toString());
    QRect checkRect;
    QRect focusRect = displayRect;
    innerDoLayout(opt, &checkRect, &decorationRect, &displayRect, false);

    drawBackground(painter, opt, index);

    QVariant margin = index.data(262);

    if (margin.isValid())
    {
        int nMargin = margin.toInt();
        displayRect.adjust((nMargin & 0x00FF0000) >> 16, (nMargin & 0x000000FF) / 2,
                           -(nMargin & 0x00FF0000) >> 16, -(nMargin & 0x000000FF) / 2);
    }

    if (type == GTypeHTML)
    {
        QImage img = pixmap.toImage();
        img.scaled(decorationRect.size());
        painter->drawImage(decorationRect, img);

        QFont font = opt.font;
        font.setUnderline(true);
        painter->save();
        painter->setPen(Qt::blue);
        painter->setFont(font);

        painter->drawText(displayRect, Qt::AlignVCenter, m_oDisplayData.toString());
        painter->restore();
    }
    else if (type == GRichText)
    {
        painter->save();

        //暂时取消对 RichText 的支持，后续需要重构 TODO  zhangjq 2015.09.24
//        RtfReader::Reader reader;
//        GByteArray byteArray(m_oDisplayData.toByteArray());
//        GMemoryStream buffer(&byteArray, false);
//        bool result = reader.open(buffer);

//        if (!result)
//        {
//            qWarning() << "failed to open file: ";
//            return;
//        }

//        painter->translate(displayRect.x(), displayRect.y());
//        QTextDocument rtfDocument;
//        RtfReader::TextDocumentRtfOutput *output = new RtfReader::TextDocumentRtfOutput(&rtfDocument);
//        reader.parseTo(output);
//        rtfDocument.drawContents(painter);
//        freeAndNil(output)
        painter->restore();

    }
    else if (type == GDiaSymbol)
    {
        painter->save();
        painter->setFont(opt.font);
        painter->setPen(opt.palette.color(QPalette::Text));
        drawDiaSymbol(painter, m_factor * 100, QString(m_oDisplayData.toString()), opt.displayAlignment, displayRect);
        painter->restore();
    }
    else if (type == GMeterSymbol)
    {
        painter->save();
        painter->setFont(opt.font);
        drawTextMx(painter, m_oDisplayData.toString(), opt.displayAlignment, displayRect);
        painter->restore();
    }

    drawFocus(painter, opt, focusRect);
}

void GlodonDefaultItemDelegate::paint(QPainter *painter,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    //for recrod,because the QItemDelegate::paint will use another QStyleOptionViewItem obj
    m_oDisplayData = index.model()->data(index, Qt::DisplayRole);
    //bool readOnly = false;
    QVariant::Type valType = m_oDisplayData.type();

    if (valType == QVariant::Bool)
    {
        paintBool(painter, option, index);
    }
    else if (valType == QVariant::Image)
    {
        paintImg(painter, option, index);
    }
    else if (valType == QVariant::Int)
    {
        paintOther(painter, option, index);
    }
    else if (valType == QVariant::Double)
    {
        paintDouble(painter, option, index);
    }
    else if (valType == QVariant::Color)
    {
        paintColor(painter, option , index);
    }
    else
    {
        GDataType type = GTypeNormal;

        emit onQueryIndexDataType(index, type);

        if (type == GTypeHTML || type == GRichText || type == GDiaSymbol || type == GMeterSymbol)
        {
            paintGType(painter, option, index, type);

            return;
        }

        paintOther(painter, option, index); // QItemDelegate::paint(painter, option, index);
    }
}

void GlodonDefaultItemDelegate::adjustRectByTextMargin(
        const QModelIndex &index, const QStyleOptionViewItem &opt, bool isIncludeMargin, QRect &rect) const
{
    Q_D(const GlodonDefaultItemDelegate);

    QVariant oTextMargin = index.data(gidrCellMargin);

    int nTextHMargin = d->textHMargin(oTextMargin, opt);
    int nTextVMargin = d->textVMargin(oTextMargin);

    if (isIncludeMargin)
    {
        rect.adjust(-nTextHMargin, -nTextVMargin, nTextHMargin, nTextVMargin);
    }
    else
    {
        rect.adjust(nTextHMargin, nTextVMargin, -nTextHMargin, -nTextVMargin);
    }
}

QSize GlodonDefaultItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::SizeHintRole);
    if (value.isValid())
        return qvariant_cast<QSize>(value);

    QStyleOptionViewItem opt = option;

    // 如果option的rect是有效时，计算textRect会把option的宽度作为固定宽度，从而计算高度
    // 见tableView的sizeHintForRow时，就是这么特殊处理的。此时，需要先把margin剪掉
    if (option.rect.isValid())
    {
        adjustRectByTextMargin(index, opt, false, opt.rect);
    }

    QRect decorationRect = calcRectByData(opt, index, Qt::DecorationRole);
    QRect displayRect = calcRectByData(opt, index, Qt::DisplayRole);
    QRect checkRect = calcRectByData(opt, index, Qt::CheckStateRole);

    innerDoLayout(option, &checkRect, &decorationRect, &displayRect, true);

    QRect oSizeHint = decorationRect|displayRect|checkRect;

    // 如果rect无效，则说明是在计算自动列宽，此时在计算完之后，需要把margin给加上，因为计算时没有包含margin
    // 而显示的时候，是包含了margin的
    if (!option.rect.isValid())
    {
        adjustRectByTextMargin(index, opt, true, oSizeHint);
    }

    return oSizeHint.size();
}

QWidget *GlodonDefaultItemDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    return const_cast<GlodonDefaultItemDelegate *>(this)->doCreateEditor(parent, option, index);
}

void GlodonDefaultItemDelegate::createTreeViewEditor(QWidget *parent, QModelIndex &dIndex, bool bReadOnly)
{
    GLDCustomComboBoxEx *comboBox = new GLDCustomComboBoxEx(parent);

    m_curEditor = comboBox;

    comboBox->setHasBorder(false);
    comboBox->setReadOnly(bReadOnly);

    initComboBoxCompleter(comboBox, dIndex);
    initComboBox(comboBox, dIndex);

    if (m_comboBoxConvenient)
    {
        connect(comboBox, SIGNAL(activated(int)), this, SLOT(doComboBoxActivated(int)));
        connect(comboBox, SIGNAL(onSetCurrentIndex(int)), this, SLOT(doComboBoxActivated(int)));
    }
}

void GlodonDefaultItemDelegate::createLineEditEditor(QWidget *parent, bool bReadOnly)
{
    GLDCustomLineEdit *lineEdit = new GLDCustomLineEdit(parent);

    m_curEditor = lineEdit;

    lineEdit->setHasBorder(false);
    lineEdit->setReadOnly(bReadOnly);
}

void GlodonDefaultItemDelegate::createDropDownWindowEditor(QWidget *parent, QModelIndex &dIndex,
        bool bReadOnly, GEditStyle eEditStyle)
{
    GLDWindowComboBoxEx *comboBox = new GLDWindowComboBoxEx(parent);

    m_curEditor = comboBox;

    if (eEditStyle == esDropDownWindowEllipsis)
    {
        comboBox->setButtonTypes(GLDWindowComboBoxEx::Ellipsis);
    }
    else if (eEditStyle == esDropDownWindowNone)
    {
        comboBox->setButtonTypes(GLDWindowComboBoxEx::None);
    }

    comboBox->getLineEdit()->setFrameShape(QFrame::NoFrame);
    comboBox->setHasBorder(false);
    comboBox->setEditable(!bReadOnly);
    initWindowComboBox(comboBox, dIndex);
    comboBox->setComboBoxPopupOffset(-2, 3);

    if (m_comboBoxConvenient)
    {
        connect(comboBox, SIGNAL(onManualColsePopup()), this, SIGNAL(onCommitDataAndCloseEditor()));
    }
}

void GlodonDefaultItemDelegate::createMonthCalendarEditor(QWidget *parent, QModelIndex &dIndex, QVariant::Type vType)
{
    QVariant value = dIndex.model()->data(dIndex, Qt::EditRole);
    QDateTime dt = QDateTime::currentDateTime();
    GLDDateTimeEditEx *pDateTimeEditEx = NULL;

    switch (vType)
    {
        case QVariant::Time:
            dt.setTime(value.value<QTime>());
            pDateTimeEditEx = new GLDDateTimeEditEx(dt, parent);
            pDateTimeEditEx->setCalendarPopup(false);
            pDateTimeEditEx->setDisplayFormat("hh:mm:ss");
            break;

        case QVariant::Date:
            dt.setDate(value.value<QDate>());
            pDateTimeEditEx = new GLDDateTimeEditEx(dt, parent);
            pDateTimeEditEx->setCalendarPopup(true);
            pDateTimeEditEx->setDisplayFormat("yyyy-MM-dd");
            pDateTimeEditEx->setCalendarPopupOffset(-2, 3);
            break;

        default:
            dt = value.value<QDateTime>();
            pDateTimeEditEx = new GLDDateTimeEditEx(dt, parent);
            pDateTimeEditEx->setCalendarPopup(true);
            pDateTimeEditEx->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
            pDateTimeEditEx->setCalendarPopupOffset(-2, 3);
            break;
    }

    m_curEditor = pDateTimeEditEx;
    pDateTimeEditEx->setHasBorder(false);
    initDateTimeEdit(pDateTimeEditEx, dIndex);
}

void GlodonDefaultItemDelegate::createColorListEditor(QWidget *parent, QModelIndex &dIndex, bool bReadOnly)
{
    QVariant value = dIndex.model()->data(dIndex, Qt::EditRole);
    QColor color = value.value<QColor>();

    GColorListEx *colorListEx = new GColorListEx(parent, color);

    m_curEditor = colorListEx;
    colorListEx->setHasBorder(false);

    if(!bReadOnly)
    {
        colorListEx->setEditable(!bReadOnly);
        colorListEx->setIsShowRGBStr(!bReadOnly);

        QIntValidator* pIntValidator = new QIntValidator(colorListEx->lineEdit());
        pIntValidator->setRange(minColorRGB, maxColorRGB);
        colorListEx->lineEdit()->setValidator(pIntValidator);
    }

    if (m_comboBoxConvenient)
    {
        connect(colorListEx, SIGNAL(colorBlockClicked(int)), this, SLOT(doComboBoxActivated(int)));
    }
}

void GlodonDefaultItemDelegate::createFontListEditor(QWidget *parent)
{
    GFontListEx *comboBox = new GFontListEx(parent);//new QFontComboBox(parent);

    m_curEditor = comboBox;

    comboBox->setHasBorder(false);

    if (m_comboBoxConvenient)
    {
        connect(comboBox, SIGNAL(activated(int)), this, SLOT(doComboBoxActivated(int)));
    }
}

void GlodonDefaultItemDelegate::createLineWidthEditor(QModelIndex &dIndex, QWidget *parent)
{
    GLDLineWidthComboBoxEx *lineWidthComboBox = new GLDLineWidthComboBoxEx(parent);

    m_curEditor = lineWidthComboBox;

    lineWidthComboBox->setHasBorder(false);

    if (m_comboBoxConvenient)
    {
        connect(lineWidthComboBox, SIGNAL(itemClicked(int)), lineWidthComboBox, SIGNAL(activated(int)));// 连接此信号与槽后, 选中后即可退出编辑状态
        connect(lineWidthComboBox, SIGNAL(activated(int)), this, SLOT(doComboBoxActivated(int)));
    }

    initLineWidthComboBox(lineWidthComboBox, dIndex);
}

QWidget *GlodonDefaultItemDelegate::doCreateEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index)
{
    QModelIndex dIndex = dataIndex(index);

    if (!editable(dIndex))
    {
        m_curEditor = NULL;

        return m_curEditor;
    }

    m_currTreeEditing = index;

    bool bReadOnly = false;
    bool bNumberRightAlign = false;

    GEditStyle eEditStyle = editStyle(dIndex, bReadOnly);
    QVariant::Type vType = dIndex.model()->data(dIndex, Qt::EditRole).type();

    if (esSimple == eEditStyle)
    {
        eEditStyle = editStyleByVariantType(vType);
    }

    QFont font = zoomFont(dIndex, QFont(), m_factor);

    switch (eEditStyle)
    {
        case esNone:
            m_curEditor = NULL;
            break;

        case esLineEdit:
        {
            createLineEditEditor(parent, bReadOnly);
            break;
        }

        case esImage:
            m_curEditor = new GImageEditor(parent);
            break;

        case esDropDown:
        case esTreeView:
        {
            createTreeViewEditor(parent, dIndex, bReadOnly);
            break;
        }

        // 修改计价提的BUG，新增可多行编辑的下拉框
        case esDropDownWindowEllipsis:
        case esDropDownWindowNone:
        case esDropDownWindow:
        {
            createDropDownWindowEditor(parent, dIndex, bReadOnly, eEditStyle);
            break;
        }

        case esMonthCalendar:
        {
            createMonthCalendarEditor(parent, dIndex, vType);
            break;
        }

        case esColorList:
        {
            createColorListEditor(parent, dIndex, bReadOnly);
            break;
        }

        case esFontList:
        {
            createFontListEditor(parent);
            break;
        }

        case esLineWidthList:
        {
            createLineWidthEditor(dIndex, parent);
            break;
        }

        case esPlainEllipsis:
        {
            createEllipsisEditor<GLDPlainButtonEditEx>(parent, dIndex, bReadOnly);
            break;
        }

        case esLineEllipsis:
        {
            createEllipsisEditor<GLDLineButtonEditEx>(parent, dIndex, bReadOnly);
            break;
        }

        case esVectorGraph:
        case esBool:
        {
            m_curEditor = NULL;
            break;
        }

        case esUpDown:
        {
            if (variantTypeIsByte(vType) || variantTypeIsShort(vType)
                    || variantTypeIsInt(vType))
            {
                GLDSpinBoxEx *pIntEdit = new GLDSpinBoxEx(parent);
                pIntEdit->setHasBorder(false);

                if ((NULL != m_pTable) && !m_pTable->alwaysShowEditorPro())
                {
                    pIntEdit->selectAll();
                }

                m_curEditor = pIntEdit;
                break;
            }
            else if (variantTypeIsFloat(vType))
            {
                GLDDoubleSpinBoxEx *pDoubleEdit = new GLDDoubleSpinBoxEx(parent);
                pDoubleEdit->setHasBorder(false);

                if ((NULL != m_pTable) && !m_pTable->alwaysShowEditorPro())
                {
                    pDoubleEdit->selectAll();
                }

                m_curEditor = pDoubleEdit;
                break;
            }
        }

        case esSimple:
        {
            if (QVariant::Bool == vType)
            {
                m_curEditor = NULL;
                break;
            }

            createSpinBox(parent, index, option, vType, bReadOnly, bNumberRightAlign);

            if (QVariant::LongLong == vType)
            {
                QRegExp regExp("\\d*");
                QLineEdit *pLineEdit = new QLineEdit(parent);
                QRegExpValidator *pRegExValidator = new QRegExpValidator(regExp, pLineEdit);
                pLineEdit->setValidator(pRegExValidator);
                m_curEditor = pLineEdit;
            }

            if (m_curEditor != NULL)
            {
                break;
            }
        }

        case esPlainEdit:
        default:
            GLDPlainTextEditEx *plainTextEdit = new GLDPlainTextEditEx(parent);
            plainTextEdit->setFrameShape(QFrame::NoFrame);
            plainTextEdit->setReadOnly(bReadOnly);
            plainTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            plainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            m_curEditor = plainTextEdit;
            break;
    }

    if (m_curEditor != NULL)
    {
        if (!bNumberRightAlign)  //非数字
        {
            QMargins margins = m_curEditor->contentsMargins();
            margins.setTop(0);
            margins.setLeft(0);
            m_curEditor->setContentsMargins(margins);
        }

        m_curEditor->setFont(font);
    }

    return m_curEditor;
}

void GlodonDefaultItemDelegate::createSpinBox(QWidget *parent,
        const QModelIndex &dIndex,
        const QStyleOptionViewItem &option,
        QVariant::Type vType, bool bReadOnly,
        bool &numberRightAlign)
{
    QAbstractSpinBox *pSpinBox = NULL;

    if (variantTypeIsByte(vType) || variantTypeIsShort(vType) || variantTypeIsInt(vType))
    {
        numberRightAlign = true;
        GLDSpinBoxEx *pGLDSpinBoxEx = new GLDSpinBoxEx(parent);
        pGLDSpinBoxEx->setMinimum(MinInt);
        pGLDSpinBoxEx->setMaximum(MaxInt);
        pGLDSpinBoxEx->setHasBorder(false);
        pSpinBox = pGLDSpinBoxEx;
    }
    else if (variantTypeIsFloat(vType))
    {
        numberRightAlign = true;
        GLDDoubleSpinBoxEx *pGLDDoubleSpinBoxEx = new GLDDoubleSpinBoxEx(parent);
        pGLDDoubleSpinBoxEx->setHasBorder(false);
        pSpinBox = pGLDDoubleSpinBoxEx;
    }

    if (NULL != pSpinBox)
    {
        pSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
        Qt::Alignment align = option.displayAlignment & ~Qt::AlignLeft;
        align |= Qt::AlignRight;
        pSpinBox->setAlignment(align);
        pSpinBox->setReadOnly(bReadOnly);
    }

    initSpinBox(pSpinBox, dIndex);

    m_curEditor = pSpinBox;
}

void GlodonDefaultItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (m_inSetModelData && !m_pTable->alwaysShowEditorPro())
    {
        return;
    }

    bool bReadOnly = false;
    QModelIndex dIndex = dataIndex(index);
    QVariant value = dIndex.model()->data(dIndex, Qt::EditRole);

#ifdef _FONTSYMBOL_
    QString displaySymbols = value.toString();
    GlodonSymbols glodonSymbols;
    QString aliases = glodonSymbols.aliasesFromDisplaySymbols(displaySymbols);
    value = QVariant(aliases);
#endif

    m_currTreeEditing = index;

    QVariant::Type vType = value.type();
    GEditStyle eEditStyle = editStyle(dIndex, bReadOnly);

    if (esSimple == eEditStyle)
    {
        eEditStyle = editStyleByVariantType(vType);
    }

    switch (eEditStyle)
    {
        case esNone:
            break;

        case esLineEdit:
        {
            QLineEdit *edit = static_cast<QLineEdit *>(editor);

            if (value.isNull())
            {
                edit->setText("");
            }
            else
            {
                edit->setText(value.toString());
            }

            if ((NULL != m_pTable) && !m_pTable->alwaysShowEditorPro())
            {
                edit->selectAll();
            }

            break;
        }

        case esImage:
        {
            QImage img = value.value<QImage>();
            GImageEditor *imageEditor = static_cast<GImageEditor *>(editor);
            imageEditor->setImage(img);
            break;
        }

        case esDropDown:
        case esTreeView:
        {
            GLDCustomComboBox *comboBox = static_cast<GLDCustomComboBox *>(editor);
            QString sEditText = value.toString();

            if (value.isNull())
            {
                sEditText = "";
            }

            int nIndex = comboBox->findText(sEditText);

            if (nIndex <= -1 && !value.isNull())
            {
                nIndex = comboBox->findData(value);
                m_oComboBoxCurIndex = nIndex;
            }

            if (nIndex > -1)
            {
                comboBox->setCurrentIndex(nIndex);
            }

            if (comboBox->isEditable())
            {
                comboBox->setEditText(sEditText);
            }

            QLineEdit *lineEdit = comboBox->lineEdit();

            if ((NULL != lineEdit) && (NULL != m_pTable) && !m_pTable->alwaysShowEditorPro())
            {
                lineEdit->selectAll();
            }

            break;
        }

        // 修改计价提的BUG，新增可多行编辑的下拉框
        case esDropDownWindowEllipsis:
        case esDropDownWindowNone:
        case esDropDownWindow:
        {
            GLDWindowComboBoxEx *pComboBox = dynamic_cast<GLDWindowComboBoxEx *>(editor);
            if (NULL == pComboBox)
            {
                break;
            }

            if (value.isNull())
            {
                pComboBox->setEditText("");
            }
            else
            {
                pComboBox->setEditText(value.toString());
            }

            if ((NULL != m_pTable) && !m_pTable->alwaysShowEditorPro())
            {
                pComboBox->selectAll();
            }

            break;
        }

        case esMonthCalendar:
        {
            GLDDateTimeEditEx *pDateTimeEditEx = static_cast<GLDDateTimeEditEx *>(editor);

            if (QVariant::Time == vType)
            {
                pDateTimeEditEx->setTime(value.toTime());
            }
            else if (QVariant::Date == vType)
            {
                pDateTimeEditEx->setDate(value.toDate());
            }
            else
            {
                pDateTimeEditEx->setDateTime(value.toDateTime());
            }

            break;
        }

        case esColorList:
        {
            GColorListEx *colorListEx = static_cast<GColorListEx *>(editor);
            QColor color = value.value<QColor>();

            if (colorListEx->lineEdit())
            {
                // 颜色的name函数返回值形如"#FF0000",需截取#后的十六进制值
                colorListEx->lineEdit()->setText(int64ToStr(strToInt64(color.name().mid(1), 16)));
            }

            colorListEx->setCurrentColor(color);
            break;
        }

        case esFontList:
        {
            QFont font = value.value<QFont>();
            GFontListEx *fontComboBox = static_cast<GFontListEx *>(editor);
            fontComboBox->setCurrentFont(font);

            if ((NULL != fontComboBox->lineEdit()) && (NULL != m_pTable) && !m_pTable->alwaysShowEditorPro())
            {
                fontComboBox->lineEdit()->selectAll();
            }

            break;
        }

        case esLineWidthList:
        {
            GLDLineWidthComboBoxEx *lineWidthComboBox = static_cast<GLDLineWidthComboBoxEx *>(editor);
            lineWidthComboBox->setCurLineWidth(value.toDouble());
            break;
        }

        case esPlainEllipsis:
        case esLineEllipsis:
        {
            GLDAbstractButtonEdit *ellipsis = static_cast<GLDAbstractButtonEdit *>(editor);
            ellipsis->setModelIndex(dIndex);

            if (value.isNull())
            {
                ellipsis->setText("");
            }
            else
            {
                ellipsis->setText(value.toString());
            }

            if ((NULL != m_pTable) && !m_pTable->alwaysShowEditorPro())
            {
                ellipsis->selectAll();
            }

            break;
        }

        case esVectorGraph:
        case esBool:
            break;

        case esUpDown:
        case esSimple:
        {
            if (QVariant::Bool == vType)
            {
                break;
            }
            else if (variantTypeIsByte(vType) || variantTypeIsShort(vType)
                     || variantTypeIsInt(vType))
            {

                GLDSpinBoxEx *pIntEdit = static_cast<GLDSpinBoxEx *>(editor);

                if ((NULL != m_pTable) && !m_pTable->alwaysShowEditorPro())
                {
                    pIntEdit->selectAll();
                }

                pIntEdit->setValue(value.toInt());
                break;
            }
            else if (variantTypeIsFloat(vType))
            {
                GLDDoubleSpinBoxEx *pDoubleEdit = static_cast<GLDDoubleSpinBoxEx *>(editor);
                pDoubleEdit->setValue(value.toDouble());

                if ((NULL != m_pTable) && !m_pTable->alwaysShowEditorPro())
                {
                    pDoubleEdit->selectAll();
                }

                break;
            }

            else if (QVariant::LongLong == vType)
            {
                QLineEdit *pLineEdit = static_cast<QLineEdit *>(editor);
                pLineEdit->setText(value.toString());

                if ((NULL != m_pTable) && !m_pTable->alwaysShowEditorPro())
                {
                    pLineEdit->selectAll();
                }

                break;
            }
        }

        case esPlainEdit:
        default:
        {
            QPlainTextEdit *plainTextEdit = static_cast<QPlainTextEdit *>(editor);

            if (value.isNull())
            {
                plainTextEdit->setPlainText("");
            }
            else
            {
                plainTextEdit->setPlainText(value.toString());
            }

            if ((NULL != m_pTable) && !m_pTable->alwaysShowEditorPro())
            {
                plainTextEdit->selectAll();
            }

            break;
        }
    }
}

bool GlodonDefaultItemDelegate::setTreeViewModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    bool result = true;

    GLDCustomComboBox *comboBox = static_cast<GLDCustomComboBox *>(editor);

    int nItemIndex = comboBox->findText(comboBox->currentText());

    if (nItemIndex >= 0)
    {
        QVariant value = comboBox->itemData(nItemIndex);

        if (value.isValid())
        {
            // 先去data中找，如果找到，就直接设置
            result = model->setData(index, value);
        }
        else
        {
            // 否则去text中去找，找到对应的text，就把data设置进去，
            result = model->setData(index, comboBox->itemText(nItemIndex));
        }
    }
    else
    {
        // 如果没找到就直接设置comboBox现在的值
        result = model->setData(index, comboBox->currentText());
    }

    //说明用的是ItemData
    if (m_comboBoxConvenient)
    {
        disconnect(comboBox, SIGNAL(activated(int)), this, SLOT(doComboBoxActivated(int)));
        disconnect(comboBox, SIGNAL(onSetCurrentIndex(int)), this, SLOT(doComboBoxActivated(int)));
    }

    return result;
}

bool GlodonDefaultItemDelegate::setDropDownWindowModelData(QWidget *editor,
        QAbstractItemModel *model,
        const QModelIndex &index) const
{
    bool result = true;
    GLDWindowComboBoxEx *comboBox = static_cast<GLDWindowComboBoxEx *>(editor);

    QString text = comboBox->editText();

#ifdef _FONTSYMBOL_
    QString aliases = text;
    GlodonSymbols glodonSymbols;
    text = glodonSymbols.displaySymbolsFromAliases(aliases);
#endif

    result = model->setData(index, text);

    if (m_comboBoxConvenient)
    {
        disconnect(comboBox, SIGNAL(onManualColsePopup()), this, SIGNAL(onCommitDataAndCloseEditor()));
    }

    return result;
}

bool GlodonDefaultItemDelegate::setMonthCalendarModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index, QVariant::Type vType) const
{
    bool result = true;

    GLDDateTimeEditEx *pDateTimeEditEx = static_cast<GLDDateTimeEditEx *>(editor);

    // 因为事件调用顺序，导致此事件在 GLDDateTimeEditEx 写入之前发生，所以手动更新其值
    pDateTimeEditEx->updateValue();

    if (pDateTimeEditEx->plainText().isEmpty())
    {
        if (QVariant::Time == vType)
        {
            result = model->setData(index, QTime());
        }
        else if (QVariant::Date == vType)
        {
            result = model->setData(index, QDate());
        }
        else
        {
            result = model->setData(index, QDateTime());
        }
    }
    else
    {
        if (QVariant::Time == vType)
        {
            result = model->setData(index, pDateTimeEditEx->time());
        }
        else if (QVariant::Date == vType)
        {
            result = model->setData(index, pDateTimeEditEx->date());
        }
        else
        {
            result = model->setData(index, pDateTimeEditEx->dateTime());
        }
    }

    return result;
}

void GlodonDefaultItemDelegate::setModelData(QWidget *editor,
        QAbstractItemModel *model,
        const QModelIndex &index) const
{
    const_cast<GlodonDefaultItemDelegate *>(this)->m_inSetModelData = true;

    try
    {
        bool bReadOnly = false;
        bool result = true;

        m_currTreeEditing = index;

        QModelIndex dIndex = dataIndex(index);

        QVariant value = dIndex.model()->data(dIndex, Qt::EditRole);
        QVariant::Type vType = value.type();

        GEditStyle eEditStyle = editStyle(dIndex, bReadOnly);

        if (esSimple == eEditStyle)
        {
            eEditStyle = editStyleByVariantType(vType);
        }

        switch (eEditStyle)
        {
            case esNone:
                break;

            case esImage:
            {
                GImageEditor *imageEditor = static_cast<GImageEditor *>(editor);
                QImage img = imageEditor->image();
                result = model->setData(index, img);
                break;
            }

            case esDropDown:
            case esTreeView:
            {
                result = setTreeViewModelData(editor, model, index);
                break;
            }

            // 修改计价提的BUG，新增可多行编辑的下拉框
            case esDropDownWindowEllipsis:
            case esDropDownWindowNone:
            case esDropDownWindow:
            {
                result = setDropDownWindowModelData(editor, model, index);
                break;
            }

            case esMonthCalendar:
            {
                result = setMonthCalendarModelData(editor, model, index, vType);
                break;
            }

            case esColorList:
            {
                GColorListEx *colorListEx = static_cast<GColorListEx *>(editor);

                if(colorListEx->isShowRGBStr())
                {
                    GString colorRGBStr = colorListEx->currentText();

                    if (colorRGBStr.isEmpty())
                    {
                        result = model->setData(index, colorListEx->currentColor());
                    }
                    else
                    {
                        GString str = intToColorHex(strToInt64(colorRGBStr));
                        result = model->setData(index, QColor(str));
                    }
                }
                else
                {
                    result = model->setData(index, colorListEx->currentColor());
                }

                break;
            }

            case esFontList:
            {
                GFontListEx *fontComboBox = static_cast<GFontListEx *>(editor);
                result = model->setData(index, fontComboBox->currentFont());
                break;
            }

            case esLineWidthList:
            {
                GLDLineWidthComboBoxEx *lineWidthComboBox = static_cast<GLDLineWidthComboBoxEx *>(editor);
                result = model->setData(index, lineWidthComboBox->curLineWidth());
                break;
            }

            case esPlainEllipsis:
            case esLineEllipsis:
            {
                GLDAbstractButtonEdit *ellipsis = static_cast<GLDAbstractButtonEdit *>(editor);
                QString text = ellipsis->text();

#ifdef _FONTSYMBOL_
                QString aliases = text;
                GlodonSymbols glodonSymbols;
                text = glodonSymbols.displaySymbolsFromAliases(aliases);
#endif

                result = model->setData(index, text);
                break;
            }

            case esLineEdit:
            {
                QLineEdit *edit = static_cast<QLineEdit *>(editor);
                QString text = edit->text();

#ifdef _FONTSYMBOL_
                QString aliases = text;
                GlodonSymbols glodonSymbols;
                text = glodonSymbols.displaySymbolsFromAliases(aliases);
#endif

                result = model->setData(index, text/*edit->text()*/);
                break;
            }

            case esUpDown:
            case esSimple:
            {
                if (variantTypeIsByte(vType) || variantTypeIsShort(vType)
                        || variantTypeIsInt(vType))
                {
                    GLDSpinBoxEx *pIntEdit = static_cast<GLDSpinBoxEx *>(editor);
                    result = model->setData(index, compareDataByType<int>(value, pIntEdit->value()));
                    break;
                }
                else if (variantTypeIsFloat(vType))
                {
                    GLDDoubleSpinBoxEx *pDoubleEdit = static_cast<GLDDoubleSpinBoxEx *>(editor);
                    result = model->setData(index, compareDataByType<double>(value, pDoubleEdit->value()));
                    break;
                }
                else if (QVariant::LongLong == vType)
                {
                    QLineEdit *pLineEdit = static_cast<QLineEdit *>(editor);
                    result = model->setData(index, compareDataByType<qlonglong>(value, pLineEdit->text().trimmed().toLongLong()));
                    break;
                }
                // 此处不需要break，如果编译方式是esSimple，默认会走QPlanTextEdit，在createEdit中和setEditData中都是这个流程
            }

            default:
                QPlainTextEdit *plainTextEdit = static_cast<QPlainTextEdit *>(editor);
                QString str = plainTextEdit->toPlainText();

                if (str.length() == 0)
                {
                    result = model->setData(index, QVariant());
                    break;
                }

#ifdef _FONTSYMBOL_
                QString aliases = str;
                GlodonSymbols glodonSymbols;
                str = glodonSymbols.displaySymbolsFromAliases(aliases);
#endif

                result = model->setData(index, str);
                break;
        }

        const_cast<GlodonDefaultItemDelegate *>(this)->setCheckSetModelData(result);
    }
    catch (...)
    {
        const_cast<GlodonDefaultItemDelegate *>(this)->m_inSetModelData = false;
        throw;
    }

    const_cast<GlodonDefaultItemDelegate *>(this)->m_inSetModelData = false;
}

void GlodonDefaultItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    // 当第一行或者第一列时，需要将Edit的边线向内调整1像素
    // 将画在里面的格线让出来
    QRect editRect = option.rect;
    if (0 == index.row())
    {
        editRect.setTop(editRect.top() + 1);
    }
    if (0 == index.column())
    {
        editRect.setLeft(editRect.left() + 1);
    }
    editor->setGeometry(editRect);
    Q_UNUSED(index);
}

void GlodonDefaultItemDelegate::drawFocus(QPainter *painter,
        const QStyleOptionViewItem &option,
        const QRect &rect) const
{
    // 现在不再绘制焦点，通过当前焦点格的背景不同，区分选择区域与焦点格
    G_UNUSED(painter);
    G_UNUSED(option);
    G_UNUSED(rect);
}

void GlodonDefaultItemDelegate::drawBackground(QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    bool bEditStyleDrawNeedEditorBg = false;
    QVariant oBackColor = index.data(Qt::BackgroundRole);

    GlodonTableView *pGldTable = dynamic_cast<GlodonTableView *>(m_pTable);

    if (NULL != m_pTable
            && (NULL != pGldTable && !pGldTable->showCurCellBackgroundColor())
            && (m_pTable->currentIndex() == index
                && (index.flags() & Qt::ItemIsSelectable) == Qt::ItemIsSelectable))
    {
        painter->fillRect(option.rect, qvariant_cast<QBrush>(oBackColor));
        return;
    }

    if (option.showDecorationSelected && (QStyle::State_Selected == (option.state & QStyle::State_Selected))
            && !bEditStyleDrawNeedEditorBg)
    {
        QPalette::ColorGroup cg = (QStyle::State_Enabled == (option.state & QStyle::State_Enabled))
                                  ? QPalette::Normal : QPalette::Disabled;

        if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
        {
            cg = QPalette::Inactive;
        }

        //使用混合色绘制背景
        if (m_bUseBlendColor)
        {
            if (!oBackColor.isValid())
            {
                oBackColor = QColor(255, 255, 255);
                QBrush brush = qvariant_cast<QBrush>(oBackColor);
                painter->fillRect(option.rect, brush.color().darker(108));
            }
            else
            {
                //设置混合色，把单元格背景色跟选中色进行等比例混合; zhangjq
                QColor oSelectedColor = selectedCellBackgroundColor();
                QColor oBackGroundColor = qvariant_cast<QColor>(oBackColor);
                QColor oBlendColor((oSelectedColor.red() + oBackGroundColor.red()) / 2,
                    (oSelectedColor.green() + oBackGroundColor.green()) / 2,
                    (oSelectedColor.blue() + oBackGroundColor.blue()) / 2,
                    (oSelectedColor.alpha() + oBackGroundColor.alpha()) / 2);

                painter->fillRect(option.rect, oBlendColor);
            }
        }
        else
        {
            painter->fillRect(option.rect, selectedCellBackgroundColor());
        }
    }
    else
    {
        QPointF oldBO = painter->brushOrigin();

        if (oBackColor.canConvert<QBrush>())
        {
            painter->setBrushOrigin(option.rect.topLeft());

//            bool readOnly = true;
            //需要editor背景，//且并不是空editor格子
            if (bEditStyleDrawNeedEditorBg)// && editStyle(index, readOnly) != esNone)
            {
                painter->fillRect(option.rect, QBrush(Qt::white));
            }
            else
            {
                painter->fillRect(option.rect, qvariant_cast<QBrush>(oBackColor));
//                painter->fillRect(option.rect, qvariant_cas);
            }

            painter->setBrushOrigin(oldBO);
        }
    }
}

void GlodonDefaultItemDelegate::drawDisplay(QPainter *painter, const QStyleOptionViewItem &option,
        const QRect &rect, const QString &text, const QModelIndex &index) const
{
    QRect newRect(rect);
    bool bReadOnly = false;

    GLDTableView *pGLDTableView = dynamic_cast<GLDTableView*>(m_pTable);
    if ((pGLDTableView != NULL) && pGLDTableView->shouldDoEditorDraw(option.index))
    {
        QModelIndex dIndex = dataIndex(option.index);

        switch (editStyle(dIndex, bReadOnly))
        {
            case esDropDown:
                newRect.adjust(0, 0, -6, 0);
                break;

            case esMonthCalendar:
                newRect.adjust(0, 0, -6, 0);
                break;

            default:
                break;
        }
    }

    QVariant textColor = index.data(Qt::TextColorRole);

    QStyleOptionViewItem opt(option);

    if (textColor.isValid())
    {
        QColor color = textColor.value<QColor>();
        opt.palette.setColor(QPalette::HighlightedText, color);
        opt.palette.setColor(QPalette::Text, color);
    }

    //使用混合色时去除选中标记，否则会使用Qt方式绘制背景
    QStyle::State preState;
    bool bRemoved = QStyle::State_Selected == (opt.state & QStyle::State_Selected);

    if (bRemoved)
    {
        preState = opt.state;
        opt.state &= ~QStyle::State_Selected;
    }

    doDrawDisplay(painter, opt, newRect, text, index);

    if (bRemoved)
    {
        opt.state = preState;
    }
}

void GlodonDefaultItemDelegate::drawEditStyleDraw(
    QPainter *painter, QStyleOptionViewItem &option, QRect &rect) const
{
    if (!option.index.isValid())
    {
        return;
    }

    if (!m_pTable)
    {
        return;
    }

    GLDTableView *pGLDTableView = dynamic_cast<GLDTableView*>(m_pTable);
    if (pGLDTableView->shouldDoEditorDraw(option.index))
    {
        QModelIndex dIndex = dataIndex(option.index);
        bool bReadOnly = false;

        if (!editable(dIndex))
        {
            return;
        }

        switch (editStyle(dIndex, bReadOnly))
        {
                // 只读下，不需要绘制
                if (bReadOnly)
                {
                    return;
                }

                //无需绘制
            case esNone:
            case esPlainEdit:
            case esLineEdit:
            case esImage:
            case esBool:
            case esDropDownWindowNone:
                break;

                // 向下箭头
            case esDropDownWindowEllipsis:
            {
                QStyleOptionButton opt;
                GLDStylePaintUtils::initPushButtonOpt(opt, rect, QString("..."));
                m_pTable->style()->drawItemPixmap(
                    painter, opt.rect, Qt::AlignCenter,
                    QPixmap(":/icons/GLDWindowComboBoxEx-ellipsis.png"));

                rect.adjust(0, 0, -opt.rect.width(), 0);
                option.rect = rect;
                break;
            }

            case esDropDownWindow:
            case esDropDown:
            case esTreeView:
            case esColorList:
            case esFontList:
            case esLineWidthList:
            {
                QStyleOptionComboBox tempOpt;
                GLDStylePaintUtils::initComboBoxOpt(tempOpt, rect);
                QRect subControlRect = m_pTable->style()->subControlRect(QStyle::CC_ComboBox, &tempOpt,
                                       QStyle::SC_ComboBoxArrow, m_pTable);
                rect.adjust(0, 0, - subControlRect.width(), 0);
                subControlRect.adjust(-4, 0, 0, 0); // 为了让非编辑状态时与进入编辑状态时，编辑方式的位置少偏移，所以减去4，下同
                m_pTable->style()->drawItemPixmap(
                    painter, subControlRect, Qt::AlignCenter,
                    QPixmap(":/icons/GLDCustomComboBoxEx-downarrow.png"));
                option.rect = rect;
                break;
            }

            case esMonthCalendar:
            {
                QStyleOptionComboBox tempOpt;
                GLDStylePaintUtils::initComboBoxOpt(tempOpt, rect);
                QRect subControlRect = m_pTable->style()->subControlRect(QStyle::CC_ComboBox, &tempOpt,
                                       QStyle::SC_ComboBoxArrow, m_pTable);
                rect.adjust(0, 0, - subControlRect.width(), 0);

                subControlRect.adjust(-8, 0, 0, 0);
                m_pTable->style()->drawItemPixmap(
                    painter, subControlRect, Qt::AlignCenter,
                    QPixmap(":/icons/calendar.png"));
                option.rect = rect;
                break;
            }

            // 带三点button
            case esPlainEllipsis:
            case esLineEllipsis:
            {
                QStyleOptionButton opt;
                GLDStylePaintUtils::initPushButtonOpt(opt, rect, QString("..."));
                m_pTable->style()->drawItemPixmap(
                    painter, opt.rect, Qt::AlignCenter,
                    QPixmap(":/icons/GLDLineButtonEditEx-ellipsis.png"));

                rect.adjust(0, 0, -opt.rect.width(), 0);
                option.rect = rect;
                break;
            }

            // 带上下箭头或者加减号button
            case esUpDown:
            {
                QStyleOptionSpinBox opt;
                GLDStylePaintUtils::initSpinBoxOpt(opt, rect);
                opt.subControls &= ~QStyle::SC_SpinBoxFrame;

                QRect spinBoxUpRect = m_pTable->style()->subControlRect(QStyle::CC_SpinBox, &opt,
                                      QStyle::SC_SpinBoxUp,
                                      m_pTable);
                rect.adjust(0, 0, - spinBoxUpRect.width(), 0);
                m_pTable->style()->drawComplexControl(QStyle::CC_SpinBox, &opt, painter);
                option.rect = rect;
                break;
            }

            default:
                break;
        }
    }
}

int GlodonDefaultItemDelegate::calculateLineCount(const QRect &textRect) const
{
    Q_D(const GlodonDefaultItemDelegate);

    d->textLayout.beginLayout();
    QTextLine line = d->textLayout.createLine();
    line.setLeadingIncluded(true);
    line.setLineWidth(textRect.width());
    line.setPosition(QPointF(0, 0));
    int nLineHeight = line.height();
    d->textLayout.endLayout();

    int nLineCount = textRect.height() / (nLineHeight);

    if (nLineCount == 0)
    {
        nLineCount = 1;
    }

    return nLineCount;
}

QString GlodonDefaultItemDelegate::elidedText(int &textLineCount,
        int rectLineCount, const QRect &textRect,
        const QString &text, const QStyleOptionViewItem &option) const
{
    QString strElided;

    if (text.isEmpty())
    {
        textLineCount = 1;
        return strElided;
    }

    QString strRestText = text;
    int nJ = 0;
    int ntextRectWidth = textRect.width();
    int nRestStartPos = 0;

    while (!strRestText.isEmpty())
    {
        nJ++;

        if (m_isTextEllipse)
        {
            if (nJ > rectLineCount)
            {
                break;
            }
        }
        else
        {
            if (nJ >= rectLineCount)
            {
                strElided += option.fontMetrics.elidedText(strRestText, option.textElideMode, ntextRectWidth);
                break;
            }
        }

        int nI = 0;

        while (true)
        {
            if (nI >= strRestText.length())
            {
                nRestStartPos += nI;
                strElided += strRestText.mid(0, nI);
                break;
            }

            QChar ch;
            int nWord = 0;
            ch = strRestText[nI + nWord];

            // 获得字母字符串
            while (ch < 128 && ch != QChar::Space && (nI + nWord) < strRestText.length())
            {
                nWord++;
                ch = strRestText[nI + nWord];
            }

            // 字母后的空格
            while (ch == QChar::Space && (nI + nWord) < strRestText.length())
            {
                nWord++;
                ch = strRestText[nI + nWord];
            }

            int noTrimeWidth;

            if (nWord > 0)
            {
                noTrimeWidth = option.fontMetrics.width(strRestText, nI + nWord);
            }
            else
            {
                noTrimeWidth = option.fontMetrics.width(strRestText, nI + 1);
            }

            if (noTrimeWidth > textRect.width())
            {
                QString trimedText = strRestText.mid(0, nI + nWord).trimmed();
                int nTrimedWidth = option.fontMetrics.width(trimedText);

                if (nTrimedWidth > ntextRectWidth && nWord > 0)
                {
                    if (nI == 0)
                    {
                        QString strMidRestText = strRestText.mid(nI, nWord);
                        int nWordWidth = option.fontMetrics.width(strMidRestText);

                        if (nWordWidth <= ntextRectWidth)
                        {
                            nI = nWord;
                        }
                        else
                        {
                            while (true)
                            {
                                int nMidPos = nWord / 2;

                                if (nMidPos == 0)
                                {
                                    nWord = 0;
                                    break;
                                }

                                nWordWidth = option.fontMetrics.width(strRestText.mid(0, nI + nMidPos));

                                if (nWordWidth > ntextRectWidth)
                                {
                                    nWord = nMidPos;
                                }
                                else
                                {
                                    nI += nMidPos;
                                    nWord -= nMidPos;
                                }
                            }
                        }
                    }
                }
                else
                {
                    nI += nWord;
                }

                nRestStartPos = nI;
                strElided += strRestText.mid(0, nI);
                break;
            }

            if (nWord > 0)
            {
                nI += nWord;
            }
            else
            {
                nI++;
            }
        }

        strRestText = strRestText.mid(nRestStartPos);
    }

    textLineCount = nJ;

    return strElided;
}

QString GlodonDefaultItemDelegate::elidedTextWithoutLineFeed(const QRect &textRect,
        const QString &text,
        const QStyleOptionViewItem &option) const
{
    int nRectLineCount = calculateLineCount(textRect);
    int nTextLineCount = 0;

    return elidedText(nTextLineCount, nRectLineCount, textRect, text, option);
}

QString GlodonDefaultItemDelegate::elidedTextWithLineFeed(const QRect &textRect,
        const QString &text,
        const QStyleOptionViewItem &option) const
{
    QString strElided;
    int nRectLineCount = calculateLineCount(textRect);
    int nTextLineCount = 0;

    int nStart = 0;
    int nEnd = 0;
    QString strRestText;

    while (nRectLineCount > 0)
    {
        if (!strElided.isEmpty())
        {
            strElided += QChar::LineSeparator;
        }

        nEnd = text.indexOf(QChar::LineFeed, nStart);

        if (nEnd != -1)
        {
            strRestText = text.mid(nStart, nEnd - nStart);
            nStart = nEnd + 1;
        }
        else
        {
            // get the string, between last linefeed and the string end
            strRestText = text.mid(nStart, text.size() - 1);
        }

        strElided += elidedText(nTextLineCount, nRectLineCount, textRect, strRestText, option);
        nRectLineCount -= nTextLineCount;
    }

    return strElided;
}

QFont GlodonDefaultItemDelegate::zoomFont(QModelIndex index, QFont font, double factor) const
{
    QFont result = font;
    QVariant value = index.data(Qt::FontRole);

    if (value.isValid())
    {
        result = qvariant_cast<QFont>(value).resolve(font);
    }

    if (result.pointSize() > 0)
    {
        result.setPointSize(result.pointSize() * factor);
    }

    return result;
}

void GlodonDefaultItemDelegate::initComboBoxCompleter(QComboBox *comboBox, QModelIndex index)
{
    if (m_bUseComboBoxCompleter)
    {
        QCompleter *pCompleter = new QCompleter(comboBox);
        pCompleter->setModel(comboBox->model());
        pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        comboBox->setCompleter(pCompleter);
    }

    G_UNUSED(index);
}

QRect GlodonDefaultItemDelegate::editStyleRect(QModelIndex index, QRect rect)
{
    QModelIndex dIndex = dataIndex(index);

    bool bReadOnly = false;

    if (!editable(dIndex))
    {
        return QRect();
    }

    switch (editStyle(dIndex, bReadOnly))
    {
            // 向下箭头
        case esDropDownWindowEllipsis:
        case esDropDownWindow:
        case esDropDown:
        case esTreeView:
        case esColorList:
        case esFontList:
        case esMonthCalendar:
        case esLineWidthList:
        {
            QStyleOptionComboBox tempOpt;
            GLDStylePaintUtils::initComboBoxOpt(tempOpt, rect);
            QRect subControlRect = m_pTable->style()->subControlRect(QStyle::CC_ComboBox, &tempOpt,
                                   QStyle::SC_ComboBoxArrow, m_pTable);
            return subControlRect;
        }

        // 带三点button
        case esPlainEllipsis:
        case esLineEllipsis:
        {
            QStyleOptionButton opt;
            GLDStylePaintUtils::initPushButtonOpt(opt, rect, QString("..."));
            return opt.rect;
        }

        // 带上下箭头或者加减号button
        case esUpDown:
        {
            QStyleOptionSpinBox opt;
            GLDStylePaintUtils::initSpinBoxOpt(opt, rect);
            opt.subControls &= ~QStyle::SC_SpinBoxFrame;

            QRect spinBoxUpRect = m_pTable->style()->subControlRect(QStyle::CC_SpinBox, &opt,
                                  QStyle::SC_SpinBoxUp,
                                  m_pTable);
            return spinBoxUpRect;
        }

        default:
            return QRect();
    }
}

bool GlodonDefaultItemDelegate::isInSubControlRect(QModelIndex index, QPoint pos)
{
    QRect oSubControlRect = editStyleRect(index, m_pTable->visualRect(index));

    if (index.data(Qt::DecorationPropertyRole) == QStyleOptionViewItem::Right
            && index.data(Qt::DecorationRole).isValid())
    {
        int nWidth = oSubControlRect.width();
        oSubControlRect.setLeft(oSubControlRect.left() - nWidth);
        oSubControlRect.setRight(oSubControlRect.right() - nWidth);
    }

    return oSubControlRect.contains(pos);
}

void GlodonDefaultItemDelegate::clickSubControl(QModelIndex index)
{
    QRect oEditStyleRect = editStyleRect(index, m_pTable->visualRect(index));

    QWidget *pCurrentEditor = curEditor();
    if (GLDAbstractButtonEdit *pButtonEdit =
            dynamic_cast<GLDAbstractButtonEdit *>(pCurrentEditor))
    {
        pButtonEdit->onEllipsisButtonClicked();
    }
    else if (pCurrentEditor != NULL)
    {
        QMouseEvent oMouseEvent(
                    QMouseEvent::MouseButtonPress,
                    curEditor()->mapFrom(m_pTable->viewport(), oEditStyleRect.center()),
                    Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(curEditor(), &oMouseEvent);
    }
}

template<class T>
QVariant GlodonDefaultItemDelegate::compareDataByType(QVariant oldData, T newData) const
{
    if (oldData.value<T>() == newData)
        return oldData;
    else
        return newData;
}

QVariant GlodonDefaultItemDelegate::compareDataByType(QVariant oldData, double newData) const
{
    if (sameValue(oldData.toDouble(), newData))
        return oldData;
    else
        return newData;
}

void GlodonDefaultItemDelegate::doComboBoxActivated(int index)
{
    m_oComboBoxCurIndex = index;
    try
    {
        QComboBox *pCombox = dynamic_cast<QComboBox *>(curEditor());
        if (pCombox)
        {
            m_bCanHideEidtOnExit = false;
            itemViewSelected(pCombox, m_currTreeEditing, pCombox->view()->currentIndex());
            m_bCanHideEidtOnExit = true;
        }

        m_bCanHideEidtOnExit = false;
        emit onCommitDataAndCloseEditor();
        m_bCanHideEidtOnExit = true;
    }
    catch (...)
    {
        m_oComboBoxCurIndex = -1;
        m_bCanHideEidtOnExit = true;
        throw;
    }

    m_oComboBoxCurIndex = -1;
}

void GlodonDefaultItemDelegate::doDrawDisplay(QPainter *painter, const QStyleOptionViewItem &option,
        const QRect &rect, const QString &text, const QModelIndex &index) const
{
    Q_D(const GlodonDefaultItemDelegate);

    QPalette::ColorGroup cg = (QStyle::State_Enabled == (option.state & QStyle::State_Enabled))
                              ? QPalette::Normal : QPalette::Disabled;

    if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
    {
        cg = QPalette::Inactive;
    }

    if (QStyle::State_Selected == (option.state & QStyle::State_Selected))
    {
        painter->fillRect(rect, option.palette.brush(cg, QPalette::Highlight));
        painter->setPen(option.palette.color(cg, QPalette::HighlightedText));
    }
    else
    {
        painter->setPen(option.palette.color(cg, QPalette::Text));
    }

    if (text.isEmpty())
    {
        return;
    }

    if (QStyle::State_Editing == (option.state & QStyle::State_Editing))
    {
        painter->save();
        painter->setPen(option.palette.color(cg, QPalette::Text));
        painter->drawRect(rect.adjusted(0, 0, -1, -1));
        painter->restore();
    }

    QVariant oTextMargin = index.data(gidrCellMargin);

    const int c_nTextHMargin = d->textHMargin(oTextMargin, option);
    const int c_nTextVMargin = d->textVMargin(oTextMargin);
    QRect textRect = rect.adjusted(c_nTextHMargin, c_nTextVMargin, -c_nTextHMargin, -c_nTextVMargin);

    const bool c_bWrapText = option.features & QStyleOptionViewItem::WrapText;
    d->textOption.setWrapMode(c_bWrapText ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::ManualWrap);

    if (d->textOption.wrapMode() != m_wrapMode)
    {
        d->textOption.setWrapMode(m_wrapMode);
    }

    d->textOption.setTextDirection(option.direction);
    d->textOption.setAlignment(QStyle::visualAlignment(option.direction, option.displayAlignment));
    d->textLayout.setTextOption(d->textOption);
    d->textLayout.setFont(option.font);
    d->textLayout.setText(d->replaceNewLine(text));

    QSizeF textLayoutSize = d->doTextLayout(textRect.width());

    if (textRect.width() < textLayoutSize.width()
            || textRect.height() < textLayoutSize.height())
    {
        QString strElided;
        int nEnd = text.indexOf(QChar::LineFeed);

        if (nEnd == -1)// 没有换行符
        {
            strElided = elidedTextWithoutLineFeed(textRect, text, option);
        }
        else
        {
            strElided = elidedTextWithLineFeed(textRect, text, option);
        }

        d->textLayout.setText(strElided);
        textLayoutSize = d->doTextLayout(textRect.width());
    }

    const QSize c_layoutSize(textRect.width(), int(textLayoutSize.height()));
    const QRect c_layoutRect = QStyle::alignedRect(option.direction, option.displayAlignment,
                               c_layoutSize, textRect);

    // if we still overflow even after eliding the text, enable clipping
    if (!hasClipping() && (textRect.width() < textLayoutSize.width()
                           || textRect.height() < textLayoutSize.height()))
    {
        painter->save();
        painter->setClipRect(c_layoutRect);
        d->textLayout.draw(painter, c_layoutRect.topLeft(), QVector<QTextLayout::FormatRange>(), c_layoutRect);
        painter->restore();
    }
    else
    {
        d->textLayout.draw(painter, c_layoutRect.topLeft(), QVector<QTextLayout::FormatRange>(), c_layoutRect);
    }
}

void GlodonDefaultItemDelegate::drawDiagonal(QPainter *painter, const QRect &rect, const QModelIndex &index) const
{
    painter->save();
    QPen pen = painter->pen();
    painter->setRenderHint(QPainter::Antialiasing);

    int nDiagonal = index.data(gidrDiagonal).toInt();
    int nAntiDiagonal = index.data(gidrAntiDiagonal).toInt();

    if (nDiagonal != 0)
    {
        pen.setWidth(nDiagonal);
        painter->setPen(pen);
        painter->drawLine(rect.left(), rect.top(), rect.right(), rect.bottom());
    }

    if (nAntiDiagonal != 0)
    {
        pen.setWidth(nAntiDiagonal);
        painter->setPen(pen);
        painter->drawLine(rect.left(), rect.bottom(), rect.right(), rect.top());
    }

    painter->restore();
}

QModelIndex GlodonDefaultItemDelegate::dataIndex(const QModelIndex &index) const
{
    const GlodonTreeModel *treeModel = dynamic_cast<const GlodonTreeModel *>(index.model());

    if (!treeModel)
    {
        return index;
    }

    const QModelIndex c_treeIndex = treeModel->dataIndex(index);

    const GlodonGroupModel *groupModel = dynamic_cast<const GlodonGroupModel *>(c_treeIndex.model());

    if (groupModel)
    {
        return groupModel->dataIndex(c_treeIndex);
    }

    return c_treeIndex;
}

QModelIndex GlodonDefaultItemDelegate::treeIndex(const QModelIndex &) const
{
    return m_currTreeEditing;
}

bool GlodonDefaultItemDelegate::editable(const QModelIndex &) const
{
    return true;
}

GEditStyle GlodonDefaultItemDelegate::editStyle(const QModelIndex &, bool &) const
{
    return esSimple;
}

GEditStyle GlodonDefaultItemDelegate::editStyleByVariantType(QVariant::Type vType)
{
    if (variantTypeIsDateTime(vType))
    {
        return esMonthCalendar;
    }

    switch (vType)
    {
        case QVariant::Bool:
            return esBool;

        case QVariant::Image:
            return esImage;

        case QVariant::Font:
            return esFontList;

        case QVariant::Color:
            return esColorList;

        default:
            return esSimple;
    }
}

void GlodonDefaultItemDelegate::setIsTextEllipse(bool value)
{
    m_isTextEllipse = value;
}

void GlodonDefaultItemDelegate::initEllipsisButtonEdit(GLDAbstractButtonEdit *ellipsis, const QModelIndex &index) const
{
    Q_UNUSED(ellipsis);
    Q_UNUSED(index);
}

void GlodonDefaultItemDelegate::initComboBox(QComboBox *comboBox, const QModelIndex &index) const
{
    Q_UNUSED(comboBox);
    Q_UNUSED(index);
}

void GlodonDefaultItemDelegate::initWindowComboBox(GLDWindowComboBoxEx *, const QModelIndex &) const
{

}

void GlodonDefaultItemDelegate::initLineWidthComboBox(GLDLineWidthComboBoxEx *, const QModelIndex &) const
{

}

void GlodonDefaultItemDelegate::initSpinBox(QAbstractSpinBox *spinBox, const QModelIndex &index)
{
    Q_UNUSED(spinBox);
    Q_UNUSED(index);
}

void GlodonDefaultItemDelegate::initDateTimeEdit(GLDDateTimeEdit *dateTimeEdit, const QModelIndex &index)
{
    Q_UNUSED(dateTimeEdit);
    Q_UNUSED(index);
}

void GlodonDefaultItemDelegate::itemViewSelected(QComboBox *, const QModelIndex &, const QModelIndex &) const
{

}

QVariant GlodonDefaultItemDelegate::currentEditorData(const QModelIndex &index, QWidget *editor)
{
    if (!index.isValid() || editor == NULL)
    {
        return QVariant();
    }

    m_currTreeEditing = index;

    QModelIndex dIndex = dataIndex(index);
    QVariant value = dIndex.model()->data(dIndex, Qt::EditRole);
    QVariant::Type vType = value.type();

    bool bReadOnly = false;
    GEditStyle eEditStyle = editStyle(dIndex, bReadOnly);

    if (esSimple == eEditStyle)
    {
        eEditStyle = editStyleByVariantType(vType);
    }

    switch (eEditStyle)
    {
        case esNone:
            return QVariant();

        case esLineEdit:
        {
            QLineEdit *edit = static_cast<QLineEdit *>(editor);
            return edit->text();
        }

        case esImage:
        {
            GImageEditor *imageEditor = static_cast<GImageEditor *>(editor);
            return imageEditor->image();
        }

        case esDropDown:
        case esTreeView:
        {
            QComboBox *comboBox = static_cast<QComboBox *>(editor);
            return comboBox->currentText();
        }

        // 修改计价提的BUG，新增可多行编辑的下拉框
        case esDropDownWindowEllipsis:
        case esDropDownWindowNone:
        case esDropDownWindow:
        {
            GLDWindowComboBoxEx *comboBox = static_cast<GLDWindowComboBoxEx *>(editor);
            return comboBox->editText();
        }

        case esMonthCalendar:
        {
            GLDDateTimeEditEx *pDateTimeEditEx = static_cast<GLDDateTimeEditEx *>(editor);

            if (QVariant::Time == vType)
            {
                return pDateTimeEditEx->time();
            }
            else if (QVariant::Date == vType)
            {
                return pDateTimeEditEx->date();
            }
            else
            {
                return pDateTimeEditEx->dateTime();
            }
        }

        case esColorList:
        {
            GColorListEx *colorListEx = static_cast<GColorListEx *>(editor);
            return colorListEx->currentColor();
        }

        case esFontList:
        {
            GFontListEx *fontComboBox = static_cast<GFontListEx *>(editor);
            return fontComboBox->currentFont();
        }

        case esLineWidthList:
        {
            GLDLineWidthComboBoxEx *lineWidthComboBox = static_cast<GLDLineWidthComboBoxEx *>(editor);
            return lineWidthComboBox->curLineWidth();
        }

        case esPlainEllipsis:
        case esLineEllipsis:
        {
            GLDAbstractButtonEdit *ellipsis = static_cast<GLDAbstractButtonEdit *>(editor);
            return ellipsis->text();
        }

        case esUpDown:
        case esSimple:
        {
            if (variantTypeIsByte(vType) || variantTypeIsShort(vType)
                    || variantTypeIsInt(vType))
            {
                GLDSpinBoxEx *pIntEdit = static_cast<GLDSpinBoxEx *>(editor);
                return pIntEdit->value();
            }
            else if (variantTypeIsFloat(vType))
            {
                GLDDoubleSpinBoxEx *pDoubleEdit = static_cast<GLDDoubleSpinBoxEx *>(editor);
                return pDoubleEdit->value();
            }
            else if (QVariant::LongLong == vType)
            {
                QLineEdit *pLineEdit = static_cast<QLineEdit *>(editor);
                return pLineEdit->text().trimmed().toLongLong();
            }
            break;
        }

        default:
            QPlainTextEdit *plainTextEdit = static_cast<QPlainTextEdit *>(editor);
            QString str = plainTextEdit->toPlainText();

            if (str.length() == 0)
            {
                return QVariant();
            }

            return str;
    }

    return QVariant();
}

QRect GlodonDefaultItemDelegate::textLayoutBounds(const QStyleOptionViewItem &option) const
{
    QRect rect = option.rect;
    const bool c_wrapText = (QStyleOptionViewItem::WrapText == (option.features & QStyleOptionViewItem::WrapText));

    switch (option.decorationPosition)
    {
        case QStyleOptionViewItem::Left:
        case QStyleOptionViewItem::Right:
        {
            rect.setWidth(c_wrapText && rect.isValid() ? rect.width() : (QFIXED_MAX));
            break;
        }

        case QStyleOptionViewItem::Top:
        case QStyleOptionViewItem::Bottom:
        {
            rect.setWidth(c_wrapText ? option.decorationSize.width() : (QFIXED_MAX));
            break;
        }
    }

    return rect;
}

bool GlodonDefaultItemDelegate::isBoolCell(const QModelIndex &index) const
{
    return m_oDisplayData.type() == QVariant::Bool;
    G_UNUSED(index);
}

bool GlodonDefaultItemDelegate::eventFilter(QObject *object, QEvent *event)
{
    QWidget *pEditor = dynamic_cast<QWidget *>(object);
    if (!pEditor)
    {
        return false;
    }

    switch (event->type())
    {
    case QEvent::KeyPress:
        return filterKeyPress(pEditor, event);
    case QEvent::FocusOut:
    case QEvent::Hide:
        return dealWithLoseFocus(event, pEditor);
    case QEvent::ShortcutOverride:
    {
        return dealWithShortCutOverride(event);
    }
    default:
        return false;
    }
}

template<typename T>
bool GlodonDefaultItemDelegate::commitIfCanElseSelectAll(QEvent *event, QWidget *editor, bool &state)
{
    T *pEditor = dynamic_cast<T *>(editor);

    state = false;

    if (!pEditor)
    {
        return false;
    }

    state = true;

    if (commitDataAndCloseEditor(editor, SubmitModelCache))
    {
        if (parent() && m_enterJump) //如果tableView是回车跳格，才发Enter时间，否则会再次进入编辑状态
        {
            QApplication::sendEvent(parent(), event);
        }
    }
    else
    {
        setTextAllSelected(pEditor);
    }

    return true;

}

bool GlodonDefaultItemDelegate::setTextAllSelected(QWidget *editor)
{
    if (!editor || !editor->isVisible() || !editor->isActiveWindow())
    {
        return false;
    }

    if (QPlainTextEdit *plainTextEdit = dynamic_cast<QPlainTextEdit *>(editor))
    {
        plainTextEdit->selectAll();
    }
    else if (QSpinBox *pSpinBox = dynamic_cast<QSpinBox *>(editor))
    {
        pSpinBox->selectAll();
    }
    else if (QDoubleSpinBox *pDoubleSpinBox = dynamic_cast<QDoubleSpinBox *>(editor))
    {
        pDoubleSpinBox->selectAll();
    }
    else if (GLDAbstractButtonEdit *pbtnEdit = dynamic_cast<GLDAbstractButtonEdit *>(editor))
    {
        //自写btnEdit类控件需要向其中的Edit发送全选命令
        return pbtnEdit->selectAll();
    }
    else
    {
        //暂时未枚举类型，可能会向其发送Ctrl + A全选命令失败，导致tableView区域被全部选中！！！ TODO
        if (editor->hasFocus())
        {
            Qt::KeyboardModifiers modifiers = (Qt::KeyboardModifiers)(Qt::CTRL);
            QKeyEvent allSelectEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_A, modifiers);

            return QApplication::sendEvent(editor, &allSelectEvent);
        }

        return false;
    }

    return true;
}

bool GlodonDefaultItemDelegate::commitDataAndCloseEditor(QWidget *editor, GlodonDefaultItemDelegate::EndEditHint hint)
{
    if (m_inCommitData)
    {
        return false;
    }

    bool bCanCloseEditor = true;

    try
    {
        emit commitData(editor, bCanCloseEditor);

        //emit commitData(editor);
        if (bCanCloseEditor)
        {
            emit closeEditor(editor, bCanCloseEditor, hint);
        }

        if (!bCanCloseEditor)
        {
            editor->setFocus();
        }
    }
    catch (...)
    {
        throw;
    }

    m_inCommitData = false;

    return bCanCloseEditor;
}

void GlodonDefaultItemDelegate::cursorMoveTextEndByEndKey(QWidget *editor)
{
    Qt::KeyboardModifiers modifiers = (Qt::KeyboardModifiers)(Qt::UNICODE_ACCEL);
    QKeyEvent cursorToEnd = QKeyEvent(QEvent::KeyPress, Qt::Key_End, modifiers);
    QCoreApplication::sendEvent(editor, &cursorToEnd);
}

bool GlodonDefaultItemDelegate::cursorMoveTextEnd(QWidget *editor)
{
    if (GLDAbstractButtonEdit *pbtnEdit = dynamic_cast<GLDAbstractButtonEdit *>(editor))
    {
        //自写btnEdit类控件需要向其中的Edit发送全选命令
        pbtnEdit->cursorMoveTextEnd();
        return true;
    }
    else if (dynamic_cast<QSpinBox *>(editor) || dynamic_cast<QDoubleSpinBox *>(editor))
    {
        cursorMoveTextEndByEndKey(editor);
        return true;
    }

#ifndef QT_NO_TEXTEDIT
    QTextEdit *textEdit = NULL;
    QPlainTextEdit *plainTextEdit = NULL;

    if ((textEdit = dynamic_cast<QTextEdit *>(editor))
            || (plainTextEdit = dynamic_cast<QPlainTextEdit *>(editor)))
    {
        if (textEdit)
        {
            textEdit->moveCursor(QTextCursor::End);

            return true;
        }
        else if (plainTextEdit)
        {
            plainTextEdit->moveCursor(QTextCursor::End);

            return true;
        }
    }

    return false;

#endif // QT_NO_TEXTEDIT

#ifndef QT_NO_LINEEDIT

    if (QLineEdit *edit = dynamic_cast<QLineEdit *>(editor))
    {
        if (edit->hasAcceptableInput())
        {
            return true;
        }
    }

    return false;

#endif // QT_NO_LINEEDIT
}

bool GlodonDefaultItemDelegate::cursorPosInsertANewLine(QWidget *editor)
{
#ifndef QT_NO_TEXTEDIT

    QTextEdit *textEdit = NULL;
    QPlainTextEdit *plainTextEdit = NULL;
    GLDWindowComboBoxEx *mutilLineWindowComboBox = NULL;

    // 对不同的Edit控件进行判断，由于GLDWindowComboBoxEx是需要进行回车特殊操作的，因此在这里需要加上
    if ((textEdit = dynamic_cast<QTextEdit *>(editor))
            || (mutilLineWindowComboBox = dynamic_cast<GLDWindowComboBoxEx *>(editor))
            || (plainTextEdit = dynamic_cast<QPlainTextEdit *>(editor)))
    {
        editor->removeEventFilter(this);
        QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
        QCoreApplication::sendEvent(editor, &keyEvent);
        editor->installEventFilter(this);

        return true;
    }
    else
    {
        // 在不需要做特殊的回车处理的，就不需要发信号了，因此返回false
        return false;
    }

#endif // QT_NO_TEXTEDIT

#ifndef QT_NO_LINEEDIT

    if (QLineEdit *edit = dynamic_cast<QLineEdit *>(editor))
    {
        if (edit->hasAcceptableInput())
        {
            return true;
        }
    }

#endif // QT_NO_LINEEDIT

    return false;
}

QStyleOptionViewItem GlodonDefaultItemDelegate::setOptions(const QModelIndex &index,
        const QStyleOptionViewItem &option) const
{
    QStyleOptionViewItem opt = option;

    // set font
    opt.font = zoomFont(index, opt.font, m_factor);
    opt.fontMetrics = QFontMetrics(opt.font);

    // set text alignment
    QVariant value = index.data(Qt::TextAlignmentRole);

    if (value.isValid())
    {
        if ((value.toInt() & Qt::AlignHorizontal_Mask) == 0)
        {
            opt.displayAlignment &= ~Qt::AlignHorizontal_Mask;

            if (variantTypeIsNumeric(m_oDisplayData.type()))
            {
                opt.displayAlignment |= Qt::AlignRight;
            }
            else
            {
                opt.displayAlignment |= Qt::AlignLeft;
            }
        }
        else
        {
            opt.displayAlignment = Qt::Alignment(value.toInt());
        }
    }

    return opt;
}

void GlodonDefaultItemDelegate::adjustDecorationAlignment(const QModelIndex &index, QStyleOptionViewItem &opt) const
{
    QVariant decorationAlignment = index.data(gidrDecorationAlignmentRole);

    if (!decorationAlignment.isValid() || decorationAlignment.isNull())
    {
        return;
    }

    const int nDecorationAlignment = decorationAlignment.toInt();

    if (nDecorationAlignment == 0)
    {
        return;
    }

    opt.decorationAlignment = Qt::Alignment(nDecorationAlignment);
}

bool GlodonDefaultItemDelegate::filterRightKeyPress(QWidget *editor, QKeyEvent *keyEvent)
{
    bool bNoModifierPressed = keyEvent->modifiers().testFlag(Qt::NoModifier);
    bool bShiftPressed = keyEvent->modifiers().testFlag(Qt::ShiftModifier);
    bool bAltPressed = keyEvent->modifiers().testFlag(Qt::AltModifier);
    bool bCtrlPressed = keyEvent->modifiers().testFlag(Qt::ControlModifier);

    if (bAltPressed)
    {
        keyEvent->accept();
    }
    else if (bNoModifierPressed)
    {
        if (editor)
        {
            try
            {
                editor->removeEventFilter(this);
                QCoreApplication::sendEvent(editor, keyEvent);
                editor->installEventFilter(this);
            }
            catch (...)
            {
                editor->installEventFilter(this);
            }
        }
    }
    else if (bCtrlPressed || bShiftPressed)
    {
        editor->removeEventFilter(this);
        QCoreApplication::sendEvent(editor, keyEvent);
        editor->installEventFilter(this);
    }

    return true;
}

bool GlodonDefaultItemDelegate::filterUpKeyPress(QWidget *editor, QKeyEvent *keyEvent)
{
    bool bAltPressed = keyEvent->modifiers().testFlag(Qt::AltModifier);
    bool bCtrlPressed = keyEvent->modifiers().testFlag(Qt::ControlModifier);
    if (bAltPressed)
    {
        //GLDAbstractButton的处理放到该控件的内部
        if (QComboBox *comboBox = dynamic_cast<QComboBox *>(editor))
        {
            comboBox->showPopup();
        }
        else if (QDateTimeEdit *dateTime = dynamic_cast<QDateTimeEdit *>(editor))
        {
            QMouseEvent *mouseEvent = new QMouseEvent(QEvent::MouseButtonPress,
                    QPointF(dateTime->width() - 10, dateTime->height() - 5),
                    Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            QApplication::sendEvent(dateTime, mouseEvent);
        }
        else if (GLDWindowComboBoxEx *comboBox = dynamic_cast<GLDWindowComboBoxEx *>(editor))
        {
            comboBox->showPopup();
        }

        keyEvent->accept();
        return true;
    }

    if (bCtrlPressed)
    {
        //do nothing
        return true;
    }

    try
    {
        bool bCanClose = commitDataAndCloseEditor(editor, SubmitModelCache);
        if (bCanClose)
        {
            QApplication::sendEvent(m_pTable, keyEvent);
        }
        else
        {
            setTextAllSelected(editor);
        }
    }
    catch (...)
    {
        setTextAllSelected(editor);
        throw;
    }

    return true;
}

bool GlodonDefaultItemDelegate::dealWithLoseFocus(QEvent *event, QWidget *pEditor)
{
    if (event->type() == QEvent::Hide && !pEditor->isWindow())
    {
        return false;
    }

    //the Hide event will take care of he editors that are in fact complete dialogs
    if ((QApplication::focusWidget() == pEditor) && pEditor->isActiveWindow())
    {
        return false;
    }

    QWidget *pFocusWidget = QApplication::focusWidget();
    while (pFocusWidget)
    {
        // don't worry about focus changes internally in the editor
        if (pFocusWidget == pEditor)
        {
            return false;
        }
        pFocusWidget = pFocusWidget->parentWidget();
    }

    if (!m_bCloseEditorOnFocusOut || !m_bCanHideEidtOnExit)
    {
        return false;
    }

    if (m_bRepeatCommit)
    {
        m_bRepeatCommit = false;
    }
    else
    {
        QFocusEvent *pFocusEvent = static_cast<QFocusEvent *>(event);
        if (!pFocusEvent)
        {
            return false;
        }

        if (m_bIgnoreActiveWindowFocusReason && pFocusEvent->reason() == Qt::ActiveWindowFocusReason)
        {
            return false;
        }

        if (m_pTable->isVisible())
        {
            return !commitDataAndCloseEditor(pEditor, NoHint);
        }
    }

    return false;
}

bool GlodonDefaultItemDelegate::dealWithShortCutOverride(QEvent *event)
{
    if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Escape)
    {
        event->accept();
        return true;
    }
    else
    {
        return false;
    }
}

QRect GlodonDefaultItemDelegate::calcRectByData(const QStyleOptionViewItem &option, const QModelIndex &index, int role) const
{
    Q_D(const GlodonDefaultItemDelegate);

    QVariant value = index.data(role);
    if (role == Qt::CheckStateRole)
        return doCheck(option, option.rect, value);

    if (value.isValid() && !value.isNull())
    {
        switch (value.type())
        {
        case QVariant::Invalid:
            break;
        case QVariant::Pixmap:
        {
            const QPixmap &pixmap = qvariant_cast<QPixmap>(value);
            return QRect(QPoint(0, 0), pixmap.size() / pixmap.devicePixelRatio() );
        }
        case QVariant::Image:
        {
            const QImage &image = qvariant_cast<QImage>(value);
            return QRect(QPoint(0, 0), image.size() /  image.devicePixelRatio() );
        }
        case QVariant::Icon:
        {
            QIcon::Mode mode = d->iconMode(option.state);
            QIcon::State state = d->iconState(option.state);
            QIcon icon = qvariant_cast<QIcon>(value);
            QSize size = icon.actualSize(option.decorationSize, mode, state);
            return QRect(QPoint(0, 0), size);
        }
        case QVariant::Color:
            return QRect(QPoint(0, 0), option.decorationSize);
        case QVariant::String:
        default:
        {
            QString text = GlodonDefaultItemDelegatePrivate::valueToText(value);
            value = index.data(Qt::FontRole);
            QFont fnt = qvariant_cast<QFont>(value).resolve(option.font);
            return textRectangle(textLayoutBounds(option), fnt, text);
        }
        }
    }
    return QRect();
}

QRect GlodonDefaultItemDelegate::textRectangle(const QRect &rect, const QFont &font, const QString &text) const
{
    Q_D(const GlodonDefaultItemDelegate);

    d->textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    d->textLayout.setTextOption(d->textOption);
    d->textLayout.setFont(font);
    d->textLayout.setText(GlodonDefaultItemDelegatePrivate::replaceNewLine(text));
    QSizeF fpSize = d->doTextLayout(rect.width());
    const QSize size = QSize(qCeil(fpSize.width()), qCeil(fpSize.height()));
    return QRect(0, 0, size.width(), size.height());
}

void GlodonDefaultItemDelegate::innerDoLayout(
        const QStyleOptionViewItem &option, QRect *checkRect, QRect *pixmapRect, QRect *textRect,
        bool hint) const
{
    Q_ASSERT(checkRect && pixmapRect && textRect);

    Q_D(const GlodonDefaultItemDelegate);

    const QWidget *widget = d->widget(option);
    QStyle *style = widget ? widget->style() : QApplication::style();
    const bool hasCheck = checkRect->isValid();
    const bool hasPixmap = pixmapRect->isValid();
    const bool hasText = textRect->isValid();
    const int pixmapMargin = hasPixmap ? style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1 : 0;
    const int checkMargin = hasCheck ? style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1 : 0;
    int x = option.rect.left();
    int y = option.rect.top();
    int w, h;

    if (textRect->height() == 0 && (!hasPixmap || !hint))
    {
        //if there is no text, we still want to have a decent height for the item sizeHint and the editor size
        textRect->setHeight(option.fontMetrics.height());
    }

    QSize pm(0, 0);
    if (hasPixmap)
    {
        pm = pixmapRect->size();
        pm.rwidth() += 2 * pixmapMargin;
    }
    if (hint)
    {
        h = qMax(checkRect->height(), qMax(textRect->height(), pm.height()));

        if (option.decorationPosition == QStyleOptionViewItem::Left
            || option.decorationPosition == QStyleOptionViewItem::Right)
        {
            w = textRect->width() + pm.width();
        }
        else
        {
            w = qMax(textRect->width(), pm.width());
        }
    }
    else
    {
        w = option.rect.width();
        h = option.rect.height();
    }

    int cw = 0;
    QRect check;
    if (hasCheck)
    {
        cw = checkRect->width() + 2 * checkMargin;
        if (hint) w += cw;

        if (option.direction == Qt::RightToLeft)
        {
            check.setRect(x + w - cw, y, cw, h);
        }
        else
        {
            check.setRect(x + checkMargin, y, cw, h);
        }
    }

    // at this point w should be the *total* width

    QRect display;
    QRect decoration;
    switch (option.decorationPosition)
    {
    case QStyleOptionViewItem::Top:
    {
        if (hasPixmap)
        {
            pm.setHeight(pm.height() + pixmapMargin); // add space
        }

        h = hint ? textRect->height() : h - pm.height();

        if (option.direction == Qt::RightToLeft)
        {
            decoration.setRect(x, y, w - cw, pm.height());
            display.setRect(x, y + pm.height(), w - cw, h);
        }
        else
        {
            decoration.setRect(x + cw, y, w - cw, pm.height());
            display.setRect(x + cw, y + pm.height(), w - cw, h);
        }
        break;
    }
    case QStyleOptionViewItem::Bottom:
    {
        if (hasText)
        {
            textRect->setHeight(textRect->height()); // add space
        }
        h = hint ? textRect->height() + pm.height() : h;

        if (option.direction == Qt::RightToLeft)
        {
            display.setRect(x, y, w - cw, textRect->height());
            decoration.setRect(x, y + textRect->height(), w - cw, h - textRect->height());
        }
        else
        {
            display.setRect(x + cw, y, w - cw, textRect->height());
            decoration.setRect(x + cw, y + textRect->height(), w - cw, h - textRect->height());
        }
        break;
    }
    case QStyleOptionViewItem::Left:
    {
        if (option.direction == Qt::LeftToRight)
        {
            decoration.setRect(x + cw, y, pm.width(), h);
            display.setRect(decoration.right() + 1, y, w - pm.width() - cw, h);
        }
        else
        {
            display.setRect(x, y, w - pm.width() - cw, h);
            decoration.setRect(display.right() + 1, y, pm.width(), h);
        }
        break;
    }
    case QStyleOptionViewItem::Right:
    {
        if (option.direction == Qt::LeftToRight)
        {
            display.setRect(x + cw, y, w - pm.width() - cw, h);
            decoration.setRect(display.right() + 1, y, pm.width(), h);
        }
        else
        {
            decoration.setRect(x, y, pm.width(), h);
            display.setRect(decoration.right() + 1, y, w - pm.width() - cw, h);
        }
        break;
    }
    default:
        qWarning("doLayout: decoration position is invalid");
        decoration = *pixmapRect;
        break;
    }

    if (!hint)
    { // we only need to do the internal layout if we are going to paint
        *checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                         checkRect->size(), check);
        *pixmapRect = QStyle::alignedRect(option.direction, option.decorationAlignment,
                                          pixmapRect->size(), decoration);
        // the text takes up all available space, unless the decoration is not shown as selected
        if (option.showDecorationSelected)
            *textRect = display;
        else
            *textRect = QStyle::alignedRect(option.direction, option.displayAlignment,
                                            textRect->size().boundedTo(display.size()), display);
    }
    else
    {
        *checkRect = check;
        *pixmapRect = decoration;
        *textRect = display;
    }
}

void GlodonDefaultItemDelegate::dealWithEscapeKeyPress(QWidget *editor)
{
    // don't commit data
    bool bCanCloseEditor = true;
    emit closeEditor(editor, bCanCloseEditor, GlodonDefaultItemDelegate::RevertModelCache);

    if (GLDTableView *pTable = dynamic_cast<GLDTableView *>(parent()))
    {
        pTable->resetEnterJumpPreState();
    }
}

bool GlodonDefaultItemDelegate::dealWithTabKeyPress(QWidget *editor)
{
    if (!commitDataAndCloseEditor(editor, EditNextItem))
    {
        setTextAllSelected(editor);
    }
    return true;
}

bool GlodonDefaultItemDelegate::dealWithBackTabPress(QWidget *editor)
{
    if (!commitDataAndCloseEditor(editor, EditPreviousItem))
    {
        setTextAllSelected(editor);
    }
    return true;
}

bool GlodonDefaultItemDelegate::filterKeyPress(QWidget *editor, QEvent *event)
{
    QKeyEvent *pKeyEvent = static_cast<QKeyEvent *>(event);
    switch (pKeyEvent->key())
    {
    case Qt::Key_Tab:
    {
        return dealWithTabKeyPress(editor);
    }
    case Qt::Key_Backtab:
    {
        return dealWithBackTabPress(editor);
    }
    case Qt::Key_Left:
    case Qt::Key_Right:
    {
        return filterRightKeyPress(editor, pKeyEvent);
    }
    case Qt::Key_Down:
    case Qt::Key_Up:
    {
        return filterUpKeyPress(editor, pKeyEvent);
    }
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        return filterReturnKeyPress(editor, pKeyEvent);
    }
    case Qt::Key_Escape:
    {
        dealWithEscapeKeyPress(editor);
        break;
    }
    case Qt::Key_F2:
    {
        cursorMoveTextEnd(editor);
        return true;
    }
    default:
        return false;
    }

    if (editor->parentWidget())
    {
        editor->parentWidget()->setFocus();
    }

    return true;
}

bool GlodonDefaultItemDelegate::filterReturnKeyPress(QWidget *editor, QKeyEvent *keyEvent)
{
    if (keyEvent)
    {
        bool bAltPressed = (Qt::ALT == (keyEvent->modifiers() & Qt::ALT));
        bool bControlPressed = (Qt::CTRL == (keyEvent->modifiers() & Qt::CTRL));

        // 当有按下alt和ctrl时，需要对回车键做特殊处理
        if (bAltPressed || bControlPressed)
        {
            return cursorPosInsertANewLine(editor);
        }
    }

    m_closeEditEvent = keyEvent;

#ifndef QT_NO_TEXTEDIT
    bool bstopEventPassOn = false;

    if (commitIfCanElseSelectAll<QLineEdit>(keyEvent, editor, bstopEventPassOn))
    {
        return bstopEventPassOn;
    }

    if (commitIfCanElseSelectAll<QPlainTextEdit>(keyEvent, editor, bstopEventPassOn))
    {
        return bstopEventPassOn;
    }

    if (commitIfCanElseSelectAll<GLDAbstractButtonEdit>(keyEvent, editor, bstopEventPassOn))
    {
        return bstopEventPassOn;
    }

    if (commitIfCanElseSelectAll<QSpinBox>(keyEvent, editor, bstopEventPassOn))
    {
        return bstopEventPassOn;
    }

    if (commitIfCanElseSelectAll<QDoubleSpinBox>(keyEvent, editor, bstopEventPassOn))
    {
        return bstopEventPassOn;
    }

    if (commitIfCanElseSelectAll<QComboBox>(keyEvent, editor, bstopEventPassOn))
    {
        return bstopEventPassOn;
    }

    if (commitIfCanElseSelectAll<QDateTimeEdit>(keyEvent, editor, bstopEventPassOn))
    {
        return bstopEventPassOn;
    }

    if (commitIfCanElseSelectAll<GLDWindowComboBoxEx>(keyEvent, editor, bstopEventPassOn))
    {
        return bstopEventPassOn;
    }

    if (dynamic_cast<QTextEdit *>(editor))
    {
        return false;    // don't filter enter key events for QTextEdit
    }

    // We want the editor to be able to process the key press
    // before committing the data (e.g. so it can do
    // validation/fixup of the input).
#endif // QT_NO_TEXTEDIT
#ifndef QT_NO_LINEEDIT

    if (QLineEdit *pEdit = dynamic_cast<QLineEdit *>(editor))
        if (!pEdit->hasAcceptableInput())
        {
            return false;
        }

#endif // QT_NO_LINEEDIT
    //            QMetaObject::invokeMethod(this, "_q_commitDataAndCloseEditor",
    //                                      Qt::QueuedConnection, Q_ARG(QWidget*, editor));
    //            return false;
    return !commitDataAndCloseEditor(editor, SubmitModelCache);
}

void GlodonDefaultItemDelegate::paintOther(QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    Q_D(const GlodonDefaultItemDelegate);
    Q_ASSERT(index.isValid());

    //TODO
    QStyleOptionViewItem opt = setOptions(index, option);

    // prepare
    painter->save();

    if (hasClipping())
    {
        painter->setClipRect(opt.rect);
    }

    // get the data and the rectangles
    QPixmap pixmap;
    QRect decorationRect;
    QVariant value = index.data(Qt::DecorationRole);

    if (value.isValid())
    {
        // ### we need the pixmap to call the virtual function
        pixmap = decoration(opt, value);
        opt.decorationPosition = (QStyleOptionViewItem::Position)index.data(Qt::DecorationPropertyRole).toInt();
        adjustDecorationAlignment(index, opt);

        if (value.type() == QVariant::Icon)
        {
            d->tmp.icon = qvariant_cast<QIcon>(value);
            d->tmp.mode = d->iconMode(option.state);
            d->tmp.state = d->iconState(option.state);
            const QSize c_size = d->tmp.icon.actualSize(option.decorationSize,
                                 d->tmp.mode, d->tmp.state);
            decorationRect = QRect(QPoint(0, 0), c_size);
        }
        else
        {
            d->tmp.icon = QIcon();
            decorationRect = QRect(QPoint(0, 0), pixmap.size());
        }
    }
    else
    {
        d->tmp.icon = QIcon();
        decorationRect = QRect();
    }


    QString text;
    QRect displayRect;
    if (m_oDisplayData.isValid() && !m_oDisplayData.isNull())
    {
        text = GlodonDefaultItemDelegatePrivate::valueToText(m_oDisplayData);
        displayRect = textRectangle(textLayoutBounds(opt), opt.font, text);
    }

    QRect checkRect;
    Qt::CheckState checkState = Qt::Unchecked;
    value = index.data(Qt::CheckStateRole);

    if (value.isValid())
    {
        checkState = static_cast<Qt::CheckState>(value.toInt());
        checkRect = doCheck(opt, opt.rect, value);
    }

    innerDoLayout(opt, &checkRect, &decorationRect, &displayRect, false);

    // draw the item
    drawBackground(painter, opt, index);

    drawEditStyleDraw(painter, opt, displayRect); // GLD

    drawCheck(painter, opt, checkRect, checkState);
    drawDecoration(painter, opt, decorationRect, pixmap);

    drawDiagonal(painter, displayRect, index); // GLD

    drawDisplay(painter, opt, displayRect, text, index);
    drawFocus(painter, opt, displayRect);

    // done
    painter->restore();
}

QSizeF GlodonDefaultItemDelegatePrivate::doTextLayout(int lineWidth) const
{
    Q_Q(const GlodonDefaultItemDelegate);
    qreal height = 0;
    qreal widthUsed = 0;
    textLayout.beginLayout();

    while (true)
    {
        QTextLine line = textLayout.createLine();

        if (!line.isValid())
        {
            break;
        }

        line.setLineWidth(lineWidth);
        line.setPosition(QPointF(0, height));

        height += line.height();

        if (q->isIncludeLeading())
        {
            if (false == line.leadingIncluded())
            {
                line.setLeadingIncluded(true);
                height += line.leading();
            }
        }

        widthUsed = qMax(widthUsed, line.naturalTextWidth());//  TODO line.naturalLineWidth() > widthUsed
    }

    textLayout.endLayout();

    return QSizeF(widthUsed, height);
}

QString GlodonDefaultItemDelegatePrivate::valueToText(const QVariant &value)
{
    QString text = value.toString();

    if (value.type() == QVariant::Time)
    {
        QTime time = value.value<QTime>();
        text = time.toString("hh:mm:ss");
    }
    else if (value.type() == QVariant::Date)
    {
        QDate date = value.value<QDate>();
        text = date.toString("yyyy-MM-dd");
    }
    else if (value.type() == QVariant::DateTime)
    {
        QDateTime datetime = value.value<QDateTime>();
        text = datetime.toString("yyyy-MM-dd");
    }
    else if (value.isNull() && (variantTypeIsByte(value.type()) || variantTypeIsShort(value.type())
                                         || variantTypeIsInt(value.type()) || variantTypeIsFloat(value.type())))
    {
        text = "";
    }

    return text;
}

bool GlodonDefaultItemDelegate::isEditing()
{
    return curEditor() != NULL;
}
