#ifndef UI_SDK_SRC_OBJECT_OBJECTSTATE_H_
#define UI_SDK_SRC_OBJECT_OBJECTSTATE_H_

#include "include/macro/xmldefine.h"

namespace ui {

struct ObjState {
  unsigned char visibility_ : 2; // 0:collapsed 1:visible 2:hidden
  bool disable : 1;
  bool press : 1;
  bool hover : 1;
  bool force_press : 1;
  bool force_hover : 1;
  bool focus : 1;
  bool default_ : 1;
  bool selected : 1;
  bool checked : 1;
  bool radio_checked : 1; // ??
  bool readonly : 1;
  bool activate : 1; // 给窗口使用
};

class ObjectState {
public:
  ObjectState();

public:
  bool IsFocus() { return m_objState.focus; }

  bool IsSelfVisible() { return m_objState.visibility_ == VISIBILITY_VISIBLE; }

  void SetDisableDirect(bool b) { m_objState.disable = b; }
  bool IsSelfDisable() { return m_objState.disable; }

  bool IsDefault() { return m_objState.default_; }
  bool IsHover() { return m_objState.force_hover || m_objState.hover; }
  bool IsPress() { return m_objState.force_press || m_objState.press; }
  bool IsForceHover() { return m_objState.force_hover; }
  bool IsForcePress() { return m_objState.force_press; }
  bool IsSelected() { return m_objState.selected; }

  bool IsEnable() { return !m_objState.disable; }
  void SetEnable(bool b, bool bNoitfy = true);

  void LoadVisibleEx(int l) { m_objState.visibility_ = static_cast<VISIBILITY_TYPE>(l); }
  int SaveVisibleEx() { return m_objState.visibility_; }

  void SetDefault(bool b, bool bNotify = true);
  void SetSelected(bool b, bool bNotify = true);
  void SetForceHover(bool b, bool bNotify = true);
  void SetForcePress(bool b, bool bNotify = true);
  void SetHover(bool b, bool bNotify = true);
  void SetPress(bool b, bool bNotify = true);

public:
  ObjState m_objState;
};

}
#endif