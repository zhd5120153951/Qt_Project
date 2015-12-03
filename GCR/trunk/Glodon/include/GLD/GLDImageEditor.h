/*!
 *@file gimageeditor.h
 *@brief {图片选择控件}
 *用于QImage类型数据的默认编辑方式
 *@author Gaosy
 *@date 2012.9.7
 *@remarks {}
 *Copyright (c) 1998-2012 Glodon Corporation
 */

#ifndef GLDIMAGEEDITOR_H
#define GLDIMAGEEDITOR_H

#include <QPushButton>
#include <QLabel>
#include "GLDWidget_Global.h"
/*!   
 *@class: GImageEditor
 *@brief {图片选择控件，作为显示图片的格子的默认编辑方式}
 *@author Gaosy 
 *@date 2012.9.10
 */
class GLDWIDGETSHARED_EXPORT GImageEditor : public QWidget
{
    Q_OBJECT
public:
    /*!
     *GImageEditor构造方法
     *@param[in]  parent  父widget
     *@return 无
     */
    explicit GImageEditor(QWidget *parent = 0);

    /*!
     *GImageEditor析构方法
     *@return 无
     */
    virtual ~GImageEditor();

    /*!
     *GFilterView的绘制方法
     *@param[in]  e  
     *@return 无
     */
    void paintEvent(QPaintEvent *);

    /*!
     *为GImageEditor设置当前图片
     *@param[in]  img  设置当前的图片
     *@return 无
     */
    void setImage(QImage img);

    /*!
     *返回当前图片
     *@return QImage
     */
    QImage image();
    
private:
    //GImageEditor右侧的按钮，点击后弹出文件选择对话框，
    //从中选择新的图片
    QPushButton *m_button;

    //用于在GImageEditor中绘制当前图片
    QLabel *m_label;

    //因为QLabel本身是透明的，因此在后面加一层QFrame绘制白色背景
    //去掉透明效果
    QFrame *m_frame;

private slots:
    void editorButtonClicked();
};

#endif // GLDIMAGEEDITER_H
