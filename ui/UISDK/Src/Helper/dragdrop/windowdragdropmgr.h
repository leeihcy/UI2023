#pragma once
#include <shobjidl.h>  // IDropTargetHelper
namespace UI
{

class WindowBase;
class Object;

class WindowDragDropMgr : public IDropTarget
{
public:
    WindowDragDropMgr();
    ~WindowDragDropMgr();

public:
    void  SetWindowBase(WindowBase* p);
    
    HRESULT  SetDroppable(bool b);
    void  OnWindowDestroy();

protected:
    // IUnknown Interface
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

protected:
    // IDropTarget Interface 
    virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
    virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
    virtual HRESULT STDMETHODCALLTYPE DragLeave(void);
    virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);

    HRESULT  DragMove(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
    HRESULT  DoDropTargetNotify(Object* pHoverObj, DROPTARGETEVENT_TYPE eEventType, DROPTARGETEVENT_DATA* pData);

    Object*  GetHoverObject2Drop();

private:
    WindowBase*  m_pWindowBase;
    
    IDropTargetHelper*  m_pDropTargetHelper;  // 用于显示拖拽时的图像
    IDataObject*  m_pDragingDataObj;   // 用于解决DragOver时没法拿到DataObjec的问题
    Object*  m_pObjHover;              // 记录当前hover对象，用于给hover对象发送enter/leave/hover消息。[注: drag的enter/leave只是针对窗口的]

    ULONG  m_lRef;
    long   m_lCanDrop;  // 大于0表示可以拖拽
};

}