#include "include/common/ptr/weak_ptr.h"
#include "src/thread/paint_op.h"
#include "src/thread/render_thread.h"
#include "include/util/rect_region.h"
#include "src/window/window.h"

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
  std::lock_guard<std::mutex> lock(self.m_paint_op_queue_mutex);
  self.m_paint_op_queue.push_back(std::move(paint_op));
}

void RenderThread::Main::Notify() { self.m_command_cv.notify_one(); }

bool RenderThread::Main::GetFrontFrameBuffer(void *key, FrameBufferWithReadLock* frame_buffer) {
  // read lock
  assert(false);
#if 0
  std::shared_lock lock(self.m_frame_buffer_rw_mutex);
  auto iter = self.m_frame_buffer_map.find(key);
  if (iter == self.m_frame_buffer_map.end()) {
    return false;
  }

  FrameBufferWithReadLock fb_with_lock = {
      iter->second, std::move(lock)};
#endif
  return true;
}

void RenderThread::Main::RemoveKey(void* key) {
  auto op = std::make_unique<PaintOp>(PaintOpType::RemoveKey);
  op->key = key;
  AddPaintOp(std::move(op));
  Notify();
}

} // namespace ui