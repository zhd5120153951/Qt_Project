#ifndef GLDPAPERTABLEVIEW_H
#define GLDPAPERTABLEVIEW_H

#include "GLDTableView.h"

class GlodonPaperTableViewPrivate;

class GlodonPaperTableView : public GlodonTableView
{
public:
    explicit GlodonPaperTableView(QWidget *parent = 0);

protected:
    explicit GlodonPaperTableView(GlodonPaperTableViewPrivate &dd, QWidget *parent = 0);

protected:
    virtual void showSectionResizingInfoFrame(const QPoint &mousePosition, Qt::Orientation direction, GlodonHeaderView::ResizeState state);

    virtual void columnResized(int column, int oldWidth, int newWidth, bool isManual = false);

private:
    Q_DECLARE_PRIVATE(GlodonPaperTableView)
    Q_DISABLE_COPY(GlodonPaperTableView)
};

#endif // GLDPAPERTABLEVIEW_H
