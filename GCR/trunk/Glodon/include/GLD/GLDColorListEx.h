#ifndef GColorListEx_H
#define GColorListEx_H

#include <QColor>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QListWidget>
#include <QPushButton>
#include "GLDWidget_Global.h"

typedef QList<QColor> GLDColorList;

/**
  * GLDColorBlock
 */
class GLDWIDGETSHARED_EXPORT GLDColorBlock : public QPushButton
{
    Q_OBJECT
public:
    explicit GLDColorBlock(const QColor &color, const QColor &edgeColor, QWidget *parent = 0);
    ~GLDColorBlock(){}

    void updateColor(const QColor &color, const QColor &edgeColor);
    inline QColor color() const
    {
        return m_color;
    }

    inline QColor edgeColor() const
    {
        return m_edgeColor;
    }

private:
    void drawColorBlock();

signals:
    void colorValue(const QColor &color, const QColor &edgeColor);
    void colorBlock(GLDColorBlock *currentBlock);
    void commitSelectedColor(const QColor &color, const QColor &edgeColor);

public slots:
    void onClick();

private:
    QColor m_color;
    QColor m_edgeColor;
};

/**
  * GLDColorTableTitle
 */
class GLDWIDGETSHARED_EXPORT GLDColorTableTitle : public QPushButton
{
    Q_OBJECT
public:
    explicit  GLDColorTableTitle(QWidget *parent = 0);
    ~GLDColorTableTitle(){}
};

/**
  * GLDColorRow
 */
class GLDWIDGETSHARED_EXPORT GLDColorRow : public QWidget
{
    Q_OBJECT
public:
    explicit GLDColorRow(const QList<QColor> &colors, const QList<QColor> &edgeColors, QWidget *parent = 0);
    ~GLDColorRow(){}

public:
    void setBackgroundColor(QColor &color);
    void updateColorBlocks(QList<QColor> &colors, QList<QColor> &edgeColors, int updateColorCount);
    GLDColorBlock* initBlock();
    GLDColorBlock* findColorBlock(const QColor color);
    int findColorBlockIndex(const QColor color);

private:
    void initColorBlocks(const QList<QColor> &colors, const QList<QColor> &edgeColors);
    void initColorBlocksLayout();

signals:
    void currentSelectedColor(const QColor &color, const QColor &edgeColor);
    void currentSelectedBlock(GLDColorBlock *block);
    void commitSelectedColor(const QColor &color, const QColor &edgeColor);

private:
    QList<GLDColorBlock *> m_colorBlocks;
};

/**
  * GLDColorTable
*/
class GLDWIDGETSHARED_EXPORT GLDColorTable : public QWidget
{
    Q_OBJECT
public:
    explicit GLDColorTable(const QList<QColor> &firstRowColors, const QList<QColor> &firstRowEdgeColors, QWidget *parent);
    ~GLDColorTable();

    void addTitle(const QString &title);
    void appendColorRow(const QList<QColor> &colors, const QList<QColor> &edgeColor);
    void updateColorRow(int row, QList<QColor> &blockColors,
                        QList<QColor> &blockEdgeColors, int updateColorCount);
    void initTableLayout();
    void setBackgroundColor(QColor &color);
    GLDColorBlock* initBlock();
    GLDColorBlock* findColorBlock(const QColor &color);
    int findColorBlockIndex(const QColor &color, const QColor &edgeColor);

signals:
    void currentSelectedColor(const QColor &color, const QColor &edgeColor);
    void currentSelectedBlock(GLDColorBlock *block);
    void findColor(const QColor &color);
    void commitSelectedColor(const QColor &color, const QColor &edgeColor);

protected:
    void mouseReleaseEvent(QMouseEvent *e);

private:
    void initTitleLayout(QVBoxLayout *layout);
    void initColorRows(QVBoxLayout *layout);

private:
    GLDColorTableTitle *m_title;
    QList<GLDColorRow *> m_colorRows;
    QColor m_BackColor;
};

/**
  * GColorListEx
*/
class GLDWIDGETSHARED_EXPORT GColorListEx : public QComboBox
{
    Q_OBJECT
public:
    // isLoadRecentColor用来控制是否配置临时文件以保存与加载从更多颜色中选择的颜色
    explicit GColorListEx(QWidget *parent = 0, QColor color = QColor("#000000"),
                          bool isLoadRecentColor = false);
    ~GColorListEx();

public:
    void setHasBorder(bool bHasBorder);
    void loadColorFile(const QString &fileName = ":/inis/GColorListEx.ini");

    /**
     * @brief 根据给定颜色,查找其是否在颜色列表中
     *        存在则返回其在列表中的位置,否则返回-1
     * @param color  需要查找的颜色
     * @return
     */
    GLDColorBlock *findColorBlock(const QColor &color);

    /**
     * @brief 返回颜色列表当前选中的颜色
     * @return
     */
    QColor currentColor() const;

    void setCurrentColor(const QColor color);

    /**
     * @brief isLoadRecentColor 是否配置临时文件以加载和保存最近从更多颜色中选择的颜色
     * @return
     */
    bool isLoadRecentColor() const;
    /**
     * @brief setIsLoadRecentColor 设置是否配置临时文件以加载和保存最近从更多颜色中选择的颜色
     * @param isLoadRecentColor
     */
    void setIsLoadRecentColor(const bool isLoadRecentColor);

    /**
     * @brief 颜色块是否显示ToolTip
     * @return
     */
    bool isShowRGBColor() const;

    /**
     * @brief 设置颜色块是否显示ToolTip
     * @param isShowColorRGB
     */
    void setIsShowRGBColor(const bool isShowColorRGB = false);

    /**
     * @brief 控件进入编辑状态后是否显示表示该颜色块的RGB值
     * @return
     */
    bool isShowRGBStr() const;

    /**
     * @brief 设置控件进入编辑状态后是否显示表示该颜色块的RGB值
     * @param isShowRGBStr
     */
    void setIsShowRGBStr(bool isShowRGBStr);

    /**
     * @brief iconUrlInEdit 设置编辑框上面的icon, iconUrl为icon的所在路径
     *                      但是icon是有限制的，必须为整体16*16 pixel，icon
     *                      中的图像为14*10 pixel，并且水平居中垂直居上，以保证界面美观
     * @return
     */
    QString iconUrlInEdit() const;
    void setIconUrlInEdit(const QString iconUrl);

signals:
    void currentSelectedColor(const QColor &color, const QColor &edgeColor);
    /**
     * @brief 当前颜色改变时才会发出此信号，如果点击更多，选择了自定义颜色，则会发出选择颜色对应的index
     * @param index
     */
    void colorIndexChange(const QColor &color);
    void findColor(const QColor &color, const GLDColorBlock *block);
    void colorBlockClicked(int);

private slots:
    void drawColor(const QColor &color, const QColor &edgeColor);
    void moreColor();
    void currentSelectedBlock(GLDColorBlock *block);
    void commitSelectedColor(const QColor &color, const QColor &edgeColor);

private:
    QColor getColor(const QString sVal);
    void addGrayColorTable();
    void addMoreButton();
    void addStandardColorTable();
    void addRecentColorTable();
    QColor getEdgeColor(const QColor &color);
    void updateRecentColorTable();
    void addTable(GLDColorTable *table, int tableWid, int tableHeight);
    void addThemeColorTable();
    void initColorComboBox();
    void initPopUpWidget();
    void addRecentColorList();
    void loadDefaultConfigColorVal();
    void loadSelectedMoreColor();
    void saveSelectedMoreColor();
    void removeRecentColorConfFile();
    void updateCurrentColor();

private:
    GLDColorList m_blockColors;               // 颜色块内部颜色
    GLDColorList m_edgeColors;                // 颜色块边框颜色
    QListWidget *m_comboBoxWidget;
    QColor m_curColor;
    QColor m_userColor;
    GLDColorBlock *m_currentBlock;
    GLDColorBlock *m_originalBlock;
    int m_recentUseColorCount;
    GLDColorList m_recentUseColor;
    GLDColorList m_recentUseEdgeColor;
    GLDColorTable *m_recentColorTable;
    QList<GLDColorTable *> m_colorTableList;
    bool m_isLoadRecentColor;                 // 控制是否配置临时文件以保存与加载从更多颜色中选择的颜色
    bool m_isShowRGBColor;                    // 控制是否显示RGB在每个颜色button上
    bool m_isShowRGBStr;                      // 控制格子内是否显示表示颜色块的RGB字符串
    QString m_iconUrlInEdit;                  // 存放编辑框中icon的Url
};

#endif // GColorListEx_H
