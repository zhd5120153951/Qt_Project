#ifndef GLDIRREGULARFORM_H
#define GLDIRREGULARFORM_H

#include "GLDMask_Global.h"

#include <QWidget>

class QPushButton;
class GLDCustomButton;

class GLDMASKSHARED_EXPORT GLDIrregularForm : public QWidget
{
    Q_OBJECT

public:
    explicit GLDIrregularForm(QWidget *parent = 0);
    explicit GLDIrregularForm(const QString & irregularImgPath, const QString & btnImgPath, QWidget *parent = nullptr);
    explicit GLDIrregularForm(const QString & irregularImgPath, QPushButton * btn, QWidget *parent = nullptr);

    ~GLDIrregularForm();

    QSize sizeHint() const;

    void setFlagAndAttribute();
    void loadPixmap(const QString & pixmapPath);
    void setPixmap(const QPixmap & pm);

Q_SIGNALS:
    void irregularFormClicked();

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QPushButton*     m_pPushBtn;
    GLDCustomButton* m_pCustomBtn;
    QPixmap          m_irregularFormPm;
};

#endif // GLDIRREGULARFORM_H
