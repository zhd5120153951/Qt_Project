/****
 * @file   : GLDLineWidthComboBoxEx.h
 * @brief  : 重写了线条宽度选择框。
 *             1.支持更多选择按钮，并且浮动在最后不随滚动条滑动
 *             2.根据用户体验的要求，将像素改为磅数，并显示在前面
 *             3.QSS美化了界面,每个控件定义了property属性，要修改可以再qss中定位再修改属性。
 *
 * @date   : 2014-08-18
 * @author : lijl-c
 * @remarks: 实现思路：QComboBox中加载2个QListWidget.上面的再嵌套QListWidget
 *                    每行是1个QLabel和QPushButton
 ****/

#ifndef GLDLINEWIDTHCOMBOBOXEX_H
#define GLDLINEWIDTHCOMBOBOXEX_H

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QListWidgetItem>
#include <QPushButton>
#include <QHBoxLayout>
#include "GLDWidget_Global.h"

class GLDWIDGETSHARED_EXPORT GLDLineWidthButton : public QPushButton
{
    Q_OBJECT
public:
    explicit GLDLineWidthButton(qreal lineWeight, QWidget *parent = 0);
    ~GLDLineWidthButton(){}

protected:
    void resizeEvent(QResizeEvent *e);

private:
    /**
     * @brief drawLineWidth 画每一行的线宽
     * @param lineWeight
     * @param lineLength
     */
    void drawLineWidth(qreal lineWeight, int lineLength);

private:
    qreal m_lineWeigth;
};

/**
 * @brief The GLDLineWidthRow class 线宽下拉框中的一行
 */
class GLDWIDGETSHARED_EXPORT GLDLineWidthRow : public QWidget
{
    Q_OBJECT
public:
    explicit GLDLineWidthRow(QWidget *parent = 0, int rowIndex = 0);
    ~GLDLineWidthRow(){}

public:
    /**
     * @brief initRowLabel 初始化每行的label
     * @param rowIndex
     */
    void initRowLabel(int rowIndex);

    /**
     * @brief initRowButton 初始化每行的线宽Button
     * @param rowIndex
     */
    void initRowButton(int rowIndex);

signals:
    void mapped(int index);

private:
    QLabel *m_label;
    GLDLineWidthButton *m_lineWidthBtn;
    QHBoxLayout *m_rowLayout;
    int m_rowIndex;
};

class GLDWIDGETSHARED_EXPORT GLDLineWidthComboBoxEx : public QComboBox
{
    Q_OBJECT
public:
    explicit GLDLineWidthComboBoxEx(QWidget *parent = 0);
    ~GLDLineWidthComboBoxEx();

    inline int minLineCount() { return m_minLineCount; }
    inline void setMinLineCount(int minLineCount) { m_minLineCount = minLineCount; }

    inline int maxLineCount() { return m_maxLineCount; }
    inline void setMaxLineCount(int maxLineCount) { m_maxLineCount = maxLineCount; }

    inline int currentIndex() {return m_curIndex;}
    inline void setCurrentIndex(const int index){ m_curIndex = index;}

    inline double curLineWidth() { return m_curLineWidth; }
    inline void setCurLineWidth(const double curLineWidth) { m_curLineWidth = curLineWidth; }

    inline int lineWidthPopupHight() { return m_lineWidthPopupHight; }
    inline void setLineWidthPopupHight(int height) { m_lineWidthPopupHight = height; }

    void setHasBorder(bool hasBorder = false);

private:
    /**
     * @brief 初始化控件模型,绘制控件
     *
     * @fn initModel
     */
    void initPopUp();

    /**
     * @brief initLineWidthPart 初始化下拉框上半部分, 包括 磅值labels和线宽buttons部分
     */
    void initLineWidth();

    /**
     * @brief initMoreLinesPart 初始化下拉框下半部分, 更多颜色Button
     */
    void initMoreLinesButton();

    /**
     * @brief createLineWidthRow 创建一行
     * @param index
     * @return
     */
    QWidget* createLineWidthRow(int index);

    /**
     * @brief 为线框项添加内容，即添加每一行的内容
     *
     * @fn createLineWidthRowItem
     * @return QListWidget
     */
    void createLineWidthRowItem(QListWidget *pLineWidthItem);

    void createMoreLinesItem(QListWidget *pMoreItem);

signals:
    void itemClicked(int index);

private slots:
    void onClicked(int index);
    void onClicked();

private:
    int m_minLineCount; /**< TODO 最小线条数量*/
    int m_maxLineCount; /**< TODO 最大线条数量*/
    int m_lineWidthPopupHight; /**< TODO 下拉框高度*/
    bool m_hasBorder;     //是否需要边框
    QListWidget *m_popupListWidgets;
    int m_curIndex;
    double m_curLineWidth;
};

#endif // GLDLINEWIDTHCOMBOBOXEX_H
