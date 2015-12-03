#ifndef GLDDOCKWIDGET_H
#define GLDDOCKWIDGET_H

#include <QWidget>
#include <QDockWidget>
#include <QToolBar>
#include <QLabel>
#include <QAction>
#include <QPoint>
#include "GLDWidget_Global.h"

class GLDWIDGETSHARED_EXPORT GLDDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit GLDDockWidget(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);

signals:

public:
    QAction *addAction(const QIcon & icon, const QString & text);
    QAction *addWidget(QWidget *widget);
    void setIcon(const QIcon &icon);

    void setPressPosOnTab(const QPoint &pos);
    void setReleasePosOnTab(const QPoint &pos);

protected:
    void resizeEvent(QResizeEvent *);
    void mouseReleaseEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
private slots:
    void allowedAreasChanged(Qt::DockWidgetAreas);
    void featuresChanged(QDockWidget::DockWidgetFeatures);
    void dockLocationChanged(Qt::DockWidgetArea);
    void topLevelChanged(bool);
    void visibilityChanged(bool);

    void onFloat();
    void onLock();
    void onClose();

private:
    void initTitleBar();

private:
    QToolBar *m_titleBar;
    QLabel *m_title;
    QAction *m_actTitle;
    QAction *m_actClose;
    QAction *m_actLock;
    QAction *m_actFloat;
};

#endif // GLDDOCKWIDGET_H
