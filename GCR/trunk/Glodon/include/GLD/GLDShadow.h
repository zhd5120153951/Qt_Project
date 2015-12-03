#ifndef GLDSHADOW_H
#define GLDSHADOW_H

#include <QWidget>
#include <QComboBox>
#include "GLDWidget_Global.h"

class GLDWIDGETSHARED_EXPORT GLDShadow : public QObject
{
    class GLDShadowWidget : public QWidget
    {
    public:
        explicit GLDShadowWidget(QWidget *parent = 0);

    protected:
        bool event(QEvent *e);
    };

public:
    explicit GLDShadow(QWidget* parent = 0);

public:
    void removeShadow();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    bool addShadow(QWidget *popup);
    bool addShadow(QComboBox *popup);
    void doRemoveShadow();
    /**
     * @brief 拉伸容器，只适用于固定大小的容器
     *
     * @fn extendWidth
     * @param container 被拉伸的容器指针
     */
    void extendWidth(QWidget *container);
    
    /**
     * @brief 还原容器大小，以免下次出现黑边
     *
     * @fn doViewHide
     * @param container
     */
    void doViewHide(QWidget *container);    
    void paintShadow(QWidget *container);

private:
    bool m_bHandleResize;
    QWidget *m_parent;
};
#endif // GLDSHADOW_H
