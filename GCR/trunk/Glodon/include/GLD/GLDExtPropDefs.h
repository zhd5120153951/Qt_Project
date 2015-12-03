#ifndef GLDEXTPROPDEFS_H
#define GLDEXTPROPDEFS_H

#include "GLDObject.h"
#include "GLDHash.h"
#include "GLDStrUtils.h"
#include "GLDTableView_Global.h"

class CGLDExtPropDef;
class CGLDExtPropDefs;

class CGLDExtPropDefsPrivate;
// 扩展属性容器
class GLDTABLEVIEWSHARED_EXPORT CGLDExtPropDefs : public GInterfaceObject
{
public:
    CGLDExtPropDefs();
    virtual ~CGLDExtPropDefs();
public:
    CGLDExtPropDef *GSPMETHODCALLTYPE add();
    CGLDExtPropDef *GSPMETHODCALLTYPE insert(long index);
    virtual void GSPMETHODCALLTYPE Delete(long index);
    virtual void GSPMETHODCALLTYPE remove(GString code);
    virtual void GSPMETHODCALLTYPE move(long fromIndex, long toIndex);
    virtual void GSPMETHODCALLTYPE clear();
    CGLDExtPropDef *GSPMETHODCALLTYPE find(GString code);
    virtual long GSPMETHODCALLTYPE indexOf(GString code);
    virtual long GSPMETHODCALLTYPE count();
    CGLDExtPropDef *GSPMETHODCALLTYPE items(long index);
    virtual GString GSPMETHODCALLTYPE asString();
    virtual void GSPMETHODCALLTYPE setAsString(GString value);
    CGLDExtPropDef *GSPMETHODCALLTYPE extPropByName(GString code);
    virtual PtrInt GSPMETHODCALLTYPE tag();
    virtual void GSPMETHODCALLTYPE setTag(PtrInt value);

public:
//    void loadFromXML(const IXMLNode node);
//    void saveToXML(const IXMLNode node);
    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);

    void assign(CGLDExtPropDefs *source);
    void compile();
    void clearCompileInfo();

    void buildCodeHash();
    void freeCodeHash();
    virtual void onChanging();
    virtual void onChanged();

    CGLDExtPropDef *addObj();
    CGLDExtPropDef *insertObj(long index);
    CGLDExtPropDef *findObj(const GString &code);
    CGLDExtPropDef *itemObjs(long index);
    CGLDExtPropDef *operator[](int index) { return itemObjs(index); }

protected:
    virtual CGLDExtPropDef *doCreateExtPropDef();
protected:
    CGLDExtPropDefsPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(CGLDExtPropDefs);
};

class CGLDExtPropDefPrivate;
// 扩展属性
class GLDTABLEVIEWSHARED_EXPORT CGLDExtPropDef : public GInterfaceObject
{
public:
    CGLDExtPropDef(CGLDExtPropDefs *owner);
    virtual ~CGLDExtPropDef();
public:
    virtual GString GSPMETHODCALLTYPE code();
    virtual void GSPMETHODCALLTYPE setCode(GString value);
    virtual GString GSPMETHODCALLTYPE value();
    virtual void GSPMETHODCALLTYPE setValue(GString value);
    virtual GString GSPMETHODCALLTYPE asString();
    virtual void GSPMETHODCALLTYPE setAsString(GString value);
    virtual PtrInt GSPMETHODCALLTYPE tag();
    virtual void GSPMETHODCALLTYPE setTag(PtrInt value);
public:
//    void loadFromXML(const IXMLNode node);
//    void saveToXML(const IXMLNode node);
    void loadFromStream(GStream *stream);
    void saveToStream(GStream *stream);
protected:
    CGLDExtPropDefPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(CGLDExtPropDef);
};

#endif // GLDEXTPROPDEFS_H
