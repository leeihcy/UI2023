#include "src/object/object_style.h"
#include "src/object/object.h"

namespace ui {

bool ObjectStyle::NeedClip() {
  if (m_objStyle.noclip)
    return false;

  //  TODO: 需要重写。
  // 	//
  // 如果子对象是一个层，并且有变换，则不更新剪裁区域，而是切换到父对象的剪裁区域
  // 	// 2015.9.23 add
  // 如果是硬件合成模式，则不用管，层与层之间的数据没有关联。 	if
  // (m_pRenderLayer
  // && m_pRenderLayer->HasTransform())
  // 	{
  // 		WindowBase* pWindow = GetWindow();
  // 		if (!pWindow || !pWindow->IsGpuComposite())
  // 			return false;
  // 	}
  //
  return true;
}

bool ObjectStyle::TestObjectStyle(const OBJSTYLE &test) {
#define TEST(x)                                                                \
  if (test.x && !m_objStyle.x)                                                 \
    return false;

  TEST(transparent);
  TEST(float_on_parent_content);
  TEST(post_paint);
  TEST(hscroll);
  TEST(vscroll);
  TEST(ncobject);
  TEST(reject_all_mouse_msg);
  TEST(reject_self_mouse_msg);
  TEST(receive_dragdrop_event);
  TEST(enable_ime);
  TEST(zindex_overlap);
  TEST(noclip);
  TEST(clip_client);
  TEST(tabstop);
  TEST(want_tab);
  TEST(want_return);
  TEST(layer);

  return true;
}

void ObjectStyle::ModifyObjectStyle(OBJSTYLE *add, OBJSTYLE *remove) {

#define __REMOVE(x)                                                            \
  if (remove->x)                                                               \
    m_objStyle.x = 0;
#define __ADD(x)                                                               \
  if (add->x)                                                                  \
    m_objStyle.x = 1;

  bool old_layer = m_objStyle.layer;

  if (add) {
    __ADD(initialized);
    __ADD(transparent);
    __ADD(float_on_parent_content);
    __ADD(post_paint);
    __ADD(hscroll);
    __ADD(vscroll);
    __ADD(ncobject);
    __ADD(reject_all_mouse_msg);
    __ADD(reject_self_mouse_msg);
    __ADD(receive_dragdrop_event);
    __ADD(enable_ime);
    __ADD(zindex_overlap);
    __ADD(noclip);
    __ADD(clip_client);
    __ADD(tabstop)
    __ADD(want_tab)
    __ADD(want_return)

    // 默认值为1时，如果没有在xml中配置，不会触发setter函数
    // 因此在设置默认值的时候，应该同步一次该值
    if (add->default_ncobject) {
      m_objStyle.default_ncobject = 1;
      m_objStyle.ncobject = 1;
    }
    if (add->default_reject_all_mouse_msg) {
      m_objStyle.default_reject_all_mouse_msg = 1;
      m_objStyle.reject_all_mouse_msg = 1;
    }
    if (add->default_reject_self_mouse_msg) {
      m_objStyle.default_reject_self_mouse_msg = 1;
      m_objStyle.reject_self_mouse_msg = 1;
    }
    if (add->default_transparent) {
      m_objStyle.default_transparent = 1;
      m_objStyle.transparent = 1;
    }
    if (add->default_tabstop) {
      m_objStyle.default_tabstop = 1;
      m_objStyle.tabstop = 1;
    }
  }

  if (remove) {
    __REMOVE(initialized);
    __REMOVE(transparent);
    __REMOVE(float_on_parent_content);
    __REMOVE(post_paint);
    __REMOVE(hscroll);
    __REMOVE(vscroll);
    __REMOVE(ncobject);
    __REMOVE(reject_all_mouse_msg);
    __REMOVE(reject_self_mouse_msg);
    __REMOVE(receive_dragdrop_event);
    __REMOVE(enable_ime);
    __REMOVE(zindex_overlap);
    __REMOVE(noclip);
    __REMOVE(clip_client);
    __REMOVE(tabstop);
    __REMOVE(want_tab)
    __REMOVE(want_return)

    if (remove->default_ncobject) {
      m_objStyle.default_ncobject = 0;
      m_objStyle.ncobject = 0;
    }
    if (remove->default_reject_all_mouse_msg) {
      m_objStyle.default_reject_all_mouse_msg = 0;
      m_objStyle.reject_all_mouse_msg = 0;
    }
    if (remove->default_reject_self_mouse_msg) {
      m_objStyle.default_reject_self_mouse_msg = 0;
      m_objStyle.reject_self_mouse_msg = 0;
    }
    if (remove->default_transparent) {
      m_objStyle.default_transparent = 0;
      m_objStyle.transparent = 0;
    }
    if (remove->default_tabstop) {
      m_objStyle.default_tabstop = 0;
      m_objStyle.tabstop = 0;
    }
  }

  if (old_layer != m_objStyle.layer) {
    static_cast<Object *>(this)->OnLayerStyleChanged(m_objStyle.layer);
  }
}
}