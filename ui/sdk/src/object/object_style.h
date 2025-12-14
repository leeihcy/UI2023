#ifndef UI_SDK_SRC_OBJECT_OBJECTSTYLE_H_
#define UI_SDK_SRC_OBJECT_OBJECTSTYLE_H_

#include "include/interface/iobject.h"

namespace ui {
 
class ObjectStyle {
public:
  ObjectStyle();

public:
  bool TestObjectStyle(const OBJSTYLE &test);
  void ModifyObjectStyle(OBJSTYLE *add, OBJSTYLE *remove);

  bool IsTabstop() { return m_objStyle.tabstop; }
  void SetTabstop(bool b) { m_objStyle.tabstop = b; }

  bool IsTransparent() { return m_objStyle.transparent; }
  void SetTransparent(bool b) { m_objStyle.transparent = b; }

  void SetNoClip(bool b) { m_objStyle.noclip = b; }
  bool NeedClip();
  bool IsNoClip() { return !NeedClip(); }

  void SetClipClient(bool b) { m_objStyle.clip_client = b; }
  bool NeedClipClient() { return m_objStyle.clip_client; }

  void SetAsNcObject(bool b) { m_objStyle.ncobject = b; }
  bool IsNcObject() { return m_objStyle.ncobject; }
  bool IsRejectMouseMsgAll() { return m_objStyle.reject_all_mouse_msg; }
  bool IsRejectMouseMsgSelf() { return m_objStyle.reject_self_mouse_msg; }
  void SetRejectMouseMsgAll(bool b) { m_objStyle.reject_all_mouse_msg = b; }
  void SetRejectMouseMsgSelf(bool b) { m_objStyle.reject_self_mouse_msg = b; }

  void load_layer_config(bool b) { m_objStyle.layer = b; }
  bool HasLayer() { return m_objStyle.layer; }

public:
  OBJSTYLE m_objStyle;
};

}
#endif