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
struct IResBundle;
struct UIMSG;

enum E_BOOL_CREATE_IMPL {
  CREATE_IMPL_FALSE = 0,
  CREATE_IMPL_TRUE = 1,
};

//
//  在UICreateInstance时调用，给对象一次在构造中初始化对象并返回成功失败的方法
//
//  message : UI_WM_FINALCONSTRUCT
//  code : NA
//  wparam : IResBundle*,对象所属资源包
//
#define UI_MSG_FINALCONSTRUCT 168252120
// long  FinalConstruct(IResBundle* p);
#define UIMSG_FINALCONSTRUCT(func)                                             \
  if (uMsg == UI_MSG_FINALCONSTRUCT) {                                         \
    SetMsgHandled(true);                                                       \
    lResult = func((ui::IResBundle *)wParam);                                  \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }
//
//  在~UIObjCreator中调用，给对象在析构前调用虚函数的方法
//
//  message : UI_WM_FINALRELEASE
//  code : NA
//  wparam : NA
//  lparam : NA
//
#define UI_MSG_FINALRELEASE 168252121
// void  FinalRelease();
#define UIMSG_FINALRELEASE(func)                                               \
  if (uMsg == UI_MSG_FINALRELEASE) {                                           \
    SetMsgHandled(true);                                                       \
    func();                                                                    \
    if (IsMsgHandled())                                                        \
      return true;                                                             \
  }

template <class T> class ObjectCreator : public T {
public:
  // 通用型
  static void CreateInstance2(IResBundle *pResBundle, void **pp) {
    *pp = (void *)ObjectCreator<T>::create(pResBundle);
  }
  // 专用型
  static T *create(IResBundle *pResBundle) {
    if (!pResBundle) {
#ifdef _DEBUG
      DebugBreak();
#endif
      return nullptr;
    }

    T *p = new ObjectCreator<T>;

    if (0 != (p->SendMessage(UI_MSG_FINALCONSTRUCT, (long)pResBundle))) {
      T::destroy(p);
      return nullptr;
    }

    return p;
  }
  ObjectCreator() : T(E_BOOL_CREATE_IMPL::CREATE_IMPL_TRUE) {}

  // 确保delete和new在同一模块，由IMessage::destroy触发
  virtual void virtual_delete_this() override {
    this->SendMessage(UI_MSG_FINALRELEASE);
    delete this;
  }

  // 调用imessage->virtualProcessMessage时，回到最外层对象来处理
  virtual bool virtualProcessMessage(UIMSG *pMsg, int nMsgMapID,
                                     bool bDoHook) override {
    return T::nvProcessMessage(pMsg, nMsgMapID, bDoHook);
  }
};

template <class T> class ObjectNoImplCreator : public T {
public:
  // 通用型
  static void CreateInstance2(IResBundle *pResBundle, void **pp) {
    *pp = (void *)ObjectNoImplCreator<T>::CreateInstance(pResBundle);
  }
  // 专用型
  static T *CreateInstance(IResBundle *pResBundle) {
    T *p = new ObjectNoImplCreator<T>;

    if (FAILED(p->SendMessage(UI_MSG_FINALCONSTRUCT, (long)pResBundle))) {
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

} // namespace ui
#endif // _UI_CREATOR_H_