/*
 *
 *�������Ĺر�,��С���Ȱ�ť
 *
 * */
#ifndef IMAGEBUTTON_H
#define IMAGEBUTTON_H

#include <QWidget>
#include "../datatypedef/datatypdef.h"

class ImageButton : public QWidget
{
    Q_OBJECT

public:
    explicit ImageButton(QWidget *parent = 0);
    explicit ImageButton(const QString &icon, int num = 4, QWidget *parent = 0);
    explicit ImageButton(const QString &icon, bool isDivision, int num = 4, QWidget *parent = 0);
    ~ImageButton();

    void setOneButtonInfo(const QString &icon, int num = 4);
    void setDivisionButtonInfo(const QString &icon, int num = 3);
    void setCursorEnable(bool cursorEnable);
    void setButtonSttaus(BUTTONSTATUS status);

signals:
    void clicked();

protected:
    void enterEvent(QEvent *e);//�������¼�
    void leaveEvent(QEvent *e);//����Ƴ�
    void mousePressEvent(QMouseEvent *e);//��갴��
    void mouseReleaseEvent(QMouseEvent *e);//����ͷ�
    void paintEvent(QPaintEvent *e);//�����ػ�
    void changeEvent(QEvent *e);

private:
    QPixmap m_currentpix;
    QList<QPixmap> m_pixList;
    int m_num;
    QCursor m_currentCursor;
    bool m_cursoeChangeAble;//�������������״,Ĭ�Ϲر�
};

#endif // IMAGEBUTTON_H
