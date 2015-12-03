/*!
 *@file gcolorlist.h
 *@brief {颜色下拉选择框}
 *
 *@author Gaosy
 *@date 2012.9.19
 *@remarks 
 *Copyright (c) 1998-2012 Glodon Corporation
 */

#ifndef GLDCOLORLIST_H
#define GLDCOLORLIST_H

#include <QComboBox>
#include <QColor>
#include "GLDWidget_Global.h"

#define ITEM_RIGHT_OFFSET 16

/*!   
 *@class: GColorList
 *@brief {颜色下拉选择框}
 *@author Gaosy 
 *@date 2012.9.19
 */
class GLDWIDGETSHARED_EXPORT GColorList : public QComboBox
{
    Q_OBJECT
public:
    explicit GColorList(QWidget *parent = 0);

    void setUserDefaultColor(bool value);
    bool userDefaultColor();
    
    /*!
     *根据给定颜色，查找其是否在颜色列表中
     *存在则返回其在列表中的位置，否则返回-1
     *@param[in]  color  需要查找的颜色
     *@return int
     */
    int findColor(QColor color);

    /*!
     *将给定颜色添加到颜色列表中，如果该颜色已存在
     *则仅返回其在列表中的下标，并不重复添加
     *不存在则添加该颜色至列表最末尾，并返回下标
     *@param[in]  color  需要添加到列表中的颜色
     *@return int
     */
    int addColor(QColor color);

    /**
     * @brief 下拉框的倒数第二项为用户自定义的颜色
     * @param color
     * @return
     */
    int setUserColor(QColor color);
    QColor userColor();

    /*!
     *返回颜色列表当前选中的颜色
     *@return QColor
     */
    QColor currentColor();

    /**
     * @brief 设置当前颜色选择框的颜色
     * @param color
     */
    void selectColor(QColor color);

    void setCurrentIndex(int index);

signals:
    /**
     * @brief 当前颜色改变时才会发出此信号，如果点击更多，选择了自定义颜色，则会发出选择颜色对应的index
     * @param index
     */
    void colorIndexChange(int index);

protected:
    void resizeEvent(QResizeEvent *e);

private slots:
    void moreColorClicked(int index);

private:
    QIcon colorToIcon(QColor color);
    void initColorList();

private:
    QSize m_newSize;
    QColor m_userColor;
    bool m_isInInit;
    int m_currentIndex;
    QVector<QColor> m_addColors;
};

#endif // GLDCOLORLIST_H
