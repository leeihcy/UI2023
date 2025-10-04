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
#include <deque>

namespace ui {
class Window;

class RenderThread {
public:
  RenderThread();
  static RenderThread& GetIntance();

  // 在主线程(其实是UI线程)调用的函数放这里面
  class Main {
  public:
    Main(RenderThread &o) : self(o) {}

    void Start();
    void Stop();

    void AddPaintOp(std::unique_ptr<PaintOp> &&op);
    void Notify();

    void SwapChain(void *key, Window *window, DirtyRegion dirty_region);
    void on_swap_chain(void *key, const DirtyRegion& dirty_region);

    bool GetFrontFrameBuffer(void *key, FrameBufferWithReadLock* fb);
    void RemoveKey(void *key);

  private:
    RenderThread &self;
  } main;

private:
  void thread_proc();
  void process_command(PaintOp *op);

  void create_swap_chain(void *key);
  void remove_key(void *key);
  void merge_and_optimize_operations(std::vector<std::unique_ptr<PaintOp>>& op_queue);

private:
  bool m_running = false;
  std::thread m_thread;

private:
  // 主线程写，渲染线程读
  std::vector<std::unique_ptr<PaintOp>> m_paint_op_queue;
  std::mutex m_paint_op_queue_mutex;

  // 在渲染线程读写
  class PaintOpGroup {
  public:
    PaintOpGroup(void* _key, std::unique_ptr<PaintOp>&& op);

    // 同一个key划在同一个group下面。
    void* key;
    // 该key已调用了EndDraw完全绘制。在end之前，其它子rt的BeginDraw仍然划归该group下。
    bool end_draw = false;

    std::vector<std::unique_ptr<PaintOp>> ops;
  };
  std::deque<std::unique_ptr<PaintOpGroup>> m_paint_op_group;

  std::condition_variable m_command_cv;

  std::map<void*, IRenderTarget*> m_surface_map_uithread;

  // 前台帧缓存，用于主线程提交到窗口上。
  // 主线程读，渲染线程读+写。使用共享读+写锁。
  // std::map<void *, FrameBuffer> m_frame_buffer_map;
  // std::shared_mutex m_frame_buffer_rw_mutex; // commit|swap chain

  // 跳线程使用
  weakptr_factory<RenderThread> m_weakptr_factory = {this};
};

} // namespace ui

#endif // _UI_SDK_SRC_THREAD_RENDER_THREAD_H_