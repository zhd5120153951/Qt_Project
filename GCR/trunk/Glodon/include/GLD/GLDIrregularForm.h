#ifndef GLDIRREGULARFORM_H
#define GLDIRREGULARFORM_H

#include "GLDMask_Global.h"

#include <QWidget>
#include <QPushButton>

class GLDCustomButton;

class GLDMASKSHARED_EXPORT GLDIrregularForm : public QWidget
{
    Q_OBJECT

public:
    explicit GLDIrregularForm(QWidget *parent = 0);
    explicit GLDIrregularForm(const QString & irregularImgPath, const QString & btnImgPath, QWidget *parent = 0);

    void setFlagAndAttribute();

    ~GLDIrregularForm();

    QSize sizeHint() const;

    void loadPixmap(const QString & pixmapPath);
    void setPixmap(const QPixmap & pm);

Q_SIGNALS:
    void irregularFormClicked();

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    GLDCustomButton* m_pCustomBtn;
    QPixmap m_irregularFormPm;
};

#endif // GLDIRREGULARFORM_H
