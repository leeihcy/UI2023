#include "stdafx.h"
#include "windowdragdropmgr.h"
#include "src/UIObject\Window\windowbase.h"
#include <ShlGuid.h>
#include "Inc\Util\dragdrop.h"

// 1. 要实现新的拖拽样式，调用SHDoDragDrop，并传递nullptr给IDropSource *pdsrc。
//   
// #if 1 // -- 最终还是没有实现文字显示，TMD 2013.6.19
//     {
//         FORMATETC fmte = {(CLIPFORMAT)RegisterClipboardFormat(CFSTR_DROPDESCRIPTION), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
// 
//         STGMEDIUM medium = {0};
//         medium.hGlobal = GlobalAlloc(GPTR, sizeof(DROPDESCRIPTION));
//         if(medium.hGlobal)
//         {
//             DROPDESCRIPTION* pDropDescription = (DROPDESCRIPTION*)GlobalLock(medium.hGlobal);
// //             _tcscpy(pDropDescription->szMessage, _T("Move to %1"));
// //             _tcscpy(pDropDescription->szInsert, _T("leeihcy"));
// 
//             StringCchCopyW(pDropDescription->szMessage, ARRAYSIZE(pDropDescription->szMessage), _T("Move to %1"));
//             StringCchCopyW(pDropDescription->szInsert, ARRAYSIZE(pDropDescription->szInsert), _T("leeihcy"));
// 
// 
//             pDropDescription->type = DROPIMAGE_LABEL;
//             GlobalUnlock(medium.hGlobal);
//             pDataObj->SetData(&fmte, &medium, TRUE);
//         }
//     }
// #endif

// 注：pdwEffect这个参数
// 是一个in/out参数，in表示拖拽源可以接受的拖拽格式，out表示最终拖拽效果
// 因此不能一上来就将*pdwEffect = DROPEFFECT_NONE

namespace ui
{

WindowDragDropMgr::WindowDragDropMgr()
{
    m_pWindowBase = nullptr;
    m_lRef = 0;
    m_lCanDrop = 0;
    m_pDropTargetHelper = nullptr;
    m_pDragingDataObj = nullptr;
    m_pObjHover = nullptr;
}

WindowDragDropMgr::~WindowDragDropMgr()
{
    if (m_lCanDrop > 0)
    {
        m_lCanDrop = 1;
        SetDroppable(false);
    }
    SAFE_RELEASE(m_pDropTargetHelper);
}

void  WindowDragDropMgr::SetWindowBase(WindowBase* p) 
{ 
    m_pWindowBase = p; 
}

void  WindowDragDropMgr::OnWindowDestroy()
{
    if (m_lCanDrop > 0)
    {
        m_lCanDrop = 1;
        SetDroppable(false);
    }
    m_pObjHover = nullptr;
}

HRESULT  WindowDragDropMgr::SetDroppable(bool b)
{
    if (nullptr == m_pWindowBase)
        return E_FAIL;

    HRESULT hr = 0;

    if (b)
    {
        if (0 == m_lCanDrop)
        {
            hr = ::RegisterDragDrop(m_pWindowBase->m_hWnd, static_cast<IDropTarget*>(this));    
            if (hr == 0x80040101)// 这个窗口已被注册成拖放目标
                return 0;

            if (FAILED(hr))
                return hr;
        }
        m_lCanDrop ++;
    }
    else
    {
        if (m_lCanDrop <= 0)
            return E_FAIL;

        m_lCanDrop --;
        if (0 == m_lCanDrop)
        {
            hr = ::RevokeDragDrop(m_pWindowBase->m_hWnd);
            if (FAILED(hr))
                return hr;
        }
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE WindowDragDropMgr::QueryInterface(REFIID riid, void **ppvObject)
{ 
    if (IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObject = static_cast<IUnknown*>(this);
        return 0;
    }
    else if (IsEqualIID(riid, IID_IDropTarget))
    {
        AddRef();
        *ppvObject = static_cast<IDropTarget*>(this);
        return 0;
    }

    return E_NOINTERFACE;
}
ULONG STDMETHODCALLTYPE WindowDragDropMgr::AddRef(void)
{
    return ++m_lRef;
}
ULONG STDMETHODCALLTYPE WindowDragDropMgr::Release(void)
{
    --m_lRef; 
    if (0 == m_lRef)
    {
        /*delete this; */
        return 0;
    } 
    return m_lRef;
}


//////////////////////////////////////////////////////////////////////////
// #include <shlobj.h>
// #include <strsafe.h>
HRESULT STDMETHODCALLTYPE WindowDragDropMgr::DragEnter(IDataObject *pDataObj, unsigned int grfKeyState, POINTL pt, unsigned int *pdwEffect)
{
    m_pDragingDataObj = pDataObj;

    // 如果放在DragEnter时再创建，会导致第一次拖拽进窗口时闪烁一下
    // 但放在初始化中又会导致窗口创建变慢
    if (nullptr == m_pDropTargetHelper)
    {
        ::CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_INPROC, IID_IDropTargetHelper, (void**)&m_pDropTargetHelper);
    }

    if (m_pDropTargetHelper)
    {
        m_pDropTargetHelper->DragEnter(m_pWindowBase->m_hWnd, pDataObj, (Point*)&pt, DROPEFFECT_NONE/**pdwEffect*/);
    }

    return DragMove(pDataObj, grfKeyState, pt, pdwEffect);    
}

HRESULT STDMETHODCALLTYPE WindowDragDropMgr::DragOver(unsigned int grfKeyState, POINTL pt, unsigned int *pdwEffect)
{
    if (m_pDropTargetHelper)
    {
        m_pDropTargetHelper->DragOver((Point*)&pt, *pdwEffect);
    }

    return DragMove(m_pDragingDataObj, grfKeyState, pt, pdwEffect);
}

// Enteer/Over将调用DrawMove，因为好像这两种事件的处理是一样的
HRESULT  WindowDragDropMgr::DragMove(IDataObject *pDataObj,
                                     unsigned int grfKeyState, POINTL pt, unsigned int *pdwEffect)
{
    Object*  pHoverObj = GetHoverObject2Drop();

    IDragFeedback* pFeedback = nullptr;
    {
        IDataObjectEx* pDataObjEx = nullptr;
        pDataObj->QueryInterface(IID_IDataObjectEx, (void**)&pDataObjEx);
        if (pDataObjEx)
        {
            pFeedback = pDataObjEx->GetDragFeedback();
            pDataObjEx->Release();
        }
    }
    DROPTARGETEVENT_DATA data = {m_pDragingDataObj, grfKeyState, pt, pdwEffect, pFeedback};

    if (m_pObjHover != pHoverObj)
    {
        if (m_pObjHover)
        {
            this->DoDropTargetNotify(m_pObjHover, _DragLeave, nullptr);
        }
        m_pObjHover = pHoverObj;
        if (pHoverObj)
        {
            return this->DoDropTargetNotify(pHoverObj, _DragEnter, &data);
        }
        else
        {
            *pdwEffect = DROPEFFECT_NONE;
        }
    }
    else
    {
        if (m_pObjHover)
        {
            return this->DoDropTargetNotify(m_pObjHover, _DragOver, &data);
        }
        else
        {
            *pdwEffect = DROPEFFECT_NONE;
        }
    }
    return 0;
}

HRESULT STDMETHODCALLTYPE WindowDragDropMgr::DragLeave(void)
{
    if (m_pDropTargetHelper)
    {
        m_pDropTargetHelper->DragLeave();
    }

    HRESULT lRet = this->DoDropTargetNotify(m_pObjHover, _DragLeave, nullptr);
    m_pDragingDataObj = nullptr;
    m_pObjHover = nullptr;
    return lRet;
}

HRESULT STDMETHODCALLTYPE WindowDragDropMgr::Drop(IDataObject *pDataObj, unsigned int grfKeyState, POINTL pt, unsigned int *pdwEffect)
{
    if (m_pDropTargetHelper)
    {
        m_pDropTargetHelper->Drop(pDataObj, (Point*)&pt, *pdwEffect);
    }

    DROPTARGETEVENT_DATA data = {pDataObj, grfKeyState, pt, pdwEffect};

    HRESULT lRet = 0;
    if (m_pObjHover)
    {
        lRet = this->DoDropTargetNotify(m_pObjHover, _Drop, &data);
    }
    else
    {
        *pdwEffect = DROPEFFECT_NONE;
    }
    m_pDragingDataObj = nullptr;
    m_pObjHover = nullptr;
    return lRet;
}


HRESULT  WindowDragDropMgr::DoDropTargetNotify(Object* pHoverObj, DROPTARGETEVENT_TYPE eEventType, DROPTARGETEVENT_DATA* pData)
{
    if (!pHoverObj)
        return DROPEFFECT_NONE;

    UIMSG  msg;
	msg.message = UI_MSG_DROPTARGETEVENT;
    msg.wParam = (long)eEventType;
    msg.lParam = (long)pData;

    msg.pMsgTo = pHoverObj->GetIObject();
    UISendMessage(&msg);
//     if (msg.bHandled)
//         break;
// 
//     if (msg.bHandled)
//         return 0;
    
    return 0;
}

// [注]: 直接在这里调用 m_pWindowBase->GetHoverObject(); 可能什么也得不到，必须重新获取
Object*  WindowDragDropMgr::GetHoverObject2Drop()
{
    Object*  pHoverObj = m_pWindowBase->GetObjectUnderCursor();
    if (!pHoverObj)
        pHoverObj = m_pWindowBase;

    while (pHoverObj)
    {
		OBJSTYLE s = {0};
		s.receive_dragdrop_event = 1;
        if (pHoverObj->TestObjectStyle(s))
            break;

        pHoverObj = pHoverObj->GetParentObject();
    }
    return pHoverObj;
}

}