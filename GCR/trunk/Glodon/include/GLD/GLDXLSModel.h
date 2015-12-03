#ifndef GLDXLSMODEL_H
#define GLDXLSMODEL_H

#include "GLDString.h"
#include "GLDAbstractItemModel.h"
#include "GLDXLS.h"

GLDTABLEVIEWSHARED_EXPORT libxl::Book *loadxlsFile(const GString &fileName);

class GLDTABLEVIEWSHARED_EXPORT GlodonXLSModel : public GlodonAbstractItemModel
{
    Q_OBJECT
public:
    explicit GlodonXLSModel(QObject *parent = 0);
    virtual ~GlodonXLSModel();

    bool load(const GString &fileName);
    bool save(const GString &fileName);
    bool setSheetIndex(int index);

    inline bool editable() const
    {
        return m_editable;
    }
    inline void setEditable(bool value)
    {
        m_editable = value;
    }
    inline libxl::Book *book() const
    {
        return m_book;
    }
    inline libxl::Book *bakBook() const
    {
        return m_bakBook;
    }
    void setXlsBook(libxl::Book *oBook);
    void setXlsBakBook(libxl::Book *oBakBook);
    inline libxl::Sheet *activeSheet() const
    {
        return m_sheet;
    }
    GString getSheetName() const;

public:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                       int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

protected:
    virtual QVariant displayData(int row, int col) const;
    virtual QVariant editData(int row, int col) const;
    bool doSetEditData(int row, int col, const QVariant &value);

private:
    libxl::Book *m_book;
    libxl::Book *m_bakBook; //专门用来解析颜色
    libxl::Sheet *m_sheet;

    QList<libxl::Book*> m_listBook; //统一存储生成的m_book和m_bakBook，在析构时统一释放

    bool m_editable;

private:
    typedef GlodonAbstractItemModel inherited;
};

#endif // GLDXLSMODEL_H
