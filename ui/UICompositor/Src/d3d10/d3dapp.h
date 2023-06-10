#pragma once
#include "common\Inputs.h"

// TODO:
// 1. ����Ļ�µ�deviceҪ��ô����
// 2. debug������Ⱦ��֡�ʣ�

//
// IA-Input Assembler State
// SO-Stream Output State
// OM-Output Merger State
// VS-Vertex Shader
// PS-Pixel Shader
// GS-Geometry Shader
//

// ��Դ���������ܱ�ֱ�Ӱ󶨵�һ�����߽׶Σ�����ֻ�ܰ�����Դ��������Դ��ͼ�󶨵���ͬ�Ĺ��߽׶Ρ�
// ���������ַ�ʽʹ������Direct3Dʼ��Ҫ�������ڳ�ʼ��ʱΪ��������ص���Դ��ͼ���������������
// ����Ч�ʣ�����SDK�ĵ���ָ����������������ʱ���������������������ͼ����ʱִ����Ӧ����֤��ӳ�䣬
// ���ٰ�ʱ�����ͼ�顱��
// Resource View����DirectX10ʱ������ĸ����ʵ����������resouce��pipeline�󶨵�֮���һ������������ͬһ����Դ���Զ�Ӧ�����ͬ��RV�����������󶨵���ͬ��pipline�׶Ρ�
// �����ֲ�ͬ���͵�RV���ֱ��Ӧ��pipeline�е�4���ɰ�λ�ã�
//   Render target view (ID3D11RenderTargetView)
//   Depth stencil view (ID3D11DepthStencilView)
//   Shader resource view (ID3D11ShaderResourceView)
//   Unordered access view (ID3D11UnorderedAccessView)


class Stage3D;
class Direct3DRenderTarget;


class D3D10App
{
public:
	D3D10App();
	~D3D10App();

public:
    void  AddRef();
    void  Release();

	bool  Init();
	void  Destroy();

	UINT  GetDeviceMultisampleQuality() { 
			return m_nMultisampleQuality; }

    bool  IsActiveSwapChain(HWND  hWnd);
    void  SetActiveSwapChain(HWND hWnd);

	void  ApplyTechnique(ID3D10EffectTechnique*  pTech, UI::RECTF*  prcDraw, UI::D3DCOLORVALUE color);
	void  ApplyTechnique(ID3D10EffectTechnique*  pTech, UI::RECTF*  prcDraw, UI::RECTF*  prcTexture, float fAlpha);

public:
    static void Startup();
    static void Shutdown();
    static D3D10App*  Get();

private:
    long  m_lRef;
    static D3D10App* s_pApp;

public:
	ID3D10Device*    m_pDevice;
	IDXGIFactory*    m_pDXGIFactory;

    //  ��ǰdevice�б�ѡ���rendertarget��������
    HWND  m_hActiveWnd;  	

private:
	UINT  m_nMultisampleQuality;
};

