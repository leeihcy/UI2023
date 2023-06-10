#pragma once


//
// ��ο���DX10�Ŀ���ݣ�
//
// 1. CreateSwapChainʱ��ָ��SampleDesc��Count����1
// 2. ��hlsl��ָ��SetRasterizerState, ��MultisampleEnable = TRUE;
//
// ���ʵ�ּ��ã�
// 1. ͨ��RSSetViewport�����ƻ��������ǲ��Եġ����ַ�ʽ�ǽ������������Ż��Ƶ��µ�
//    viewport���У�������������Ҫ�Ľ����õ�Ч����
//
// 2. RSSetScissorRects
//    The scissor rectangles will only be used if ScissorEnable is set to true 
//    in the rasterizer state
//

namespace UI
{
class GpuRenderLayer;
class GpuComposition : public IHardwareComposition
{
public:
	GpuComposition(HWND hWnd);
	~GpuComposition();

	virtual void Release() override { delete this; }
	IGpuRenderLayer*  CreateLayerTexture();
    bool  BeginCommit();
    void  EndCommit();
    void  SetRootLayerTexture(IGpuRenderLayer* p);
    GpuRenderLayer*  GetRootLayerTexture();

    void  Resize(UINT nWidth, UINT nHeight);
    SIZE  GetSize() {
            return m_sizeBackBuffer; }

    void  ClearStencil();

protected:
    void  CreateSwapChain();
    void  ReCreateRenderTargetView();
	void  ReCreateStencilView();

public:

private:
	GpuRenderLayer*  m_pRootTexture;
	HWND  m_hWnd;

    IDXGISwapChain*  m_pSwapChain;
    ID3D10RenderTargetView*  m_pRenderTargetView;
	
	ID3D10Texture2D*  m_pDepthStencilBuffer;
	ID3D10DepthStencilView*  m_pDepthStencilView;

    SIZE   m_sizeBackBuffer;
};
}