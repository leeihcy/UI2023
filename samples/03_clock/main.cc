#include "sdk/include/interface/ianimate.h"
#include "sdk/include/interface/iobject.h"
#include "sdk/include/interface/irenderlayer.h"
#include "sdk/include/interface/iuiapplication.h"
#include "sdk/include/interface/iwindow.h"
#include "svg/include/inc.h"
#include <cstdio>
#include <ctime>

// https://cssanimation.rocks/clocks/

void on_window_destroy(ui::IApplication *uiapp, ui::Event *) {
  printf("on_window_destroy\n");
  uiapp->Quit();
}
void on_window_paint(ui::Event *e) {
  ui::IRenderTarget *pRT = static_cast<ui::WindowPaintEvent *>(e)->rt;
}

class ClockAnimate : public uia::IAnimateEventCallback {
public:
  void OnAnimateStart(uia::IStoryboard *) override {
    printf("OnAnimateStart\n");
  };
  void OnAnimateEnd(uia::IStoryboard *, uia::E_ANIMATE_END_REASON e) override {
    printf("OnAnimateEnd\n");
  };
  uia::E_ANIMATE_TICK_RESULT
  OnAnimateTick(uia::IStoryboard *storyboard) override {
    ui::IWindow *window = (ui::IWindow *)storyboard->GetWParam();
    update_rotate(window);
    return uia::ANIMATE_TICK_RESULT_CONTINUE;
  };

  void OnAnimateRepeat(uia::IStoryboard *) override {
    printf("OnAnimateRepeat\n");
  };
  void OnAnimateReverse(uia::IStoryboard *) override {
    printf("OnAnimateReverse\n");
  };

  void update_rotate(ui::IWindow *window) {
    ui::IObject *hour = window->FindObject("hour");
    ui::IObject *min = window->FindObject("min");
    ui::IObject *sec = window->FindObject("sec");

    ui::ILayer *hour_layer = hour->GetLayer();
    ui::ILayer *min_layer = min->GetLayer();
    ui::ILayer *sec_layer = sec->GetLayer();

    std::time_t now = std::time(nullptr);
    // 转换为本地时间
    std::tm *local_time = std::localtime(&now);

    // 提取时、分、秒
    int h = local_time->tm_hour;
    int m = local_time->tm_min;
    int s = local_time->tm_sec;

    hour_layer->RotateZTo(30 * h + 0.5f * m + 0.5f * s / 120);
    min_layer->RotateZTo(6 * m + 0.1f * s);
    sec_layer->RotateZTo(local_time->tm_sec * 6.0f);
  }
};
ClockAnimate g_clock_animate;

void start_animate(ui::IApplication *app, ui::IWindow *window) {
  uia::IAnimate *animate = app->GetAnimate();
  uia::IStoryboard *story = animate->CreateStoryboard(&g_clock_animate);
  story->SetWParam((ui::llong)window);
  story->CreateIdleTimeline(0);
  story->Begin();
}

bool on_test_timer(ui::IWindow *window,  ui::TimerID timer_id) {

#if 0 // for show/hide test
  if (window->IsVisible()) {
    printf("on timer to hide\n");
    window->Hide();
  } else {
    printf("on timer to show\n");
    window->Show();
  }
#endif
  return true;
}

int main() {
  bool use_gpu = false;

  ui::ApplicationPtr app;
  if (use_gpu) {
    app->EnableGpuComposite();
  }
  ui::SvgRegisterObjects(app.get());
  ui::IResource *resource = app->LoadResource("sample/clock");

  ui::WindowPtr window(resource);
  if (use_gpu) {
    window->EnableGpuComposite(true);
  }

  window->Create("clock", nullptr);
  window->SetTitle("clock demo -- vulkan硬件合成");
  window->Show();

  window->connect(WINDOW_DESTROY_EVENT, ui::Slot(on_window_destroy, app.get()));
  window->connect(WINDOW_PAINT_EVENT, ui::Slot(on_window_paint));

  if (use_gpu) {
    // g_clock_animate.update_rotate(window.get());
    // start_animate(app.get(), window.get());
  }

  ui::TimerID timer_id =
      app->SetTimer(2000, ui::Slot(&on_test_timer, window.get()));
  app->Run();
  app->KillTimer(timer_id);

  return 0;
}