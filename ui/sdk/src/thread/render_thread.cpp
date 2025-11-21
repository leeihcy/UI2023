#include "src/thread/render_thread.h"
#include "gpu/include/api.h"
#include "include/util/log.h"
#include "src/graphics/async_render_target.h"
#include "src/graphics/skia/skia_render.h"
#include "src/thread/paint_op.h"
#include <mutex>
#include <vector>
#include <set>

#if defined(OS_WIN)
#include <windows.h>
#include <processthreadsapi.h>
#endif

namespace ui {

RenderThread::RenderThread() : m_running(false), main(*this) {}

RenderThread &RenderThread::GetIntance() {
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

void RenderThread::process_command(PaintOp *op) {
  if (op->type == PaintOpType::RemoveKey) {
    remove_key(op->key);
  } else if (op->type == PaintOpType::AsyncTask) {
    AsyncTaskCommand* command = static_cast<AsyncTaskCommand*>(op);
    command->callback.emit();
  } else if (op->type == PaintOpType::Resize) {
    op->processOnRenderThread(op->key);
  }
}

void RenderThread::thread_proc() {
  set_thread_name("RenderThread");

  while (m_running) {
    if (m_paint_op_group.empty()) {
      std::unique_lock<std::mutex> lock(m_paint_op_queue_mutex);
      m_command_wait.wait(
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

    // 指令分组
    groupOperations(local_queue);
    // 指令优化(去重)
    mergePaintOperations();
    mergeCommandOperations();

    if (m_paint_op_group.empty()) {
      continue;
    }

    for (auto iter = m_paint_op_group.begin(); iter != m_paint_op_group.end();
         ++iter) {
      for (auto &cmd : (*iter)->ops) {
        PaintOp *op = cmd.get();
        if (op->type > PaintOpType::PostCommandStart) {
          process_command(op);
          continue;
        }
        op->processOnRenderThread((SkiaRenderTarget *)op->key);
      }
    }
    m_paint_op_group.clear();
  }

  // 释放其它数据
  m_paint_op_queue.clear();
  m_paint_op_group.clear();
}

RenderThread::PaintOpGroup::PaintOpGroup(void *_key,
                                         std::unique_ptr<PaintOp> &&op)
    : key(_key) {
  ops.push_back(std::move(op));
}

// 针对同一个surface，在begindraw,enddraw之间包围的命令作为一个整体（支持内嵌，如child
// layer）
//
void RenderThread::groupOperations(
    std::vector<std::unique_ptr<PaintOp>> &op_queue) {
  // 1. 按照begindraw/enddraw进行分组，以组为单位，将队列中更早的分组移除掉。
  for (auto &cmd : op_queue) {
    void *key = cmd->key;

    // 命令类型的，不要混在PaintOp里，避免被合并 。
    if (cmd->type >= PaintOpType::PostCommandStart) {
      auto group = std::make_unique<PaintOpGroup>(key, std::move(cmd));
      group->is_command = true;
      m_paint_op_group.push_back(std::move(group));
      continue;
    }

    // 空队列，直接添加
    if (m_paint_op_group.empty()) {
      m_paint_op_group.push_back(
          std::make_unique<PaintOpGroup>(key, std::move(cmd)));
      continue;
    }

    // 对于root surface的新绘制起点，划到新group。
    // sub surface仍然划在root surface下面。
    // 
    // TODO: 
    // 1. 如何确定这是一个root surface。有可能收到了以sub layer的begindraw开始的子数据。
    // 2. 每次draw的脏区域其实是不一样的，如何合并？
    //
    PaintOpGroup *group = m_paint_op_group.back().get();
    if (cmd->type == PaintOpType::BeginDraw) {
      if (group->end_draw) {
        auto new_group = std::make_unique<PaintOpGroup>(key, std::move(cmd));
        new_group->begin_draw = true;
        m_paint_op_group.push_back(std::move(new_group));
        continue;
      }
    }

    if (cmd->type == PaintOpType::EndDraw && key == group->key) {
      group->end_draw = true;
    }
    group->ops.push_back(std::move(cmd));
  }
}

//
// 将重复的绘制指令踢除掉
// 1. 以BeginDraw，EndDraw为分界线，只要两个Group都是针对同一RenderTarget，则可优化掉前一个。
// 2. 合并同一个RenderTarget的Resize事件
// TODO: 需要将Resize之前的BeginDraw、EndDraw也去掉，避免数据不同步。
//
void RenderThread::mergePaintOperations() {
  if (m_paint_op_group.size() <= 1) {
    return;
  }

  std::vector<int> remove_index;

  // 记录每个RenderTarget的BeginDraw/EndDraw上一个出现索引。
  // 再次出现时，将上一个索引添加到移除列表。
  std::map<void*, int> rt_draw_index_map;

  // 记录每个RenderTarget的Resize上一个出现索引。
  // Resize被作为command类型，当独在一个Group里。
  std::map<void*, int> rt_resize_index_map;

  for (int i = 0; i < m_paint_op_group.size() - 1; i++) {
    auto &group = m_paint_op_group[i];
    auto& op = group->ops[0];
    void* key = op->key;
    if (!key) {
      continue;
    }

    if (op->type == PaintOpType::Resize) {
      auto iter = rt_resize_index_map.find(key);
      if (iter == rt_resize_index_map.end()) {
        rt_resize_index_map[key] = i;
        continue;
      }

      remove_index.push_back(iter->second);
      rt_resize_index_map[key] = i;
      continue;
    }

    // 当前函数只合并渲染指令
    if (group->is_command) {
      continue;
    }
    // 注：没有begindraw表示这个group不是一套完整的渲染。
    //     begindraw后的一些操作已经被执行完了，后续的操作才到来
    //     这种group必须继续执行，不能被合并，否则会造成状态异常。
    if (!group->begin_draw) {
      continue;
    }
    if (!group->end_draw) {
      continue;
    }

    auto iter = rt_draw_index_map.find(key);
    if (iter == rt_draw_index_map.end()) {
      rt_draw_index_map[key] = i;
      continue;
    }

    remove_index.push_back(iter->second);
    rt_draw_index_map[key] = i;
  }

#if 1
  // 将Resize之前的BeginDraw、EndDraw也去掉，避免数据不同步。
  // 这里只需要检测最后一个即可，重复的数据在上一轮已经处理了。
  for (auto iter : rt_resize_index_map) {
    void* key = iter.first;
    auto draw_iter = rt_draw_index_map.find(key);
    if (draw_iter == rt_draw_index_map.end()) {
      continue;
    }
    // 在resize前面的才删除。
    if (iter.second > draw_iter->second) {
      remove_index.push_back(draw_iter->second);
    }
  }
#endif
  if (remove_index.empty()) {
    return;
  }

  std::sort(remove_index.begin(), remove_index.end(), std::greater<int>());
  for (int i : remove_index) {
    m_paint_op_group.erase(m_paint_op_group.begin() + i);
  }
  UI_LOG_DEBUG("Merge %d paint & resize op groups", remove_index.size());
}

// 合并 AsyncTask中的冗余命令。例如 窗口变更事件，只需要取最新那个即可。
void RenderThread::mergeCommandOperations() {
  if (m_paint_op_group.size() <= 1) {
    return;
  }

  std::vector<int> remove_index;

  // 记录每个type类型的上一个出现索引。
  // 再次出现时，将上一个索引添加到移除列表。
  std::map<AsyncTaskType, int> type_index_map;

  for (int i = 0; i < m_paint_op_group.size() - 1; i++) {
    auto& op = m_paint_op_group[i]->ops[0];
    if (op->type != PaintOpType::AsyncTask) {
      continue;
    }
    AsyncTaskCommand* async_task = static_cast<AsyncTaskCommand*>(op.get());
    if (async_task->type == AsyncTaskType::Unknown) {
      continue;
    }

    auto iter = type_index_map.find(async_task->type);
    if (iter == type_index_map.end()) {
      type_index_map[async_task->type] = i;
      continue;
    }
    remove_index.push_back(iter->second);
    type_index_map[async_task->type] = i;
  }
  if (remove_index.empty()) {
    return;
  }

  std::sort(remove_index.begin(), remove_index.end(), std::greater<int>());
  for (int i : remove_index) {
    m_paint_op_group.erase(m_paint_op_group.begin() + i);
  }
  UI_LOG_DEBUG("Merge %d async command task", remove_index.size());
}

void RenderThread::remove_key(void *key) {
  // auto iter = m_surface_map.find(key);
  // if (iter == m_surface_map.end()) {
  //   return;
  // }
  // m_surface_map.erase(key);

  // // write lock
  // std::unique_lock lock(m_frame_buffer_rw_mutex);
  // m_frame_buffer_map.erase(key);

  // 主线程已安全释放了record rt，继续在渲染线程安全释放rt.
  delete (SkiaRenderTarget*)key;
}

#if 0
void on_swap_chain(weak_ptr<RenderThread>, void *key, DirtyRegion dirty_region);


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