#include "stdafx.h"
#include "D3DApp.h"
#include "common\Effects.h"
#include "common\RenderStates.h"
#include "common\Font.h"

D3D10App* D3D10App::s_pApp = nullptr;

D3D10App::D3D10App()
{
	m_pDevice = nullptr;
    m_hActiveWnd = nullptr;
	m_nMultisampleQuality = 0;

    m_lRef = 0;
    s_pApp = this;
}

D3D10App::~D3D10App()
{
    s_pApp = nullptr;
	Destroy();
}

D3D10App*  D3D10App::Get()
{
    return s_pApp;
}

void D3D10App::Startup()
{
    if (!s_pApp)
    {
        s_pApp = new D3D10App;
        s_pApp->Init();
    }
    s_pApp->AddRef();
}
void D3D10App::Shutdown()
{
    assert (s_pApp);

    if (s_pApp)
    {
        s_pApp->Release();
    }
}

void  D3D10App::AddRef()
{
    m_lRef++;
}
void  D3D10App::Release()
{
    m_lRef--;
    if (0 == m_lRef)
    {
        delete this;
        return;
    }
}

void  D3D10App::Destroy()
{
	if (m_pDevice)
		m_pDevice->ClearState();

    // 释放所有元素的static变量

    RenderStates::Release();
    Effects::Release();
    Inputs::Rlease();
	Font::Release();

	SAFE_RELEASE(m_pDXGIFactory);

// 	ID3D11Debug* pD3dDebug;
// 	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pD3dDebug));
// 	if (SUCCEEDED(hr))
// 	{
// 		hr = pD3dDebug->ReportLiveDeviceObjects(D3D10_RLDO_DETAIL);
// 	}

	if (m_pDevice)
	{
		ULONG lRet = m_pDevice->Release();
		UIASSERT(0 == lRet);
		m_pDevice = nullptr;
	}
}


HRESULT CreateD3DDevice(
						IDXGIAdapter *pAdapter,
						D3D10_DRIVER_TYPE driverType,
						UINT flags,
						ID3D10Device1 **ppDevice
						)
{
	HRESULT hr = S_OK;

	static const D3D10_FEATURE_LEVEL1 levelAttempts[] =
	{
		D3D10_FEATURE_LEVEL_10_0,
		D3D10_FEATURE_LEVEL_9_3,
		D3D10_FEATURE_LEVEL_9_2,
		D3D10_FEATURE_LEVEL_9_1,
	};

	for (UINT level = 0; level < ARRAYSIZE(levelAttempts); level++)
	{
		ID3D10Device1 *pDevice = nullptr;
		hr = D3D10CreateDevice1(
			pAdapter,
			driverType,
			nullptr,
			flags,
			levelAttempts[level],
			D3D10_1_SDK_VERSION,
			&pDevice
			);

		/* 应用程序请求的操作依赖于已缺失或不匹配的 SDK 组件。

		 Windows 8 users: If you upgrade to Windows 8.1, remember that all 
		 attempts to use D3Dxx_CREATE_DEVICE_DEBUG will fail until you 
		 upgrade the Developer Runtime. You can do this by installing VS 
		 2013, the standalone Windows 8.1 SDK, or by installing the VS 2013 
		 Remote Debugging Tools. If you are missing the updated SDK Debug 
		 Layers, you should see the following message in your debug output:

         D3D11CreateDevice: Flags (0x2) were specified which require the
		 D3D11 SDK Layers for Windows 10, but they are not present on
		 the system.
		 These flags must be removed, or the Windows 10 SDK must be
		 installed.
		 Flags include: D3D11_CREATE_DEVICE_DEBUG

		*/
		if (hr == 0x887a002d && flags & D3D10_CREATE_DEVICE_DEBUG) 
		{
			flags &= ~D3D10_CREATE_DEVICE_DEBUG;
			return CreateD3DDevice(pAdapter, driverType, false, ppDevice);
		}

		if (SUCCEEDED(hr))
		{
			// transfer reference
			*ppDevice = pDevice;
			pDevice = nullptr;
			break;
		}
	}

	return hr;
}
bool D3D10App::Init()
{
	// 若要使用 Direct2D，必须使用 D3D10_CREATE_DEVICE_BGRA_SUPPORT 
	// 标志创建提供 IDXGISurface 的 Direct3D 设备。
    UINT  nCreateDeviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT; 
#ifdef _DEBUG
    nCreateDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	ID3D10Device1*  pDevice = nullptr;
	IDXGIDevice*    pDXGIDevice = nullptr;
	IDXGIAdapter*   pAdapter = nullptr;

	HRESULT hr = S_OK;
	// Create device
	hr = CreateD3DDevice(
		nullptr,
		D3D10_DRIVER_TYPE_HARDWARE,
		nCreateDeviceFlags,
		&pDevice 
		);

	if (!pDevice)
		return false;

	hr = pDevice->QueryInterface(__uuidof(ID3D10Device), (void**)&m_pDevice);
	if (!m_pDevice)
		return false;

 	if (FAILED(hr))
 	{
 		hr = D3D10CreateDevice(
 			nullptr,
			D3D10_DRIVER_TYPE_HARDWARE,
			nullptr,
 			nCreateDeviceFlags,
			D3D10_SDK_VERSION,
 			&m_pDevice
 			);
 	}

	

	hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
	hr = pDXGIDevice->GetAdapter(&pAdapter);
	hr = pAdapter->GetParent(IID_PPV_ARGS(&m_pDXGIFactory));

	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pDXGIDevice);
	SAFE_RELEASE(pAdapter);

	hr = m_pDevice->CheckMultisampleQualityLevels(
			DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_nMultisampleQuality);

    Effects::Init(m_pDevice);
    Inputs::Init(m_pDevice);
    RenderStates::Init(m_pDevice);
	Font::Init(m_pDevice);

	// Create an orthographic projection matrix for 2D rendering.
	//D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

    return true;
}

void  _ApplyTechnique(ID3D10EffectTechnique* pTech, DXUT_SCREEN_VERTEX_10 vertices[4])
{
// TODO:    Inputs::SetVertex4(vertices);

    D3D10_TECHNIQUE_DESC techDesc;
    pTech->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        pTech->GetPassByIndex(p)->Apply(0);
        D3D10App::Get()->m_pDevice->Draw(4, 0);
    }
}

void  D3D10App::ApplyTechnique(ID3D10EffectTechnique* pTech, UI::RECTF*  prcDraw, UI::D3DCOLORVALUE color)
{
	DXUT_SCREEN_VERTEX_10 vertices[4] =
	{
		{ prcDraw->left,     prcDraw->top, 0, color, 0.0f, 0.0f },
		{ prcDraw->right,    prcDraw->top, 0, color, 1.0f, 0.0f },
		{ prcDraw->left,  prcDraw->bottom, 0, color, 0.0f, 1.0f },
		{ prcDraw->right, prcDraw->bottom, 0, color, 1.0f, 1.0f },
	};
	_ApplyTechnique(pTech, vertices);
}
void  D3D10App::ApplyTechnique(ID3D10EffectTechnique* pTech, UI::RECTF* prcDraw, UI::RECTF* prcTexture, float fAlpha)
{
	// {1,1,1, fAlpha},然后预乘后的结果
    UI::D3DCOLORVALUE color = {fAlpha, fAlpha, fAlpha, fAlpha};

	DXUT_SCREEN_VERTEX_10 vertices[4] =
	{
 		{ prcDraw->left,  prcDraw->top,    0.f, color, prcTexture->left,  prcTexture->top},   
 		{ prcDraw->right, prcDraw->top,    0.f, color, prcTexture->right, prcTexture->top},   
 		{ prcDraw->left,  prcDraw->bottom, 0.f, color, prcTexture->left,  prcTexture->bottom},
		{ prcDraw->right, prcDraw->bottom, 0.f, color, prcTexture->right, prcTexture->bottom},
	};
	_ApplyTechnique(pTech, vertices);
}

bool  D3D10App::IsActiveSwapChain(HWND  hWnd)
{
    return m_hActiveWnd == hWnd ? true : false;
}
void  D3D10App::SetActiveSwapChain(HWND hWnd)
{
    m_hActiveWnd = hWnd;
}