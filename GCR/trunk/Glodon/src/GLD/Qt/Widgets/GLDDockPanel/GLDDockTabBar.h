#ifndef GLDDockTABBAR_H
#define GLDDockTABBAR_H

#include <QTabBar>

class GLDDockTabBar : public QTabBar
{
    Q_OBJECT

public:
    GLDDockTabBar(QWidget *parent);
    ~GLDDockTabBar();

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

private:
    bool isLButtonDown_;
    bool sendDragOut_;
    int insertPos_;

signals:
    void dragTabOut(int index);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void dragMoveEvent(QDragMoveEvent *);
    virtual void dropEvent(QDropEvent *);
    virtual void dragEnterEvent(QDragEnterEvent *);
};

#endif // GLDDockTABBAR_H
