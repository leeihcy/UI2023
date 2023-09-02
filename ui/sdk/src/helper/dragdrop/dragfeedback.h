#pragma once
#include "Src\Atl\image.h"
#include "Inc\Util\dragdrop.h"

namespace ui
{

class CDragFeedback : public IDragFeedback
{
public:
    CDragFeedback();
    ~CDragFeedback();

    void  Create(HBITMAP hDragImage, Point ptOffset);
    void  Destroy();
    void  OnMouseMove(Point pt);

    void  Bind2DataObject(IDataObject* p);
    void  Unbind2DataObject(IDataObject* p);
    void  Bind2DropSource(IDropSource* p);
    void  Unbind2DropSource(IDropSource* p);

public:
    virtual void  SetDescription(HBITMAP hIcon, LPCWSTR szText) override;
    virtual HRESULT  GiveFeedback(unsigned int dwEffect) override;

private:
    void  RegisterDragBitmapWndClass();
    BOOL  InstallDragMouseHook();
    void  UninstallDragMouseHook();
    HWND  CreateDragImageWnd(HBITMAP hBitmap, int x, int y);
    void  UpdateTextWnd(HBITMAP hIcon, LPCWSTR szText, int x, int y);

public:
    HHOOK  m_hHook;
private:
    HWND   m_hImageWnd;   // 拖拽的图片窗口
    HWND   m_hTextWnd;    // 拖拽的文字描述窗口
    HFONT  m_hFont;

    Point  m_ptOffset;
    std::string  m_strDescription;
	HBITMAP  m_hIcon;     // 仅引用，不负责销毁

	// 如果本次的文本与上次一样，则不用更新窗口
    std::string  m_strPrevDescription;  

	UI::Image  m_imageBkg;
};


}