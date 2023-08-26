#pragma once
#include <vector>
#include <string>

#include "include/interface/ilayout.h"
#include "include/uiapi.h"


namespace ui {
struct IObject;
struct ILayout;

class LayoutFactory {
public:
  LayoutFactory();
  ~LayoutFactory();

  void Init();
  void Clear();

  bool Register(const char *szName, pfnUICreateLayoutPtr);
  bool Create(int nType, IObject *pObject, ILayout **ppOut);
  bool CreateByName(const char *szName, IObject *pObject,
                    bool bCreateDefault, ILayout **ppOut);
  void EnumLayoutType(pfnEnumLayoutTypeCallback callback, llong wParam,
                      llong lParam);

private:
  struct UILAYOUT_CREATE_INFO {
    std::string m_strName;
    // Uuid m_guid;
    pfnUICreateLayoutPtr m_func;
  };
  typedef std::vector<UILAYOUT_CREATE_INFO *> UILAYOUT_CREATE_DATA;

  UILAYOUT_CREATE_DATA m_vecData;
};
} // namespace ui