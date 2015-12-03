#ifndef GLDPAPERWIDGET_H
#define GLDPAPERWIDGET_H

#include "GLDHeaderView.h"

#include <QWidget>

#define ONE_INCHES 25.4 // 1 inches = 25.4mm

#define POINT_NUMBER_FROM_MM(mm, nDPI) (double(double(mm) * nDPI) / ONE_INCHES)

class GLDDocViewTableViewFactory;
class GLDDocView;
class GlodonPaperTableView;

enum ZoomFactor { NoneFactor = -1, percent_50 = 0, percent_100 = 1, percent_200 = 2, percent_400 = 4 };

class GLDTABLEVIEWSHARED_EXPORT GLDPaperWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GLDPaperWidget(GLDDocView *docView, int curPageNo, int pageCount,
                            GlodonHeaderView::ResizeMode resizeMode, GLDDocViewTableViewFactory *tableViewFactory,
                            QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~GLDPaperWidget();

public:
    inline GlodonPaperTableView *tableView()
    {
        return m_tableView;
    }
    int curPageNo();

private slots:
    void rowHeightResized(int row, int oldSize, int newSize, bool isManual = false);
    void columnResized(int column, int oldSize, int newSize, bool isManual = false);

private:
    ZoomFactor factor() const;
    void setFactor(const ZoomFactor &value);

    virtual QSize sizeHint() const;

    int headerHeight() const;
    void setHeaderHeight(int headerHeight);

    int footerHeight() const;
    void setFooterHeight(int footerHeight);

    int tableViewHeight() const;
    void setTableViewHeight(int tableViewHeight);

    int tableViewWidth() const;
    void setTableViewWidth(int tableViewWidth);

    int leftMargin() const;
    void setLeftMargin(int leftMargin);

    int rightMargin() const;
    void setRightMargin(int rightMargin);

    void reLayout();

    QWidget *headerWidget() const;
    void setHeaderWidget(QWidget *headerWidget);

    QWidget *footerWidget() const;
    void setFooterWidget(QWidget *footerWidget);

    int paperWidth() const;
    void setPaperWidth(int value);

    int paperHeight() const;
    void setPaperHeight(int value);

    void createTableView();
    void initTableView();
    void initHeaderFooter();
    void initPageGeometry();
    void initHeaderView();
    void initDelegate();
    void initColWidths();
    void initRowHeights();
    void installEventFilter();
    void initDataModel();
    void initTableViewMisc();
    void initColumnHidden();
    void configGridLine();
    void connectTableViewSignalSlot();

private:
    ZoomFactor m_factor;

    QWidget *m_headerWidget;
    QWidget *m_footerWidget;

    int m_tableViewWidth;
    int m_tableViewHeight;
    int m_rightMargin;
    int m_pageCount;
    int m_dPageHeight;
    int m_dPageWidth;
    int m_leftMargin;
    int m_headerHeight;
    int m_footerHeight;
    int m_curPageNo;

    GlodonPaperTableView *m_tableView;
    GlodonHeaderView::ResizeMode m_resizeMode;
    GLDDocViewTableViewFactory *m_tableViewFactory;
    GLDDocView *m_docView;

    friend class GLDDocView;
};

#endif // GLDPAPERWIDGET_H
