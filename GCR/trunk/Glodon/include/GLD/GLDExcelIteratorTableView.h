#ifndef GLDEXCELITERATORTABLEVIEW_H
#define GLDEXCELITERATORTABLEVIEW_H

#include <QMenu>
#include "GLDTableView.h"
#include "GLDExcelGridIterator.h"

class GLDExcelIteratorTableViewPrivate;
class GLDTABLEVIEWSHARED_EXPORT GLDExcelIteratorTableView : public GlodonTableView
{
    Q_OBJECT
    Q_PROPERTY(QMenu * colPopMenu READ colPopMenu)
    Q_PROPERTY(QMenu * rowPopMenu READ rowPopMenu)
    Q_PROPERTY(QMenu * selectPopMenu READ selectPopMenu)
public:
    GLDExcelIteratorTableView(QWidget *parent);

    void setModel(GEGExcelModel *model);
    GEGExcelModel *model() const;
    IGEGExcelGrid *getExcelGridIterator();
public slots:
    void hideMatchedRows();
    void showAllRows();
    void selectAllClick();
    void cancelAllClick();
    void clearMatchRowIndentity();

public:
    QMenu *colPopMenu();
    QMenu *rowPopMenu();
    QMenu *selectPopMenu();
    QMenu *contextMenu();
protected:
    void mouseReleaseEvent(QMouseEvent *event);

    void createPopupMenus();
    void bindColumSetting();
    void bindRowSetting();

private Q_SLOTS:
    void refresh();

    void prevUnmatchedRow();
    void nextUnmatchedRow();

    void autoMatch();

    void setCurrentIndexValue(const GString &value);

    void doGridContextMenuRequest(QPoint value);
private:
    //GLDExcelIteratorTableViewPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(GLDExcelIteratorTableView);
};

class GLDTABLEVIEWSHARED_EXPORT GLDExcellIteratorDelegate : public GlodonDefaultItemDelegate
{
public:
    explicit GLDExcellIteratorDelegate(QObject *parent = 0);

public:
    GEditStyle editStyle(const QModelIndex &index, bool &readOnly) const;
    bool editable(const QModelIndex &index) const;
};

#endif // GLDEXCELITERATORTABLEVIEW_H
