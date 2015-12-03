#ifndef GLDGROUPHEADERVIEW_H
#define GLDGROUPHEADERVIEW_H

#include "GLDHeaderView.h"

class GlodonColumnGroupFrame;
class GColumnGroupLabel;
class GlodonGroupHeaderViewPrivate;

class GLDTABLEVIEWSHARED_EXPORT GlodonGroupHeaderView : public GlodonHeaderView
{
    Q_OBJECT
public:
    GlodonGroupHeaderView(QWidget *parent);

    int drawWidth() const;
    void setModel(QAbstractItemModel *model);
    void updateGeometries();

public Q_SLOTS:
    void cgFrameChanged(int col, bool dragIn);
    void onGroupChanged();

Q_SIGNALS:
    void cgFrameChanged();
    void groupChanged(QVector<int> newGroups);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void dragEnterEvent(QDragEnterEvent *dee);
    void dragMoveEvent(QDragMoveEvent *dme);
    void dropEvent(QDropEvent *de);

private:
    GlodonColumnGroupFrame *cgFrame;
    GColumnGroupLabel *sectionLabel;

private:
    Q_DECLARE_PRIVATE(GlodonGroupHeaderView)
    void setupSectionLabel(int section);
    QVector<int> currentGroups();
};

class GLDTABLEVIEWSHARED_EXPORT GColumnGroupLabel : public QLabel
{
    Q_OBJECT
public:
    explicit GColumnGroupLabel(int col, QString text, QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *e);
    void draggedLeaveFrame();

public:
    int column;

Q_SIGNALS:
    void leaveFrame();
};

class GLDTABLEVIEWSHARED_EXPORT GlodonColumnGroupFrame : public QFrame
{
    Q_OBJECT
public:
    explicit GlodonColumnGroupFrame(QWidget *parent = 0);

    void paintEvent(QPaintEvent *e);

    void dragEnterEvent(QDragEnterEvent *dee);
    void dropEvent(QDropEvent *de);
    void dragMoveEvent(QDragMoveEvent *dme);
    void dragLeaveEvent(QDragLeaveEvent *);

    int frameHeight() const;

public Q_SLOTS:
    void onLabelDragOut();

public:
    QList<GColumnGroupLabel *> labels;

Q_SIGNALS:
    void cgFrameChanged(int col, bool dragOut);
    void groupChanged();

private:
    QRect labelRect(int level);
    void drawBranch(int level, QRect labelRect);
    int moveTarget(QPoint pos);
    QRect branchRect(int level);

private:
    int moveStart;
    int movePos;
};

#endif // GLDGROUPHEADERVIEW_H
