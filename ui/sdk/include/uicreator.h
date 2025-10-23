#ifndef _UI_CREATOR_H_
#define _UI_CREATOR_H_

//////////////////////////////////////////////////////////////////////////
//
//    对象创建者，为了支持在对象创建和销毁的时候能调用虚函数，要求所有对象
//    调用UICreateInstance进行创建，将初始化代码和释放代码放在FinalConstruct
//    /FinalRelease里面
//
//////////////////////////////////////////////////////////////////////////

namespace ui {
struct IApplication;
struct IResourceBundle;
struct UIMSG;

#if 0
// 已废弃 2023/8/19
// 使用 XxxMeta 代替。
template <class T> class ObjectCreator : public T {
public:
  // 通用型
  static void CreateInstance2(IResourceBundle *pResource, void **pp) {
    // *pp = (void *)ObjectCreator<T>::create(pResource);
    // TODO:
  }
  // 专用型
//   static T *create(IResourceBundle *pResource) {
//     if (!pResource) {
// #ifdef _DEBUG
//       DebugBreak();
// #endif
//       return nullptr;
//     }

//     T *p = new ObjectCreator<T>;

//     if (0 != (p->SendMessage(UI_MSG_FINALCONSTRUCT, (long)pResource))) {
//       delete p;
//       return nullptr;
//     }

//     return p;
//   }
  ObjectCreator() : T(eCreateImpl::True) {}

  // 确保delete和new在同一模块，由IMessage::destroy触发
  // virtual void virtual_delete_this() override {
  //   this->SendMessage(UI_MSG_FINALRELEASE);
  //   delete this;
  // }

  // 调用imessage->virtualProcessMessage时，回到最外层对象来处理
  virtual bool virtualProcessMessage(UIMSG *pMsg, int nMsgMapID,
                                     bool bDoHook) override {
    return T::nvProcessMessage(pMsg, nMsgMapID, bDoHook);
  }
};

template <class T> class ObjectNoImplCreator : public T {
public:
  // 通用型
  static void CreateInstance2(IResourceBundle *pResource, void **pp) {
    *pp = (void *)ObjectNoImplCreator<T>::CreateInstance(pResource);
  }
  // 专用型
  static T *CreateInstance(IResourceBundle *pResource) {
    T *p = new ObjectNoImplCreator<T>;

    if (FAILED(p->SendMessage(UI_MSG_FINALCONSTRUCT, (long)pResource))) {
      delete p;
      return nullptr;
    }

    return p;
  }
  ObjectNoImplCreator() : T() {}

  // 确保delete和new在同一模块，由IMessage::Destroy触发
  virtual void virtual_delete_this() override {
    this->SendMessage(UI_MSG_FINALRELEASE);
    delete this;
  }
};
#endif
} // namespace ui
#endif // _UI_CREATOR_H_