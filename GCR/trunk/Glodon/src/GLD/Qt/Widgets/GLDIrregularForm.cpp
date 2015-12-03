#include "GLDCustomButton.h"
#include "GLDIrregularForm.h"

#include <QLabel>
#include <QDebug>
#include <QPainter>
#include <QPalette>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>

GLDIrregularForm::GLDIrregularForm(QWidget *parent)
    : QWidget(parent)
    , m_pCustomBtn(nullptr)
    , m_irregularFormPm("")
{
    setFlagAndAttribute();
}

GLDIrregularForm::GLDIrregularForm(const QString & irregularImgPath, const QString & btnImgPath, QWidget *parent)
    : QWidget(parent)
    , m_pCustomBtn(new GLDCustomButton(btnImgPath, this))
    , m_irregularFormPm("")
{
    setFlagAndAttribute();

    loadPixmap(irregularImgPath);

    connect(m_pCustomBtn, &QPushButton::clicked, this, &GLDIrregularForm::irregularFormClicked);
}

void GLDIrregularForm::setFlagAndAttribute()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
}

GLDIrregularForm::~GLDIrregularForm()
{

}

QSize GLDIrregularForm::sizeHint() const
{
    if (!m_irregularFormPm.isNull())
    {
        return m_irregularFormPm.size();
    }

    return QWidget::sizeHint();
}

void GLDIrregularForm::loadPixmap(const QString & pixmapPath)
{
    QPixmap tempPixmap;
    tempPixmap.load(pixmapPath);
    setPixmap(tempPixmap);
}

void GLDIrregularForm::setPixmap(const QPixmap & pm)
{
    m_irregularFormPm = pm;
    update();
}

void GLDIrregularForm::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    m_pCustomBtn->move(this->rect().topRight().x() - m_pCustomBtn->width(), this->rect().topRight().y());

    painter.drawPixmap(0, 0, m_irregularFormPm);

    QWidget::paintEvent(event);
}
