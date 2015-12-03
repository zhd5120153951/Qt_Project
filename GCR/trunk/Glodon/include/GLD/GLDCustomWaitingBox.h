#ifndef GCUSTOMWAITINGBOX_H
#define GCUSTOMWAITINGBOX_H

#include <QtWidgets>
#include "GLDWidget_Global.h"

class GLDWIDGETSHARED_EXPORT GLDCustomWaitingBox : public QWidget
{
    Q_OBJECT

public:
    explicit GLDCustomWaitingBox(QWidget *parent = 0);
    ~GLDCustomWaitingBox();

    /**
     * @brief loadWaitMap
     * @param fileName
     * @return 是否加载成功
     * 加载一组图标，图标个数为 2行6列。可以参见 CLDCustomWaitingBox.png
     * 注意是 2 行 6列
     */
    bool loadWaitMap(const QString &fileName);
signals:
    void start();
    void end();

public Q_SLOTS:
    void hide();
    void show();

public:
    void setBackGroundColor(const QColor &color);
    QColor backGroundColor();

protected:
    bool eventFilter(QObject *target, QEvent *event);
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *event);

private:
    QPixmap m_mapWait;
    int m_nimageIndex;
    int m_nTimerId;
    QColor m_bkColor;
};

#endif // GCUSTOMWAITINGBOX_H
