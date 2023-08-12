#include "stdafx.h"
#if 0
#include <ddraw.h>
#include <assert.h>
#pragma comment(lib,"ddraw.lib")


// VBLANK: 显示器扫描线完成最后一行后，需要重返左上角。这个过程叫做: vblank，
// 也叫VBI(vertical blank interval) ，因为扫描线变得blank，以防止看到一个斜线
// 显示在屏幕上。

void  /*VSYNC_API*/ WaitForVerticalBlank()
{
    const UUID IID_IDirectDraw7 = { 0x15e65ec0,0x3b9c,0x11d2,0xb9,0x2f,0x00,0x60,0x97,0x97,0xea,0x5b };

    static IDirectDraw7*  m_pDD = nullptr;
    if (!m_pDD)
    {
        CoInitialize(0);

        HRESULT hr = DirectDrawCreateEx(nullptr, (VOID**)&m_pDD, 
            IID_IDirectDraw7, nullptr);

        assert(SUCCEEDED(hr));

        DDSURFACEDESC2 ddsd;
        //DDSCAPS2 ddscaps;

        HWND hWnd = CreateWindow(L"Static", 0, WS_POPUP, 0, 0, 100, 100, 0, 0, 0, 0);
        hr = m_pDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
        assert(SUCCEEDED(hr));

        memset(&ddsd, 0, sizeof(ddsd)); // Set all fields of struct to 0 and set .dwSize to
        ddsd.dwSize = sizeof(ddsd);     // Sizeof the variable - these two steps required for most DDraw structs
        ddsd.dwFlags = DDSD_CAPS;       // Set flags for variables we're using...
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;  // Set the variables we said we would in dwFlags

        LPDIRECTDRAWSURFACE7  m_pDDSPrimary = nullptr;
        hr = m_pDD->CreateSurface(&ddsd, &m_pDDSPrimary, nullptr);
        assert(SUCCEEDED(hr));

    }

    HRESULT hr = 0;
//     hr = m_pDD->WaitForVerticalBlank(DDWAITVB_BLOCKEND, nullptr);
//     assert(SUCCEEDED(hr));

    int nTick = GetTickCount();
    for (int i = 0; i < 2; i++)
    {
        hr = m_pDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, nullptr);
        assert(SUCCEEDED(hr));
        if (i==0)
            Sleep(5);
    }
    int nTick2 = GetTickCount() - nTick;
    int n = 30;

// 
//     if( m_pDDSPrimary )
//         m_pDDSPrimary->Release();
//     m_pDDSPrimary = nullptr;
//     if( m_pDD )
//         m_pDD->Release();
//     m_pDD = nullptr;
// 
//     CoUninitialize();
}


#endif