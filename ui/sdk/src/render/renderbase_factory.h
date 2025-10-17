#pragma once
#include "include/uiapi.h"
#include <string>
#include <vector>

namespace ui {
class Application;
struct IRenderBase;
struct IResource;
struct IObject;
struct IRenderBaseMeta;

class RenderBaseFactory {
public:
  RenderBaseFactory(Application &app);
  ~RenderBaseFactory();

  void Init();
  void Clear();

  bool RegisterUIRenderBase(IRenderBaseMeta &meta);
  std::shared_ptr<IRenderBase> CreateRenderBaseByName(IResource *pSkinRes,
                                                      const char *szName,
                                                      IObject *pObject);
  std::shared_ptr<IRenderBase> CreateRenderBase(IResource *pSkinRes, int nType,
                                                IObject *pObject);
  const char *GetRenderBaseName(int nType);
  void EnumRenderBaseName(pfnEnumRenderBaseNameCallback callback, llong wParam,
                          llong lParam);

private:
  Application &m_app;
  std::vector<IRenderBaseMeta*>  m_vecUIRenderBase; // 用于创建RenderBase
};

} // namespace ui