#include "GLDExcelGridIterator.h"

#include <algorithm>
#include <QFont>
#include <QColor>
#include <QRect>
#include "GLDGlobal.h"
#include "GLDStrUtils.h"
#include "GLDException.h"
#include "GLDStrings.h"

#include "libxl.h"

const int c_minColWidth               = 10;
const int c_isCheckWidth              = 50;
const int c_symbolWidth               = 100;
const int c_indexWidth                = 50;
const int c_minColWidthPix            = c_minColWidth * 8.5;

/*GEGExcelGrid*/
IGEGRecordIterator *GEGExcelGrid::recordIterator()
{
    return p->recordIterator();
}

IGEGFieldIterator *GEGExcelGrid::fieldIterator()
{
    return p->fieldIterator();
}

IGEGRecordIterator *GEGExcelGrid::createRecordIterator()
{
    return p->createRecordIterator();
}

IGEGFieldIterator *GEGExcelGrid::createFieldIterator()
{
    return p->createFieldIterator();
}

/*CGEGExcelGridX*/

CGEGExcelGrid::CGEGExcelGrid(GEGExcelModel *model):
    m_recordIterator(NULL), m_fieldIterator(NULL), m_xlsModel(model)
{

}

CGEGExcelGrid::~CGEGExcelGrid()
{
    freeAndNil(m_recordIterator);
    freeAndNil(m_fieldIterator);
}

IGEGRecordIterator* CGEGExcelGrid::recordIterator()
{
    if (NULL == m_recordIterator)
    {
        m_recordIterator = new CGEGRecordIterator(m_xlsModel);
    }
    return m_recordIterator;
}

IGEGFieldIterator* CGEGExcelGrid::fieldIterator()
{
    if (NULL == m_fieldIterator)
    {
        m_fieldIterator = new CGEGFieldIterator(m_xlsModel);
    }
    return m_fieldIterator;
}

IGEGRecordIterator* CGEGExcelGrid::createRecordIterator()
{
    freeAndNil(m_recordIterator);
    m_recordIterator = new CGEGRecordIterator(m_xlsModel);
    return m_recordIterator;
}

IGEGFieldIterator *CGEGExcelGrid::createFieldIterator()
{
    freeAndNil(m_fieldIterator);
    m_fieldIterator = new CGEGFieldIterator(m_xlsModel);
    return m_fieldIterator;
}

HRESULT CGEGExcelGrid::_QueryInterface(const IID &riid, void **ppvObject)
{
    if (riid == __uuidof(IGEGExcelGrid))
    {
        this->AddRef();
        *ppvObject = static_cast<IGEGExcelGrid *>(this);
        return NOERROR;
    }
    else
        return GInterfaceObject::_QueryInterface(riid, ppvObject);
}

/*GEGRecordIterator*/
void GEGRecordIterator::first()
{
    p->first();
}

void GEGRecordIterator::next()
{
    p->next();
}

void GEGRecordIterator::prev()
{
    p->prev();
}

void GEGRecordIterator::last()
{
    p->last();
}

IGEGRecord *GEGRecordIterator::current()
{
    return p->current();
}

bool GEGRecordIterator::isDone()
{
    return p->isDone();
}

/*CGEGRecordIterator*/
CGEGRecordIterator::CGEGRecordIterator(GEGExcelModel *model) : m_currentIndex(0),
    m_model(model)
{
    loadItems();
}

void CGEGRecordIterator::first()
{
    m_currentIndex = 0;
}

void CGEGRecordIterator::next()
{
    m_currentIndex++;
}

void CGEGRecordIterator::prev()
{
    if (m_currentIndex > 0)
    {
        m_currentIndex--;
    }
}

void CGEGRecordIterator::last()
{
    m_currentIndex = m_recordList.count() - 1;
}

IGEGRecord* CGEGRecordIterator::current()
{
    return m_recordList.at(m_currentIndex);
}

bool CGEGRecordIterator::isDone()
{
    return (m_currentIndex == m_recordList.count());
}

HRESULT CGEGRecordIterator::_QueryInterface(const IID &riid, void **ppvObject)
{
    if (riid == __uuidof(IGEGRecordIterator))
    {
        this->AddRef();
        *ppvObject = static_cast<IGEGRecordIterator *>(this);
        return NOERROR;
    }
    else
        return GInterfaceObject::_QueryInterface(riid, ppvObject);
}

void CGEGRecordIterator::loadItems()
{
    m_recordList.clear();
    for (int i = c_DATA_BEGIN_ROWNO; i <= m_model->rowCount(); i++)
    {
        GString strIdentity = "";
        if (m_model->rowIsIdentity(m_model->dataRow(i), strIdentity))
        {
            CGEGRecord *record = new CGEGRecord();
            record->setExcelRowNo(m_model->dataRow(i));//excel中的行号
            record->setCode(strIdentity);
            record->setIsChecked(m_model->rowIsChecked(m_model->dataRow(i)));
            for (int j = 0; j < m_model->colRuleCount(); j++)
            {
                GEGColRule *rule = m_model->colRule(j);
                if (rule->isMatched())
                {
                    GEGValue value;
                    QModelIndex oIndex = m_model->index(i , rule->matchCol() + 3);
                    value.value = m_model->data(oIndex, Qt::DisplayRole);
                    value.type = m_model->dataType(m_model->dataRow(i) , rule->matchCol());
                    record->addValues(rule->identity(), value);
                }
            }
            m_recordList.push_back(record);
        }
    }
}

/*GEGFieldIteratorX*/
void GEGFieldIterator::first()
{
   p->first();
}

void GEGFieldIterator::next()
{
   p->next();
}

void GEGFieldIterator::prev()
{
    p->prev();
}

void GEGFieldIterator::last()
{
    p->last();
}

IGEGField* GEGFieldIterator::current()
{
    return p->current();
}

bool GEGFieldIterator::isDone()
{
    return p->isDone();
}

/*CGEGFieldIteratorX*/
CGEGFieldIterator::CGEGFieldIterator(GEGExcelModel *model) : m_currentIndex(0),
    m_model(model)
{
    loadItems();
}

CGEGFieldIterator::~CGEGFieldIterator()
{

}

void CGEGFieldIterator::first()
{
    m_currentIndex = 0;
}

void CGEGFieldIterator::next()
{
    m_currentIndex++;
}

void CGEGFieldIterator::prev()
{
    if (m_currentIndex > 0)
    {
        m_currentIndex--;
    }
}

void CGEGFieldIterator::last()
{
    m_currentIndex = m_fieldList.count() - 1;
}

IGEGField* CGEGFieldIterator::current()
{
    return m_fieldList.at(m_currentIndex);
}

bool CGEGFieldIterator::isDone()
{
    return (m_currentIndex == m_fieldList.count());
}

HRESULT CGEGFieldIterator::_QueryInterface(const IID &riid, void **ppvObject)
{
    if (riid == __uuidof(IGEGFieldIterator))
    {
        this->AddRef();
        *ppvObject = static_cast<IGEGFieldIterator *>(this);
        return NOERROR;
    }
    else
        return GInterfaceObject::_QueryInterface(riid, ppvObject);
}

void CGEGFieldIterator::loadItems()
{
    m_fieldList.clear();
    for (int i = 0; i <= m_model->activeSheet()->lastCol(); i++)
    {
        GString strRuleIdentity = "";
        if (m_model->colIsIdentity(i, strRuleIdentity))
        {
            CGEGField *field = new CGEGField();
            field->setCode(strRuleIdentity);
            GEGColRule *rule = m_model->findColRule(strRuleIdentity);
            field->setDataType(rule->dataType());
            field->setName(rule->name());
            m_fieldList.push_back(field);
        }
    }
}

/*GEGRecordX*/
GEGValue GEGRecord::fullValues(const GString &fieldName)
{
    return p->fullValues(fieldName);
}

bool GEGRecord::addValues(const GString &fieldName, const GEGValue &value)
{
    return p->addValues(fieldName, value);
}

/*CGEGRecordX*/
CGEGRecord::CGEGRecord() : m_code(""), m_isChecked(false)
{

}

GEGValue CGEGRecord::fullValues(const GString &fieldName)
{
    QHash<GString, GEGValue>::const_iterator it = m_values.find(fieldName);
    if (it != m_values.end())
    {
        return it.value();
    }
    else
    {
        return GEGValue();
    }
}

bool CGEGRecord::addValues(const GString &fieldName, const GEGValue &value)
{
    QHash<GString, GEGValue>::const_iterator it = m_values.find(fieldName);
    if (it == m_values.end())
    {
        m_values.insert(fieldName, value);
        return true;
    }
    return false;
}

HRESULT CGEGRecord::_QueryInterface(const IID &riid, void **ppvObject)
{
    if (riid == __uuidof(IGEGRecord))
    {
        this->AddRef();
        *ppvObject = static_cast<IGEGRecord *>(this);
        return NOERROR;
    }
    else
        return GInterfaceObject::_QueryInterface(riid, ppvObject);
}

/*CGEGFieldX*/
CGEGField::CGEGField() : m_code(""), m_value(GVariant())
{

}

HRESULT CGEGField::_QueryInterface(const IID &riid, void **ppvObject)
{
    if (riid == __uuidof(IGEGField))
    {
        this->AddRef();
        *ppvObject = static_cast<IGEGField *>(this);
        return NOERROR;
    }
    else
        return GInterfaceObject::_QueryInterface(riid, ppvObject);
}

/*GEGRule*/
GEGRule::GEGRule(GEGExcelModel *model) :  m_owner(model),
    m_identity(""), m_visible(true), m_tag(0)
{

}

GEGRule::~GEGRule()
{

}

/*GEGColRule*/
GEGColRule::GEGColRule(GEGExcelModel *model) : GEGRule(model), m_matchCol(-1), m_dataType(GEGdtEmply)
{

}

GEGColRule::~GEGColRule()
{

}

bool GEGColRule::macthKeyWord(const GString &value)
{
    bool result = false;
    for (int i = 0; i < m_tokens.count(); i++)
    {
        QRegExp *oExpr = m_tokens.at(i);
        if (oExpr->exactMatch(value))
        {
            result = true;
            break;
        }
    }
    return result;
}

bool GEGColRule::match(int col, bool force)
{
    bool result = doMatch(col, force);
    if (result)
    {
        m_matchCol = col;
//        writeColIdentity();
    }
    else
    {
        m_matchCol = -1;
    }
    return result;
}

void GEGColRule::clear()
{
    if (isMatched())
    {
        //todo
        m_matchCol = -1;
    }
}

void GEGColRule::reset()
{
    if (visible())
    {
        m_matchCol = -1;
    }
}

bool GEGColRule::isMatched()
{
    return (m_matchCol != -1);
}

void GEGColRule::compile()
{
    buildTokens();
}

bool GEGColRule::doMatch(int col, bool force)
{
    bool result = false;
    if (force)
    {
        result = true;
    }
    else
    {
        int nLastRow = model()->activeSheet()->lastRow();
        for (int i = 0; i <= nLastRow; i++)
        {
            GString value = GString((const QChar *) model()->activeSheet()->readStr(i, col));
            if (macthKeyWord(value))
            {
                result = true;
                break;
            }
        }
    }
    return result;
}

//void GEGColRule::writeColIdentity()
//{
//    if (visible())
//    {
//        //todo
//    }
//}

void GEGColRule::buildTokens()
{
    GStringList oKeyWords = m_keyWords;
    GString strIdentity = identity();
    if (-1 == oKeyWords.indexOf(strIdentity))
    {
        oKeyWords.push_back(strIdentity);
    }
    m_tokens.clear();
    for (int i = 0; i < oKeyWords.count(); ++i)
    {
        GString sKeyWord = oKeyWords.at(i);
        GString sExpr = "(\\w)*";
        for (int j = 0; j < sKeyWord.length(); ++j)
        {
            if (0 == j)
            {
                sExpr += sKeyWord.at(j);
            }
            else
            {
                sExpr = sExpr + "(\\w|\\s)*" + sKeyWord.at(j);
            }
        }
        sExpr += "(\\w)*";

        QRegExp *oExpr = new QRegExp();
        oExpr->setPattern(sExpr);
        m_tokens.push_back(oExpr);
    }
}

/*GEGRowRule*/
GEGRowRule::GEGRowRule(GEGExcelModel *model) : GEGRule(model), m_regExpr(new QRegExp)
{

}

GEGRowRule::~GEGRowRule()
{
    freeAndNil(m_regExpr);
}

bool GEGRowRule::match(int row, bool force)
{
    bool result = doMatch(row, force);
    if (result)
    {
        writeRowIdentity(row);
    }
    return result;
}

void GEGRowRule::clear(int row)
{
    G_UNUSED(row);
}

bool GEGRowRule::doMatch(int row, bool force)
{
    if (force)
    {
        return true;
    }
    else
    {
        return false;
    }
    G_UNUSED(row);
}

bool GEGRowRule::excuteRegExpr(const GString &regExpr, const GString &inputStr)
{
    m_regExpr->setPattern(regExpr);
    return m_regExpr->exactMatch(inputStr);
}

GString GEGRowRule::asString(int row, int col)
{
    return model()->asString(row, col);
}

GString GEGRowRule::asString(GEGColRule *colRule, int row)
{
    return model()->asString(colRule, row);
}

GString GEGRowRule::asString(const GString &identity, int row)
{
    return model()->asString(identity, row);
}

bool GEGRowRule::asBool(int row, int col)
{
    return model()->asBool(row, col);
}

int GEGRowRule::asInt(int row, int col)
{
    return model()->asInteger(row, col);
}

double GEGRowRule::asDouble(int row, int col)
{
    return model()->asDouble(row, col);
}

void GEGRowRule::writeRowIdentity(int row)
{
    if (visible())
    {
        GString id = identity();
        model()->addRowIdentity(row, id);
    }
}

/* GEGExcelModel */

GEGExcelModel::GEGExcelModel(QObject *parent) : GlodonXLSModel(parent),
    m_firstDataCol(0), m_firstDataRow(0), m_allowAutoMatch(false)
{
//    registerRules();
//    initializeRules();
//    loadRules();
}

GEGExcelModel::~GEGExcelModel()
{

}

GEGColRule *GEGExcelModel::addColRule()
{
    GEGColRule *result = new GEGColRule(this);
    m_colRules.push_back(result);
    return result;
}

void GEGExcelModel::addColRule(GEGColRule *rule)
{
    m_colRules.push_back(rule);
}

GEGColRule *GEGExcelModel::addColRule(const GString &identity, const GLDVector<GString> keywords)
{
    GEGColRule *result = addColRule();
    result->setIdentity(identity);
    for (int i = 0; i < keywords.count(); i++)
    {
        GString strKeyWord = keywords.at(i);
        result->keyWords().push_back(strKeyWord);
    }
    return result;
}

GEGColRule *GEGExcelModel::addColRule(const GString &identity)
{
    return addColRule(identity, GLDVector<GString>());
}

GEGRowRule *GEGExcelModel::addRowRule()
{
    GEGRowRule *result = new GEGRowRule(this);
    m_rowRules.push_back(result);
    return result;
}

void GEGExcelModel::addRowRule(GEGRowRule *rule)
{
    m_rowRules.push_back(rule);
}

GEGColRule *GEGExcelModel::findColRule(const GString &identity)
{
    int nIndex = indexOfColRule(identity);
    if (nIndex != -1)
        return m_colRules.at(nIndex);
    else
        return NULL;
}

GEGRowRule *GEGExcelModel::findRowRule(const GString &identity)
{
    int nIndex = indexOfRowRule(identity);
    if (-1 != nIndex)
        return m_rowRules.at(nIndex);
    else
        return NULL;
}

GEGColRule *GEGExcelModel::colRule(int index)
{
    if ((index < 0) || (index > m_colRules.count() - 1))
        return NULL;
    return m_colRules.at(index);
}

GEGRowRule *GEGExcelModel::rowRule(int index)
{
    if ((index < 0) || (index > m_rowRules.count() - 1))
        return NULL;
    return m_rowRules.at(index);
}

void GEGExcelModel::clearColRules()
{
    m_colRules.clear();
}

void GEGExcelModel::clearRowRules()
{
    m_rowRules.clear();
}

void GEGExcelModel::setActive(bool value)
{
    if (value)
    {
        initializeRules();
        if (m_allowAutoMatch)
        {
            autoMatch();
        }
    }
}

GString GEGExcelModel::asString(int dataRow, int dataCol)
{
    GString result = "";

    if ((dataRow < 0) || (dataCol < 0))
    {
        return result;
    }

    if (NULL != activeSheet())
    {
        GEGDataType oType = dataType(dataRow, dataCol);
        switch (oType)
        {
        case GEGdtString:
        {
            result = GString((const QChar *)(activeSheet()->readStr(dataRow, dataCol)));
            break;
        }
        case GEGdtNumber:
        {
            double dValue = activeSheet()->readNum(dataRow, dataCol);
            result = floatToStr(dValue);
            break;
        }
        default:
            break;
        }
    }
    return result;
}

GString GEGExcelModel::asString(GEGColRule *rule, int dataRow)
{
    GString result = "";
    if ((NULL != rule) && (rule->isMatched()))
    {
        result = asString(dataRow, rule->matchCol());
    }
    return result;
}

/*!
 * \brief 通过列规则的name和行号来取得格子的内容（Excel对应的行号）
 * \param name:列规则的name
 * \param dataRow:excel当中的行号
 * \return 格子的内容
 */
GString GEGExcelModel::asString(const GString &name, int dataRow)
{
    bool bFind = false;
    GEGColRule *rule = NULL;
    for (int i = 0; i < m_colRules.count(); i++)
    {
        rule = m_colRules.at(i);
        if (rule->name() == name)
        {
            bFind = true;
            break;
        }
    }
    if (! bFind)
    {
        return asString(findColRule(name), dataRow);
    }
    else
    {
        return asString(rule, dataRow);
    }
}

bool GEGExcelModel::asBool(int dataRow, int dataCol)
{
    bool result = false;

    if ((dataRow < 0) || (dataCol < 0))
    {
        return false;
    }

    if (NULL != activeSheet())
    {
        result = activeSheet()->readBool(dataRow, dataCol);
    }
    return result;
}

int GEGExcelModel::asInteger(int dataRow, int dataCol)
{
    double result = 0;

    if ((dataRow < 0) || (dataCol < 0))
    {
        return result;
    }

    if (NULL != activeSheet())
    {
        result = activeSheet()->readNum(dataRow, dataCol);
    }
    return result;
}

double GEGExcelModel::asDouble(int dataRow, int dataCol)
{
    double result = 0;

    if ((dataRow < 0) || (dataCol < 0))
    {
        return result;
    }

    if (NULL != activeSheet())
    {
        result = activeSheet()->readNum(dataRow, dataCol);
    }
    return result;
}

GEGDataType GEGExcelModel::dataType(int dataRow, int dataCol)
{
    if (NULL == activeSheet())
        return GEGdtEmply;


    if ((dataRow < 0) || dataCol < 0)
    {
        return GEGdtEmply;
    }

    switch (activeSheet()->cellType(dataRow, dataCol))
    {
    case libxl::CELLTYPE_EMPTY:
        return GEGdtEmply;
    case libxl::CELLTYPE_NUMBER:
        return GEGdtNumber;
    case libxl::CELLTYPE_STRING:
        return GEGdtString;
    case libxl::CELLTYPE_BOOLEAN:
        return GEGdtBool;
    case libxl::CELLTYPE_BLANK:
        return GEGdtBlank;
    case libxl::CELLTYPE_ERROR:
        return GEGdtError;
    default:
        return GEGdtEmply;
    }
}

QRect GEGExcelModel::getMergeRect(int row, int col)
{
    if (NULL == activeSheet())
    {
        gldError(getGLDi18nStr(g_rsHasNoActiveSheet));
    }
    QRect rResult;
    int nRowFirst, nRowLast, nColFirst, nColLast;
    if (activeSheet()->getMerge(row, col, &nRowFirst, &nRowLast, &nColFirst, &nColLast))
    {
        rResult.setTop(nRowFirst);
        rResult.setBottom(nRowLast);
        rResult.setLeft(nColFirst);
        rResult.setRight(nColLast);
    }
    else
    {
        rResult.setTop(row);
        rResult.setBottom(row);
        rResult.setLeft(col);
        rResult.setRight(col);
    }
    return rResult;
}

GString GEGExcelModel::findRowIdentityByRow(int row)
{
    GString result;
    rowIsIdentity(row, result);
    return result;
}

void GEGExcelModel::doRegisterRules()
{

}

void GEGExcelModel::doAutoMatchRow()
{
    if (NULL == activeSheet())
        return;
    m_rowIdentityHash.clear();
    for (int i = 0; i < this->rowCount(); ++i)
    {
        m_rowCheckHash.insert(i, false);
    }
    GObjectList<GEGRowRule *> oList;
    for (int i = 0; i < m_rowRules.count(); i++)
    {
        oList.push_back(m_rowRules.at(i));
    }
    QHash<int, GString>::const_iterator it;
    while (oList.count() > 0)
    {
        GEGRowRule *rule = oList.at(0);
        oList.remove(rule);
        for (int i = 0; i < rowCount(); i++)
        {
            it = m_rowIdentityHash.find(dataRow(i));
            if (it != m_rowIdentityHash.end())
                continue;
            m_rowCheckHash.insert(dataRow(i), rule->match(i, false));
        }
    }
}

void GEGExcelModel::doAutoMatchCol()
{
    if (NULL == activeSheet())
        return;
    for (int i = 0; i < m_colRules.count(); ++i)
    {
        m_colRules.at(i)->reset();
    }
    m_colIdentityHash.clear();
    for (int col = m_firstDataCol; col <= activeSheet()->lastCol(); ++col)
    {
        for (int nRule = 0; nRule < m_colRules.count(); nRule++)
        {
            GEGColRule *rule = m_colRules.at(nRule);
            if (rule->isMatched())
                continue;
            if (rule->match(col, false))
            {
                if (activeSheet()->colWidth(col) < c_minColWidth)
                {
                    activeSheet()->setCol(col, col + 1, c_minColWidth);
                }
                m_colIdentityHash.insert(col, rule->identity());
                break;
            }
        }
    }
}

QVariant GEGExcelModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return GVariant();
    int nCol = index.column();
    int nRow = index.row();
    if (inTitleRow(index))
    {
        return indexTitleData(index, role);
    }
    else if (!inDRange(nRow, nCol))
    {
        if (Qt::DisplayRole == role)
        {
            if (nRow < c_DATA_BEGIN_ROWNO
                    && nCol < c_DATA_BEGIN_COLNO)
            {
                return GString("");
            }
            else if (inIsExportCol(index))
            {
                return configDisplayIsExportData(nRow);
            }
            else if (inColMatchedResultRow(index))
            {
                return configDisplayColData(nCol);
            }
            else if (inRowMatchedResultCol(index))
            {
                return configDisplayRowData(nRow);
            }
            else
                return GVariant();
        }
        else if (Qt::FontRole == role)
        {
            //识别成功的加粗
            QFont headerfont = QFont();
            if (c_COL_METCHED_RESULT_ROWNO == nRow)
            {
                QHash<int, GString>::const_iterator it = m_colIdentityHash.find(dataCol(nCol));
                if (it != m_colIdentityHash.end())
                {
                    headerfont.setBold(true);
                }
            }
            else if (c_ROW_METCHED_RESULT_COLNO == nCol)
            {
                QHash<int, GString>::const_iterator it = m_rowIdentityHash.find(dataRow(nRow));
                if (it != m_rowIdentityHash.end())
                {
                    headerfont.setBold(true);
                }
            }
            return headerfont;
        }
        else if (Qt::TextColorRole == role)
        {
            if (index.row() == c_COL_METCHED_RESULT_ROWNO
                    || index.column() == c_ROW_METCHED_RESULT_COLNO)
            {
                return QColor(Qt::blue);
            }
        }
        else if (Qt::TextAlignmentRole == role)
        {
            if (index.row() == c_COL_METCHED_RESULT_ROWNO
                    || index.column() == c_ROW_METCHED_RESULT_COLNO)
            {
                return Qt::AlignCenter;
            }
        }
        else if (Qt::BackgroundColorRole == role)
        {
            GVariant oColor = QColor(255, 255, 255);
            GString sheetName = getSheetName();

            if (m_queryCellXLSAttributeEventList.count() > 0)
            {
                m_queryCellXLSAttributeEventList.doEvent(sheetName, index.row() - 1, index.column(),
                                                       Qt::BackgroundColorRole, oColor);
            }
            return oColor;
        }
    }
    else if (inIndexCol(index))
    {
        return indexColData(index, role); //序号那列的设置
    }
    if (Qt::TextAlignmentRole == role)
    {
        return Qt::AlignVCenter; //计价需求
    }
    else
    {
        int nDCol = dataCol(index.column());
        int nDRow = dataRow(index.row());
        QModelIndex oIndex = createIndex(nDRow, nDCol, index.internalPointer());
        GVariant oValue = GlodonXLSModel::data(oIndex, role);
        GString sheetName = getSheetName();

        if (role == Qt::BackgroundColorRole)
        {
            if (m_queryCellXLSAttributeEventList.count() > 0)
            {
                m_queryCellXLSAttributeEventList.doEvent(sheetName, index.row() - 1, index.column() - 2,
                                                       Qt::BackgroundColorRole, oValue);
            }
        }
        return oValue;
    }
}

bool GEGExcelModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role)
    {
    case Qt::EditRole:
        if (index.row() >= c_DATA_BEGIN_ROWNO
                && index.column() >= c_DATA_BEGIN_COLNO)
            return doSetEditData(dataRow(index.row()), dataCol(index.column()), value);
        else
        {
            if (inIsExportCol(index))
            {
                setRowCheckData(index.row() - c_DATA_BEGIN_ROWNO);
            }
            // 操作列
            if (inColMatchedResultRow(index))
            {
                GString id = value.toString();
                addColIdentity(index.column() - c_IndexCol - 1, id);
            }
            // 操作行
            if (inRowMatchedResultCol(index))
            {
                GString id = value.toString();
                addRowIdentity(index.row() - c_DATA_BEGIN_ROWNO, id);
            }
        }
        return true;
    default:
        return GlodonXLSModel::setData(index, value, role);
    }
    return false;
}

GVariant GEGExcelModel::configDisplayColData(int col) const
{
    QHash<int, GString>::const_iterator it = m_colIdentityHash.find(dataCol(col));
    GString result;
    if (it != m_colIdentityHash.end())
    {
        result = it.value();
    }
    else
    {
        result = tr("Unrecognized");//"未识别");
    }
    return result;
}

GVariant GEGExcelModel::configDisplayRowData(int row) const
{
    QHash<int, GString>::const_iterator it = m_rowIdentityHash.find(dataRow(row));
    GString result;
    if (it != m_rowIdentityHash.end())
    {
        result = it.value();
    }
    else
    {
        result = tr("Unrecognized");//"未识别");
    }
    return result;
}

GVariant GEGExcelModel::configDisplayIsExportData(int row) const
{
    QHash<int, bool>::const_iterator it = m_rowCheckHash.find(dataRow(row));
    if (it != m_rowCheckHash.end())
    {
        return it.value();
    }
    else
    {
        return GVariant(false);
    }
}

int GEGExcelModel::dataCol(int col) const
{
    return col - c_DATA_BEGIN_COLNO - 1;
}

int GEGExcelModel::dataRow(int row) const
{
    return row - c_DATA_BEGIN_ROWNO;
}

void GEGExcelModel::addRowIdentity(int row, GString &identity)
{
    if (m_rowIdentityChangeEventList.count() > 0)
    {
        GString strOldValue = "";
        QHash<int, GString>::const_iterator it = m_rowIdentityHash.find(row);
        if (it != m_rowIdentityHash.end())
        {
            strOldValue = it.value();
        }
        m_rowIdentityChangeEventList.doEvent(row, strOldValue, identity);
    }

    QHash<int, GString>::const_iterator it = m_rowIdentityHash.find(row);
    if (it != m_rowIdentityHash.end())
    {
        if (sameText(it.value(), identity))
        {
            m_rowIdentityHash.remove(row);
            m_rowCheckHash.remove(row);
            return;
        }
    }
    m_rowIdentityHash.insert(row, identity);
    m_rowCheckHash.insert(row, true);
}

void GEGExcelModel::addColIdentity(int col, GString &identity)
{
    GString strOldValue = "";
    int nSameIdentityCol = -1;
    int nSameCol = -1;
    for (QHash<int, GString>::const_iterator it = m_colIdentityHash.begin()
         ; it != m_colIdentityHash.end()
         ; ++it)
    {
        if (it.value() == identity)
        {
            nSameIdentityCol = it.key();
            strOldValue = it.value();
        }
        if (it.key() == col)
        {
            GEGColRule *oRule = findColRule(it.value());
            oRule->setMatchCol(-1);
            nSameCol = it.key();
        }
    }
    if (nSameIdentityCol > -1)
    {
        m_colIdentityHash.remove(nSameIdentityCol);
    }
    if (nSameCol > -1)
    {
        m_colIdentityHash.remove(nSameCol);
    }
    GEGColRule *rule = findColRule(identity);
    if (NULL == rule)
    {
        gldError(getGLDi18nStr(g_rsInvalideRule));
        return;
    }
    if (m_colIdentityChangeEventList.count() > 0)
    {
        m_colIdentityChangeEventList.doEvent(col, strOldValue, identity);
    }
    if (nSameIdentityCol != col)
    {
        rule->setMatchCol(col);
        if (activeSheet()->colWidth(col) < c_minColWidth)
        {
            activeSheet()->setCol(col, col + 1, c_minColWidth);
        }
        m_colIdentityHash.insert(col, identity);
    }
    else
    {
        rule->setMatchCol(-1);
    }
}

void GEGExcelModel::setRowCheckData(int row)
{
    QHash<int, bool>::const_iterator it = m_rowCheckHash.find(row);
    bool bValue = it.value();
    m_rowCheckHash.insert(row, !bValue);
}

void GEGExcelModel::setAllRowsCheckData(bool value)
{
    for (QHash<int, bool>::const_iterator it = m_rowCheckHash.begin()
         ; it != m_rowCheckHash.end()
         ; ++it)
    {
        m_rowCheckHash.insert(it.key(), value);
    }
}

/*!
 * \brief 设置行选中
 * \param row
 * \param check
 */
void GEGExcelModel::setRowCheck(int row, bool check)
{
    m_rowCheckHash.insert(row, check);
}

void GEGExcelModel::removeRowIdentity(int row)
{
    m_rowIdentityHash.remove(row);
    m_rowCheckHash.insert(row, false);
}

void GEGExcelModel::removeColIdentity(int col)
{
    m_colIdentityHash.remove(col);
}

bool GEGExcelModel::colIsIdentity(int col, GString &identity)
{
    QHash<int, GString>::const_iterator it = m_colIdentityHash.find(col);
    if (it != m_colIdentityHash.end())
    {
        identity = it.value();
        return true;
    }
    else
    {
        identity = "";
        return false;
    }
}

bool GEGExcelModel::rowIsIdentity(int row, GString &identity)
{
    QHash<int, GString>::const_iterator it = m_rowIdentityHash.find(row);
    if (it != m_rowIdentityHash.end())
    {
        identity = it.value();
        return true;
    }
    else
    {
        identity = "";
        return false;
    }
}

bool GEGExcelModel::rowIsChecked(int row)
{
    QHash<int, bool>::const_iterator it = m_rowCheckHash.find(row);
    if (it != m_rowCheckHash.end())
    {
        return it.value();
    }
    else
    {
        return false;
    }
}

int GEGExcelModel::getColWidth(int col)
{
    if (col == c_IS_EXPORT_COLNO)
    {
        return c_isCheckWidth;
    }
    else if (col == c_ROW_METCHED_RESULT_COLNO)
    {
        return c_symbolWidth;
    }
    else if (col == c_IndexCol)
    {
        return c_indexWidth;
    }
    else
    {
        QModelIndex oIndex = createIndex(c_DATA_BEGIN_ROWNO, col);
        GVariant oVar = data(oIndex, gidrColWidthRole);
        int nWidth = oVar.toInt();
        return nWidth < c_minColWidthPix ? c_minColWidthPix : nWidth;
    }
}

int GEGExcelModel::getRowHeight(int row)
{
    if ((row == c_COL_METCHED_RESULT_ROWNO) || (row == c_titleRow))
    {
        return 20;
    }
    else
    {
        // 96 是一般的分辨率；1/72 是 1皮克 = 1/72 英寸；
        return this->activeSheet()->rowHeight(dataRow(row)) * 96 * 1.0 / 72;
    }
}

QModelIndex GEGExcelModel::getPrevUnmatchedRow(QModelIndex &index)
{
    for (int nRow = index.row() - 1; nRow >= c_DATA_BEGIN_ROWNO; --nRow)
    {
        GString sIdentity;
        if (!rowIsIdentity(nRow, sIdentity))
        {
            return createIndex(nRow, index.column(), index.internalPointer());
        }
    }
    return QModelIndex();
}

QModelIndex GEGExcelModel::getNextUnmatchedRow(QModelIndex &index)
{
    for (int nRow = index.row() + 1; nRow < this->rowCount(); ++nRow)
    {
        GString sIdentity;
        if (!rowIsIdentity(nRow, sIdentity))
        {
            return createIndex(nRow, index.column(), index.internalPointer());
        }
    }
    return QModelIndex();
}

bool GEGExcelModel::inRowMatchedResultCol(const QModelIndex &index) const
{
    return (index.row() >= c_DATA_BEGIN_ROWNO
            && index.column() == c_ROW_METCHED_RESULT_COLNO);
}

bool GEGExcelModel::inColMatchedResultRow(const QModelIndex &index) const
{
    return (index.row() == c_COL_METCHED_RESULT_ROWNO
            && index.column() > c_IndexCol);
}

bool GEGExcelModel::inIndexCol(const QModelIndex &index) const
{
    return (index.row() >= c_DATA_BEGIN_ROWNO)
            && (index.column() == c_IndexCol);
}

bool GEGExcelModel::inTitleRow(const QModelIndex &index) const
{
    return (index.row() == c_titleRow);
}

bool GEGExcelModel::inIsExportCol(const QModelIndex &index) const
{
    return (index.row() >= c_DATA_BEGIN_ROWNO
            && index.column() == c_IS_EXPORT_COLNO);
}

bool GEGExcelModel::isDataCol(int col) const
{
    return col >= c_DATA_BEGIN_COLNO;
}

bool GEGExcelModel::isDataRow(int row) const
{
    return row >= c_DATA_BEGIN_ROWNO - 1;
}

bool GEGExcelModel::inDRange(int row, int col) const
{
    return isDataCol(col) && isDataRow(row);
}

int GEGExcelModel::rowCount(const QModelIndex &parent) const
{
    return GlodonXLSModel::rowCount(parent) + c_DATA_BEGIN_ROWNO;
}

int GEGExcelModel::columnCount(const QModelIndex &parent) const
{
    return GlodonXLSModel::columnCount(parent) + c_DATA_BEGIN_COLNO + 1;
}

int GEGExcelModel::indexOfColRule(const GString &identity)
{
    for (int i = 0; i < m_colRules.count(); i++)
    {
        if (sameText(m_colRules.at(i)->identity(), identity))
            return i;
    }
    return -1;
}

int GEGExcelModel::indexOfRowRule(const GString &identity)
{
    for (int i = 0; i < m_rowRules.count(); i++)
    {
        if (sameText(m_rowRules.at(i)->identity(), identity))
            return i;
    }
    return -1;
}

int GEGExcelModel::lastMatchDataCol()
{
    int result = -1;
    for (int i = 0; i < m_colRules.count(); i++)
    {
        result = std::max(m_colRules.at(i)->matchCol(), result);
    }
    return result;
}

void GEGExcelModel::autoMatchCol()
{
    doAutoMatchCol();
}

void GEGExcelModel::autoMatchRow()
{
    doAutoMatchRow();
}

void GEGExcelModel::autoMatch()
{
    autoMatchCol();
    autoMatchRow();
}

void GEGExcelModel::initializeRules()
{
    for (int i = 0; i < m_colRules.count(); i++)
    {
        m_colRules.at(i)->compile();
    }
    for (int i = 0; i < m_rowRules.count(); i++)
    {
        m_rowRules.at(i)->compile();
    }
}

void GEGExcelModel::registerRules()
{
    //todo
    doRegisterRules();
}

void GEGExcelModel::loadRules()
{

}

QVariant GEGExcelModel::indexColData(const QModelIndex &index, int role) const
{
    if (Qt::DisplayRole == role)
    {
        if (index.row() == c_DATA_BEGIN_ROWNO - 1)
        {
            return getGLDi18nStr(g_rsTitleIndex); //序号
        }
        else
        {
            return QString::number(index.row() - c_DATA_BEGIN_ROWNO + 1);
        }
    }
    else if (Qt::TextAlignmentRole == role)
    {
        return Qt::AlignCenter;
    }
    else if (Qt::BackgroundColorRole == role)
    {
        GVariant oColor = QColor(255, 255, 255);
        GString sheetName = getSheetName();

        if (m_queryCellXLSAttributeEventList.count() > 0)
        {
            m_queryCellXLSAttributeEventList.doEvent(sheetName, index.row() - 1, index.column(),
                                                   Qt::BackgroundColorRole, oColor);
        }
        return oColor;
    }
    return QVariant();
}

QVariant GEGExcelModel::indexTitleData(const QModelIndex &index, int role) const
{
    if (Qt::DisplayRole == role)
    {
        if (0 == index.column())
        {
            return getGLDi18nStr(g_rsTitleSelect); //选择
        }
        else if (1 == index.column())
        {
            return getGLDi18nStr(g_rsTitleSymbol);//标识
        }
        else if (2 == index.column())
        {
            return getGLDi18nStr(g_rsTitleIndex); //序号
        }
        else
        {
            return QChar(QLatin1Char('A' + index.column() - 3));
        }
    }
    else if (Qt::TextAlignmentRole == role)
    {
        return Qt::AlignCenter;
    }
    return QVariant();
}

void GEGExcelRowIdentityChangeEventList::doEvent(int row, const GString &oldValue, GString &newValue)
{
    for (int i = 0; i < count(); i++)
    {
        at(i)->onRowIdentityChanged(row, oldValue, newValue);
    }
}

void GEGExcelColIdentityChangeEventList::doEvent(int col, const GString &oldValue, GString &newValue)
{
    for (int i = 0; i < count(); i++)
    {
        at(i)->onColIdentityChanged(col, oldValue, newValue);
    }
}

bool GLDXLSQueryCellAttributeEventList::doEvent(GString &sheetName, int row, int col, int role, GVariant &attribute) const
{
    bool result = false;

    for (int i = 0; i < count(); ++i)
    {
        at(i)->onQueryCellXLSAttribute(sheetName, row, col, role, attribute, result);
        if (result)
        {
            break;
        }
    }

    return result;
}
