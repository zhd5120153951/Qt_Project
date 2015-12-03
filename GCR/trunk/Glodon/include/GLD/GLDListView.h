#ifndef GLDLISTVIEW_H
#define GLDLISTVIEW_H

#include <QTime>
#include <QTreeView>
#include <QListView>
#include <QTextLayout>
#include <QProxyStyle>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <private/qlistview_p.h>
#include <QSortFilterProxyModel>
#include "GLDWidget_Global.h"

class GLDListView;
class GLDCommonStyle;
class GLDShellTreeView;
class GLDListViewPrivate;

namespace innerGLD{

class GLDWIDGETSHARED_EXPORT innerShellTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit innerShellTreeView(QWidget *parent = 0);

public:
    void setModel(QAbstractItemModel *model);
    QAbstractItemModel *dataModel();

    QModelIndex dataIndex(const QModelIndex &index);
    QModelIndex proxyIndex(const QModelIndex &index);

    bool showBranches() const { return m_bShowBranches; }
    void setShowBranches(bool b){ m_bShowBranches = b; }

public slots:
    void adjustColumnWidth(const QModelIndex &);

signals:
    void intoPath(const QModelIndex &);
    void currentItemJustChanged(const QModelIndex &current, const QModelIndex &previous);
    void itemPressed(const QModelIndex &index);

protected slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

protected:
    bool event(QEvent *e);
    void drawBranches(QPainter *painter,
                                  const QRect &rect,
                                  const QModelIndex &index) const;
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    virtual QSortFilterProxyModel *createSortModel();

private slots:
    void onItemPressed(const QModelIndex &index);

protected:
    /*!
     * \brief m_bShowBranches 是否绘制表示展开/收起的小图标
     */
    bool m_bShowBranches;

private:
    QAbstractItemModel *m_pDataModel;
    QSortFilterProxyModel *m_pModel;

private:
    friend class GLDListView;
    typedef QTreeView inherit;
};
}

class GLDWIDGETSHARED_EXPORT GLDListView : public QListView
{
    Q_OBJECT
public:
    GLDListView(QWidget *parent = 0);
    ~GLDListView();

    void setModel(QAbstractItemModel *model);
    void setRootIndex(const QModelIndex &index);
    QRect visualRect(const QModelIndex &index) const;

    enum GLDViewMode { VSListMode, VSIconMode, VSSmallIconMode, VSReportMode};


    virtual void setVsViewMode(GLDViewMode mode);
    virtual GLDViewMode vsViewMode() const;

    bool showToolTip()const{ return m_bShowToolTip; }

    void setShowToolTip(bool l){ m_bShowToolTip = l; }

    void enterEdit(const QModelIndex &index);
    void setSectionResizeMode(int logicalIndex, QHeaderView::ResizeMode mode);

    /*!
     * \brief setDragActionEnable 设置拖拽功能是否启用。注意，不要使用setDragDropMode和setDragEnabled等Qt方法，不保证有效。
     * \note 默认是开启状态
     * \param enable
     */
    void setDragActionEnable(bool enable);
    bool dragActionEnabled() const;

    /*!
     * \brief currentVSViewStateName
     * \return 当前的VsViewMode的状态的名称{list,icon,smallicon,report}四种
     */
    virtual QString currentVSViewStateName() const;

    /*!
     * \brief 需要先切换到ReportModel，再调用该方法
     * \return
     */
    innerGLD::innerShellTreeView *shellTreeView();

Q_SIGNALS:
    void itemJustBeSelected(const QModelIndex &index);
    void itemJustBeDoubleClicked(const QModelIndex &index);
    void currentItemJustChanged(const QModelIndex &current, const QModelIndex &previous);

protected slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void initVsViewState();

protected:
    bool event(QEvent *event);
    void paintEvent(QPaintEvent *e);
    void timerEvent(QTimerEvent *e);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void dropEvent(QDropEvent *e);

    void setViewMode(ViewMode mode);
    void switchDragDropState();
    void switchChildTreeDragDropState();
    inline bool currentReportMode() const { return VSReportMode == m_viewMode; }
    virtual innerGLD::innerShellTreeView* createTreeView();

protected:
    mutable QTime stopDelayedLayoutTime;
    mutable bool stopDelayedLayoutTimer;
    bool m_bShowToolTip;
    bool m_bDragEnabled;
    bool m_bVsModeSetted;
    GLDViewMode m_viewMode;
    innerGLD::innerShellTreeView *m_shellTree;

private slots:
    void onInnerShellTreeViewClick(const QModelIndex &index);

private:
    Q_DECLARE_PRIVATE(GLDListView)
    Q_DISABLE_COPY(GLDListView)
    friend class innerGLD::innerShellTreeView;
    void init();
    typedef QListView inherit;
};

class GLDListViewPrivate: public QListViewPrivate
{
    Q_DECLARE_PUBLIC(GLDListView)
public:
    GLDListViewPrivate():QListViewPrivate(){}

    ~GLDListViewPrivate(){}

    void interruptDelayedItemsLayout() const;
    QRect mapToViewport(const QRect &rect, bool extend) const;
    QStyleOptionViewItem viewOptions() const;

    inline QVector<QModelIndex> intersectingSet(const QRect &area, bool doLayout = true) const {
        if (doLayout) executePostedLayout();
        QRect a = (q_func()->isRightToLeft() ? flipX(area.normalized()) : area.normalized());
        return commonListView->intersectingSet(a);
    }

    inline void executePostedLayout() const
    {
        if (delayedPendingLayout && state != (GLDListView::CollapsingState))
        {
            interruptDelayedItemsLayout();
            const_cast<GLDListView*>(q_func())->doItemsLayout();
        }
    }
protected:
    void updateStyledFrameWidths();
    QStyleOptionViewItem abstractViewOptions() const;

private:
    Q_DISABLE_COPY(GLDListViewPrivate)
};

class GLDWIDGETSHARED_EXPORT GLDListStyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    GLDListStyledItemDelegate(QObject *parent = 0):QStyledItemDelegate(parent){}

    ~GLDListStyledItemDelegate(){}

    void paint(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;
};

class GLDWIDGETSHARED_EXPORT GLDWindowsVistaStyle : public QProxyStyle
{
    Q_OBJECT
public:
    GLDWindowsVistaStyle(GLDListView *list):QProxyStyle(), m_textLineHeight(0), m_custom(list){}

    QRect subElementRect(SubElement sr, const QStyleOption *opt,
                                       const QWidget *widget) const;
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                       QPainter *painter, const QWidget *widget = 0) const;

    mutable double m_textLineHeight;
protected:
    GLDListView *m_custom;
private:
    void* parentOffset;
    void* grandparentOffset;
};

class GLDWIDGETSHARED_EXPORT GLDFileSortModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    GLDFileSortModel(QObject *parent = 0);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    bool sizeCompare(const QVariant &oLeftData, const QVariant &oRightData) const;

private:
    QHash<QString, quint64> m_oSizeUnits;
};

#endif // GLDLISTVIEW_H
