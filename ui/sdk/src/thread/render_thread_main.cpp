#include "include/interface/renderlibrary.h"
#include "render_thread.h"
#include "src/application/config/config.h"
#include "src/thread/paint_op.h"
#include "src/thread/render_thread.h"
#include <memory>

namespace ui {

void RenderThread::Main::Start() {
  self.m_running = true;
  self.m_thread = std::thread(&RenderThread::thread_proc, &self);
}

void RenderThread::Main::Stop() {
  self.m_running = false;
  Notify();
  if (self.m_thread.joinable()) {
    self.m_thread.join();
  }
}

void RenderThread::Main::AddPaintOp(std::unique_ptr<PaintOp> &&paint_op) {
  if (!self.m_running) {
    // 没有开启render thread，直接运行
    paint_op->processOnRenderThread((SkiaRenderTarget*)paint_op->key);
    return;
  }
  bool is_command = paint_op->type >= PaintOpType::PostCommandStart;

  std::lock_guard<std::mutex> lock(self.m_paint_op_queue_mutex);
  self.m_paint_op_queue.push_back(std::move(paint_op));

  if (is_command) {
    Notify();
  }
}

void RenderThread::Main::AddTask(slot<void()> &&callback) {
  if (self.m_running) {
    AddPaintOp(std::make_unique<AsyncTaskCommand>(std::move(callback)));
  } else {
    callback.emit();
  }
}

// 如果启用子多线程渲染，则在render thread上执行。如果没启用，直接执行。
// static 
void RenderThread::Main::PostTask(slot<void()> &&callback) {
  if (Config::GetInstance().enable_render_thread) {
    RenderThread::GetIntance().main.AddTask(std::move(callback));
  } else {
    callback.emit();
  }
}

void RenderThread::Main::Notify() { self.m_command_wait.notify_one(); }

bool RenderThread::Main::GetFrontFrameBuffer(void *key, FrameBufferWithReadLock* frame_buffer) {
  return ((SkiaRenderTarget*)key)->GetFrontFrameBuffer(frame_buffer);
}

void RenderThread::Main::RemoveKey(IRenderTarget* key) {
  auto op = std::make_unique<PaintOp>(PaintOpType::RemoveKey);
  op->key = key;
  AddPaintOp(std::move(op));
  Notify();
}

} // namespace ui