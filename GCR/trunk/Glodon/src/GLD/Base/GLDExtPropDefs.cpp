#include "GLDExtPropDefs.h"
#include "GLDStreamUtils.h"
#include "GLDException.h"
#include "GLDStrUtils.h"
#include "GLDGlobal.h"
#include "GLDStrings.h"

class CGLDExtPropDefsPrivate
{
public:
  CGLDExtPropDefsPrivate(CGLDExtPropDefs *parent)
    : q_ptr(parent), m_tag(0)
  {
  }

private:
  CGLDExtPropDefs * const q_ptr;
  Q_DECLARE_PUBLIC(CGLDExtPropDefs);

  GLDHash<GString, int> m_codeHash;
  GInterfaceObjectList<CGLDExtPropDef *> m_list;
  PtrInt m_tag;
};

/* CGLDExtPropDefs */
CGLDExtPropDefs::CGLDExtPropDefs() : GInterfaceObject()
    , d_ptr(new CGLDExtPropDefsPrivate(this))
{
}

CGLDExtPropDefs::~CGLDExtPropDefs()
{
    Q_D(CGLDExtPropDefs);
    d->m_codeHash.clear();
    d->m_list.clear();
    freePtr(d);
}

CGLDExtPropDef *CGLDExtPropDefs::insertObj(long index)
{
    Q_D(CGLDExtPropDefs);
    onChanging();
    CGLDExtPropDef *result = doCreateExtPropDef();
    if (index < 0 || index >= count())
    {
        d->m_list.append(result);
    }
    else
    {
        d->m_list.insert(index, result);
    }
    freeCodeHash();
    onChanged();
    return result;
}

CGLDExtPropDef *CGLDExtPropDefs::findObj(const GString &code)
{
    Q_D(CGLDExtPropDefs);
    int nIndex = indexOf(code);
    if (nIndex < 0)
        return NULL;
    else
        return d->m_list[nIndex];
}

CGLDExtPropDef *CGLDExtPropDefs::itemObjs(long index)
{
    Q_D(CGLDExtPropDefs);
    return d->m_list[index];
}

void CGLDExtPropDefs::loadFromStream(GStream *stream)
{
    clear();
    int nIntCount = readIntFromStream(stream);
    for (int i = 0; i < nIntCount; i++)
    {
        insertObj(-1)->loadFromStream(stream);
    }
}

void CGLDExtPropDefs::saveToStream(GStream *stream)
{
    Q_D(CGLDExtPropDefs);
    writeIntToStream(stream, d->m_list.count());
    for (int i = 0; i < d->m_list.count(); i++)
    {
        d->m_list[i]->saveToStream(stream);
    }
}

void CGLDExtPropDefs::compile()
{
    buildCodeHash();
}

void CGLDExtPropDefs::clearCompileInfo()
{
    freeCodeHash();
}

void CGLDExtPropDefs::assign(CGLDExtPropDefs *source)
{
    GMemoryStream stream;
    // todo stream.setDevice();
    source->saveToStream(&stream);
    stream.seek(0);
    loadFromStream(&stream);
}

CGLDExtPropDef *CGLDExtPropDefs::doCreateExtPropDef()
{
    return new CGLDExtPropDef(this);
}

void CGLDExtPropDefs::onChanging()
{
    // 派生类实现
}

void CGLDExtPropDefs::onChanged()
{
    // 派生类实现
}

CGLDExtPropDef *CGLDExtPropDefs::addObj()
{
    return insertObj(-1);
}

void CGLDExtPropDefs::buildCodeHash()
{
    Q_D(CGLDExtPropDefs);
    d->m_codeHash.clear();
    for (int i = 0; i < d->m_list.count(); ++i)
    {
        GString code = d->m_list[i]->code();
        if (code.length() == 0)
        {
            gldError(getGLDi18nStr(g_MissingExtPropCode));
        }
        else if (d->m_codeHash.contains(code))
        {
            gldErrorFmt(getGLDi18nStr(g_ExtPropCodeExist), code);
        }
        else
        {
            d->m_codeHash.insert(code, i);
        }
    }
}

void CGLDExtPropDefs::freeCodeHash()
{
    Q_D(CGLDExtPropDefs);
    d->m_codeHash.clear();
}

CGLDExtPropDef * GSPMETHODCALLTYPE CGLDExtPropDefs::add()
{
    return insert(-1);
}

CGLDExtPropDef * GSPMETHODCALLTYPE CGLDExtPropDefs::insert(long index)
{
    CGLDExtPropDef *result = insertObj(index);
    if (result != NULL)
    {
        result->AddRef();
    }
    return result;
}

void GSPMETHODCALLTYPE CGLDExtPropDefs::Delete(long index)
{
    if (index < 0 || index >= count())
        return;

    Q_D(CGLDExtPropDefs);
    d->m_list.removeAt(index);
}

void GSPMETHODCALLTYPE CGLDExtPropDefs::remove(GString code)
{
    Delete(indexOf(code));
}

void GSPMETHODCALLTYPE CGLDExtPropDefs::move(long fromIndex, long toIndex)
{
    Q_D(CGLDExtPropDefs);
    d->m_list.move(fromIndex, toIndex);
}

void GSPMETHODCALLTYPE CGLDExtPropDefs::clear()
{
    Q_D(CGLDExtPropDefs);
    d->m_codeHash.clear();
    d->m_list.clear();
}

CGLDExtPropDef * GSPMETHODCALLTYPE CGLDExtPropDefs::find(GString code)
{
    CGLDExtPropDef *result = findObj(code);
    if (result != NULL)
    {
        result->AddRef();
    }
    return result;
}

long GSPMETHODCALLTYPE CGLDExtPropDefs::indexOf(GString code)
{
    Q_D(CGLDExtPropDefs);
    if (!d->m_codeHash.isEmpty())
    {
        return d->m_codeHash.value(code, -1);
    }
    else
    {
        for (int i = 0; i < d->m_list.count(); i++)
        {
            if (sameText(d->m_list[i]->code(), code))
                return i;
        }
        return -1;
    }
}

long GSPMETHODCALLTYPE CGLDExtPropDefs::count()
{
    Q_D(CGLDExtPropDefs);
    return d->m_list.count();
}

CGLDExtPropDef * GSPMETHODCALLTYPE CGLDExtPropDefs::items(long index)
{
    CGLDExtPropDef *result = itemObjs(index);
    if (result != NULL)
    {
        result->AddRef();
    }
    return result;
}

GString GSPMETHODCALLTYPE CGLDExtPropDefs::asString()
{
    Q_D(CGLDExtPropDefs);
    GString result;
    if (d->m_list.count() == 0)
        return result;
    for (int i = 0; i < d->m_list.count(); i++)
    {
        result.append(" | ").append(d->m_list[i]->asString());
    }
    return result;
}

void GSPMETHODCALLTYPE CGLDExtPropDefs::setAsString(GString value)
{
    GStringList values = value.split("|");
    clear();
    for (int i = 0; i < values.count(); i++)
    {
        GString value = trim(values[i]);
        if (value.length() > 0)
        {
            insertObj(-1)->setAsString(value);
        }
    }
}

CGLDExtPropDef * GSPMETHODCALLTYPE CGLDExtPropDefs::extPropByName(GString code)
{
    CGLDExtPropDef *result = findObj(code);
    if (result != NULL)
        result->AddRef();
    return result;
}

PtrInt GSPMETHODCALLTYPE CGLDExtPropDefs::tag()
{
    Q_D(CGLDExtPropDefs);
    return d->m_tag;
}

void GSPMETHODCALLTYPE CGLDExtPropDefs::setTag(PtrInt value)
{
    Q_D(CGLDExtPropDefs);
    d->m_tag = value;
}

class CGLDExtPropDefPrivate
{
public:
  CGLDExtPropDefPrivate(CGLDExtPropDef *parent)
    : q_ptr(parent)
  {
  }

private:
  CGLDExtPropDef * const q_ptr;
  Q_DECLARE_PUBLIC(CGLDExtPropDef);

  CGLDExtPropDefs *m_owner;
  GString m_code;
  GString m_value;
  PtrInt m_tag;
};
/* CGLDExtPropDef */

CGLDExtPropDef::CGLDExtPropDef(CGLDExtPropDefs *owner)
    : d_ptr(new CGLDExtPropDefPrivate(this)), GInterfaceObject()

{
    Q_D(CGLDExtPropDef);
    assert(owner != NULL);
    d->m_owner = owner;
    d->m_tag = 0 ;
}

CGLDExtPropDef::~CGLDExtPropDef()
{
    Q_D(CGLDExtPropDef);
    freePtr(d);
}

void CGLDExtPropDef::loadFromStream(GStream *stream)
{
    setCode(readStrFromStream(stream));
    setValue(readMemoFromStream(stream));
}

void CGLDExtPropDef::saveToStream(GStream *stream)
{
    Q_D(CGLDExtPropDef);
    writeStrToStream(stream, d->m_code);
    writeMemoToStream(stream, d->m_value);
}

GString GSPMETHODCALLTYPE CGLDExtPropDef::code()
{
    Q_D(CGLDExtPropDef);
    return d->m_code;
}

void GSPMETHODCALLTYPE CGLDExtPropDef::setCode(GString value)
{
    Q_D(CGLDExtPropDef);
    d->m_owner->onChanging();
    d->m_code = trim(value);
    d->m_owner->freeCodeHash();
    d->m_owner->onChanged();
}

GString GSPMETHODCALLTYPE CGLDExtPropDef::value()
{
    Q_D(CGLDExtPropDef);
    return d->m_value;
}

void GSPMETHODCALLTYPE CGLDExtPropDef::setValue(GString value)
{
    Q_D(CGLDExtPropDef);
    d->m_owner->onChanging();
    d->m_value = value;
    d->m_owner->onChanged();
}

GString GSPMETHODCALLTYPE CGLDExtPropDef::asString()
{
    Q_D(CGLDExtPropDef);
    return d->m_code + "=" + d->m_value;
}

void GSPMETHODCALLTYPE CGLDExtPropDef::setAsString(GString value)
{
    Q_D(CGLDExtPropDef);
    GStringList list = value.split("=");

    if (list.count() != 2)
    {
        d->m_code = "";
        d->m_value = "";
    }
    else
    {
        d->m_code = list[0];
        d->m_value = list[1];
    }
}

PtrInt GSPMETHODCALLTYPE CGLDExtPropDef::tag()
{
    Q_D(CGLDExtPropDef);
    return d->m_tag;
}

void GSPMETHODCALLTYPE CGLDExtPropDef::setTag(PtrInt value)
{
    Q_D(CGLDExtPropDef);
    d->m_tag = value;
}
