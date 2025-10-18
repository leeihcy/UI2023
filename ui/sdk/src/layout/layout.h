#ifndef LAYOUT_H_70402CBF_A4C2_4a35_AA14_829F26BA9A5C
#define LAYOUT_H_70402CBF_A4C2_4a35_AA14_829F26BA9A5C

#include "include/interface/ilayout.h"
#include "include/interface/imapattr.h"
#include "src/object/object.h"

namespace ui {
class Panel;
class Window;

#if 0

/*
**	注意：DockLayout只允许有一个center对象，但该对象可以是一个Panel、Object
**		  如果在配置文件中有多个center对象，则只取第一个对象
*/
class DockLayout : public Layout
{
public:
	virtual Size  MeasureChildObject();
	virtual void  ArrangeChildObject(Object* pObjToArrage = nullptr);
};


class LayoutManagerFactory 
{
public:
	static Layout* GetLayout(const wchar_t* szType, Object* pPanel);
	static Layout* GetLayoutFromAttribute(IMapAttribute* pMapAttrib, Object* pObject, bool bCreateDefaultCanvasLayout=true);
};
#endif


void LoadConfigWH(const char *szText, int &wh, int &whtype);
const char *SaveConfigWH(int wh, int whtype);

template <class T, class IT, class TParam> class LayoutImpl : public IT {
public:
  LayoutImpl() {
    m_pPanel = nullptr;
    m_bDirty = true;
  }
  virtual ~LayoutImpl() {
    if (m_pPanel) {
      Object *pChild = nullptr;

      while ((pChild = this->m_pPanel->EnumChildObject(pChild))) {
        pChild->layout.SetLayoutParam(nullptr);
      }
    }
  }

  static bool CreateInstance(IObject *p, ILayout **ppOut) {
    if (nullptr == ppOut || nullptr == p)
      return false;

    T *pLayout = new T;
    pLayout->m_pPanel = p->GetImpl();
    *ppOut = pLayout;
    return true;
  }

  void Release() override { delete this; }

  bool IsDirty() override { return m_bDirty; }
  void SetDirty(bool b) override { m_bDirty = b; }

  // Arrage由基类统管，子类实现DoArrage
  virtual void DoArrange(ArrangeParam& param) = 0;
  virtual void Arrange(ArrangeParam& param) override {
    m_bDirty = false;
    DoArrange(param);
  }

  ILayoutParam *CreateLayoutParam(IObject *pObj) override {
    if (!pObj)
      return nullptr;

    TParam *p = new TParam(pObj->GetImpl());
    std::shared_ptr<IMapAttribute> pMapAttr = pObj->GetImpl()->GetMapAttribute();

    SerializeParam data = {0};
    data.resource = pObj->GetResource();
    data.nFlags = SERIALIZEFLAG_LOAD | SERIALIZEFLAG_LOAD_ERASEATTR;
    data.pMapAttrib = pMapAttr.get();

    p->Serialize(&data);

    // 节省内存
    if (pMapAttr && pMapAttr->GetAttrCount() == 0) {
      pObj->GetImpl()->ClearMapAttribute();
    }

    return static_cast<ILayoutParam *>(p);
  }

  // 自己在布局的时候调用
  TParam *GetObjectLayoutParam(Object *pObj) {
    if (!pObj)
      return nullptr;

    ILayoutParam *pParam = pObj->layout.GetLayoutParam();
    if (pParam && pParam->UUID() == T::UUID()) {
      return static_cast<TParam *>(pParam);
    }

    pParam = CreateLayoutParam(pObj->GetIObject());
    pObj->layout.SetLayoutParam(pParam);
    return static_cast<TParam *>(pParam);
  }

protected:
  Object *m_pPanel; // 与该布局关联的panel
  bool m_bDirty;    // 是否需要布局
};

} // namespace ui

#endif // LAYOUT_H_70402CBF_A4C2_4a35_AA14_829F26BA9A5C