
typedef int VkStringErrorFlags;
#define VK_STRING_ERROR_NONE 0
#define VK_STRING_ERROR_LENGTH 1
#define VK_STRING_ERROR_BAD_DATA 2
// #define UTF8_ONE_BYTE_MASK 0x10000000
// #define UTF8_TWO_BYTE_MASK 0x20000000
// #define UTF8_THREE_BYTE_MASK 0x40000000
// #define UTF8_ONE_BYTE_CODE UTF8_ONE_BYTE_MASK
// #define UTF8_TWO_BYTE_CODE UTF8_TWO_BYTE_MASK
// #define UTF8_THREE_BYTE_CODE UTF8_THREE_BYTE_MASK
static const uint8_t UTF8_ONE_BYTE_CODE = 0xC0;
static const uint8_t UTF8_ONE_BYTE_MASK = 0xE0;
static const uint8_t UTF8_TWO_BYTE_CODE = 0xE0;
static const uint8_t UTF8_TWO_BYTE_MASK = 0xF0;
static const uint8_t UTF8_THREE_BYTE_CODE = 0xF0;
static const uint8_t UTF8_THREE_BYTE_MASK = 0xF8;
static const uint8_t UTF8_DATA_BYTE_CODE = 0x80;
static const uint8_t UTF8_DATA_BYTE_MASK = 0xC0;

VkStringErrorFlags vk_string_validate(const int max_length, const char *utf8) {
    VkStringErrorFlags result = VK_STRING_ERROR_NONE;
    int num_char_bytes = 0;
    int i, j;
    for (i = 0; i <= max_length; i++) {
        if (utf8[i] == 0) {
            break;
        } else if (i == max_length) {
            result |= VK_STRING_ERROR_LENGTH;
            break;
        } else if ((utf8[i] >= 0x20) && (utf8[i] < 0x7f)) {
            num_char_bytes = 0;
        } else if ((utf8[i] & UTF8_ONE_BYTE_MASK) == UTF8_ONE_BYTE_CODE) {
            num_char_bytes = 1;
        } else if ((utf8[i] & UTF8_TWO_BYTE_MASK) == UTF8_TWO_BYTE_CODE) {
            num_char_bytes = 2;
        } else if ((utf8[i] & UTF8_THREE_BYTE_MASK) == UTF8_THREE_BYTE_CODE) {
            num_char_bytes = 3;
        } else {
            result = VK_STRING_ERROR_BAD_DATA;
        }
        // Validate the following num_char_bytes of data
        for (j = 0; (j < num_char_bytes) && (i < max_length); j++) {
            if (++i == max_length) {
                result |= VK_STRING_ERROR_LENGTH;
                break;
            }
            if ((utf8[i] & UTF8_DATA_BYTE_MASK) != UTF8_DATA_BYTE_CODE) {
                result |= VK_STRING_ERROR_BAD_DATA;
            }
        }
    }
    return result;
}


// template <class T> class LayoutParamImpl : public T {
// public:
//   LayoutParamImpl() { m_pObj = nullptr; }
//   virtual ~LayoutParamImpl() {}
//   virtual void Release() { delete this; }

//   void SetObject(Object *p) { m_pObj = p; }
//   virtual void UpdateByRect() {}
//   virtual void Serialize(SerializeParam *pData) {}

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
    std::shared_ptr<IMapAttribute> pMapAttr = pObj->GetImpl()->GetMapAttribute();

    if (!pMapAttr) {
      pMapAttr = UICreateIMapAttribute();
    }
    if (pMapAttr) {
      SerializeParam data = {0};
      data.pUIApplication = pObj->GetUIApplication();
      data.pSkinRes = pObj->GetResBundle();
      data.nFlags = SERIALIZEFLAG_LOAD | SERIALIZEFLAG_LOAD_ERASEATTR;
      data.pMapAttrib = pMapAttr;

      p->Serialize(&data);

      // 节省内存
      if (pMapAttr->GetAttrCount() == 0) {
        pObj->GetImpl()->ClearMapAttribute();
      }
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
  virtual void DoArrange(ArrangeParam* param) = 0;
  virtual void Arrange(ArrangeParam* param) override {
    m_bDirty = false;
    DoArrange(pObjToArrage);
  }

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