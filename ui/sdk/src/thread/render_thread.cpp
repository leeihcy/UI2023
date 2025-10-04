#include "src/thread/render_thread.h"
#include "include/common/ptr/weak_ptr.h"
#include "include/common/signalslot/slot.h"
#include "include/interface/renderlibrary.h"
#include "include/uiapi.h"
#include "include/util/log.h"
#include "include/util/rect.h"
#include "include/util/rect_region.h"
#include "src/application/config/config.h"
#include "src/graphics/record/record_render_target.h"
#include "src/graphics/skia/skia_render.h"
#include "src/message_loop/message_loop.h"
#include "src/thread/paint_op.h"
#include <mutex>
#include <type_traits>
#include <vector>

namespace ui {

RenderThread::RenderThread()
    : m_running(false), main(*this) {}

static RenderThread& GetIntance() {
  static RenderThread s;
  return s;
}

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
  auto iter = m_surface_map.find(key);
  if (iter != m_surface_map.end()) {
    return iter->second;
  }

  Surface surface;
  surface.back = std::make_unique<SkiaRenderTarget>();
  surface.front = nullptr; // 只有调用了CreateSwapChain的才会创建。
  m_surface_map.emplace(key, std::move(surface));
  return m_surface_map[key];
}

void RenderThread::process_command(PaintOp *op) {
  if (op->type == PaintOpType::RemoveKey) {
    remove_key(op->key);
  }
}

void RenderThread::thread_proc() {
  set_thread_name("RenderThread");

  while (m_running) {
    if (m_paint_op_group.empty())
    {
      std::unique_lock<std::mutex> lock(m_paint_op_queue_mutex);
      m_command_cv.wait(
          lock, [this] { return !m_paint_op_queue.empty() || !m_running; });
      if (!m_running) {
        break;
      }
    }

    // 转移指令队列中的元素
    std::vector<std::unique_ptr<PaintOp>> local_queue;
    {
      std::lock_guard<std::mutex> lock(m_paint_op_queue_mutex);
      local_queue.swap(m_paint_op_queue);
      // m_paint_op_queue_on_thread.insert(m_paint_op_queue_on_thread.end(),
      //   std::make_move_iterator(m_paint_op_queue.begin()),
      //   std::make_move_iterator(m_paint_op_queue.end()));
      // m_paint_op_queue.clear();
    }
    // 指令优化
    merge_and_optimize_operations(local_queue);
    if (m_paint_op_group.empty()) {
      continue;
    }

    auto &op_group = m_paint_op_group.front();
    for (auto &cmd : op_group->ops) {
      PaintOp *op = cmd.get();
      if (op->type > PaintOpType::PostCommandStart) {
        process_command(op);
        continue;
      }

      RenderThread::Surface &surface = get_layer_render_target(op->key);
      // if (op->type == PaintOpType::BeginDraw && surface.front) {
      //   frames_sync_size(surface);
      // } else 
      // if (op->type == PaintOpType::SetDirtyRegion && surface.front) {
      //   frames_sync_dirty(surface);
      // } else 
      if (op->type == PaintOpType::EndDraw) {
        surface.Reset();
      } else if (op->type == PaintOpType::Resize) {
        auto *resize_op = static_cast<ResizeOp *>(op);
        surface.width = resize_op->width;
        surface.height = resize_op->height;
        surface.back_resized = true;
      }
      op->processOnRenderThread(surface.back.get());
    }
    m_paint_op_group.pop_front();
  }
}

RenderThread::PaintOpGroup::PaintOpGroup(void *_key,
                                         std::unique_ptr<PaintOp> &&op)
    : key(_key) {
  ops.push_back(std::move(op));
}

// 指令集优化
// 针对同一个surface，在begindraw,enddraw之间包围的命令作为一个整体（支持内嵌，如child
// layer）
//
void RenderThread::merge_and_optimize_operations(
    std::vector<std::unique_ptr<PaintOp>> &op_queue) {
  // 1. 按照begindraw/enddraw进行分组，以组为单位，将队列中更早的分组移除掉。
  for (auto &cmd : op_queue) {
    void *key = cmd->key;

    // 空队列，直接添加
    if (m_paint_op_group.empty()) {
      m_paint_op_group.push_back(
          std::make_unique<PaintOpGroup>(key, std::move(cmd)));
      continue;
    }

    // 对于root surface的新绘制起点，划到新group。
    // sub surface仍然划在root surface下面。
    PaintOpGroup *group = m_paint_op_group.back().get();
    if (cmd->type == PaintOpType::BeginDraw) {
      auto iter_surface = m_surface_map.find(cmd->key);
      if (iter_surface != m_surface_map.end()) {
        m_paint_op_group.push_back(
            std::make_unique<PaintOpGroup>(key, std::move(cmd)));
        continue;
      }
    }

    group->ops.push_back(std::move(cmd));
  }

  // 将重复的绘制指令踢除掉
  if (m_paint_op_group.size() <= 2) {
    return;
  }

  std::vector<int> remove_index;

  for (int i = 2; i < m_paint_op_group.size()-1; i++) {
    auto& group = m_paint_op_group[i];

    for (int j = i+1; j < m_paint_op_group.size(); j++) {
      auto& group2 = m_paint_op_group[j];
      if (group2->key != group->key) {
        continue;
      }
      if (group2->ops[0]->type != PaintOpType::BeginDraw) {
        continue;
      }
      remove_index.insert(remove_index.begin(), i);
      break;
    }
  }
  for (int i: remove_index) {
    m_paint_op_group.erase(m_paint_op_group.begin()+i);
    UI_LOG_DEBUG("merge paint op group.");
  }
  
}

void RenderThread::remove_key(void *key) {
  auto iter = m_surface_map.find(key);
  if (iter == m_surface_map.end()) {
    return;
  }
  m_surface_map.erase(key);

  // write lock
  std::unique_lock lock(m_frame_buffer_rw_mutex);
  m_frame_buffer_map.erase(key);
}

void on_swap_chain(weak_ptr<RenderThread>, void *key, DirtyRegion dirty_region);

#if 0
void RenderThread::swap_chain(void *key, const DirtyRegion &dirty_region) {
  assert(false);
  auto &surface = m_surface_map[key];
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
#endif

} // namespace ui