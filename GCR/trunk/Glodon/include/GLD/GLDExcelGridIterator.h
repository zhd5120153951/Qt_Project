#ifndef GLDEXCELGRIDITERATOR_H
#define GLDEXCELGRIDITERATOR_H

#include <QRegExp>
#include <QHash>
#include "GLDExcelGridIntf.h"
#include "GLDString.h"
#include "GLDStringList.h"
#include "GLDXLSModel.h"
#include "GLDObjectList.h"

#include "GLDComptr.h"
#include "GLDUnknwn.h"
#include "GLDObject.h"
#include "GLDTableView_Global.h"

static const int c_IS_EXPORT_COLNO          = 0; // 是否导出标志所在列
static const int c_ROW_METCHED_RESULT_COLNO = 1; // 行匹配结果所在的列
static const int c_COL_METCHED_RESULT_ROWNO = 0; // 列匹配结果所在的行

static const int c_DATA_BEGIN_ROWNO         = c_COL_METCHED_RESULT_ROWNO + 2;
static const int c_DATA_BEGIN_COLNO         = c_ROW_METCHED_RESULT_COLNO + 1;

static const int c_titleRow                 = 1;

static const int c_IndexCol                 = c_ROW_METCHED_RESULT_COLNO + 1; //序号列

class CGEGFieldIterator;
class CGEGRecordIterator;
class CGEGField;
class CGEGRecord;
class GEGExcelModel;
class GEGRecord;
class GEGField;
class GEGRecordIterator;
class GEGFieldIterator;

class GLDTABLEVIEWSHARED_EXPORT GEGExcelRowIdentityChangeEvent
{
public:
    virtual void onRowIdentityChanged(int row, const GString &oldValue, GString &newValue) = 0;
};

class GLDTABLEVIEWSHARED_EXPORT GEGExcelColIdentityChangeEvent
{
public:
    virtual void onColIdentityChanged(int col, const GString &oldValue, GString &newValue) = 0;
};

class GLDTABLEVIEWSHARED_EXPORT GEGExcelRowIdentityChangeEventList : public GLDVector<GEGExcelRowIdentityChangeEvent *>
{
public:
    void doEvent(int row, const GString &oldValue, GString &newValue);
};

class GLDTABLEVIEWSHARED_EXPORT GEGExcelColIdentityChangeEventList : public GLDVector<GEGExcelColIdentityChangeEvent *>
{
public:
   void doEvent(int col, const GString &oldValue, GString &newValue);
};

class GLDTABLEVIEWSHARED_EXPORT GLDXLSQueryCellAttributeEvent
{
public:
    virtual ~GLDXLSQueryCellAttributeEvent() {}

    virtual void onQueryCellXLSAttribute(GString &sheetName, int row, int col,
                                       int role, GVariant &attribute, bool &handled) = 0;
};

class GLDTABLEVIEWSHARED_EXPORT GLDXLSQueryCellAttributeEventList : public GLDVector<GLDXLSQueryCellAttributeEvent *>
{
public:
    bool doEvent(GString &sheetName, int row, int col,
                 int role, GVariant &attribute) const;
};

class GLDTABLEVIEWSHARED_EXPORT GEGExcelGrid : public GComPtr<IGEGExcelGrid>
{
public:
    inline GEGExcelGrid(): GComPtr<IGEGExcelGrid>(){}
    inline GEGExcelGrid(IGEGExcelGrid *lp) : GComPtr<IGEGExcelGrid>(lp){}
public:
    IGEGRecordIterator *recordIterator();
    IGEGFieldIterator *fieldIterator();
    IGEGRecordIterator *createRecordIterator();
    IGEGFieldIterator *createFieldIterator();
};

class GLDTABLEVIEWSHARED_EXPORT CGEGExcelGrid : public GInterfaceObject, public IGEGExcelGrid
{
public:
    CGEGExcelGrid(GEGExcelModel *model);
    ~CGEGExcelGrid();
public:
    DECLARE_IUNKNOWN
    IGEGRecordIterator * GLDMETHODCALLTYPE recordIterator();
    IGEGFieldIterator * GLDMETHODCALLTYPE fieldIterator();
    IGEGRecordIterator * GLDMETHODCALLTYPE createRecordIterator();
    IGEGFieldIterator * GLDMETHODCALLTYPE createFieldIterator();
protected:
    HRESULT STDMETHODCALLTYPE _QueryInterface(REFIID riid, void **ppvObject);
private:
    CGEGRecordIterator *m_recordIterator;
    CGEGFieldIterator *m_fieldIterator;
    GEGExcelModel *m_xlsModel;
};

// GEGRecordIterator
class GLDTABLEVIEWSHARED_EXPORT GEGRecordIterator : public GComPtr<IGEGRecordIterator>
{
public:
    inline GEGRecordIterator() : GComPtr<IGEGRecordIterator>(){}
    inline GEGRecordIterator(IGEGRecordIterator *lp) : GComPtr<IGEGRecordIterator>(lp){}
public:
    void first();
    void next();
    void prev();
    void last();
    IGEGRecord *current();
    bool isDone();
};

class GLDTABLEVIEWSHARED_EXPORT CGEGRecordIterator : public GInterfaceObject, public IGEGRecordIterator
{
public:
    CGEGRecordIterator(GEGExcelModel *model);
    ~CGEGRecordIterator() {}

public:
    DECLARE_IUNKNOWN
    void GLDMETHODCALLTYPE first();
    void GLDMETHODCALLTYPE next();
    void GLDMETHODCALLTYPE prev();
    void GLDMETHODCALLTYPE last();
    IGEGRecord * GLDMETHODCALLTYPE current();
    bool GLDMETHODCALLTYPE isDone();
protected:
    HRESULT STDMETHODCALLTYPE _QueryInterface(REFIID riid, void **ppvObject);
private:
    void loadItems();
private:
    GObjectList<CGEGRecord *> m_recordList; // todo linw ref
    int m_currentIndex;
    GEGExcelModel *m_model;
};

// GEGFieldIterator 接口
class GLDTABLEVIEWSHARED_EXPORT GEGFieldIterator : public GComPtr<IGEGFieldIterator>
{
public:
    inline GEGFieldIterator(): GComPtr<IGEGFieldIterator>(){}
    inline GEGFieldIterator(IGEGFieldIterator *lp): GComPtr<IGEGFieldIterator>(lp){}
public:
    void first();
    void next();
    void prev();
    void last();
    IGEGField *current();
    bool isDone();
};

class GLDTABLEVIEWSHARED_EXPORT CGEGFieldIterator : public GInterfaceObject, public IGEGFieldIterator
{
public:
    CGEGFieldIterator(GEGExcelModel *model);
    ~CGEGFieldIterator();
public:
    DECLARE_IUNKNOWN
    void GLDMETHODCALLTYPE first();
    void GLDMETHODCALLTYPE next();
    void GLDMETHODCALLTYPE prev();
    void GLDMETHODCALLTYPE last();
    IGEGField * GLDMETHODCALLTYPE current();
    bool GLDMETHODCALLTYPE isDone();
protected:
    HRESULT STDMETHODCALLTYPE _QueryInterface(REFIID riid, void **ppvObject);
private:
    void loadItems();
private:
    GObjectList<CGEGField *> m_fieldList; // todo linw ref
    int m_currentIndex;
    GEGExcelModel *m_model;
};

class GLDTABLEVIEWSHARED_EXPORT GEGRecord : public GComPtr<IGEGRecord>
{
public:
    inline GEGRecord(): GComPtr<IGEGRecord>(){}
    inline GEGRecord(IGEGRecord *lp): GComPtr<IGEGRecord>(lp){}
public:
    inline GString code() { return p->code(); }
    inline void setCode(const GString &value) { p->setCode(value); }
    inline bool isChecked() { return p->isChecked(); }
    inline void setIsChecked(bool isChecked) { p->setIsChecked(isChecked); }
    GEGValue fullValues(const GString &fieldName);
    bool addValues(const GString &fieldName, const GEGValue &value);
    inline int excelRowNo() { return p->excelRowNo(); }
    inline void setExcelRowNo(int rowNo) { p->setExcelRowNo(rowNo); }
};

class GLDTABLEVIEWSHARED_EXPORT CGEGRecord : public GInterfaceObject, public IGEGRecord
{
public:
    CGEGRecord();
    ~CGEGRecord() {}

public:
    DECLARE_IUNKNOWN
    inline GString GLDMETHODCALLTYPE code() { return m_code; }
    inline void GLDMETHODCALLTYPE setCode(const GString &value) { m_code = value; }
    inline bool GLDMETHODCALLTYPE isChecked() { return m_isChecked; }
    inline void GLDMETHODCALLTYPE setIsChecked(bool isChecked) { m_isChecked = isChecked; }
    GEGValue GLDMETHODCALLTYPE fullValues(const GString &fieldName);
    bool GLDMETHODCALLTYPE addValues(const GString &fieldName, const GEGValue &value);
    int GLDMETHODCALLTYPE excelRowNo() { return m_excelRowNo; }

    void GLDMETHODCALLTYPE setExcelRowNo(int rowNo) { m_excelRowNo = rowNo; }

protected:
    HRESULT STDMETHODCALLTYPE _QueryInterface(REFIID riid, void **ppvObject);
private:
    QHash<GString, GEGValue> m_values;
    GString m_code;
    bool m_isChecked;
    int m_excelRowNo;
};

class GLDTABLEVIEWSHARED_EXPORT GEGField : public GComPtr<IGEGField>
{
public:
    inline GEGField() : GComPtr<IGEGField>(){}
    inline GEGField(IGEGField *lp) : GComPtr<IGEGField>(lp){}
public:
    inline GString code() { return p->code(); }
    inline GEGDataType dataType() { return p->dataType(); }
    inline void setCode(const GString &code) { p->setCode(code); }
    inline void setDataType(GEGDataType type) { p->setDataType(type); }
    inline GString name() { return p->name(); }
    inline void setName(const GString &name) { p->setName(name); }
};

class GLDTABLEVIEWSHARED_EXPORT CGEGField : public GInterfaceObject, public IGEGField
{
public:
    CGEGField();
    ~CGEGField() {}

public:
    DECLARE_IUNKNOWN
    inline GString GLDMETHODCALLTYPE code() { return m_code; }
    inline GEGDataType GLDMETHODCALLTYPE dataType() { return m_dataType; }
    inline void GLDMETHODCALLTYPE setCode(const GString &code) { m_code = code; }
    inline void GLDMETHODCALLTYPE setDataType(GEGDataType type) { m_dataType = type; }
    inline GString GLDMETHODCALLTYPE name() { return m_name; }
    inline void GLDMETHODCALLTYPE setName(const GString &name) { m_name = name; }
protected:
    HRESULT STDMETHODCALLTYPE _QueryInterface(REFIID riid, void **ppvObject);
private:
    GString m_code;
    GVariant m_value;
    GEGDataType m_dataType;
    GString m_name;
};

class GLDTABLEVIEWSHARED_EXPORT GEGRule
{
public:
    GEGRule(GEGExcelModel *model);
    virtual ~GEGRule();
public:
    inline GString identity() { return m_identity; }
    inline void setIdentity(const GString &value) { m_identity = value; }
    inline bool visible() { return m_visible; }
    inline void setVisible(bool value) { m_visible = value; }
    inline int tag() { return m_tag; }
    inline void setTag(int value) { m_tag = value; }
    inline GEGExcelModel *model() { return m_owner; }
    virtual void compile() {}

private:
    GEGExcelModel *m_owner;
    GString m_identity;
    bool m_visible;
    int m_tag;
};

class GLDTABLEVIEWSHARED_EXPORT GEGColRule : public GEGRule
{
public:
    GEGColRule(GEGExcelModel *model);
    virtual ~GEGColRule();
public:
    virtual bool macthKeyWord(const GString &value);
    bool match(int col, bool force);
    void clear();
    void reset();
    bool isMatched();
    inline GStringList keyWords() { return m_keyWords; }
    inline int matchCol() { return m_matchCol; }
    inline void setMatchCol(int col) { m_matchCol = col; }
    inline GEGDataType dataType() { return m_dataType; }
    inline void setDataType(GEGDataType type) { m_dataType = type; }
    inline GString name() { return m_name; }
    inline void setName(const GString &name) { m_name = name; }
    virtual void compile();
protected:
    virtual bool doMatch(int col, bool force);
private:
//    void writeColIdentity();
    void buildTokens();
private:
    GStringList m_keyWords;
    GObjectList<QRegExp *> m_tokens;
    int m_matchCol;
    GEGDataType m_dataType;
    GString m_name;
};

class GLDTABLEVIEWSHARED_EXPORT GEGRowRule : public GEGRule
{
public:
    GEGRowRule(GEGExcelModel *model);
    virtual ~GEGRowRule();
public:
    bool match(int row, bool force);
    void clear(int row);
protected:
    virtual bool doMatch(int row, bool force);
    bool excuteRegExpr(const GString &regExpr, const GString &inputStr);
    GString asString(int row, int col);
    GString asString(GEGColRule *colRule, int row);
    GString asString(const GString &identity, int row);
    bool asBool(int row, int col);
    int asInt(int row, int col);
    double asDouble(int row, int col);
private:
    void writeRowIdentity(int row);
private:
    QRegExp *m_regExpr;
};

class  GLDTABLEVIEWSHARED_EXPORT GEGExcelModel : public GlodonXLSModel
{
    Q_OBJECT
public:
    GEGExcelModel(QObject *parent = 0);
    ~GEGExcelModel();
public:
    GEGColRule *addColRule();
    void addColRule(GEGColRule *rule);
    GEGColRule *addColRule(const GString &identity, const GLDVector<GString> keywords);
    GEGColRule *addColRule(const GString &identity);
    GEGRowRule *addRowRule();
    void addRowRule(GEGRowRule *rule);
    GEGColRule *findColRule(const GString &identity);
    GEGRowRule *findRowRule(const GString &identity);
    inline int colRuleCount() { return m_colRules.count(); }
    GEGColRule *colRule(int index);
    inline int rowRuleCount() { return m_rowRules.count(); }
    GEGRowRule *rowRule(int index);
    void clearColRules();
    void clearRowRules();

    inline void clearMatchedRow() { m_rowIdentityHash.clear(); }
    inline void clearMatchedCol() { m_colIdentityHash.clear(); }

     inline GLDXLSQueryCellAttributeEventList &queryCellXLSAttributeEventList() { return m_queryCellXLSAttributeEventList; }
public:
    void setActive(bool value);
    QModelIndex getPrevUnmatchedRow(QModelIndex &index);
    QModelIndex getNextUnmatchedRow(QModelIndex &index);
    bool inIsExportCol(const QModelIndex &index) const;
    bool inRowMatchedResultCol(const QModelIndex &index) const;
    bool inColMatchedResultRow(const QModelIndex &index) const;
    bool inIndexCol(const QModelIndex &index) const;
    bool inTitleRow(const QModelIndex &index) const;

    void addRowIdentity(int row, GString &identity);
    void addColIdentity(int col, GString &identity);
    void setRowCheckData(int row);
    void setAllRowsCheckData(bool value);
    void setRowCheck(int row, bool check);

    void removeRowIdentity(int row);
    void removeColIdentity(int col);

    bool colIsIdentity(int col, GString &identity);
    bool rowIsIdentity(int row, GString &identity);
    bool rowIsChecked(int row);

    int getColWidth(int col);//tableView的列
    int getRowHeight(int row);//tableView的行
public:
    void autoMatchCol();
    void autoMatchRow();
    void autoMatch();

    inline bool allowAutoMatch() { return m_allowAutoMatch; }
    inline void setAllowAutoMatch(bool value) { m_allowAutoMatch = value; }

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
public:
    GString asString(int dataRow, int dataCol);
    GString asString(GEGColRule *rule, int dataRow);
    GString asString(const GString &name, int dataRow);
    bool asBool(int dataRow, int dataCol);
    int asInteger(int dataRow, int dataCol);
    double asDouble(int dataRow, int dataCol);
    GEGDataType dataType(int dataRow, int dataCol);
    QRect getMergeRect(int row, int col);
    GString findRowIdentityByRow(int row);
public:
    GEGExcelRowIdentityChangeEventList m_rowIdentityChangeEventList;
    GEGExcelColIdentityChangeEventList m_colIdentityChangeEventList;
protected:
    virtual void doRegisterRules();
    virtual void doAutoMatchRow();
    virtual void doAutoMatchCol();
    bool isDataCol(int col) const;//以下为TableView的行列
    bool isDataRow(int row) const;
    int dataCol(int col) const;
    int dataRow(int row) const;
    bool inDRange(int row, int col) const;
private:
    GVariant configDisplayColData(int col) const;
    GVariant configDisplayRowData(int row) const;
    GVariant configDisplayIsExportData(int row) const;
    int indexOfColRule(const GString &identity);
    int indexOfRowRule(const GString &identity);
    int lastMatchDataCol();
    void initializeRules();
    void registerRules();
    void loadRules();
    QVariant indexColData(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant indexTitleData(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
    GLDXLSQueryCellAttributeEventList m_queryCellXLSAttributeEventList;
    GObjectList<GEGColRule *> m_colRules;
    GObjectList<GEGRowRule *> m_rowRules;
    QHash<int, GString> m_rowIdentityHash;
    QHash<int, bool> m_rowCheckHash;
    QHash<int, GString> m_colIdentityHash;
    int m_firstDataCol;
    int m_firstDataRow;
    bool m_allowAutoMatch;

    friend class GEGColRule;
    friend class GEGRowRule;
    friend class CGEGRecordIterator;
    friend class CGEGFieldIterator;
};

#endif // GLDEXCELGRIDITERATOR_H
