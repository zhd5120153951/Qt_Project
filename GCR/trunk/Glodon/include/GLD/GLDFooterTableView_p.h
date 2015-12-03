#ifndef GLDFOOTERTABLEVIEW_P_H
#define GLDFOOTERTABLEVIEW_P_H

#include "GLDFilterTableView_p.h"

class GlodonFooterTableViewPrivate : public GlodonFilterTableViewPrivate
{
    Q_DECLARE_PUBLIC(GlodonFooterTableView)
public:
    GlodonFooterTableViewPrivate() : m_dataModel(NULL), footerModel(NULL), footBodyModel(NULL),
        footer(NULL), totalRowAtFooter(true), footerRowCount(0) {}

    void init();
    void connectFooterSignal();
    //ºÏ¼ÆÐÐ
    QAbstractItemModel *m_dataModel;
    GlodonFooterModel *footerModel;
    GlodonFooterBodyModel *footBodyModel;
    GFooterView *footer;
    bool totalRowAtFooter;
    int footerRowCount;
};

#endif // GLDFOOTERTABLEVIEW_P_H
