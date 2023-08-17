

// template <class T> class LayoutParamImpl : public T {
// public:
//   LayoutParamImpl() { m_pObj = nullptr; }
//   virtual ~LayoutParamImpl() {}
//   virtual void Release() { delete this; }

//   void SetObject(Object *p) { m_pObj = p; }
//   virtual void UpdateByRect() {}
//   virtual void Serialize(SERIALIZEDATA *pData) {}

//   virtual Size CalcDesiredSize() {
//     Size s = {0, 0};

//     if (m_pObj) {
//       // 计算 margin 的大小
//       s.width += m_pObj->GetMarginW();
//       s.height += m_pObj->GetMarginH();
//     }

//     return s;
//   }

// protected:
//   Object *m_pObj;
// };


enum LAYOUTTYPE {
  LAYOUT_TYPE_CANVAS = 0,
  // LAYOUT_TYPE_STACK,
  LAYOUT_TYPE_HORZ,
  LAYOUT_TYPE_VERT,
  LAYOUT_TYPE_GRID,
  LAYOUT_TYPE_DOCK,
  LAYOUT_TYPE_CARD,
  LAYOUT_TYPE_AVERAGE,
};



template <class TLayout, class TLayoutInterface, class TLayoutParam,
          LAYOUTTYPE tLayoutType>
class LayoutImpl : public TLayoutInterface {
public:

  virtual ~LayoutImpl() {
    if (m_pPanel) {
      Object *pChild = nullptr;
      while ((pChild = this->m_pPanel->EnumChildObject(pChild))) {
        pChild->SetLayoutParam(nullptr);
      }
    }
  }


  virtual LAYOUTTYPE GetLayoutType() override { return tLayoutType; }

  static long CreateInstance(IObject *p, ILayout **ppOut) {
    if (nullptr == ppOut || nullptr == p)
      return 1; // E_INVALIDARG;

    TLayout *pLayout = new TLayout;
    pLayout->m_pPanel = p->GetImpl();
    *ppOut = pLayout;
    return 0;
  }
  ILayoutParam *CreateLayoutParam(IObject *pObj) override {
    return s_CreateLayoutParam(pObj);
  }

  static ILayoutParam *s_CreateLayoutParam(IObject *pObj) {
    if (!pObj)
      return nullptr;

    TLayoutParam *p = new TLayoutParam();
    p->SetObject(pObj->GetImpl());
    IMapAttribute *pMapAttr = nullptr;
    pObj->GetImpl()->GetMapAttribute(&pMapAttr);

    if (!pMapAttr) {
      UICreateIMapAttribute(&pMapAttr);
    }
    if (pMapAttr) {
      SERIALIZEDATA data = {0};
      data.pUIApplication = pObj->GetUIApplication();
      data.pSkinRes = pObj->GetResBundle();
      data.nFlags = SERIALIZEFLAG_LOAD | SERIALIZEFLAG_LOAD_ERASEATTR;
      data.pMapAttrib = pMapAttr;

      p->Serialize(&data);

      // 节省内存
      if (pMapAttr->GetAttrCount() == 0) {
        pObj->GetImpl()->ClearMapAttribute();
      }
      SAFE_RELEASE(pMapAttr);
    }

    return static_cast<ILayoutParam *>(p);
  }

  // 自己在布局的时候调用
  static TLayoutParam *s_GetObjectLayoutParam(Object *pObj) {
    if (!pObj)
      return nullptr;

    ILayoutParam *pParam = pObj->GetLayoutParam();
    if (pParam && pParam->GetLayoutType() == tLayoutType) {
      return static_cast<TLayoutParam *>(pParam);
    }

    pParam = s_CreateLayoutParam(pObj->GetIObject());
    pObj->SetLayoutParam(pParam);
    return static_cast<TLayoutParam *>(pParam);
  }

  // Arrage由基类统管，子类实现DoArrage
  virtual void DoArrage(IObject *pObjToArrage = nullptr) = 0;
  virtual void Arrange(IObject *pObjToArrage = nullptr) override {
    m_bDirty = false;
    DoArrage(pObjToArrage);
  }

 

  virtual void ChildObjectContentSizeChanged(IObject *pObj) override {}


};







int Object::GetConfigWidth() {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
      return NDEF;
    else
      return static_cast<CanvasLayoutParam *>(m_pLayoutParam)->GetConfigWidth();
  } else if (m_pIMapAttributeRemain) {
    int nRet = NDEF;
    m_pIMapAttributeRemain->GetAttr_int(XML_WIDTH, false, &nRet);
    return nRet;
  }

  return NDEF;
}

int Object::GetConfigHeight() {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
      return NDEF;
    else
      return static_cast<CanvasLayoutParam *>(m_pLayoutParam)
          ->GetConfigHeight();
  } else if (m_pIMapAttributeRemain) {
    int nRet = NDEF;
    m_pIMapAttributeRemain->GetAttr_int(XML_HEIGHT, false, &nRet);
    return nRet;
  }
  return NDEF;
}

int Object::GetConfigLayoutFlags() {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
      return 0;
    else
      return static_cast<CanvasLayoutParam *>(m_pLayoutParam)
          ->GetConfigLayoutFlags();
  } else {
    const wchar_t *szText =
        m_pIMapAttributeRemain->GetAttr(XML_LAYOUT_ITEM_ALIGN, false);
    if (!szText)
      return 0;

    return CanvasLayoutParam::ParseAlignAttr(szText);
  }
}
int Object::GetConfigLeft() {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
      return NDEF;
    else
      return static_cast<CanvasLayoutParam *>(m_pLayoutParam)->GetConfigLeft();
  } else {
    int nRet = NDEF;
    if (m_pIMapAttributeRemain)
      m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_LEFT, false, &nRet);
    return nRet;
  }
}
int Object::GetConfigRight() {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
      return NDEF;
    else
      return static_cast<CanvasLayoutParam *>(m_pLayoutParam)->GetConfigRight();
  } else {
    int nRet = NDEF;
    if (m_pIMapAttributeRemain)
      m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_RIGHT, false, &nRet);
    return nRet;
  }
}
int Object::GetConfigTop() {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
      return NDEF;
    else
      return static_cast<CanvasLayoutParam *>(m_pLayoutParam)->GetConfigTop();
  } else {
    int nRet = NDEF;
    if (m_pIMapAttributeRemain)
      m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_TOP, false, &nRet);
    return nRet;
  }
}
int Object::GetConfigBottom() {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
      return NDEF;
    else
      return static_cast<CanvasLayoutParam *>(m_pLayoutParam)
          ->GetConfigBottom();
  } else {
    int nRet = NDEF;
    if (m_pIMapAttributeRemain)
      m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_BOTTOM, false, &nRet);
    return nRet;
  }
}

void Object::SetConfigWidth(int n) {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
      static_cast<CanvasLayoutParam *>(m_pLayoutParam)->SetConfigWidth(n);
  } else {
    CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigWidth(n);
  }
}
void Object::SetConfigHeight(int n) {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
      static_cast<CanvasLayoutParam *>(m_pLayoutParam)->SetConfigHeight(n);
  } else {
    CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigHeight(n);
  }
}
void Object::SetConfigLayoutFlags(int n) {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
      static_cast<CanvasLayoutParam *>(m_pLayoutParam)->SetConfigLayoutFlags(n);
  } else {
    CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigLayoutFlags(n);
  }
}
void Object::SetConfigLeft(int n) {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
      static_cast<CanvasLayoutParam *>(m_pLayoutParam)->SetConfigLeft(n);
  } else {
    CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigLeft(n);
  }
}
void Object::SetConfigRight(int n) {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
      static_cast<CanvasLayoutParam *>(m_pLayoutParam)->SetConfigRight(n);
  } else {
    CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigRight(n);
  }
}
void Object::SetConfigTop(int n) {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
      static_cast<CanvasLayoutParam *>(m_pLayoutParam)->SetConfigTop(n);
  } else {
    CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigTop(n);
  }
}
void Object::SetConfigBottom(int n) {
  if (m_pLayoutParam) {
    if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
      static_cast<CanvasLayoutParam *>(m_pLayoutParam)->SetConfigBottom(n);
  } else {
    CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigBottom(n);
  }
}




class ApplicationPtr {
public:
  ApplicationPtr() { m_p = IApplication::create(); }
  ~ApplicationPtr() {
    if (m_p) {
      IApplication::destroy(m_p);
      m_p = nullptr;
    }
  }
  ApplicationPtr(const ApplicationPtr &) = delete;
  ApplicationPtr &operator=(const ApplicationPtr &) = delete;

  ApplicationPtr(ApplicationPtr &&o) {
    m_p = o.m_p;
    o.m_p = nullptr;
  }

  IApplication *operator->() { return m_p; }
  operator IApplication *() const { return m_p; }
  IApplication *get() const { return m_p; }

private:
  IApplication *m_p;
};

class WindowPtr {
public:
  WindowPtr(IResBundle *bundle) { m_p = IWindow::CreateInstance(bundle); }
  ~WindowPtr() {
    if (m_p) { m_p->Release(); }
  }
  WindowPtr(const WindowPtr&) = delete;
  WindowPtr& operator=(const WindowPtr&) = delete;

  IWindow *operator->() { return m_p; }
  operator IWindow *() const { return m_p; }

private:
  IWindow *m_p;
};