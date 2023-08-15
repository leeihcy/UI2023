#pragma once
#include "include/interface/ilayout.h"
#include "include/uiapi.h"
#include "include/common/uuid/uuid.h"
#include <vector>


namespace ui {
struct IObject;
struct ILayout;

class LayoutFactory {
public:
  LayoutFactory();
  ~LayoutFactory();

  void Init();
  void Clear();

  bool Register(const wchar_t *szName, pfnUICreateLayoutPtr);
  bool Create(int nType, IObject *pObject, ILayout **ppOut);
  bool CreateByName(const wchar_t *szName, IObject *pObject,
                    bool bCreateDefault, ILayout **ppOut);
  void EnumLayoutType(pfnEnumLayoutTypeCallback callback, long wParam,
                      long lParam);

private:
  struct UILAYOUT_CREATE_INFO {
    String m_strName;
    // Uuid m_guid;
    pfnUICreateLayoutPtr m_func;
  };
  typedef std::vector<UILAYOUT_CREATE_INFO *> UILAYOUT_CREATE_DATA;

  UILAYOUT_CREATE_DATA m_vecData;
};
} // namespace ui