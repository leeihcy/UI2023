#include "sdk/include/interface/ianimate.h"
#include "sdk/include/interface/iobject.h"
#include "sdk/include/interface/iuiapplication.h"
#include "sdk/include/interface/iwindow.h"
#include "svg/include/inc.h"
#include <cstdio>

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
  uia::E_ANIMATE_TICK_RESULT OnAnimateTick(uia::IStoryboard *) override {
    // printf("OnAnimateTick\n");
    return uia::ANIMATE_TICK_RESULT_CONTINUE;
  };
  void OnAnimateRepeat(uia::IStoryboard *) override {
    printf("OnAnimateRepeat\n");
  };
  void OnAnimateReverse(uia::IStoryboard *) override {
    printf("OnAnimateReverse\n");
  };
};
ClockAnimate g_clock_animate;

void start_animate(ui::IApplication *app, ui::IWindow *window) {
  /*ui::IObject *hour = window->FindObject("hour");
  ui::IObject *min = window->FindObject("min");
  ui::IObject *sec = window->FindObject("sec");
  if (!hour || !min || !sec) {
    return;
  }

  uia::IAnimate *animate = app->GetAnimate();
  uia::IStoryboard* story = animate->CreateStoryboard(&g_clock_animate);
  uia::IFromToTimeline* timeline1 = story->CreateTimeline(0);
  timeline1->SetParam(0, 100, 3*1000);
  story->Begin();
  */
}

int main() {
  ui::ApplicationPtr app;
  ui::SvgRegisterObjects(app.get());
  ui::IResource *resource = app->LoadResource("sample/clock");

  ui::WindowPtr window(resource);

  window->Create("clock", nullptr);
  window->SetTitle("clock demo");
  window->Show();

  window->connect(WINDOW_DESTROY_EVENT, ui::Slot(on_window_destroy, app.get()));
  window->connect(WINDOW_PAINT_EVENT, ui::Slot(on_window_paint));

  start_animate(app.get(), window.get());
  app->Run();

  return 0;
}