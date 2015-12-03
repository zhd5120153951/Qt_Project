/*!
*@file
*@brief 进度条
*@author wangdz
*@date 2014年12月13日
*@remarks
*
*Copyright (c) 1998-25 Glodon Corporation
*/

#ifndef GLDPROGRESSBAR_H
#define GLDPROGRESSBAR_H

#include <QEvent>
#include <QWaitCondition>
#include <QProgressDialog>
#include "GLDWidget_Global.h"

class GLDWIDGETSHARED_EXPORT GLDProgressBar: public QObject
{
    Q_OBJECT

public:
    GLDProgressBar(QWidget *parent = NULL);
    ~GLDProgressBar();

public:
    /**
    * @brief 设置当前进度条的值
    */
    void setValue(const int);
    /**
    * @brief 设置进度条的边界范围
    * @param[in] minimum 进度条最小的边界值
    * @param[in] maximum 进度条最大的边界值
    * @note 当出现minimum = maximum = 0时，会出现进度条busy的状态指示
    * @see QProgressDialog::setRange,setMinimum,setMaximum
    *
    */
    void setRange(int minimum, int maximum);
    /**
    * @brief 在非主线程调用刷新方法，需要通知主线程更新m_progressDlg的value
    * @param[in] pos 进度条当前边界值
    */
    void updateProgress(int pos);
    /**
    * @brief 显示进度条
    */
    void show();

    virtual bool event(QEvent *event);
    /**
    * @brief 隐藏进度条
    */
    void hide();
    /**
    * @brief 得到当前进度条位置
    * @param[in] pos 进度条当前边界值
    */
    int getProgressPos();
    /**
    * @brief 设置进度条的大小
    * @param[in] width  宽度
    * @param[in] height  高度
    */
    void setSize(int width, int height);
    void setPos(int x, int y);
    /**
    * @brief 设置进度条上方文本信息
    * 注：此方法一定要在resize方法之前使用  否则resize方法会失效
    */
    void setText(const QString &str);

private:
    bool isCreateThread();

private:
    int m_cachePos;
    Qt::HANDLE m_barThreadId;
    QProgressDialog *m_progressDlg;
    QWaitCondition m_waitobj;
};
#endif
