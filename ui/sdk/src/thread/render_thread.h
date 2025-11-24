#ifndef _UI_SDK_SRC_THREAD_RENDER_THREAD_H_
#define _UI_SDK_SRC_THREAD_RENDER_THREAD_H_

#include "include/common/ptr/weak_ptr.h"
#include "include/common/signalslot/signal.h"
#include "include/interface/iuiapplication.h"
#include "include/interface/graphics.h"
#include "src/thread/paint_op.h"
#include "include/util/rect_region.h"
#include <cstddef>
#include <map>
#include <shared_mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <deque>

namespace ui {
class Window;
class WindowRenderRT;

//
// 1. 对render target的渲染操作
// 2. TODO: 图片文件资源的加载
//
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
    void AddTask(slot<void()> &&callback, AsyncTaskType type=AsyncTaskType::Unknown);
    static void PostTask(slot<void()> &&callback, AsyncTaskType type=AsyncTaskType::Unknown);

    void SwapChain(void *key, Window *window, DirtyRegion dirty_region);
    void on_swap_chain(void *key, const DirtyRegion& dirty_region);

    bool GetFrontFrameBuffer(void *key, FrameBufferWithReadLock* fb);
    void RemoveKey(IRenderTarget *key);

  private:
    RenderThread &self;
  } main;

private:
  void thread_proc();
  void process_command(PaintOp *op);

  void create_swap_chain(void *key);
  void remove_key(void *key);
  void groupOperations(std::vector<std::unique_ptr<PaintOp>>& op_queue);
  void mergePaintOperations();
  void mergeCommandOperations();

private:
  bool m_running = false;
  std::thread m_thread;

private:
  // paint/render操作队列。
  // 主线程写，渲染线程读
  std::vector<std::unique_ptr<PaintOp>> m_paint_op_queue;
  std::mutex m_paint_op_queue_mutex;

  // 在渲染线程读写
  class PaintOpGroup {
  public:
    PaintOpGroup(void* _key, std::unique_ptr<PaintOp>&& op);

    // 同一个key划在同一个group下面。
    void* key;

    // 是否包含了BeginDraw指令
    bool begin_draw = false;
    // 是否包含了EndDraw指令 
    bool end_draw = false;
    
    // 这是一个命令GROUP。
    bool is_command = false;

    std::vector<std::unique_ptr<PaintOp>> ops;
  };
  std::deque<std::unique_ptr<PaintOpGroup>> m_paint_op_group;
  std::condition_variable m_command_wait;

  // 跳线程使用
  weakptr_factory<RenderThread> m_weakptr_factory = {this};
};

} // namespace ui

#endif // _UI_SDK_SRC_THREAD_RENDER_THREAD_H_