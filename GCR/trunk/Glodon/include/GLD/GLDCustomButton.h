#ifndef GLDCUSTOMBUTTON_H
#define GLDCUSTOMBUTTON_H

#include "GLDMask_Global.h"

#include <QPushButton>

class GLDMASKSHARED_EXPORT GLDCustomButton : public QPushButton
{
    Q_OBJECT

public:
    explicit GLDCustomButton(QWidget* parent = nullptr);
    explicit GLDCustomButton(const QString &iconPath, QWidget* parent = nullptr);

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent* e);

private:
    void setPixmap(const QPixmap& pm);

private:
    QPixmap m_pixmap;
};

#endif // GLDCUSTOMBUTTON_H
