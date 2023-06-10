#pragma once
#include "Inc\Util\dragdrop.h"

namespace UI
{

class CDropSource : 
        public IDropSource,
        public IDropSourceEx
{
public:
    CDropSource();
    ~CDropSource();

public:
    // IUnknown Interface
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

public:
    // IDropSource Interface
    virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
    virtual HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD dwEffect);

    // IDropSourceEx
    virtual void  SetDragFeedback(IDragFeedback* p) override;
    virtual IDragFeedback*  GetDragFeedback() override;

protected:
    ULONG  m_lRef;
    IDragFeedback*  m_pDragFeedback;
};

}