#ifndef GLDABSTRACTITEMMODEL_H
#define GLDABSTRACTITEMMODEL_H

#include <QAbstractItemModel>
#include "GLDVariant.h"
#include "GLDGlobal.h"

enum GLDItemDataRole
{
    gidrBaseRole = 256,
    gidrRowExpandedRole = 257,
    gidrColExpandedRole = 258,
    gidrRowHeightRole = 259,
    gidrColWidthRole = 260,
    gidrCommentRole = 261,
    gidrCellMargin = 262,
    gidrRowHeaderData = 263,
    gidrFooterRowCount = 264,
    gidrDiagonal = 265,
    gidrAntiDiagonal = 266,
    gidrMergeIDRole = 268,
    gidrConerDataRole = 269,
    gidrHeaderDecorationRole = 270,
    gidrConerTextAlignmentRole = 271,
    gidrHeaderTextAlignmentRole = 272,
    gidrHeaderFontRole = 273,
    gidrBoundLineRole = 274,
    gidrDecorationAlignmentRole = 275,
    gidrRightBoundLineColor = 276, // m_bDrawBoundLine为true时右边框线的颜色
    gidrBottomBoundLineColor = 277, // m_bDrawBoundLine为true时下边框线的颜色

    gidrGSPBaseDataColRole = 384,
    gidrUserBaseRole = 512,

    gidrAddUnEnableRole = 513,
    gidrRemoveUnEnableRole = 514,
    gidrCheckUnEnableRole = 515
};

class GlodonAbstractItemModelPrivate;
class GLDCOMMONSHARED_EXPORT GlodonAbstractItemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit GlodonAbstractItemModel(QObject *parent = 0);
    virtual ~GlodonAbstractItemModel();

public:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool showCheckBox();
    void setShowCheckBox(const bool &value);
    bool isItemIsTristate();
    void setIsTristate(const bool &value);
    bool isRecursiveCheck();
    void setRecursiveCheck(const bool &value);

    virtual void afterCurrentChanged(const QModelIndex &index);

signals:

public slots:

protected:
    GlodonAbstractItemModel(GlodonAbstractItemModelPrivate &dd, QObject *parent = 0);

private:
    bool recursiveCheck(const QModelIndex &index, const GVariant &value);
    bool recursiveCheckParent(const QModelIndex &index, const GVariant &value);
    void recursiveCheckParentPartCheck(const QModelIndex &index);

private:
    Q_DECLARE_PRIVATE(GlodonAbstractItemModel)
    Q_DISABLE_COPY(GlodonAbstractItemModel)
};

#endif // GLDABSTRACTITEMMODEL_H
