#ifndef GLDFILTERTABLEVIEW_P_H
#define GLDFILTERTABLEVIEW_P_H

#include "GLDTableView_p.h"

class GLDTABLEVIEWSHARED_EXPORT GlodonFilterTableViewPrivate : public GlodonTableViewPrivate
{
    Q_DECLARE_PUBLIC(GlodonFilterTableView)
public:
    GlodonFilterTableViewPrivate();
    void init();
    void initFilter();
    void connectFilterSignal();
public:
    //É¸Ñ¡ÐÐ
    GFilterView *filter;
};

#endif // GLDFILTERTABLEVIEW_P_H
