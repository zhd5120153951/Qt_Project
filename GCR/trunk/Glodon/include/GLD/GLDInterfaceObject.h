#ifndef GLDINTERFACEOBJECT_H
#define GLDINTERFACEOBJECT_H

#include <QObject>
#include "GLDUnknwn.h"
#include "GLDObject.h"
#include "GLDCommon_Global.h"

#define DECLARE_IUNKNOWN_Q DECLARE_IUNKNOWN

#ifndef QT_NO_QOBJECT
class GLDCOMMONSHARED_EXPORT QInterfaceObject : public QObject, public IUnknown
{
    Q_OBJECT
public:
    explicit QInterfaceObject(QObject *parent = 0) : QObject(parent){m_cRef = 0;}

public:
    DECLARE_IUNKNOWN
signals:
    
public slots:
    
protected:
    virtual HRESULT STDMETHODCALLTYPE _QueryInterface(REFIID riid, void **ppvObject)
    {
        if (riid == IID_IUnknown)
        {
            this->AddRef();
            *ppvObject = static_cast<IUnknown *>(this);
            return NOERROR;
        }
        else
        {
            *ppvObject = NULL;
            return E_NOINTERFACE;
        }
    }

    virtual ULONG STDMETHODCALLTYPE _AddRef(void)
    {
        return m_cRef++;
    }

    virtual ULONG STDMETHODCALLTYPE _Release(void)
    {
        m_cRef--;
        if (m_cRef == 0)
        {
            delete this;
            return ULONG(0);
        }
        else
            return m_cRef;
    }

private:
    LONG m_cRef;
};

#endif

#endif // GLDINTERFACEOBJECT_H
