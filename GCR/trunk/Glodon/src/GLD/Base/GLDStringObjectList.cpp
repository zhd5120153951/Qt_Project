#include "GLDStringObjectList.h"

#include <QTextStream>

#include "GLDStream.h"
#include "GLDFile.h"
#include "GLDStrUtils.h"
#include "GLDException.h"
#include "GLDStreamUtils.h"
#include "GLDStrings.h"

const char *g_DuplicateString	 = QT_TRANSLATE_NOOP("GLD", "String list does not allow duplicates");// "String list does not allow duplicates");
const char *g_SortedListError	 = QT_TRANSLATE_NOOP("GLD", "Operation not allowed on sorted list");// "Operation not allowed on sorted list");
const char *g_ListIndexError		 = QT_TRANSLATE_NOOP("GLD", "List index out of bounds (%d)");// "List index out of bounds (%d)");


class GStringObjectListCompareEvent : public CSortCompareEvent
{
public:
    explicit GStringObjectListCompareEvent(GStringObjectList *stringObjectList);
    CompareResult compare(void *item1, void *item2);
private:
    explicit GStringObjectListCompareEvent();
private:
    GStringObjectList *m_stringObjectList;
};

GStringObjectListCompareEvent::GStringObjectListCompareEvent(GStringObjectList *stringObjectList)
{
    assert(stringObjectList != NULL);
    m_stringObjectList = stringObjectList;
}

CompareResult GStringObjectListCompareEvent::compare(void *item1, void *item2)
{
    GStringItem *it1 = (GStringItem*)item1;
    GStringItem *it2 = (GStringItem*)item2;
    int result = m_stringObjectList->compareStrings(it1->str, it2->str);
    if (result > 0)
    {
        return crGreaterThan;
    }
    else if (result < 0)
    {
        return crLessThan;
    }
    else
    {
        return crEqual;
    }
}

class GStringObjectListPrivate
{
public:
  GStringObjectListPrivate(GStringObjectList *parent)
    : q_ptr(parent)
  {
  }

  GStringObjectListPrivate()
    : q_ptr(NULL)
  {
  }

protected:
  GStringObjectList * const q_ptr;
  Q_DECLARE_PUBLIC(GStringObjectList);

private:
  bool m_caseSensitive;
  bool m_sorted;
  GStringObjectList::GDuplicates m_duplicates;
  char m_delimiter;
  byte m_defined;
  GString m_lineBreak;
  char m_quoteChar;
  char m_nameValueSeparator;
  bool m_strictDelimiter;
  GObjectList<GStringItem*> m_list;
  GString m_delimitedTextist;
  int m_updateCount;
};

GStringObjectList::GStringObjectList()
    : d_ptr(new GStringObjectListPrivate(this))
{
    Q_D(GStringObjectList);
    d->m_caseSensitive = false;
    d->m_sorted = false;
    d->m_duplicates = dupIgnore;
    d->m_delimiter = '\0';
    d->m_defined = 0;
    d->m_quoteChar = '\0' ;
    d->m_nameValueSeparator = '\0';
    d->m_strictDelimiter = false;
    d->m_updateCount = 0;
}

GStringObjectList::GStringObjectList(GStringObjectListPrivate &dd)
    : d_ptr(&dd)
{
    Q_D(GStringObjectList);
    d->m_caseSensitive = false;
    d->m_sorted = false;
    d->m_duplicates = dupIgnore;
    d->m_delimiter = '\0';
    d->m_defined = 0;
    d->m_quoteChar = '\0' ;
    d->m_nameValueSeparator = '\0';
    d->m_strictDelimiter = false;
    d->m_updateCount = 0;
}

GStringObjectList::GStringObjectList(const GStringObjectList &other)
    :  d_ptr(new GStringObjectListPrivate(this))
{
    Q_D(GStringObjectList);
    d->m_caseSensitive = other.d_ptr->m_caseSensitive;
    d->m_sorted = other.d_ptr->m_sorted;
    d->m_duplicates = other.d_ptr->m_duplicates;
    d->m_delimiter = other.d_ptr->m_delimiter;
    d->m_defined = other.d_ptr->m_defined;
    d->m_lineBreak = other.d_ptr->m_lineBreak;
    d->m_quoteChar = other.d_ptr->m_quoteChar;
    d->m_nameValueSeparator = other.d_ptr->m_nameValueSeparator;
    d->m_strictDelimiter = other.d_ptr->m_strictDelimiter;

    for (int i = 0; i < other.count(); ++i)
    {
        GStringItem *item = new GStringItem();
        item->str = other.d_ptr->m_list.at(i)->str;
        item->object = other.d_ptr->m_list.at(i)->object;

        d->m_list.push_back(item);
    }
    d->m_delimitedTextist = other.d_ptr->m_delimitedTextist;
    d->m_updateCount = other.d_ptr->m_updateCount;
}

GStringObjectList::~GStringObjectList()
{
    Q_D(GStringObjectList);
    clear();
    freePtr(d);
}

int GStringObjectList::add(const GString &s)
{
    return addObject(s, NULL);
}

int GStringObjectList::add(GStringItem *item)
{
    assert(item != NULL);
    int result = findItem(item->str, item->object);
    insertItem(result, item);
    return result;
}

int GStringObjectList::findItem(const GString &s, void *object)
{
    int result = 0;

    Q_D(GStringObjectList);
    if (!d->m_sorted)
    {
        result = d->m_list.count();
    }
    else if (find(s, result))
    {
        switch (d->m_duplicates)
        {
        case dupIgnore:
            return result;
        case dupError:
            gldError(getGLDi18nStr(g_DuplicateString), 0);
            break;
        default:
            break;
        }
    }
    return result;
    G_UNUSED(object)
}

int GStringObjectList::addObject(const GString &s, void *object)
{
    int result = findItem(s, object);
    insertItem(result, s, object);
    return result;
}

int GStringObjectList::addStrings(GStringObjectList *list)
{
    Q_D(GStringObjectList);
    beginUpdate();
    try
    {
        for (int i = 0; i < list->count(); ++i)
        {
            addObject(list->at(i)->str, list->at(i)->object);
        }
    }
    catch(...)
    {
        endUpdate();
        throw;
    }
    endUpdate();
    return d->m_list.count();
}

void GStringObjectList::insert(int index, const GString &s)
{
    insertObject(index, s, NULL);
}

void GStringObjectList::insertObject(int index, const GString &s, void *object)
{
    Q_D(GStringObjectList);
    if (d->m_sorted)
    {
        gldError(getGLDi18nStr(g_SortedListError), 0);
    }
    if (index < 0)
    {
        index = 0;
    }
    else if (index > d->m_list.count())
    {
        index = d->m_list.count();
    }
    insertItem(index, s, object);
}

bool GStringObjectList::find(const GString &s, int &index)
{
    Q_D(GStringObjectList);
    bool result = false;
    int nlow = 0;
    int nHigh = d->m_list.count() - 1;
    while (nlow <= nHigh)
    {
        int nIter = (nlow + nHigh) >> 1;
        int nCompare = compareStrings(d->m_list.at(nIter)->str, s);
        if (nCompare < 0)
        {
            nlow = nIter + 1;
        }
        else
        {
            nHigh = nIter - 1;
            if (nCompare == 0)
            {
                result = true;
                if (d->m_duplicates != dupAccept)
                {
                    nlow = nIter;
                }
            }
        }
    }
    index = nlow;
    return result;
}

bool GStringObjectList::caseSensitive() const
{
    Q_D(const GStringObjectList);
    return d->m_caseSensitive;
}

void GStringObjectList::setCaseSensitive(bool caseSensitive)
{
    Q_D(GStringObjectList);
    if (d->m_caseSensitive != caseSensitive)
    {
        d->m_caseSensitive = caseSensitive;
        if (d->m_sorted)
        {
            sort();
        }
    }
}

char GStringObjectList::delimiter()
{
    Q_D(GStringObjectList);
    if (!contains(d->m_defined, sdDelimiter))
    {
        d->m_delimiter = ',';
    }
    return d->m_delimiter;
}

void GStringObjectList::setDelimiter(char delimiter)
{
    Q_D(GStringObjectList);
    if ((d->m_delimiter != delimiter) || !contains(d->m_defined, sdDelimiter))
    {
        include(d->m_defined, sdDelimiter);
        d->m_delimiter = delimiter;
    }
}

GStringObjectList::GDuplicates GStringObjectList::duplicates() const
{
    Q_D(const GStringObjectList);
    return d->m_duplicates;
}

void GStringObjectList::setDuplicates(const GDuplicates &duplicates)
{
    Q_D(GStringObjectList);
    d->m_duplicates = duplicates;
}

bool GStringObjectList::sorted() const
{
    Q_D(const GStringObjectList);
    return d->m_sorted;
}

void GStringObjectList::setSorted(bool sorted)
{
    Q_D(GStringObjectList);
    if (d->m_sorted != sorted)
    {
        if (sorted)
        {
            sort();
        }
        d->m_sorted = sorted;
    }
}

GString GStringObjectList::lineBreak()
{
    Q_D(GStringObjectList);
    if (!contains(d->m_defined, sdLineBreak))
    {
        d->m_lineBreak = sLineBreak;
    }
    return d->m_lineBreak;
}

void GStringObjectList::setLineBreak(const GString &lineBreak)
{
    Q_D(GStringObjectList);
    if ((d->m_lineBreak != lineBreak) || !contains(d->m_defined, sdLineBreak))
    {
        include(d->m_defined, sdLineBreak);
        d->m_lineBreak = lineBreak;
    }
}

char GStringObjectList::quoteChar()
{
    Q_D(GStringObjectList);
    if (!contains(d->m_defined, sdQuoteChar))
    {
        d->m_quoteChar = '"';
    }
    return d->m_quoteChar;
}

void GStringObjectList::setQuoteChar(char quoteChar)
{
    Q_D(GStringObjectList);
    if ((d->m_quoteChar != quoteChar) || !contains(d->m_defined, sdQuoteChar))
    {
        include(d->m_defined, sdQuoteChar);
        d->m_quoteChar = quoteChar;
    }
}

char GStringObjectList::nameValueSeparator()
{
    Q_D(GStringObjectList);
    if (!contains(d->m_defined, sdNameValueSeparator))
    {
        d->m_nameValueSeparator = '=';
    }
    return d->m_nameValueSeparator;
}

void GStringObjectList::setNameValueSeparator(char nameValueSeparator)
{
    Q_D(GStringObjectList);
    if ((d->m_nameValueSeparator != nameValueSeparator) || !contains(d->m_defined, sdNameValueSeparator))
    {
        include(d->m_defined, sdNameValueSeparator);
        d->m_nameValueSeparator = nameValueSeparator;
    }
}

bool GStringObjectList::strictDelimiter()
{
    Q_D(GStringObjectList);
    if (!contains(d->m_defined, sdStrictDelimiter))
    {
        d->m_strictDelimiter = false;
    }
    return d->m_strictDelimiter;
}

void GStringObjectList::setStrictDelimiter(bool strictDelimiter)
{
    Q_D(GStringObjectList);
    if ((d->m_strictDelimiter != strictDelimiter) || !contains(d->m_defined, sdStrictDelimiter))
    {
        include(d->m_defined, sdStrictDelimiter);
        d->m_strictDelimiter = strictDelimiter;
    }
}

GString GStringObjectList::delimitedText()
{
    gint64 lDelimiters(0);

    GString result;
    int nCount = count();
    if ((nCount == 1) && (at(0)->str.length() == 0))
    {
        result = quoteChar() + quoteChar();
    }
    else
    {
        includeOfGint64(lDelimiters, 0);
        includeOfGint64(lDelimiters, quoteChar());
        includeOfGint64(lDelimiters, delimiter());
        if (!strictDelimiter())
        {
            for (gint64 i = 1; i <= ' '; ++i)
            {
                includeOfGint64(lDelimiters, i);
            }
        }
        for (int i = 0; i < nCount; ++i)
        {
            GString strTmp = at(i)->str;
            int nIndex = 0;
            while ((nIndex < strTmp.length()) &&
                   ((strTmp.at(nIndex).unicode() >= 64) || (!containsOfGint64(lDelimiters, gint64(strTmp.at(nIndex).unicode())))))
            {
                nIndex++;
            }

            if (nIndex <= strTmp.length() - 1)
            {
                strTmp = quotedStr(strTmp, quoteChar());
            }
            result = result + strTmp + delimiter();
        }
        result = copy(result, 0, result.length() - 1);
    }

    return result;
}

bool GStringObjectList::containsBeforeSpace(const gint64 set)
{
    bool result = false;
    for (gint64 i = 1; i <= ' '; ++i)
    {
        result = containsOfGint64(set, i);
        if (result)
        {
            break;
        }
    }
    return result;
}

void GStringObjectList::setDelimitedText(const GString &value)
{
    Q_D(GStringObjectList);
    GString delimitedText(value);
    beginUpdate();
    try
    {
        clear();
        if (value.length() == 0)
        {
            return;
        }
        int nIndex = 0;
        int nIndex1 = 0;
        if (!strictDelimiter())
        {
            gint64 context = 1;
            while ((delimitedText.at(nIndex).unicode() >= 1) && (delimitedText.at(nIndex).unicode() <= ' '))
            {
                ++nIndex;
                ++context;
            }
        }
        while (nIndex < delimitedText.length())
        {
            GString strTmp;
            if (delimitedText.at(nIndex).unicode() == quoteChar())
            {
                delimitedText = copy(delimitedText, nIndex);
                strTmp = extractQuotedStr(delimitedText, quoteChar());
            }
            else
            {
                nIndex1 = nIndex;
                int nLength = delimitedText.length();
                while ((nIndex < nLength)
                       && ((!d->m_strictDelimiter && (delimitedText.at(nIndex).unicode() > ' '))
                           || (d->m_strictDelimiter && (delimitedText.at(nIndex) != delimiter()))))
                {
                    nIndex++;
                }
                strTmp = copy(delimitedText, nIndex1, nIndex - nIndex1);
            }
            add(strTmp);
            int nLength = delimitedText.length();
            if (!d->m_strictDelimiter)
            {
                while ((nIndex < nLength)
                       && (delimitedText.at(nIndex).toLatin1() >= 1) && (delimitedText.at(nIndex).toLatin1() <= ' '))
                {
                    ++nIndex;
                }
            }
            if ((nIndex < nLength) && (delimitedText.at(nIndex).unicode() == delimiter()))
            {
                nIndex1 = nIndex;
                if (nIndex1 == delimitedText.length())
                {
                    add("");
                }
                do
                {
                    ++nIndex;
                } while ((nIndex < delimitedText.length())
                         && (!d->m_strictDelimiter
                             && ((delimitedText.at(nIndex).toLatin1() >= 1)
                                 && (delimitedText.at(nIndex).toLatin1() <= ' '))));
            }
        }
    }
    catch(...)
    {
        endUpdate();
        throw;
    }
    endUpdate();
}

void GStringObjectList::setCommaText(const GString &value)
{
    setDelimiter(',');
    setQuoteChar('\"');
    setDelimitedText(value);
}

GString GStringObjectList::commaText()
{
    Q_D(GStringObjectList);
     byte lOldDefined = d->m_defined;
     char cOldDelimiter = d->m_delimiter;
     char cOldQuoteChar = d->m_quoteChar;
     setDelimiter(',');
     setQuoteChar('"');
     GString result;
     try
     {
         result = delimitedText();
     }
     catch(...)
     {
         d->m_delimiter = cOldDelimiter;
         d->m_quoteChar = cOldQuoteChar;
         d->m_defined = lOldDefined;
         throw;
     }
     d->m_delimiter = cOldDelimiter;
     d->m_quoteChar = cOldQuoteChar;
     d->m_defined = lOldDefined;
     return result;
}

void GStringObjectList::insertItem(int index, const GString &s, void* object)
{
    Q_D(GStringObjectList);
    changing();
    GStringItem *item = new GStringItem();
    item->str = s;
    item->object = object;
    d->m_list.insert(index, item);
    changed();
}

void GStringObjectList::insertItem(int index, GStringItem *item)
{
    Q_D(GStringObjectList);
    assert(item != NULL);
    changing();
    d->m_list.insert(index, item);
    changed();
}

void GStringObjectList::changed()
{
    // todo
}

void GStringObjectList::loadFromFile(const GString &fileName)
{
    GFileStream file(fileName);
    if (!file.open(GStream::ReadOnly))
    {
        return;
    }
    file.seek(0);
    try
    {
        loadFromStream(&file);
    }
    catch(...)
    {
        file.close();
        throw;
    }
    file.close();
}

void GStringObjectList::loadFromStream(GStream *stream)
{
    beginUpdate();
    try
    {
        int nSize = stream->size() - stream->pos();
        GString strReaded = readFromStream(stream, nSize);
        setTextStr(strReaded);
    }
    catch(...)
    {
        endUpdate();
        throw;
    }
    endUpdate();
}

void GStringObjectList::saveToFile(const GString &fileName)
{
    GFileStream file(fileName);
    if (!file.open(GStream::WriteOnly))
    {
        return;
    }
    file.seek(0);
    try
    {
        saveToStream(&file);
    }
    catch(...)
    {
        file.close();
        throw;
    }
    file.close();
}

void GStringObjectList::saveToStream(GStream *stream)
{
    GString value = textStr();
    QTextStream textStream(stream);
    textStream << value;
    textStream.flush();
}

void GStringObjectList::beginUpdate()
{
    Q_D(GStringObjectList);
    if (d->m_updateCount == 0)
    {
        setUpdateState(true);
    }
    d->m_updateCount++;
}

void GStringObjectList::endUpdate()
{
    Q_D(GStringObjectList);
    d->m_updateCount--;
    if (d->m_updateCount == 0)
    {
        setUpdateState(false);
    }
}

void GStringObjectList::setUpdateState(bool updating)
{
    if (updating)
    {
        changing();
    }
    else
    {
        changed();
    }
}

void GStringObjectList::assign(GStringObjectList *source)
{
    Q_D(GStringObjectList);
    beginUpdate();
    try
    {
        clear();
        d->m_defined = source->d_ptr->m_defined;
        d->m_nameValueSeparator = source->d_ptr->m_nameValueSeparator;
        d->m_quoteChar = source->d_ptr->m_quoteChar;
        d->m_delimiter = source->d_ptr->m_delimiter;
        d->m_lineBreak = source->d_ptr->m_lineBreak;
        d->m_strictDelimiter = source->d_ptr->m_strictDelimiter;
        addStrings(source);
    }
    catch(...)
    {
        endUpdate();
        throw;
    }
    endUpdate();
}

void GStringObjectList::changing()
{
    // todo
}

void GStringObjectList::put(int index, const GString &s)
{
    Q_D(GStringObjectList);
    if (sorted())
    {
        gldError(getGLDi18nStr(g_SortedListError), 0);
    }
    checkIndex(index);
    changing();
    d->m_list.at(index)->str = s;
    changed();
}

GString GStringObjectList::extractName(const GString &s)
{
    int nPos = pos(nameValueSeparator(), s);
    if (nPos >= 0)
    {
        return copy(s, 0, nPos);
    }
    else
    {
        return GString();
    }
}

int GStringObjectList::indexOfObject(void* value)
{
    Q_D(GStringObjectList);
    for (int i = 0; i < d->m_list.count(); ++i)
    {
        if (d->m_list.at(i)->object == value)
        {
            return i;
        }
    }
    return -1;
}

int GStringObjectList::indexOf(const GString &value)
{
    Q_D(GStringObjectList);
    for (int i = 0; i < d->m_list.count(); ++i)
    {
        if (sameText(d->m_list.at(i)->str, value))
        {
            return i;
        }
    }
    return -1;
}

void GStringObjectList::checkIndex(int index) const
{
    Q_D(const GStringObjectList);
    if ((index < 0) || (index >= d->m_list.count()))
    {
        gldErrorFmt(getGLDi18nStr(g_ListIndexError), index);
    }
}

GString GStringObjectList::string(int index)
{
    Q_D(GStringObjectList);
    checkIndex(index);
    return d->m_list.at(index)->str;
}

void GStringObjectList::putString(int index, const GString &s)
{
    put(index, s);
}

void *GStringObjectList::object(int index)
{
    Q_D(GStringObjectList);
    checkIndex(index);
    return d->m_list.at(index)->object;
}

void GStringObjectList::putObject(int index, void *object)
{
    Q_D(GStringObjectList);
    checkIndex(index);
    changing();
    d->m_list.at(index)->object = object;
    changed();
}

void GStringObjectList::clear()
{
    Q_D(GStringObjectList);
    changing();
    d->m_list.clear();
    changed();
}

bool GStringObjectList::isEmpty()
{
    Q_D(GStringObjectList);
    return d->m_list.isEmpty();
}

int GStringObjectList::count() const
{
    Q_D(const GStringObjectList);
    return d->m_list.count();
}

int GStringObjectList::size()
{
    Q_D(GStringObjectList);
    return int(d->m_list.size());
}

GStringItem *GStringObjectList::at(int index) const
{
    Q_D(const GStringObjectList);
    checkIndex(index);
    return d->m_list.at(index);
}

void GStringObjectList::setIndex(int index, GStringItem *item)
{
    Q_D(GStringObjectList);
    if (d->m_sorted)
    {
        gldError(getGLDi18nStr(g_SortedListError), 0);
    }
    checkIndex(index);
    freeAndNil(d->m_list[index]);
    changing();
    d->m_list[index] = item;
    changed();
}

GStringItem *&GStringObjectList::operator [](int index)
{
    Q_D(GStringObjectList);
    checkIndex(index);
    return d->m_list[index];
}

void GStringObjectList::push_back(GStringItem *item)
{
    Q_D(GStringObjectList);
    if (d->m_sorted)
    {
        gldError(getGLDi18nStr(g_SortedListError), 0);
    }
    changing();
    d->m_list.push_back(item);
    changed();
}

void GStringObjectList::append(GStringItem *item)
{
    Q_D(GStringObjectList);
    if (d->m_sorted)
    {
        gldError(getGLDi18nStr(g_SortedListError), 0);
    }
    changing();
    d->m_list.append(item);
    changed();
}

void GStringObjectList::append(GObjectList<GStringItem *> &items)
{
    Q_D(GStringObjectList);
    if (d->m_sorted)
    {
        gldError(getGLDi18nStr(g_SortedListError), 0);
    }
    changing();
    d->m_list.append(items);
    changed();
}

GObjectList<GStringItem *> &GStringObjectList::list()
{
    Q_D(GStringObjectList);
    return d->m_list;
}

void GStringObjectList::Delete(int index)
{
    changing();
    removeAt(index);
    changed();
}

void GStringObjectList::removeAt(int index)
{
    Q_D(GStringObjectList);
    changing();
    d->m_list.removeAt(index);
    changed();
}

void GStringObjectList::exchange(int index1, int index2)
{
    Q_D(GStringObjectList);
    if (d->m_sorted)
    {
        gldError(getGLDi18nStr(g_SortedListError), 0);
    }
    changing();
    swap(index1, index2);
    changed();
}

void GStringObjectList::swap(int index1, int index2)
{
    Q_D(GStringObjectList);
    if (d->m_sorted)
    {
        gldError(getGLDi18nStr(g_SortedListError), 0);
    }
    changing();
    d->m_list.swap(index1, index2);
    changed();
}

void GStringObjectList::sort(bool ascend)
{
    GStringObjectListCompareEvent event(this);
    customSort(&event, ascend);
}

void GStringObjectList::customSort(CSortCompareEvent *customSortEvent, bool ascend)
{
    Q_D(GStringObjectList);
    changing();
    quickSort(d->m_list.list(), customSortEvent, ascend);
    changing();
}

GString GStringObjectList::value(const GString &name)
{
    Q_D(GStringObjectList);
    int nIndex = indexOfName(name);
    if (nIndex >= 0)
    {
        return copy(d->m_list.at(nIndex)->str, name.length() + 1, MaxInt);
    }
    else
    {
        return GString();
    }
}

void GStringObjectList::setValue(const GString &name, const GString &value)
{
    Q_D(GStringObjectList);
    int nIndex = indexOfName(name);
    if (value.length() > 0)
    {
        if (nIndex < 0)
        {
            nIndex = add("");
        }
        put(nIndex, name + nameValueSeparator() + value);
    }
    else
    {
        if (nIndex >= 0)
        {
            d->m_list.removeAt(nIndex);
        }
    }
}

GString GStringObjectList::valueFromIndex(int index)
{
    Q_D(GStringObjectList);
    if (index >= 0)
    {
        if (index >= 0)
        {
            QString sValue = d->m_list.at(index)->str;
            QString sName = names(index);
            if (!sName.isEmpty())
            {
                sValue = copy(sValue, sName.length() + 1, MaxInt); //+1:Ìø¹ýµÈÓÚ·ûºÅ
            }
            return sValue;
        }
    }
    return GString();
}

void GStringObjectList::setValueFromIndex(int index, const GString &value)
{
    Q_D(GStringObjectList);
    if (value.length() > 0)
    {
        if (index < 0)
        {
            index = add("");
        }
        put(index, names(index) + nameValueSeparator() + value);
    }
    else
    {
        if (index >= 0)
        {
            changing();
            d->m_list.removeAt(index);
            changed();
        }
    }
}

GString GStringObjectList::names(int index)
{
    Q_D(GStringObjectList);
    return extractName(d->m_list.at(index)->str);
}

int GStringObjectList::indexOfName(const GString &name)
{
    Q_D(GStringObjectList);
    int result = 0;
    for (result = 0; result < d->m_list.count(); ++result)
    {
        GString strTmp = d->m_list.at(result)->str;
        int nPos = pos(nameValueSeparator(), strTmp);
        if ((nPos >= 0) && (compareStrings(copy(strTmp, 0, nPos), name) == 0))
        {
            return result;
        }
    }
    return -1;
}

int GStringObjectList::compareStrings(const GString &s1, const GString &s2)
{
    Q_D(GStringObjectList);
    int result = 0;
    if (d->m_caseSensitive)
    {
        result = compareStr(s1, s2);
    }
    else
    {
        result = compareText(s1, s2);
    }
    return result;
}

void GStringObjectList::setTextStr(const GString &value)
{
    beginUpdate();
    try
    {
        clear();
        int nPos = value.length();
        if (nPos > 0)
        {
#ifdef WIN32
            nPos = 0;
            while (nPos < value.length())
            {
                int nStart = nPos;
                ushort ch = value.at(nPos).unicode();
                while ((ch != '\0') && (ch != '\r') && (ch != '\n'))
                {
                    nPos++;
                    if (nPos >= value.size())
                    {
                        break;
                    }
                    ch = value.at(nPos).unicode();
                }
                GString strTmp = copy(value, nStart, nPos - nStart);
                add(strTmp);
                if (nPos >= value.size())
                {
                    break;
                }
                ch = value.at(nPos).unicode();
                if (ch == '\r')
                {
                    nPos++;
                }
                ch = value.at(nPos).unicode();
                if (ch == '\n')
                {
                    nPos++;
                }
            }
#else
            // todo
            assert(false);
#endif
        }
    }
    catch(...)
    {
        endUpdate();
        throw;
    }
    endUpdate();
}

GString GStringObjectList::textStr()
{
    GString result;
    GString sLineBreak = lineBreak();
    for (int i = 0; i != count(); ++i)
    {
        result += (at(i)->str + sLineBreak);
    }
    return result;
}

class GHashedStringObjectListPrivate : public GStringObjectListPrivate
{
public:
  GHashedStringObjectListPrivate()
      : m_valueHashValid(false), m_nameHashValid(false)
  {
  }

private:
  Q_DECLARE_PUBLIC(GHashedStringObjectList);

  GLDHash<GString, int> m_valueHash;
  GLDHash<GString, int> m_nameHash;
  bool m_valueHashValid;
  bool m_nameHashValid;
};

GHashedStringObjectList::GHashedStringObjectList()
    : GStringObjectList(* (new GHashedStringObjectListPrivate))
{
}

GHashedStringObjectList::~GHashedStringObjectList()
{
    Q_D(GHashedStringObjectList);
    d->m_valueHash.clear();
    d->m_nameHash.clear();
}

int GHashedStringObjectList::indexOfName(const GString &name)
{
    Q_D(GHashedStringObjectList);
    int result = -1;
    updateNameHash();
    if (!caseSensitive())
    {
        result = d->m_nameHash.value(upperCase(name), -1);
    }
    else
    {
        result = d->m_nameHash.value(name, -1);
    }
    return result;
}

int GHashedStringObjectList::indexOf(const GString &value)
{
    Q_D(GHashedStringObjectList);
    int result = -1;
    updateValueHash();
    if (!caseSensitive())
    {
        result = d->m_valueHash.value(upperCase(value), -1);
    }
    else
    {
        result = d->m_valueHash.value(value, -1);
    }
    return result;
}

void GHashedStringObjectList::changed()
{
    Q_D(GHashedStringObjectList);
    GStringObjectList::changed();
    d->m_valueHashValid = false;
    d->m_nameHashValid = false;
}

void GHashedStringObjectList::updateValueHash()
{
    Q_D(GHashedStringObjectList);
    if (d->m_valueHashValid)
    {
        return;
    }
    d->m_valueHash.clear();
    for (int i = 0; i < count(); ++i)
    {
        if (!caseSensitive())
        {
            d->m_valueHash[upperCase(at(i)->str)] = i;
        }
        else
        {
            d->m_valueHash[at(i)->str] = i;
        }
        d->m_valueHashValid = true;
    }
}

void GHashedStringObjectList::updateNameHash()
{
    Q_D(GHashedStringObjectList);
    if (d->m_nameHashValid)
    {
         return;
    }
    d->m_nameHash.clear();
    for (int i = 0; i < count(); ++i)
    {
        GString key = at(i)->str;
        int nPos = pos(nameValueSeparator(), key);
        if (nPos >= 0)
        {
            if (!caseSensitive())
            {
                key = upperCase(copy(key, 0, nPos));
            }
            else
            {
                key = copy(key, 0, nPos);
            }
            d->m_nameHash[key] = i;
        }
    }
    d->m_nameHashValid = true;
}
