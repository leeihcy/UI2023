#pragma once
#include "include/uiapi.h"
#include <string>
#include <vector>

namespace ui {
class Application;
struct IRenderBase;
struct IResource;
struct IObject;
struct IMeta;

class RenderBaseFactory {
public:
  RenderBaseFactory(Application &app);
  ~RenderBaseFactory();

  void Init();
  void Clear();

  bool RegisterUIRenderBase(IMeta& meta);
  bool CreateRenderBaseByName(IResource *pSkinRes, const char *szName,
                              IObject *pObject, IRenderBase **ppOut);

  bool CreateRenderBase(IResource *pSkinRes, int nType, IObject *pObject,
                        IRenderBase **ppOut);

  const char *GetRenderBaseName(int nType);

  void EnumRenderBaseName(pfnEnumRenderBaseNameCallback callback, llong wParam,
                          llong lParam);

private:
  Application &m_app;
  std::vector<IMeta*>  m_vecUIRenderBase; // 用于创建RenderBase
};

} // namespace ui