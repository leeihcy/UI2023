// 1. 将gpu渲染的所有调用放在单独的线程中进行处理。
#include "include/api.h"
#include "matrix.h"
#include "sdk/include/util/rect.h"
#include <assert.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlimage.h>
#include <atlwin.h>
#include <map>
#include <minwindef.h>
#include <thread>
#include <windows.h>
#include <winuser.h>


HWND g_render_hwnd = nullptr;
std::thread render_thread;
std::atomic<bool> gpu_ready{false};

#define WM_RENDER_EXIT (WM_USER + 1)
#define WM_RENDER_CREATECOMPOSITOR (WM_USER + 2)
#define WM_RENDER_DESTROYCOMPOSITOR (WM_USER + 3)
#define WM_RENDER_RESIZECOMPOSITOR (WM_USER + 4)

class GpuSampleWindow : public CWindowImpl<GpuSampleWindow>,
                        public ui::IGpuCompositorWindowHWND {
public:
  DECLARE_WND_CLASS(_T("ui gpu sample"))

  BEGIN_MSG_MAP(GpuSampleWindow)
  MESSAGE_HANDLER(WM_CREATE, OnCreate)
  MESSAGE_HANDLER(WM_SIZE, OnSize)
  MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
  MESSAGE_HANDLER(WM_PAINT, OnPaint)
  MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
  END_MSG_MAP()

  LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL &) {
    // this的生命周期如何控制？
    ::PostMessage(g_render_hwnd, WM_RENDER_CREATECOMPOSITOR, (WPARAM)m_hWnd,
                  (LPARAM)static_cast<ui::IGpuCompositorWindow *>(this));
    return 0;
  }
  LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL &) {
    ::PostMessage(g_render_hwnd, WM_RENDER_DESTROYCOMPOSITOR, (WPARAM)m_hWnd,
                  (LPARAM)0);

    ::PostQuitMessage(0);
    return 0;
  }
  LRESULT OnSize(UINT, WPARAM, LPARAM lParam, BOOL &) {
    ::PostMessage(g_render_hwnd, WM_RENDER_RESIZECOMPOSITOR, (WPARAM)m_hWnd,
                  (LPARAM)lParam);
    return 0;
  }
  
  LRESULT OnPaint(UINT, WPARAM, LPARAM lParam, BOOL &) {
    PAINTSTRUCT ps;
    BeginPaint(&ps);
    EndPaint(&ps);
    return 0;
  }
  LRESULT OnEraseBkgnd(UINT, WPARAM, LPARAM lParam, BOOL &) {
    return 0;
  }

protected:
  void *GetHWND() override { return m_hWnd; }
};

class Compositor {};
class RenderThreadWindow : public CWindowImpl<RenderThreadWindow> {
public:
  DECLARE_WND_CLASS(_T("RenderThreadMessageWindow"))

  BEGIN_MSG_MAP(RenderThreadWindow)
  MESSAGE_HANDLER(WM_RENDER_EXIT, OnRenderExit)
  MESSAGE_HANDLER(WM_RENDER_CREATECOMPOSITOR, OnCreateCompositor)
  MESSAGE_HANDLER(WM_RENDER_DESTROYCOMPOSITOR, OnDestroyCompositor)
  MESSAGE_HANDLER(WM_RENDER_RESIZECOMPOSITOR, OnResizeCompositor)
  MESSAGE_HANDLER(WM_TIMER, OnTimer)
  END_MSG_MAP()

  LRESULT OnRenderExit(UINT, WPARAM, LPARAM, BOOL &) {
    PostQuitMessage(0);
    return 0;
  }
  LRESULT OnCreateCompositor(UINT, WPARAM, LPARAM lParam, BOOL &) {
    m_gpu_composition =
        ui::CreateGpuComposition((ui::IGpuCompositorWindow *)lParam);
    m_gpu_composition->Resize(100, 100); // default setting.
    BuildLayers();

    SetTimer(0, 16, 0);
    return 0;
  }
  LRESULT OnDestroyCompositor(UINT, WPARAM, LPARAM, BOOL &) {
    m_root_layer->Release();
    m_gpu_composition->Release();
    return 0;
  }
  LRESULT OnResizeCompositor(UINT, WPARAM, LPARAM lParam, BOOL &) {
    if (m_gpu_composition) {
      m_gpu_composition->Resize(LOWORD(lParam), HIWORD(lParam));
      Commit();
    }
    return 0;
  }
  LRESULT OnTimer(UINT, WPARAM, LPARAM lParam, BOOL &) {
    Commit();
    return 0;
  }
  void BuildLayers() {
    ATL::CImage image;
    image.Load(L"bundle/uigpu/bg.png");
    assert(image.GetBPP() == 32); // must be 32 bpp
    m_root_size.cx = image.GetWidth();
    m_root_size.cy = image.GetHeight();

    m_root_layer = m_gpu_composition->CreateLayerTexture();
    m_root_layer->Resize(image.GetWidth(), image.GetHeight());

    ui::UploadGpuBitmapInfo info;
    info.bpp = image.GetBPP();
    info.hasAlphaChannel = false;
    info.width = image.GetWidth();
    info.height = image.GetHeight();
    info.pitch = image.GetPitch();
    info.pFirstLineBits = (unsigned char *)(image.GetBits());

    ui::Rect range =
        ui::Rect::MakeXYWH(0, 0, image.GetWidth(), image.GetHeight());
    info.prcArray = &range;
    info.nCount = 1;
    m_root_layer->UploadBitmap(info);
  }
  void Commit() {
    static int count = 2;
    count++;

    ui::GpuLayerCommitContext context;
    m_gpu_composition->BeginCommit(&context);

    // 绘制每一个layer
    {
      context.SetOffset(200, 200);
      int rotate_center_x = (m_root_size.cx) / 2 + context.m_xOffset;
      int rotate_center_y = (m_root_size.cy) / 2 + context.m_yOffset;

      Matrix44 matrix;
      matrix.Translate(rotate_center_x, rotate_center_y, 0);

      matrix.Perspective(2000);
      matrix.Translate(0, 0, (m_root_size.cx) / 2);
      matrix.Rotate(0, count, 0);

      matrix.Translate(-rotate_center_x, -rotate_center_y, 0);

      m_root_layer->Compositor(&context, (float *)&matrix);
    }

    m_gpu_composition->EndCommit(&context);
  }

private:
  ui::IGpuCompositor *m_gpu_composition = nullptr;
  ui::IGpuLayer *m_root_layer = nullptr;
  SIZE m_root_size;

  // sample简单处理，这里只有一个窗口。
  // std::map<HWND, Compositor> m_compositor_map;
};

void RenderThreadFunc() {
  ui::GpuStartup();
  gpu_ready = true;

  RenderThreadWindow wnd;
  g_render_hwnd = wnd.Create(HWND_MESSAGE);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  wnd.DestroyWindow();
  g_render_hwnd = nullptr;
  ui::GpuShutdown();
}

void Initialize() {
  // 在exe所在目录加载shader
  char exe_dir[MAX_PATH] = {0};
  GetModuleFileNameA(NULL, exe_dir, MAX_PATH);
  char *p = strrchr(exe_dir, '\\');
  if (p) {
    *(p + 1) = 0;
  }
  ::SetCurrentDirectoryA(exe_dir);
  ::SetProcessDPIAware();

  render_thread = std::thread(RenderThreadFunc);
  // 等待GpuStartup完成
  while (!gpu_ready) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
  CAppModule _Module;
  _Module.Init(nullptr, hInstance);

  Initialize();

  GpuSampleWindow wnd;
  CRect rect(0, 0, 1600, 1200);
  wnd.Create(NULL, rect, _T("ui gpu sample [render thread]"),
             WS_OVERLAPPEDWINDOW);
  wnd.ShowWindow(nCmdShow);
  wnd.UpdateWindow();
  wnd.CenterWindow();

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  // 通知渲染线程退出
  if (g_render_hwnd) {
    PostMessage(g_render_hwnd, WM_RENDER_EXIT, 0, 0);
  }
  if (render_thread.joinable()) {
    render_thread.join();
  }
  _Module.Term();
  return (int)msg.wParam;
}