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
    virtual void  SetMeta(HBITMAP hIcon, LPCWSTR szText) override;
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
    HWND   m_hImageWnd;   // ��ק��ͼƬ����
    HWND   m_hTextWnd;    // ��ק��������������
    HFONT  m_hFont;

    Point  m_ptOffset;
    String  m_strDescription;
	HBITMAP  m_hIcon;     // �����ã�����������

	// ������ε��ı����ϴ�һ�������ø��´���
    String  m_strPrevDescription;  

	UI::Image  m_imageBkg;
};


}