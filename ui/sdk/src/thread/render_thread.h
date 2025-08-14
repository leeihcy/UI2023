#ifndef _UI_SDK_SRC_THREAD_RENDER_THREAD_H_
#define _UI_SDK_SRC_THREAD_RENDER_THREAD_H_

#include "include/common/ptr/weak_ptr.h"
#include "include/interface/iuiapplication.h"
#include "include/interface/renderlibrary.h"
#include "src/thread/paint_op.h"
#include "include/util/rect_region.h"
#include <cstddef>
#include <map>
#include <shared_mutex>
#include <thread>
#include <vector>

namespace ui {
class Window;

class RenderThread {
public:
  RenderThread(ui::Application &app);

  // 每个layer对应的Skia RenderTarget
  // 对于root layer，需要有一个swapchain(front/back)。
  // 其它layer，只需要一个back即可。
  struct Surface {
    Surface() { }
    Surface(Surface &&o)
        : back(std::move(o.back)), front(std::move(o.front)),
          last_dirty_region(o.last_dirty_region) {}
    void Reset() { 
      back_resized = false;
    }
    std::unique_ptr<IRenderTarget> back;
    std::unique_ptr<IRenderTarget> front;
#if defined(_DEBUG) 
    // 记录后创建的那个rt，用于区分back/front
    IRenderTarget* distinguish_rt_for_debug = nullptr;
#endif

    DirtyRegion last_dirty_region;

    // 调用ResizeOp时记录下来，用于同步所有缓存
    unsigned int width = 0;
    unsigned int height = 0;
    // 记录back是否被调用了resize。
    // 如果已经resized了，则不去同步front上一次的size
    bool back_resized = false;
  };

  struct FrameBufferWithReadLock {
    const FrameBuffer &fb;
    std::shared_lock<std::shared_mutex> read_lock;
  };

  // 在主线程(其实是UI线程)调用的函数放这里面
  class Main {
  public:
    Main(RenderThread &o) : self(o) {}

    void Start();
    void Stop();

    void AddPaintOp(std::unique_ptr<PaintOp> &&op);
    void Notify();

    void CreateSwapChain(void *key);
    void SwapChain(void *key, Window *window, DirtyRegion dirty_region);
    void on_swap_chain(void *key, const DirtyRegion& dirty_region);

    const FrameBufferWithReadLock GetFrameBuffer(void *key);
    void RemoveKey(void *key);

  private:
    RenderThread &self;

    // 用于关联rt <--> window，在swap chain之后切回主线程，通知窗口commit
    std::map<void *, Window *> m_window_map;
  } main;

private:
  Surface &get_layer_render_target(void *key);

  void thread_proc();
  void process_command(PaintOp *op);

  void create_swap_chain(void *key);
  void swap_chain(void *key, const DirtyRegion &dirty_region);
  void remove_key(void *key);
  void frames_sync(Surface &layer);

private:
  bool m_running = false;
  std::thread m_thread;

private:
  // read & write in render thread
  std::vector<std::unique_ptr<PaintOp>> m_paint_op_queue;
  std::mutex m_paint_op_queue_mutex;

  std::condition_variable m_command_cv;

  // 后台帧缓存
  // 读写都在渲染线程中，不需要加锁
  std::map<void *, Surface> m_layer_map;

  // 前台帧缓存，用于主线程提交到窗口上。
  // 主线程读，渲染线程读+写。使用共享读+写锁。
  std::map<void *, FrameBuffer> m_frame_buffer_map;
  std::shared_mutex m_frame_buffer_rw_mutex; // commit|swap chain

  // 跳线程使用
  weakptr_factory<RenderThread> m_weakptr_factory = {this};
};

} // namespace ui

#endif // _UI_SDK_SRC_THREAD_RENDER_THREAD_H_