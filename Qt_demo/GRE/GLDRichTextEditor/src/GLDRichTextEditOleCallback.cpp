#include "GLDRichTextEditOleCallback.h"
#include "GLDRichTextEditGlobal.h"

// we use IID_IUnknown because richedit doesn't define an IID

GLDRichTextEditOleCallback::GLDRichTextEditOleCallback(LPRICHEDITOLE lpRichEditOle)
    : m_dwRef(0)
    , m_lpRichEditOle(lpRichEditOle)
    , m_iNumStorages(0)
    , m_lpStorage(NULL)
{
    HRESULT hr = ::StgCreateDocfile(
                NULL,
                STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE /*| STGM_DELETEONRELEASE */ | STGM_CREATE,
                0, &m_lpStorage);
    if (FAILED(hr))
    {
        GLD_RICH_ERROR_POS("StgCreateDocfile fail.");
    }
}

GLDRichTextEditOleCallback::~GLDRichTextEditOleCallback()
{
    if (NULL != m_lpStorage)
    {
        m_lpStorage->Release();
    }
}

STDMETHODIMP_(ULONG) GLDRichTextEditOleCallback::AddRef()
{
    return InterlockedIncrement(&m_dwRef);
}

STDMETHODIMP_(ULONG) GLDRichTextEditOleCallback::Release()
{
    if (m_dwRef == 0)
        return 0;

    LONG lResult = InterlockedDecrement(&m_dwRef);
    if (lResult == 0)
    {
        delete this;
    }
    return lResult;
}

STDMETHODIMP GLDRichTextEditOleCallback::QueryInterface(REFIID iid, LPVOID* ppvObj)
{

    HRESULT hr = S_OK;
    *ppvObj = NULL;

    if (iid == IID_IUnknown || iid == IID_IRichEditOleCallback)
    {
        *ppvObj = this;
        AddRef();
        hr = S_OK;
    }
    else
    {
        hr = E_NOINTERFACE;
    }

    return hr;
}

STDMETHODIMP GLDRichTextEditOleCallback::GetNewStorage(LPSTORAGE* ppstg)
{
    //ͨ���÷����洢��ճ������ı���(RTF)�ж�ȡ���¶���

    HRESULT hRes = E_OUTOFMEMORY;
    ++m_iNumStorages;
    WCHAR tName[50];
    swprintf(tName, L"REOLEStorage%d", m_iNumStorages);
    hRes = m_lpStorage->CreateStorage(
                tName,
                STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE,
                0,
                0,
                ppstg
                );
    return hRes;
}

STDMETHODIMP GLDRichTextEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME* lplpFrame, LPOLEINPLACEUIWINDOW* lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    // ͨ���÷����ṩ��Ӧ�ó��򼶺��ĵ����ӿڣ��Լ���Ҫ��֧��In-place�������Ϣ
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::ShowContainerUI(THIS_ BOOL fShow)
{
    //ͨ���÷�����֪Ӧ�ó����Ƿ���ʾ�Լ��Ĳ�������
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::QueryInsertObject(THIS_ LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp) 
{
    //ͨ���÷�����Ӧ�ó���ѯ��ĳ�������Ƿ���Ա�����
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::DeleteObject(THIS_ LPOLEOBJECT lpoleobj)
{
    //����֪ͨ��һ�����󼴽���RichEdit�ؼ���ɾ��
    lpoleobj->Close(OLECLOSE_NOSAVE);
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::QueryAcceptData(THIS_ LPDATAOBJECT lpdataobj, CLIPFORMAT FAR * lpcfFormat, DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
    //ͨ���÷���������ճ���������ϷŲ���������������Ƿ���Ա����ܡ�
    
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::ContextSensitiveHelp(THIS_ BOOL fEnterMode)
{
    //ͨ���÷���֪ͨӦ�ó��������������Ĺ�����ʽ���Ȱ�����
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::GetClipboardData(THIS_ CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj)
{
    //ͨ���÷�������RichEdit�Ŀͻ��ˣ����ó����ṩ�Լ���ճ������
    LPDATAOBJECT lpOrigDataObject = NULL;

    // get richedit's data object
    if (FAILED(m_lpRichEditOle->GetClipboardData(lpchrg, reco, &lpOrigDataObject)))
    {
        return E_NOTIMPL;
    }

    // allow changes
    HRESULT hRes = E_NOTIMPL;// pThis->GetClipboardData(lpchrg, reco, lpOrigDataObject, lplpdataobj);

    // if changed then free original object
    if (SUCCEEDED(hRes))
    {
        if (lpOrigDataObject != NULL)
            lpOrigDataObject->Release();
        return hRes;
    }
    else
    {
        // use richedit's data object
        *lplpdataobj = lpOrigDataObject;
        return S_OK;
    }
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::GetDragDropEffect(THIS_ BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect) 
{
    //ͨ���÷�������RichEdit�Ŀͻ��ˣ����ó��������϶�������Ч��
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::GetContextMenu(THIS_ WORD seltype, LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR * lphmenu) 
{
    //ͨ���÷�����Ӧ�ó������ͨ������Ҽ��¼�����ȡ�����Ĳ˵�������
    return S_OK;
}
