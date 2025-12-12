#ifndef _UI_SDK_SRC_THREAD_PAINTOPPOOL_H_
#define _UI_SDK_SRC_THREAD_PAINTOPPOOL_H_

#include <memory>

namespace ui {
class PaintOp;

//
// 解决多线程渲染时，指令对象需要频繁创建的问题。
// 
// 注：待定，使用Pool的收益不一定会高。


// template <class _Ty, class... _Types>
// inline PaintOp* AllocPaintOp(_Types&&... _Args) {
//      return new (memory) T(std::forward<Args>(args)...);
// }
// void deallocate(T* object) {
//   object->~T();
// }

}

#endif