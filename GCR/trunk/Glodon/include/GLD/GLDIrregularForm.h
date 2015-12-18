#ifndef GLDIRREGULARFORM_H
#define GLDIRREGULARFORM_H

#include "GLDMask_Global.h"

#include <QWidget>

class QPushButton;
class GLDCustomButton;

namespace GlodonMask
{
    class GLDMASKSHARED_EXPORT GLDIrregularForm : public QWidget
    {
        Q_OBJECT

    public:
        explicit GLDIrregularForm(QWidget *parent = 0);
        explicit GLDIrregularForm(const QString & irregularImgPath, const QString & btnImgPath, QWidget *parent = nullptr);

        ~GLDIrregularForm();

        /**
        * @brief 返回窗体大小
        */
        QSize sizeHint() const;

        /**
        * @brief 设置标志和属性
        */
        void setFlagAndAttribute();

        /**
        * @brief 加载pixmap
        */
        void loadPixmap(const QString & pixmapPath);

        /**
        * @brief 设置pixmap
        */
        void setPixmap(const QPixmap & pm);

        /**
        * @brief 设置按钮位置
        */
        void setBtnPos(QPushButton* pBtn, const int x, const int y);

    Q_SIGNALS:
        void irregularFormClicked();

    protected:
        virtual void paintEvent(QPaintEvent *event);

    private:
        QPushButton*     m_pPushBtn;        // 用户按钮
        GLDCustomButton* m_pCustomBtn;      // 自定义按钮
        QPixmap          m_irregularFormPm; // 提示信息pixmap
        int              m_xPosition;
        int              m_yPosition;
    };
}

#endif // GLDIRREGULARFORM_H
