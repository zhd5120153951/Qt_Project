/*!
 *@file GLDDocView.h
 *@brief {}
 *@author duanb
 *@date 2013.12.19
 *@remarks {}
 *Copyright (c) 1998-2013 Glodon Corporation
 */
#ifndef GLDDOCVIEW_H
#define GLDDOCVIEW_H

#include <QWidget>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QScrollArea>
#include <QQueue>

#include "GLDString.h"
#include "GLDIntList.h"
#include "GLDTreeModel.h"
#include "GLDTableView_p.h"
#include "GLDTableView.h"
#include "GLDPaperWidgetModel.h"
#include "GLDPaperWidget.h"
#include "GLDObjectList.h"
#include "GLDHash.h"
#include "GLDDefaultItemDelegateFactory.h"
#include "GLDAbstractItemModel.h"

class QVBoxLayout;
class QPrinter;
class QHBoxLayout;

class GLDDocViewToExcel;
class GLDDocViewTableViewFactory;
class GLDDocView;

/*!
 *@enum: ScrollHint
 *@brief {滚动条滚动位置控制}
 *@author liurx
 *@date 2014.2.27
 */
enum ScrollHint
{
    shEnsureVisible,
    shPositionAtTop,
    shPositionAtBottom,
    shPositionAtCenter
};

typedef GLDList<QRect> GRectList;

struct GLDDocInfo
{
public:
    int pageOf(int nValue);
    GIntList pageOf(int nValue, const QRect &rc);

public:
    GIntList rowCountPerPageList;
    GIntList startRowList;
    GRectList pageGeometryInfoList;

    int nPageSplitterHeight;
    int nPageCount;
};

struct GLDDocFocusInfo
{
    GLDDocFocusInfo(): nFocusRow(-1), nFocusPageNo(-1), nFocusColumn(-1), isInEditing(false) {}
    int nFocusRow;
    int nFocusPageNo;
    int nFocusColumn;
    bool isInEditing;
};

class GLDTABLEVIEWSHARED_EXPORT GLDScrollArea : public QScrollArea
{
public:
    explicit GLDScrollArea(GLDDocView *parent = 0);

protected:
    bool event(QEvent *);
    bool eventFilter(QObject *, QEvent *);
    void resizeEvent(QResizeEvent *);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event);
#endif

private:
    GLDDocView *m_parent;
};

class GLDTABLEVIEWSHARED_EXPORT GLDBaseWidgetFactory : public QObject
{
    Q_OBJECT
public:
    virtual ~GLDBaseWidgetFactory();
    // nPage从1开始，nWidth、nHeight的单位是像素
    virtual QWidget *createHeaderWidget(int nWidth, int nHeight, int nLeftMargin, int nRightMargin, ZoomFactor factor, int nPage, int nPageCount) = 0;
    virtual QWidget *createFooterWidget(int nWidth, int nHeight, int nLeftMargin, int nRightMargin, ZoomFactor factor, int nPage, int nPageCount) = 0;
};

class GLDTABLEVIEWSHARED_EXPORT GLDDocViewTableViewFactory : public QObject
{
    Q_OBJECT
public:
    virtual ~GLDDocViewTableViewFactory();
    virtual GlodonPaperTableView *createTableView(QWidget *parent) = 0;
};

class GLDTABLEVIEWSHARED_EXPORT GLDDocView : public QWidget
{
    Q_OBJECT
public:
    explicit GLDDocView(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~GLDDocView();

public:
    void setModel(QAbstractItemModel *model);
    QAbstractItemModel *dataModel();

    ZoomFactor factor() const;
    void setFactor(const ZoomFactor &value);

    virtual QSize sizeHint() const;

    int pageCount() const;

    void print();
    void printPreview();
    void exportToPdf();
    /**
     * @brief 直接导出pdf而不带有预览效果
     * @param 外部传进来文件路径+文件名
     */
    void exportToPdf(QString fileName);

    void fistPage();
    void lastPage();
    void prevPage();
    void nextPage();

    void gotoPage(int value);    // 从0开始

    int footerHeight() const;    // 单位是像素
    void setFooterHeight(int footerHeight);    // 单位是mm

    int headerHeight() const;    // 单位是像素
    void setHeaderHeight(int headerHeight);    // 单位是mm

    int tableViewHorizontalHeaderHeight() const;    // 单位是像素
    void setTableViewHorizontalHeaderHeight(int tableViewHorizontalHeaderHeight);    // 单位是mm

    int leftMargin() const;    // 单位是像素
    void setLeftMargin(int leftMargin);

    int rightMargin() const;    // 单位是像素
    void setRightMargin(int rightMargin);    // 单位是mm

    /**
     * @brief 返回当前的QModelIndex,行和列对应的是在dataModel中的
     * @return
     */
    QModelIndex currentIndex()
    {
        return m_currentIndex;
    }

    /**
     * @brief 把PaperWidgetModel中对应的QModelIndex转成dataModel中的QModelIndex
     * @param pageNo index所在的页
     * @param index  paperWidgetModel对应的QModelIndex
     * @return
     */
    QModelIndex dataIndex(int pageNo, const QModelIndex &index);
    inline QModelIndex dataIndex(const QModelIndex &tableViewIndex)
    {
        return dataIndex(curPageNo(tableViewIndex), tableViewIndex);
    }

    void registerWidgetFactory(GLDBaseWidgetFactory *factory);
    GLDBaseWidgetFactory *registeredWidgetFactory() const;

    void registerDelegateFactory(GlodonDefaultItemDelegateFactory *factory);
    GlodonDefaultItemDelegateFactory *registeredDelegateFactory() const;

    int curPageNo();
    int curFocusPageNo();

    bool removePageAt(int nPage);
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool appendRows(int count, const QModelIndex &parent = QModelIndex());

    void setColumnWidth(int column, int size);    // size单位是mm
    void setColumnWidthPixel(int column, int size); //单位是像素
    int columnWidth(int column);    // 单位是像素
    int columnCount() const;

    GIntList fitColWidths() const;
    void setFitColWidths(const GIntList &fitColWidths);

    bool hasFitColumn() const;

    /**
     * @brief 设置当前的焦点
     * @param dataIndex
     */
    void setCurrentIndex(QModelIndex dataIndex);
    /**
     * @brief 设置格子进入编辑状态
     * @param dataIndex
     */
    void setIndexEdit(QModelIndex dataIndex);
    void reLayoutPaperWidget();

    int paperWidth() const;
    void setPaperWidth(int value);

    int paperHeight() const;
    void setPaperHeight(int value);

    void setBackgroundPalette(QPalette palette);

    GlodonHeaderView::ResizeMode resizeMode() const;
    void setResizeMode(const GlodonHeaderView::ResizeMode &resizeMode);

    bool allowZoom() const;
    void setAllowZoom(bool bAllowZoom);

    int rowPosition(QModelIndex dataIndex);
    int columnPosition(QModelIndex dataIndex);
    /**
     * @brief 根据dataIndex，返回该格子的全局绘制位置
     * @param dataIndex
     * @return
     */
    QRect visualRect(const QModelIndex dataIndex);
    void scrollTo(QModelIndex dataIndex, ScrollHint hint = shEnsureVisible);
    QQueue<GLDPaperWidget *> &paperWidgetList();


    /**
     * @brief 刷新一列
     * @param col
     */
    void updateCol(QModelIndex dataIndex);

    /**
     * @brief 刷新一行
     * @param row
     */
    void updateRow(QModelIndex dataIndex);

    /**
     * @brief 刷新整个tableView
     */
    void updateAll();

    /**
     * @brief 刷新焦点处的控件
     */
    void updateFocusWidget();

    /**
     * @brief 设置表头是否为多行表头，需要在分页之前调用
     * @param value
     */
    void setMutilHeaderView(bool value, Qt::Orientation orientation = Qt::Horizontal);
    bool isMutilHeaderViewEnable() const ;

    QScrollArea *scrollArea();

    int cellHeight() const;
    void setCellHeight(int cellHeight);

    bool suitRowHeight() const;
    void setSuitRowHeight(bool suitRowHeight);

    void registerTableViewFactory(GLDDocViewTableViewFactory *tableViewFactory);

    /**
     * @brief 设置允许通过格线调整行高列宽, 需要在分页之前调用
     * @param value
     */
    void setAllowToResize(bool value);
    bool allowToResize() const;

    /**
     * @brief 获取当前正在处于编辑状态的index
     * @return  返回的是dataIndex
     */
    QModelIndex editingIndex();

    /**
     * @brief 强制关闭当前的编辑状态
     */
    void forceCloseEditor();

    void setShowHorizontalHeaderView(bool value);
    bool isShowHorizontalHeaderView();

    void setDrawFrameLine(bool value);
    bool drawFrameLine();

    void setShowVerticalLine(bool value);
    bool isShowVerticalLine();

    void setShowHorizontalLine(bool value);
    bool isShowHorizontalLine();

    void setGridLineWidth(int value);
    int gridLineWidth();

    /*!
     * \brief 设置打印方向
     * \param oritation
     */
    void setPrintOrientation(QPrinter::Orientation oritation);
    QPrinter::Orientation printOrientation();


    /*!
     *设置给定逻辑列的隐藏状态
     *@param[in]  column   逻辑列号
     *@param[in]  hide     true表示隐藏，false表示显示
     *@return 无
     */
    void setColumnHidden(int column, bool hide);
    bool columnHidden(int column) const;
    bool hasColumnHidden() const;

    QMap<int, bool> &columnsVisableState();

    void handleKeyDownEvent(QEvent *event, QKeyEvent *keyEvent, GlodonTableView *currentTableView);

    void initTableView(GlodonTableView *pTableView, GLDPaperWidget *pPaperWidget, int nPageNo);

    QModelIndex indexAt(QPoint pos);


    QModelIndexList selectedIndexes();

    int curPageNo(const QModelIndex &currentIndex) const;

    int dataIndexPageNo(QModelIndex dataIndex);

    QModelIndex index(QModelIndex dataIndex);

    void updateModelIndex(QModelIndex dataIndex);

public slots:
    void calculatePages();

Q_SIGNALS:
    void currPageChanged(int nPage);
    void currentFocusChanged(const QModelIndex &previous,
                             const QModelIndex &current);
    void focusToNextRow(const QModelIndex &oldIndex,
                        const QModelIndex &newIndex,
                        bool &canMove);
    void onbeforeMoveCursor(const QModelIndex &oldDataIndex,
                            const QModelIndex &newDataIndex,
                            QItemSelectionModel::SelectionFlags &command,
                            MoveReason moveReason, bool &canMove);

    void onKeyPress(const QModelIndex &index, QKeyEvent &keyEvent, bool &handled);
    void onCommitEditor(const QModelIndex &index, QVariant& data, bool &commit);
    void onQueryIndexDataType(const QModelIndex &index, GlodonDefaultItemDelegate::GDataType &dataType) const;
    void onMousePress(const QModelIndex &index, QPoint globalPos, QMouseEvent *mousePressEvent, bool &handled);
    void onMouseButtonDblClick(const QModelIndex &index, QPoint globalPos, QMouseEvent *mousePressEvent, bool &handled);
    void onRangeFill(const QModelIndexList &srcModelIndexList,
                     const QModelIndexList &destModelIndexList,
                     int srcRowCount,
                     int srcColumnCount,
                     bool &handled);

protected:

#ifndef QT_NO_WHEELEVENT
    virtual void wheelEvent(QWheelEvent *);
#endif

    virtual void resizeEvent(QResizeEvent *);

    void customEvent(QEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

    bool keyPressEventFilter(QObject *object, QKeyEvent *keyEvent);
    bool mousePressEventFilter(QObject *object, QMouseEvent *pMouseEvent);

    bool onKeyUp(GlodonTableView *tableView, QKeyEvent *keyEvent);
    bool onKeyDown(GlodonTableView *tableView, QKeyEvent *keyEvent);
    bool onKeyPress(GlodonTableView *tableView, QKeyEvent *keyEvent);
    bool onKeyPageUp(GlodonTableView *tableView, QKeyEvent *keyEvent);
    bool onKeyPageDown(GlodonTableView *tableView, QKeyEvent *keyEvent);

private:
    double colWidthRate(int col) const;
    double zoomFactorAsDouble() const;

    GLDPaperWidget *editingPaper();
    GLDPaperWidget *findPaper(QModelIndex& tableViewIndex);
    GLDPaperWidget *findPaper(int nPageNo, bool createIfNotFound = false);
    GLDPaperWidget *newPaper(int nPageNo);

    bool paperVisible(int nPageNo);
    bool hasPager(int nPageNo);
    int indexPaper(int nPageNo);
    void removePaper(int nPageNo);

    void printPage(int index, QPainter &painter);
    void printPage(int index, QPainter &painter, QPrinter *printer);

    int verticalPosition(const QModelIndex& tableViewIndex);
    int horizontalPosition(const QModelIndex& tableViewIndex);

    int paperVerticalPosition(const QModelIndex &tableViewIndex);
    int paperHorizontalPosition(const QModelIndex& tableViewIndex);

    int rowHeight(const QModelIndex &tableViewIndex);

    void initScrollArea();
    void updateScrollArea();

    void updateVisablePapersList();

    void updatePaperLayout(int nVpos, int nHpos, const QRect &rc);
    int updateLeftLayout(int nHpos, const QRect &rc, GLDPaperWidget *pPaperWidget);
    int updateRightLayout(int nHpos, const QRect &rc, GLDPaperWidget *pPaperWidget);
    int updatePageLayout(int nHpos, const QRect &rc, GLDPaperWidget *pPaperWidget);

    void recoverFocusState();

    void hidePapersOutOfView();

    void updatePapers();
    void cacheNewPapers();
    bool isPaperCacheFull();
    void clearPaperCache();
    void clearPagers();
    void clearOtherIndex();
    void prepareLoadPages();

    void calculatePageCount();
    void calculatePageGeometryInfo();
    void calculatePageWidth();
    void calculatePageHeight();

    void setInitPageWidth(int value);
    void setInitPageHeight(int value);

    void initPageGeometry();

    void setPaperColumnWidth(int column, int size);

    GLDDocInfo &docInfo();
    QAbstractItemModel *model() const;

private slots:
    void verticalScrollBarValueChanged(int value);
    void horizontalScrollBarValueChanged(int value);

    void postUpdatePaperEvent();

    void printDocument(QPrinter *printer, QPrintDialog *printDialog);
    void printDocument(QPrinter *printer);
    void printALLDocument(QPrinter *printer);

    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void onCurrentFocusChanged(const QModelIndex &current, const QModelIndex &previous);
    void onAfterCurrentFocusChanged(const QModelIndex &current, const QModelIndex &previous);

    void onbeforeMoveCurrent(const QModelIndex &oldIndex,
                             const QModelIndex &newIndex,
                             QItemSelectionModel::SelectionFlags &command,
                             MoveReason moveReason,
                             bool &canMove);

    void onTableViewRangeFill(QModelIndexList src,
                              QModelIndexList dest,
                              int rowCount,
                              int columnCount,
                              bool &handled);

    void doNewWidthsToPapers(GIntList *widths);
    void enableFitColWidths(bool enable);

    void commitEditor(const QModelIndex &index, QVariant data, bool &commit);
    void queryIndexDataType(const QModelIndex &index, GlodonDefaultItemDelegate::GDataType &dataType);

private:
    bool m_allowToResize;
    bool m_allowZoom;
    bool m_isHorizontalMutilHeaderView;
    bool m_isInClearOtherIndex;
    bool m_isInManualControlScrollBar;
    bool m_suitRowHeight;

    GIntList m_fitColWidths;
    GLDBaseWidgetFactory *m_headerAndFooterWidgetFactory;
    GLDDocFocusInfo m_docFocusInfo;
    GLDDocInfo m_docInfo;
    GLDDocViewTableViewFactory *m_tableViewFactory;
    GIntList m_visiblePageNoList;
    GlodonDefaultItemDelegateFactory *m_delegateFactory;
    GlodonHeaderView::ResizeMode m_resizeMode;
    GlodonTreeModel *m_treeModel;

    int m_nCellHeight;

    int m_nFooterHeight;
    int m_nHeaderHeight;

    int m_nLeftMargin;
    int m_nRightMargin;

    int m_nCurFocusPageNo;    // 从0开始
    int m_nCurPageNo;    // 从0开始

    int m_nPageWidth;
    int m_nPageHeight;

    int m_nCurrentDPI; // 需要同步设备DPI
    int m_nSettedWidthsSum;
    int m_nPageSplitterHeight;
    int m_nTableViewHHeaderHeight;

    QAbstractItemModel *m_dataModel;
    QAbstractItemModel *m_model;

    QModelIndex m_currentIndex;
    QModelIndex m_preIndex;

    QQueue<GLDPaperWidget *> m_papers;
    QScrollArea *m_scrollArea;
    QVector<int> m_colWidth;
    ZoomFactor m_zoomFactor;
    QMap<int, bool> columnsVisableMap;

private:
    bool m_bDrawFrameLine;
    bool m_bShowHHeaderView;

    bool m_bHorzLine; //显示水平格线
    bool m_bVertLine; //显示垂直格线

    double m_dInitPageWidth;     // 单位为毫米
    double m_dInitPageHeight;

    double m_dInitFooterHeight;
    double m_dInitHeaderHeight;

    double m_dInitLeftMargin;
    double m_dInitRightMargin;

    int m_nGridLineWidth;
    int m_nInitMaxPageNo;
    int m_nInitPageLabelFontSize;
    int m_nInitPageSplitterHeight;
    int m_nInitTableViewHHeaderHeight;

    QPrinter::Orientation m_printerOrientation;

    friend class GLDPaperWidgetModel;
    friend class GLDScrollArea;
    friend class GLDPaperWidget;
    friend class GLDDocViewToExcel;
};

extern const GString c_GLDDocView_HHeader;

#endif // GLDDOCVIEW_H
