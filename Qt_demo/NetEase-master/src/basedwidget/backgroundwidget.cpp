#include "backgroundwidget.h"

#include <QPainter>

BackGroundWidget::BackGroundWidget(QWidget *parent)
    : BasedStyleShetWidget(parent)
{
    //��ʹ�ô˷�������
    //ֱ��ʹ����ʽ���㻻��
    //m_opacity = 1.0;
    //m_backgrounColor = QColor("#2abf1d");
    //m_wenliPixmap = QPixmap(":/default/wenli");
    //m_guanPixmap = QPixmap(":/default/guangyun_green");

    setObjectName("BackGround");
}

BackGroundWidget::~BackGroundWidget()
{

}

void BackGroundWidget::paintEvent(QPaintEvent * /*e*/)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);

    //ʹ��ͼƬ����ʱqproperty-backPixmap�����Ա���Ϊ"default"
    //����ʹ�õ�һ��ɫ��Ϊ����
    if (m_backgroundPixmap == "default")
    {
        painter.setBackground(m_backgrounColor);
        painter.drawRect(rect());
        painter.setOpacity(m_opacity);
        painter.drawPixmap(rect(), m_guanPixmap, rect());
        painter.setOpacity(1.0);
        painter.drawPixmap(rect(), m_wenliPixmap, rect());
    }
    else
    {
        painter.drawPixmap(rect(), QPixmap(m_backgroundPixmap), rect());
    }
}

void BackGroundWidget::setBackPixmap(const QString &pix)
{
    m_backgroundPixmap = pix;
    update();
}

void BackGroundWidget::setColor(const QColor &color)
{
    m_backgrounColor = color;
    update();
}

void BackGroundWidget::setWenliPixmap(const QString &pix)
{
    m_wenliString = pix;
    m_wenliPixmap = QPixmap(pix);
    update();
}

void BackGroundWidget::setGuanPixmap(const QString &pix)
{
    m_guanString = pix;
    m_guanPixmap = QPixmap(pix);
    update();
}

void BackGroundWidget::setOpacity(qreal real)
{
    m_opacity = real;
    update();
}

