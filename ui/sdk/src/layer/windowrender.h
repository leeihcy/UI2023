#pragma once
//#pragma comment(lib, "UICompositor.lib")


////
// ���𴰿ڵ���Ⱦ�������ǲ��������ʽ������Ӳ���ϳ�
//
// 2014.3.10
//
// ͳһGraphics Render Library��ʹ�ã�һ�����ڲ��ٻ��ʹ�ã�ֻ��ʹ��һ��Graphics Render Library
// ���򴦴�Ҫ���Ǵ���ʲô���͵���Ⱦ�⣬��Ĵ�����ң��߼�Ҳ��
//
namespace ui
{
class Object;
class WindowBase;
struct IWindowRender;
struct IWindowCommitListener;
class Compositor;
class Layer;
struct IObjectLayerContent;
struct IListItemLayerContent;

class WindowRender
{
public:
    WindowRender(WindowBase*  p);
    ~WindowRender();

    IWindowRender*  GetIWindowRender();

public:
#if defined(OS_WIN)
    void  BindHWND(HWND hWnd);   
	void  OnSerialize(SERIALIZEDATA* pData);
    void  OnWindowSize(unsigned int nWidth, unsigned int nHeight);

    bool  CreateRenderTarget(IRenderTarget** pp);
    
    bool  GetRequireAlphaChannel();
    void  SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE  eTpye);
    GRAPHICS_RENDER_LIBRARY_TYPE  GetGraphicsRenderType();
    void  SetCanCommit(bool b);
    bool  CanCommit();

    void  SetCommitListener(IWindowCommitListener*);
    IWindowCommitListener*  GetCommitListener();

    void  InvalidateNow();
    void  OnPaint(HDC hDC, RECT* prcInvalid);

	Layer*  CreateLayer(IObjectLayerContent*);
	Layer*  CreateLayer(IListItemLayerContent*);
	void  RequestInvalidate();

private:
    Compositor*  get_create_compositor();

public:
    IWindowRender*  m_pIWindowRender;
    GRAPHICS_RENDER_LIBRARY_TYPE  m_eGRL;
    WindowBase*    m_pWindow;

    long  m_lRefCanCommit;

    // �ô��ڵ���Ⱦ�Ƿ���Ҫalphaͨ�������ٸ���graphics render lib type�����������û��Լ�����
    bool  m_bNeedAlphaChannel;       

private:
    Compositor*  m_pCompositor;
    IWindowCommitListener*  m_pCommitListener;
#endif
};

}