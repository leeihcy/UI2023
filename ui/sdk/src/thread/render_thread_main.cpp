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

void RenderThread::Main::CreateSwapChain(void *key) {
  auto op = std::make_unique<PaintOp>(PaintOpType::CreateSwapChain);
  op->key = key;
  AddPaintOp(std::move(op));
  Notify();
}

void RenderThread::Main::SwapChain(void *key, Window* window, DirtyRegion dirty_region) {
  // 切换到render thread进行处理。
  auto swap_chain_op = std::make_unique<SwapChainOp>();
  swap_chain_op->key = key;
  swap_chain_op->dirty_region = dirty_region;
  AddPaintOp(std::move(swap_chain_op));

  m_window_map[key] = window;
  Notify();
}

const RenderThread::FrameBufferWithReadLock
RenderThread::Main::GetFrameBuffer(void *key) {
  // read lock
  std::shared_lock lock(self.m_frame_buffer_rw_mutex);

  RenderThread::FrameBufferWithReadLock fb_with_lock = {
      self.m_frame_buffer_map[key], std::move(lock)};

  return fb_with_lock;
}

void RenderThread::Main::RemoveKey(void* key) {
  m_window_map.erase(key);

  auto op = std::make_unique<PaintOp>(PaintOpType::RemoveKey);
  op->key = key;
  AddPaintOp(std::move(op));
  Notify();
}

void on_swap_chain(weak_ptr<RenderThread> self, void *key, DirtyRegion dirty_region) {
  if (!self) {
    return;
  }
  self->main.on_swap_chain(key, dirty_region);
}
void RenderThread::Main::on_swap_chain(void *key, const DirtyRegion& dirty_region) {
  auto iter = m_window_map.find(key);
  if (iter == m_window_map.end()) {
    return;
  }
  iter->second->GetWindowRender().DirectCommit(dirty_region);
}

} // namespace ui