#include "src/thread/render_thread.h"
#include "include/common/ptr/weak_ptr.h"
#include "include/common/signalslot/slot.h"
#include "include/interface/renderlibrary.h"
#include "include/uiapi.h"
#include "include/util/rect.h"
#include "include/util/rect_region.h"
#include "src/graphics/record/record_render_target.h"
#include "src/graphics/skia/skia_render.h"
#include "src/message_loop/message_loop.h"
#include "src/thread/paint_op.h"
#include <mutex>
#include <type_traits>

namespace ui {

RenderThread::RenderThread(ui::Application &app)
    : m_running(false), main(*this) {}

static void set_thread_name(const char *name) {
#if defined(OS_MAC)
  pthread_setname_np("RenderThread");
#elif defined(OS_LINUX)
  pthread_setname_np(pthread_self(), "RenderThread");
#elif defined(OS_WIN)
  SetThreadDescription(GetCurrentThread(), L"RenderThread");
#endif
}

RenderThread::Surface &RenderThread::get_layer_render_target(void *key) {
  auto iter = m_layer_map.find(key);
  if (iter != m_layer_map.end()) {
    return iter->second;
  }

  Surface surface;
  surface.back = std::make_unique<SkiaRenderTarget>();
  surface.front = nullptr; // 只有调用了CreateSwapChain的才会创建。
  m_layer_map.emplace(key, std::move(surface));
  return m_layer_map[key];
}

void RenderThread::process_command(PaintOp *op) {
  if (op->type == PaintOpType::SwapChain) {
    SwapChainOp *swap_chain_op = static_cast<SwapChainOp *>(op);
    swap_chain(op->key, swap_chain_op->dirty_region);
  } else if (op->type == PaintOpType::CreateSwapChain) {
    create_swap_chain(op->key);
  } else if (op->type == PaintOpType::RemoveKey) {
    remove_key(op->key);
  }
}

void RenderThread::thread_proc() {
  set_thread_name("RenderThread");

  while (m_running) {
    {
      std::unique_lock<std::mutex> lock(m_paint_op_queue_mutex);
      m_command_cv.wait(
          lock, [this] { return !m_paint_op_queue.empty() || !m_running; });
      if (!m_running) {
        break;
      }
    }
    std::vector<std::unique_ptr<PaintOp>> local_queue;
    {
      std::lock_guard<std::mutex> lock(m_paint_op_queue_mutex);
      local_queue.swap(m_paint_op_queue);
    }

    for (auto &cmd : local_queue) {
      PaintOp *op = cmd.get();
      if (op->type > PaintOpType::PostCommandStart) {
        process_command(op);
        continue;
      }

      RenderThread::Surface &surface = get_layer_render_target(op->key);
      if (op->type == PaintOpType::SetDirtyRegion && surface.front) {
        frames_sync(surface);
      } else if (op->type == PaintOpType::EndDraw) {
        surface.Reset();
      } else if (op->type == PaintOpType::Resize) {
        auto *resize_op = static_cast<ResizeOp *>(op);
        surface.width = resize_op->width;
        surface.height = resize_op->height;
        surface.back_resized = true;
      }
      op->processOnRenderThread(surface.back.get());

#if defined(_DEBUGx)
      if (op->type == PaintOpType::DrawRect) {
        auto param = static_cast<DrawRectOp *>(op);
        if (surface.back.get() == surface.distinguish_rt_for_debug) {
          // 再继续画一道
          Rect deflat = ui::Rect::MakeLTRB(20, 20, 20, 20);
          param->rect.Deflate(deflat);
          surface.back->DrawRect(param->rect, ui::Color::black());
        }
      }
#endif
    }
  }
}

void RenderThread::create_swap_chain(void *key) {
  auto &surface = m_layer_map[key];
  surface.front = std::make_unique<SkiaRenderTarget>();
#if defined(_DEBUG)
  surface.distinguish_rt_for_debug = surface.front.get();
#endif
}

void RenderThread::remove_key(void *key) {
  auto iter = m_layer_map.find(key);
  if (iter == m_layer_map.end()) {
    return;
  }
  m_layer_map.erase(key);

  // write lock
  std::unique_lock lock(m_frame_buffer_rw_mutex);
  m_frame_buffer_map.erase(key);
}

void on_swap_chain(weak_ptr<RenderThread>, void *key, DirtyRegion dirty_region);

void RenderThread::swap_chain(void *key, const DirtyRegion &dirty_region) {
  auto &surface = m_layer_map[key];
  if (!surface.front) {
    return;
  }

  // 交换指针
  std::swap(surface.back, surface.front);

  // frame buffer
  {
    // write lock
    std::unique_lock lock(m_frame_buffer_rw_mutex);

    auto &frame_buffer = m_frame_buffer_map[key];
    surface.front->GetFrameBuffer(&frame_buffer);
  }

  // 在下一次BeginDraw中同步缓存差异，将本次back上的新增数据，也绘制到front上。
  surface.last_dirty_region = dirty_region;

  // 切到主线程，通知主线程Commit
  PostTaskToUIThread(Slot(&on_swap_chain, m_weakptr_factory.get(), key,
                          surface.last_dirty_region));
}

// 将front/back数据内容进行同步，避免使用完back后，再使用front时，丢了一帧内容的问题。
void RenderThread::frames_sync(Surface &surface) {
  if (!surface.back || !surface.front) {
    return;
  }

  // 同步尺寸
  SkiaRenderTarget *sk_back =
      static_cast<SkiaRenderTarget *>(surface.back.get());
  SkiaRenderTarget *sk_front =
      static_cast<SkiaRenderTarget *>(surface.front.get());
  if (!sk_front->GetSkiaSurface()) {
    return;
  }

  unsigned int front_width = sk_front->GetSkiaSurface()->width();
  unsigned int front_height = sk_front->GetSkiaSurface()->height();

  bool full_dirty = false;
  if (!sk_back->GetSkiaSurface()) {
    sk_back->Resize(surface.width, surface.height);
    full_dirty = true;
  } else if (!surface.back_resized) {
    // 只在本次外部没用调用Resize时，再同步上一轮的大小。
    // 由于render target size的冗余特性，
    // 出现front/back的最终大小不一致是可能的，
    // 只要满足大于surface大小即可。
    if (sk_back->GetSkiaSurface()->width() != front_width ||
        sk_back->GetSkiaSurface()->height() != front_height) {
      sk_back->Resize(front_width, front_height);
      full_dirty = true;
    }
  }

  // 优化：如果本次的脏区域范围大于等于上一帧的变动范围，则不需要做帧同步
  const DirtyRegion &dirty_region_current_frame = sk_back->GetDirtyRegion();
  DirtyRegion dirty_region_prev_frame = surface.last_dirty_region;

  if (full_dirty) {
    dirty_region_prev_frame.AddRect(
        ui::Rect::MakeXYWH(0, 0, surface.width, surface.height));
  } else {
    dirty_region_prev_frame = surface.last_dirty_region;
  }

  if (dirty_region_current_frame.Contains(dirty_region_prev_frame)) {
    return;
  }

  // read lock
  std::shared_lock lock(m_frame_buffer_rw_mutex);

  // TODO: 继续优化，不仅仅是Contains，更应该是Sub
  for (int i = 0; i < dirty_region_prev_frame.Count(); i++) {
    Rect *rect = dirty_region_prev_frame.GetRectPtrAt(i);
    Render2TargetParam param = {0};
    param.xSrc = param.xDst = rect->left;
    param.ySrc = param.yDst = rect->top;
    param.wSrc = param.wDst = rect->Width();
    param.hSrc = param.hDst = rect->Height();
    param.opacity = 255;
    sk_front->Render2Target(sk_back, &param);
  }
}

} // namespace ui