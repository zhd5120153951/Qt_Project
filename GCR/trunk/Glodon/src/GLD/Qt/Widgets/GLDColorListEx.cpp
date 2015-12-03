#include "GLDColorListEx.h"

#include "GLDWidget_Global.h"
#include "GLDShadow.h"
#include "GLDFileUtils.h"

#include <QApplication>
#include <QColorDialog>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QIcon>
#include <QListWidgetItem>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QPen>
#include <QPixmap>
#include <QSettings>
#include <QSize>
#include <QTextStream>
#include <QTranslator>
#include <QVBoxLayout>

// GLDColorBlock 常量
const int c_colorBlockWidth         = 16;
const int c_colorBlockHeight        = 16;
const int c_colorBtnWidth           = 24;       // 22 + 1 + 1
const int c_colorBtnHeight          = 24;       // 22 + 1 + 1

// GLDColorRow 常量
const int c_colorRowWidth           = 242;      // 5 + 22 * 9 + 4 * 8 + 5
const int c_colorRowHeight          = 26;       // 16 + 8 + 2
const int c_colorBlockSpacing       = 4;        // 10 - 3 - 3
const int c_colorRowLeftMargin      = 4;
const int c_colorRowRightMargin     = 6;

// 最近使用色所在的行号
const int c_recentColorTableRow     = 2;
const int c_blockColorDeviation     = 50;

// GLDColorTable 常量
const int c_tabTitleLeftMargin      = 8;
const int c_tabTitleTopMargin       = 5;
const int c_tabTitleBottomMargin    = 2;
const int c_tabRowSpacing           = 4;
const int c_tabRowBottomMargin      = 4;
const int c_tabTopMargin            = 2;
const int c_tabTitleFontSize        = 12;

// GColorListEx 常量
const int c_colorColumnCount        = 9;
const int c_grayColorRowCount       = 1;
const int c_standardColorRowCount   = 1;
const int c_themeColorRowCount      = 5;
const int c_edgeColorRowCount       = 7;        // 1 + 1 + 5
const int c_listWidgetCount         = 4;
const int c_listWidgetMargin        = 2;
const int c_listWidgetHeight        = 284;
const int c_grayTableMargin         = 7;
const int c_grayTableHeight         = 34;
const int c_standardTabMargin       = 4;
const int c_standardTabHeight       = 54;
const int c_recentColorTableHeight  = 32;
const int c_themeTabHeight          = 164;
const int c_moreBtnHeight           = 30;
const int c_moreBtnTop              = 5;
const int c_moreBtnBottom           = 10;

const int c_yStartPos               = 11;
const int c_blockHeightInEdit       = 4;

const QString c_recentColorConfPath = "/GLDRes/colorTempConf.txt";

const char *c_hu                    = QT_TRANSLATE_NOOP("QColorDialog", "Hu&e:");                 // 色调(&E):
const char *c_sat                   = QT_TRANSLATE_NOOP("QColorDialog", "&Sat:");                 // 饱和度(&S):
const char *c_val                   = QT_TRANSLATE_NOOP("QColorDialog", "&Val:");                 // 亮度(&V):
const char *c_red                   = QT_TRANSLATE_NOOP("QColorDialog", "&Red:");                 // 红色(&R):
const char *c_green                 = QT_TRANSLATE_NOOP("QColorDialog", "&Green:");               // 绿色(&G):
const char *c_blue                  = QT_TRANSLATE_NOOP("QColorDialog", "Bl&ue:");                // 蓝色(&U):
const char *c_alphaChannel          = QT_TRANSLATE_NOOP("QColorDialog", "A&lpha channel:");       // Alpha通道(&A):
const char *c_selectColor           = QT_TRANSLATE_NOOP("QColorDialog", "Select Color");          // 选择颜色
const char *c_basicColors           = QT_TRANSLATE_NOOP("QColorDialog", "&Basic colors");         // 基本颜色(&B)
const char *c_customColors          = QT_TRANSLATE_NOOP("QColorDialog", "&Custom colors");        // 自定义颜色(&C)
const char *c_addToCustomColors     = QT_TRANSLATE_NOOP("QColorDialog", "&Add to Custom Colors"); // 添加到自定义颜色(&A)
const char *c_pickScreenColor       = QT_TRANSLATE_NOOP("QColorDialog", "Pick Screen Color");     // 获取屏幕颜色
const char *c_okBtn                 = QT_TRANSLATE_NOOP("QDialogButtonBox", "&OK");               // 确定
const char *c_cancelBtn             = QT_TRANSLATE_NOOP("QDialogButtonBox", "&Cancel");           // 取消
const char *c_moreColorBtn          = QT_TRANSLATE_NOOP("GColorListEx", "More Color(w)...");      // 更多颜色按键上的Text
const char *c_moreColorTitle        = QT_TRANSLATE_NOOP("GColorListEx", "More Color");            // 更多颜色对话框的标题
const char *c_themeColor            = QT_TRANSLATE_NOOP("GColorListEx", "theme color");           // 主题颜色
const char *c_commonlyUsedColor     = QT_TRANSLATE_NOOP("GColorListEx", "commonly used color");   // 主题颜色

const GString c_sColorListQssFile = ":/qsses/GColorListEx.qss";

GLDColorBlock::GLDColorBlock(const QColor &color, const QColor &edgeColor, QWidget *parent) :
    QPushButton(parent),
    m_color(color),
    m_edgeColor(edgeColor)
{
    setProperty("GLDColorBlock", true);
    drawColorBlock();
}

void GLDColorBlock::updateColor(const QColor &color, const QColor &edgeColor)
{
    m_color = color;
    m_edgeColor = edgeColor;
    drawColorBlock();
}

void GLDColorBlock::drawColorBlock()
{
    QPixmap pixColor(c_colorBlockWidth, c_colorBlockHeight);
    QPainter painter(&pixColor);
    QPen pen = painter.pen();

    pen.setColor(m_color);
    pen.setWidth(c_colorBlockWidth * 2);
    painter.setPen(pen);
    painter.drawPoint(0, 0);

    // 处理第一个颜色块为透明,画红色线条
    if (m_color.alpha() == 0)
    {

        painter.setPen(QPen(QColor(Qt::red), 1));
        painter.drawLine(c_colorBlockWidth - 1, 1, 1, c_colorBlockWidth - 1);
    }

    painter.setPen(QPen(QColor(m_edgeColor), 1));
    painter.drawRect(0, 0, c_colorBlockWidth - 1, c_colorBlockWidth - 1);

    QIcon iconColor(pixColor);
    setIcon(iconColor);
    setIconSize(QSize(c_colorBlockWidth, c_colorBlockWidth));
    setFixedSize(c_colorBtnWidth, c_colorBtnHeight);
    connect(this, &GLDColorBlock::clicked, this, &GLDColorBlock::onClick);
}

/**
 *  颜色块的点击事件
 */
void GLDColorBlock::onClick()
{
    emit colorValue(m_color, m_edgeColor);
    emit colorBlock(this);
    emit commitSelectedColor(m_color, m_edgeColor);
    setStyleSheet("border: 1px solid #39a9d1; border-radius: 2px; background-color: #dbf3fc;");
}

/**
  * GColorListEx class 构造函数
 */
GColorListEx::GColorListEx(QWidget *parent, QColor color ,bool isLoadRecentColor) :
    QComboBox(parent),
    m_comboBoxWidget(NULL),
    m_userColor(QColor("#ffffff")),
    m_curColor(color),
    m_recentColorTable(NULL),
    m_recentUseColorCount(0),
    m_isLoadRecentColor(isLoadRecentColor),
    m_isShowRGBColor(false),
    m_isShowRGBStr(false),
    m_iconUrlInEdit(":/icons/GColorListEx-backgroundColor.png")
{
    setObjectName("GColorListEx");
    loadColorFile();
    // 判断是否从临时配置文件中加载最近使用的颜色
    if (m_isLoadRecentColor)
    {
        loadSelectedMoreColor();
    }
    initColorComboBox();
    setModel(m_comboBoxWidget->model());
    setView(m_comboBoxWidget);
    updateCurrentColor();
    setHasBorder(true);
    this->setStyleSheet(loadQssFile(":/qsses/GColorListEx.qss"));
    GLDShadow *pShadow = new GLDShadow(this);
    pShadow->removeShadow();
}

GColorListEx::~GColorListEx()
{
    freeAndNil(m_comboBoxWidget);
}

/**
 * @brief GColorListEx::setHasBorder 设置GColorListEx有无边框
 * @param flag  false-无边框  true-有边框
 */
void GColorListEx::setHasBorder(bool bHasBorder)
{
    if (bHasBorder)
    {
        setProperty("ComboBoxBorder", true);
    }
    else
    {
        setProperty("ComboBoxBorder", false);
    }

    this->setStyleSheet(loadQssFile(c_sColorListQssFile));
}

GLDColorBlock *GColorListEx::findColorBlock(const QColor &color)
{
    GLDColorBlock *findResult = NULL;

    foreach (GLDColorTable *colorTable, m_colorTableList)
    {
        findResult = colorTable->findColorBlock(color);

        if (NULL != findResult)
        {
            break;
        }
    }

    return findResult;
}

QColor GColorListEx::currentColor() const
{
    return m_curColor;
}

void GColorListEx::setCurrentColor(const QColor color)
{
    m_curColor = color;
    GLDColorBlock *findBlock = findColorBlock(color);

    if (NULL == findBlock)
    {
        m_userColor = color;
        addRecentColorList();
    }

    emit colorIndexChange(color);
}

bool GColorListEx::isLoadRecentColor() const
{
    return m_isLoadRecentColor;
}

void GColorListEx::setIsLoadRecentColor(const bool isLoadRecentColor)
{
    if (!isLoadRecentColor)
    {
        removeRecentColorConfFile();
    }

    m_isLoadRecentColor = isLoadRecentColor;
}

bool GColorListEx::isShowRGBColor() const
{
    return m_isShowRGBColor;
}

void GColorListEx::setIsShowRGBColor(const bool isShowColorRGB)
{
    m_isShowRGBColor = isShowColorRGB;

    if (isShowColorRGB)
    {
        if (!m_blockColors.isEmpty())
        {
            // 不显示第一个透明颜色，所以从1开始
            for (int i = 1; i < m_blockColors.size(); ++i)
            {
                GLDColorBlock *pColorBlock = findColorBlock(m_blockColors.at(i));

                if (pColorBlock)
                {
                    pColorBlock->setToolTip(m_blockColors.at(i).name());
                }
            }
        }
    }
}

bool GColorListEx::isShowRGBStr() const
{
    return m_isShowRGBStr;
}

void GColorListEx::setIsShowRGBStr(bool isShowRGBStr)
{
    m_isShowRGBStr = isShowRGBStr;
}

QString GColorListEx::iconUrlInEdit() const
{
    return m_iconUrlInEdit;
}

void GColorListEx::setIconUrlInEdit(const QString iconUrl)
{
    m_iconUrlInEdit = iconUrl;
    updateCurrentColor();
}

/**
 *  画颜色块并显示到GColorListEx的编辑框中
 */
void GColorListEx::drawColor(const QColor &color, const QColor &edgeColor)
{
    m_curColor = color;
//    QPixmap pixColor(c_colorBlockWidth, c_colorBlockHeight);
    QPixmap pixColor(m_iconUrlInEdit);

    QPainter painter(&pixColor);
    QImage image(m_iconUrlInEdit);
    painter.drawImage(QRect(0, 0, c_colorBlockWidth, c_colorBlockHeight), image);

    painter.fillRect(0, c_yStartPos, c_colorBlockWidth, c_blockHeightInEdit + 1, color);
    //分透明色需要画红线,其他色不需要
    if (color.alpha() == 0)
    {
        painter.setPen(QPen(QColor(Qt::red), 1));
        painter.drawLine(c_colorBlockWidth - 1, c_yStartPos + 1, 1, c_yStartPos + 4);
    }
    else
    {
        if (m_isShowRGBColor)
        {
            setToolTip(color.name());
        }
    }

    setCurrentColor(color);

    painter.setPen(QPen(QColor(edgeColor), 1));
    painter.drawRect(0, c_yStartPos, c_colorBlockWidth - 1, c_blockHeightInEdit);

    QIcon iconColor(pixColor);
    QListWidgetItem * item = m_comboBoxWidget->item(0);
    item->setIcon(iconColor);
    m_comboBoxWidget->setIconSize(QSize(0, 0));
    setCurrentIndex(0);
    hidePopup();
}

void GColorListEx::moreColor()
{
    // 更多颜色对话框汉化
    QColor originalColor = QColor(Qt::gray);
    QColor color = QColorDialog::getColor(currentColor(), this, tr("More Color"));

    if (color.isValid())
    {
        GLDColorBlock* selectColorBlock = findColorBlock(color);
        if (NULL != selectColorBlock)
        {
            selectColorBlock->onClick();
        }
        else
        {
            setCurrentColor(color);
            updateRecentColorTable();
            if (m_isLoadRecentColor)
            {
                saveSelectedMoreColor();
            }
        }

        drawColor(color, originalColor);
    }

    emit colorIndexChange(color);
}

void GColorListEx::currentSelectedBlock(GLDColorBlock *block)
{
    m_originalBlock = m_currentBlock;
    m_currentBlock = block;
    m_originalBlock->setStyleSheet(NULL);
}

QColor GColorListEx::getColor(const QString sVal)
{
    if (sVal.isNull() || !QColor(sVal).isValid())
    {
        return QColor(Qt::white);
    }
    else if (sVal.length() > 7 && sVal.endsWith("00"))
    {
        return QColor(255, 255, 255, 0);
    }
    else
    {
        return QColor(sVal);
    }
}

/**
 *  添加GLDColorTable到listWidget
 */
void GColorListEx::addTable(GLDColorTable *table, int tableWid, int tableHeight)
{
    table->initTableLayout();
    m_colorTableList.append(table);
    connect(table, &GLDColorTable::currentSelectedColor,
            this, &GColorListEx::currentSelectedColor);
    connect(table, &GLDColorTable::currentSelectedColor,
            this, &GColorListEx::drawColor);
    connect(table, &GLDColorTable::currentSelectedBlock,
            this, &GColorListEx::currentSelectedBlock);
    connect(table, SIGNAL(commitSelectedColor(QColor, QColor)),
            this, SLOT(commitSelectedColor(QColor, QColor)));
    QListWidgetItem *item = new QListWidgetItem(m_comboBoxWidget);
    item->setSizeHint(QSize(tableWid, tableHeight));
    m_comboBoxWidget->setItemWidget(item, table);
    m_comboBoxWidget->addItem(item);
}

void GColorListEx::addThemeColorTable()
{
    GLDColorTable *pThemeColorTable = NULL;

    const QList<QColor> blokColors = m_blockColors.mid((c_grayColorRowCount + c_standardColorRowCount)* c_colorColumnCount, c_colorColumnCount);
    const QList<QColor> edgeColors = m_edgeColors.mid((c_grayColorRowCount + c_standardColorRowCount) * c_colorColumnCount, c_colorColumnCount);

    pThemeColorTable  = new GLDColorTable(blokColors, edgeColors, this);
    pThemeColorTable->setContentsMargins(0, 0, 0, 0);
    pThemeColorTable->addTitle(tr("theme color"));

    for (int i = 1; i < c_themeColorRowCount; ++i)
    {
        pThemeColorTable->appendColorRow(
        m_blockColors.mid((c_grayColorRowCount + c_standardColorRowCount + i) * c_colorColumnCount, c_colorColumnCount),
        m_edgeColors.mid((c_grayColorRowCount + c_standardColorRowCount + i) * c_colorColumnCount, c_colorColumnCount));
    }

    addTable(pThemeColorTable, c_colorRowWidth, c_themeTabHeight);
}

void GColorListEx::addGrayColorTable()
{
    GLDColorTable *pGrayColorTable = NULL;
    pGrayColorTable = new GLDColorTable(m_blockColors.mid(0, c_colorColumnCount),
                  m_edgeColors.mid(0, c_colorColumnCount), this);

    //初始化前一个选中颜色块和当前选中颜色块, 以增加选中效果
    m_originalBlock = pGrayColorTable->initBlock();
    m_currentBlock = pGrayColorTable->initBlock();
    m_currentBlock->setStyleSheet("border: 1px solid #39a9d1; border-radius: 2px; background-color: #dbf3fc;");

    pGrayColorTable->setProperty("GLDGrayColorTable", true);
    pGrayColorTable->setContentsMargins(0, 0, 0, c_grayTableMargin);
    addTable(pGrayColorTable, c_colorRowWidth, c_grayTableHeight);
}

void GColorListEx::addMoreButton()
{
    QPushButton *pMoreBtn = new QPushButton(this);
    pMoreBtn->setText(tr("More Color(w)..."));
    pMoreBtn->setProperty("moreBtn", true);

    QHBoxLayout *pBtnLayout = new QHBoxLayout(m_comboBoxWidget);
    pBtnLayout->setContentsMargins(0, c_moreBtnTop, 0, c_moreBtnBottom);

    QListWidgetItem *pMoreColorItem = new QListWidgetItem(m_comboBoxWidget);
    pMoreColorItem->setSizeHint(QSize(c_colorRowWidth, c_moreBtnHeight));

    m_comboBoxWidget->setLayout(pBtnLayout);
    m_comboBoxWidget->setItemWidget(pMoreColorItem, pMoreBtn);
    m_comboBoxWidget->addItem(pMoreColorItem);

    connect(pMoreBtn, &QPushButton::clicked, this, &GColorListEx::moreColor);
}

void GColorListEx::addStandardColorTable()
{
    GLDColorTable *standardColorTable =
            new GLDColorTable(m_blockColors.mid(c_grayColorRowCount * c_colorColumnCount, c_colorColumnCount),
            m_edgeColors.mid(c_grayColorRowCount * c_colorColumnCount, c_colorColumnCount), this);
    standardColorTable->addTitle(tr("commonly used color"));
    standardColorTable->setContentsMargins(0, 0, 0, c_standardTabMargin);
    addTable(standardColorTable, c_colorRowWidth, c_standardTabHeight);
}

void GColorListEx::addRecentColorTable()
{
    for (int i = 0; i < c_colorColumnCount; ++i)
    {
        // 从临时文件中加载，此时 m_recentUseColor 不为空， 计算颜色块的边界颜色,否则，添加默认颜色
        if (m_recentUseColor.count() > 0 && isLoadRecentColor())
        {
            if (i < m_recentUseColorCount)
            {
                m_recentUseEdgeColor.append(getEdgeColor(m_recentUseColor.at(i)));
            }
            else
            {
                m_recentUseEdgeColor.append(QColor(Qt::white));
            }
        }
        else
        {
            m_recentUseColor.append(QColor("#ffffff"));
            m_recentUseEdgeColor.append(QColor(Qt::white));
        }
    }

    m_recentColorTable = new GLDColorTable(m_recentUseColor, m_recentUseEdgeColor, this);
    m_recentColorTable->setContentsMargins(0, 0, 0, 0);

    // 从临时文件中加载，改变下拉框的高度，以显示最近使用色，否则隐藏最近使用色
    if (isLoadRecentColor())
    {
        m_comboBoxWidget->setFixedHeight(c_listWidgetHeight + c_recentColorTableHeight);
        addTable(m_recentColorTable, c_colorRowWidth , c_recentColorTableHeight);
    }
    else
    {
        addTable(m_recentColorTable, c_colorRowWidth , 0);
    }
}

QColor GColorListEx::getEdgeColor(const QColor &color)
{
    QColor result(Qt::white);

    int nRed = color.red() - c_blockColorDeviation;
    if (nRed < 0)
    {
        nRed = 0;
    }

    int nGreen = color.green() - c_blockColorDeviation;
    if (nGreen < 0)
    {
        nGreen = 0;
    }

    int nBlue = color.blue() - c_blockColorDeviation;
    if (nBlue < 0)
    {
        nBlue = 0;
    }

    result.setRgb(nRed, nGreen, nBlue);
    return result;
}

void GColorListEx::updateRecentColorTable()
{
    QListWidgetItem *item = m_comboBoxWidget->item(c_recentColorTableRow);

    if (0 == item->sizeHint().height())
    {
        item->setSizeHint(QSize(c_colorRowWidth, c_recentColorTableHeight));
        m_comboBoxWidget->setFixedHeight(c_listWidgetHeight + c_recentColorTableHeight);
        QWidget *parentWidget = m_comboBoxWidget->parentWidget();
        if (NULL != parentWidget)
        {
            m_comboBoxWidget->parentWidget()->setFixedHeight(c_listWidgetHeight + c_recentColorTableHeight);
        }
    }

    for (int i = 0; i < m_recentUseColorCount; ++i)
    {
        QColor color = m_recentUseColor.at(i);
        m_recentUseEdgeColor[i] = getEdgeColor(color);
    }

    m_recentColorTable->updateColorRow(0, m_recentUseColor, m_recentUseEdgeColor,
                                       m_recentUseColorCount);
}

/**
 *  初始化GColorListEx
 */
void GColorListEx::initColorComboBox()
{
    // 设置ListWidget的样式
    initPopUpWidget();
    // 灰度颜色行
    addGrayColorTable();
    // 标准颜色行
    addStandardColorTable();
    // 最近使用颜色
    addRecentColorTable();
    // 主题颜色行
    addThemeColorTable();
    // 更多 Button
    addMoreButton();
}

void GColorListEx::initPopUpWidget()
{
    m_comboBoxWidget = new QListWidget(this);
    m_comboBoxWidget->setContentsMargins(0, 0, 0, c_listWidgetMargin);
    m_comboBoxWidget->setFixedSize(c_colorRowWidth + c_listWidgetMargin, c_listWidgetHeight);
    m_comboBoxWidget->setSelectionMode(QAbstractItemView::NoSelection);
    m_comboBoxWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_comboBoxWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void GColorListEx::addRecentColorList()
{
    if (!m_recentUseColor.contains(m_userColor))
    {
        if (m_recentUseColorCount >= 9)
        {
            m_recentUseColor.removeLast();
        }
        else
        {
            ++m_recentUseColorCount;
            if (m_recentUseColor.count() >= 9)
            {
                m_recentUseColor.removeLast();
            }
        }
    }
    else
    {
        int existIndex = m_recentUseColor.indexOf(m_userColor);
        m_recentUseColor.removeAt(existIndex);
    }

    m_recentUseColor.insert(0, m_userColor);
}

void GColorListEx::loadDefaultConfigColorVal()
{
    m_blockColors << getColor("#ffffff00") << getColor("#d7d2d1") << getColor("#c2c2c2")
                  << getColor("#c2c2c2") << getColor("#86848c") << getColor("#707070")
                  << getColor("#5f5f5f") << getColor("#4f4e4e") << getColor("#000000")

                  << getColor("#ff010f") << getColor("#f9865a") << getColor("#fedf7d")
                  << getColor("#91d151") << getColor("#37b64a") << getColor("#00b1f0")
                  << getColor("#0171c0") << getColor("#005c7e") << getColor("#662d91")

                  << getColor("#ffffff") << getColor("#ffd5aa") << getColor("#fcfecc")
                  << getColor("#daffeb") << getColor("#a5e0dd") << getColor("#c9fefd")
                  << getColor("#c6aae0") << getColor("#f1b097") << getColor("#ffabfe")

                  << getColor("#d8d8d8") << getColor("#feac54") << getColor("#ffff00")
                  << getColor("#d1ebb7") << getColor("#96e2dd") << getColor("#9fe0ef")
                  << getColor("#bd99de") << getColor("#f38871") << getColor("#b983f0")

                  << getColor("#bfbfbf") << getColor("#fe8100") << getColor("#facb3a")
                  << getColor("#7ec377") << getColor("#16c0ba") << getColor("#4bc2e5")
                  << getColor("#9c7bbd") << getColor("#fb6f6b") << getColor("#8560a8")

                  << getColor("#a5a5a5") << getColor("#bf6100") << getColor("#feb500")
                  << getColor("#54c665") << getColor("#2e9c94") << getColor("#00aeee")
                  << getColor("#7c589d") << getColor("#f31f17") << getColor("#6c4d8c")

                  << getColor("#7f7f7f") << getColor("#7a5102") << getColor("#e9a61e")
                  << getColor("#2d9c48") << getColor("#00746a") << getColor("#0075ab")
                  << getColor("#662b90") << getColor("#9c450b") << getColor("#662d91");

    m_edgeColors << getColor("#979797") << getColor("#c2c2c2") << getColor("#a4a4a1")
                 << getColor("#868d83") << getColor("#707070") << getColor("#464646")
                 << getColor("#262626") << getColor("#000000") << getColor("#000000")

                 << getColor("#a7302f") << getColor("#c95f26") << getColor("#d5a927")
                 << getColor("#00b150") << getColor("#187c2f") << getColor("#0075ab")
                 << getColor("#005c7e") << getColor("#0c4758") << getColor("#551a83")

                 << getColor("#e2e5e7") << getColor("#feac56") << getColor("#e6ea89")
                 << getColor("#a9e4a9") << getColor("#75ce9f") << getColor("#6ed6f4")
                 << getColor("#c7aade") << getColor("#dd6045") << getColor("#ff57ff")

                 << getColor("#bfbfbf") << getColor("#ff8000") << getColor("#e6e873")
                 << getColor("#92d14f") << getColor("#00aa9d") << getColor("#01bff6")
                 << getColor("#8560a8") << getColor("#de5f49") << getColor("#aa58ff")

                 << getColor("#a5a5a5") << getColor("#bf6001") << getColor("#ff9100")
                 << getColor("#37b64a") << getColor("#289f96") << getColor("#30a8d5")
                 << getColor("#8560a8") << getColor("#f31f19") << getColor("#8361a8")

                 << getColor("#7f7f7f") << getColor("#bf6100") << getColor("#cb883c")
                 << getColor("#2c9e3d") << getColor("#1e9087") << getColor("#0075ab")
                 << getColor("#652c90") << getColor("#ec6c64") << getColor("#642c90")

                 << getColor("#626262") << getColor("#803f00") << getColor("#f8b258")
                 << getColor("#187c2f") << getColor("#005b51") << getColor("#005c7e")
                 << getColor("#532f6e") << getColor("#ba1512") << getColor("#450f60");
}

void GColorListEx::loadSelectedMoreColor()
{
    QString tempDir = QDir::tempPath() + c_recentColorConfPath;
    QFile file(tempDir);

    if (!file.open(QFile::ReadOnly))
    {
        file.close();
        return;
    }

    QString strSelectColor = QString::fromLatin1(file.readAll());
    QStringList selectColorList = strSelectColor.split(",");

    if (!selectColorList.empty())
    {
        foreach (QString str, selectColorList)
        {
            if (m_recentUseColor.count() < 9)
            {
                if ("#ffffff" != str)
                {
                    ++m_recentUseColorCount;
                }
                m_recentUseColor.append(QColor(str));
            }
        }
    }

    file.close();
}

void GColorListEx::saveSelectedMoreColor()
{
    // 获取临时文件路径
    QString tempDir = QDir::tempPath() + c_recentColorConfPath;
    QFile file(tempDir);

    if (!file.exists())
    {
        QDir::temp().mkdir("GLDRes");
    }
    file.setFileName(tempDir);
    // 以覆盖之前的记录打开
    if (!file.open(QFile::ReadWrite | QFile::Truncate))
    {
        file.close();
        return;
    }

    QTextStream textout(&file);
    // 加载已选的更多颜色到临时文件中
    foreach (QColor recentColor, m_recentUseColor)
    {
        textout << recentColor.name() + ",";
    }
    file.close();
}

void GColorListEx::removeRecentColorConfFile()
{
    QString tempDir = QDir::tempPath() + c_recentColorConfPath;
    if (!tempDir.isEmpty())
    {
        QFile file(tempDir);
        if (file.exists())
        {
            QFile::remove(tempDir);
        }
    }
}

void GColorListEx::updateCurrentColor()
{
    GLDColorBlock *initBlock = findColorBlock(m_curColor);
    if (NULL != initBlock)
    {
        drawColor(initBlock->color(), initBlock->edgeColor());
        initBlock->onClick();
    }
    else
    {
        m_userColor = m_curColor;
        addRecentColorList();
        updateRecentColorTable();
        drawColor(m_curColor, getEdgeColor(m_curColor));
    }
}

/**
 *  加载GColorListEx.ini配置文件(配置了颜色板中的颜色值)
 */
void GColorListEx::loadColorFile(const QString &fileName)
{
    if (fileName.isEmpty() || !fileExists(fileName))
    {
        // 加载默认颜色值
        loadDefaultConfigColorVal();
        return;
    }

    QSettings *pSetting = new QSettings(fileName, QSettings::IniFormat);
    if (NULL == pSetting)
    {
        delete pSetting;
        return;
    }

    for (int i = 0; i < c_edgeColorRowCount * c_colorColumnCount; ++i)
    {
        QString strColorkey = "blockColor/blockColor" + QString("%1").arg(i);
        m_blockColors.append(getColor(pSetting->value(strColorkey).toString()));
    }

    for (int i = 0; i < c_edgeColorRowCount * c_colorColumnCount; ++i)
    {
        QString strColorkey = "edgeColor/edgeColor" + QString("%1").arg(i);
        m_edgeColors.append(getColor(pSetting->value(strColorkey).toString()));
    }

    delete pSetting;
}

void GColorListEx::commitSelectedColor(const QColor &color, const QColor &edgeColor)
{
    int index;
    foreach (GLDColorTable * colorTable, m_colorTableList)
    {
        index = colorTable->findColorBlockIndex(color, edgeColor);
        if (index != -1)
        {
            emit colorBlockClicked(index);
        }
    }
}

GLDColorRow::GLDColorRow(const QList<QColor> &colors, const QList<QColor> &edgeColors, QWidget *parent) :
    QWidget(parent)
{
    initColorBlocks(colors, edgeColors);
    initColorBlocksLayout();
    setFixedWidth(c_colorRowWidth);
    setFixedHeight(c_colorRowHeight);
}

/**
 *  设置行的背景颜色
 */
void GLDColorRow::setBackgroundColor(QColor &color)
{
    setAutoFillBackground(true);
    QPalette oPalette;
    oPalette.setColor(QPalette::Background, color);
    setPalette(oPalette);
}

void GLDColorRow::updateColorBlocks(QList<QColor> &colors, QList<QColor> &edgeColors,
                                    int updateColorCount)
{
    for (int i = 0; i < m_colorBlocks.count(); ++i)
    {
        GLDColorBlock *pColorBlock = m_colorBlocks.at(i);
        // 初始设置每个块disable为false, 可以hover
        pColorBlock->setDisabled(false);
        pColorBlock->updateColor(colors.at(i), edgeColors.at(i));
        // 当超过更多颜色中添加颜色， 设置后面的颜色块disable
        if (i >= updateColorCount)
        {
            pColorBlock->setDisabled(true);
        }
        // 设置此行(常用色第二行, 即从系统颜色框中选择的颜色)最左边的为最新选择色
        if (0 == i)
        {
            pColorBlock->onClick();
        }
    }
}

GLDColorBlock* GLDColorRow::initBlock()
{
    return m_colorBlocks.last();
}

int GLDColorRow::findColorBlockIndex(const QColor color)
{
    int index = -1;
    for (int i = 0; i < m_colorBlocks.size(); i++)
    {
        if (color.name() == m_colorBlocks.at(i)->color().name())
        {
            index = i;
            break;
        }
    }
    return index;
}

GLDColorBlock *GLDColorRow::findColorBlock(const QColor color)
{
    GLDColorBlock *findBlock = NULL;
    foreach (GLDColorBlock *colorBlock, m_colorBlocks)
    {
        if (color.name() == colorBlock->color().name())
        {
            findBlock = colorBlock;
            break;
        }
    }
    return findBlock;
}

/**
 *  初始化颜色Row
 */
void GLDColorRow::initColorBlocks(const QList<QColor> &colors, const QList<QColor> &edgeColors)
{
    for (int i = 0; i < colors.count(); ++i)
    {
        GLDColorBlock *pColorBlock = new GLDColorBlock(colors.at(i), edgeColors.at(i), this);

        connect(pColorBlock, &GLDColorBlock::colorValue, this,
                &GLDColorRow::currentSelectedColor);
        connect(pColorBlock, &GLDColorBlock::colorBlock, this,
                &GLDColorRow::currentSelectedBlock);
        connect(pColorBlock, &GLDColorBlock::commitSelectedColor, this,
                &GLDColorRow::commitSelectedColor);
        pColorBlock->setProperty("colorBlock", true);
        m_colorBlocks.append(pColorBlock);
    }
}

void GLDColorRow::initColorBlocksLayout()
{
    QHBoxLayout *pLayout = new QHBoxLayout;
    pLayout->setSpacing(c_colorBlockSpacing);
    pLayout->setAlignment(Qt::AlignTop);
    pLayout->setContentsMargins(c_colorRowLeftMargin, 0, c_colorRowRightMargin, 0);
    setLayout(pLayout);
    foreach (GLDColorBlock *pColorBlock, m_colorBlocks)
    {
        pLayout->addWidget(pColorBlock);
    }
}

/**
  * GLDColorTableTitle
*/
GLDColorTableTitle::GLDColorTableTitle(QWidget *parent) :
    QPushButton(parent)
{
    setProperty("GLDColorLabel", true);
}

/**
  * GLDColorTable
 */
GLDColorTable::GLDColorTable(const QList<QColor> &firstRowColors,
                             const QList<QColor> &firstRowEdgeColors, QWidget *parent) :
    QWidget(parent),
    m_title(NULL),
    m_BackColor(Qt::white)
{
    setBackgroundColor(m_BackColor);
    GLDColorRow *pFirstRow = new GLDColorRow(firstRowColors, firstRowEdgeColors, this);
    m_colorRows.append(pFirstRow);
    connect(pFirstRow, &GLDColorRow::currentSelectedColor,
            this, &GLDColorTable::currentSelectedColor);
    connect(pFirstRow, &GLDColorRow::currentSelectedBlock,
            this, &GLDColorTable::currentSelectedBlock);
    connect(pFirstRow, &GLDColorRow::commitSelectedColor,
            this, &GLDColorTable::commitSelectedColor);
}

GLDColorTable::~GLDColorTable()
{
    freeAndNil(m_title);
}

/**
 *  设置颜色Table的背景颜色
 */
void GLDColorTable::setBackgroundColor(QColor &color)
{
    setAutoFillBackground(true);
    QPalette oPalette;
    oPalette.setColor(QPalette::Background, color);
    setPalette(oPalette);
}

GLDColorBlock* GLDColorTable::initBlock()
{
    return m_colorRows.first()->initBlock();
}

GLDColorBlock *GLDColorTable::findColorBlock(const QColor &color)
{
    GLDColorBlock *findResult = NULL;
    foreach (GLDColorRow *colorRow, m_colorRows)
    {
        findResult = colorRow->findColorBlock(color);
        if (NULL != findResult)
        {
            break;
        }
    }
    return findResult;
}

/**
 * @brief GLDColorTable::mouseReleaseEvent 截获鼠标释放事件,
 *        不然点击到button之外的地方下拉框会收起
 * @param e
 */
void GLDColorTable::mouseReleaseEvent(QMouseEvent *e)
{
   e->accept();
}

/**
 *  对颜色Table添加标题
 */
void GLDColorTable::addTitle(const QString &title)
{
    m_title = new GLDColorTableTitle(this);
    m_title->setText(title);
}

/**
 *  向颜色Table中添加一行颜色
 */
void GLDColorTable::appendColorRow(const QList<QColor> &colors, const QList<QColor> &edgeColor)
{
    GLDColorRow *pColorRow = new GLDColorRow(colors, edgeColor);
    m_colorRows.append(pColorRow);
    connect(pColorRow, &GLDColorRow::currentSelectedColor,
            this, &GLDColorTable::currentSelectedColor);
    connect(pColorRow, &GLDColorRow::currentSelectedBlock,
            this, &GLDColorTable::currentSelectedBlock);
    connect(pColorRow, &GLDColorRow::commitSelectedColor,
            this, &GLDColorTable::commitSelectedColor);
}

void GLDColorTable::updateColorRow(int row, QList<QColor> &blockColors,
                    QList<QColor> &blockEdgeColors, int updateColorCount)
{
    GLDColorRow *pColorRow = m_colorRows.at(row);
    pColorRow->updateColorBlocks(blockColors, blockEdgeColors, updateColorCount);
}

/**
 *  对颜色Table中的title布局
 */
void GLDColorTable::initTitleLayout(QVBoxLayout *layout)
{
    if (NULL != m_title)
    {
        m_title->setProperty("tabTitle", true);
        QVBoxLayout *pTitleLayout = new QVBoxLayout();
        pTitleLayout->setSpacing(0);
        pTitleLayout->addWidget(m_title);
        pTitleLayout->setContentsMargins(c_tabTitleLeftMargin, c_tabTitleTopMargin, 0, c_tabTitleBottomMargin);
        layout->addLayout(pTitleLayout);
    }
}

/**
 *  对颜色Table中的一行布局
 */
void GLDColorTable::initColorRows(QVBoxLayout *layout)
{
    QVBoxLayout *pColorRowsLayout = new QVBoxLayout();
    pColorRowsLayout->addSpacing(c_tabRowSpacing);
    pColorRowsLayout->setContentsMargins(0, 0, 0, c_tabRowBottomMargin);
    foreach (GLDColorRow *colorRow, m_colorRows)
    {
        pColorRowsLayout->addWidget(colorRow);
    }
    layout->addLayout(pColorRowsLayout);
}

/**
 *  对颜色Table布局
 */
void GLDColorTable::initTableLayout()
{
    QVBoxLayout *pLayout = new QVBoxLayout();
    pLayout->setSpacing(0);
    pLayout->setContentsMargins(0, c_tabTopMargin, 0, 0);
    setLayout(pLayout);

    initTitleLayout(pLayout);
    initColorRows(pLayout);
}

int GLDColorTable::findColorBlockIndex(const QColor &color, const QColor &edgeColor)
{
    G_UNUSED(edgeColor);
    int index = -1;

    foreach (GLDColorRow* colorRow, m_colorRows)
    {
        if (colorRow->findColorBlockIndex(color) != -1)
        {
            index = colorRow->findColorBlockIndex(color);
        }
    }

    return index;
}
