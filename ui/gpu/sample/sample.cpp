// 1. gpu模块是个独立的模块，可单独使用，不需要依赖ui sdk
// 2. 这里使用原生窗口，通过gpu模块进行功能验证。
#include "include/api.h"
#include "sdk/include/util/rect.h"
#include "matrix.h"
#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlimage.h>
#include <assert.h>
#include <windows.h>

class GpuSampleWindow : public CWindowImpl<GpuSampleWindow>,
                        public ui::IGpuCompositorWindowHWND {
public:
  DECLARE_WND_CLASS(_T("ui gpu sample"))

  BEGIN_MSG_MAP(GpuSampleWindow)
  MESSAGE_HANDLER(WM_CREATE, OnCreate)
  MESSAGE_HANDLER(WM_SIZE, OnSize)
  MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
  MESSAGE_HANDLER(WM_TIMER, OnTimer)
  END_MSG_MAP()

  LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL &) {
    m_gpu_composition =
        ui::CreateGpuComposition(static_cast<ui::IGpuCompositorWindow *>(this));
    SetTimer(0, 16, 0);
    BuildLayers();
    return 0;
  }
  LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL &) {
    m_root_layer.reset();
    m_gpu_composition.reset();
    ::PostQuitMessage(0);
    return 0;
  }
  LRESULT OnSize(UINT, WPARAM, LPARAM lParam, BOOL &) {
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
    assert(image.GetBPP() == 32);  // must be 32 bpp
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

    ui::Rect range = ui::Rect::MakeXYWH(0, 0, image.GetWidth(), image.GetHeight());
    info.prcArray = &range;
    info.nCount = 1;
    m_root_layer->UploadBitmap(info);
  }
  void Commit() {
    static int count = 2;
    count ++;

    ui::GpuLayerCommitContext context;
    m_gpu_composition->BeginCommit(&context);

    // 绘制每一个layer
    {
        context.SetOffset(200, 200);
        int rotate_center_x = (m_root_size.cx)/2  + context.m_xOffset;
        int rotate_center_y = (m_root_size.cy)/2  + context.m_yOffset;

        Matrix44 matrix;
        matrix.Translate(rotate_center_x, rotate_center_y, 0);
        
        matrix.Perspective(2000);
        matrix.Translate(0, 0, (m_root_size.cx)/2);
        matrix.Rotate(0, count, 0);

        matrix.Translate(-rotate_center_x, -rotate_center_y, 0);
        
        m_root_layer->Compositor(&context, (float*)&matrix);
    }

    m_gpu_composition->EndCommit(&context);
  }

protected:
  void *GetHWND() override { return m_hWnd; }

private:
  std::shared_ptr<ui::IGpuCompositor> m_gpu_composition;
  std::shared_ptr<ui::IGpuLayer> m_root_layer;
  SIZE m_root_size;
};

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
  ui::GpuStartup();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
  CAppModule _Module;
  _Module.Init(nullptr, hInstance);

  Initialize();

  GpuSampleWindow wnd;
  CRect rect(0, 0, 1600, 1200);
  wnd.Create(NULL, rect, _T("ui gpu sample"),
             WS_OVERLAPPEDWINDOW);
  wnd.ShowWindow(nCmdShow);
  wnd.UpdateWindow();
  wnd.CenterWindow();

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  ui::GpuShutdown();
  _Module.Term();
  return (int)msg.wParam;
}