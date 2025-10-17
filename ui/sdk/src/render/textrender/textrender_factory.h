#pragma once
#include <vector>
#include "include/uiapi.h"

namespace ui {
class Application;
struct IResource;
struct IObject;
struct ITextRenderBaseMeta;
struct ITextRenderBase;

class TextRenderFactory {
public:
  TextRenderFactory(Application &app);
  ~TextRenderFactory();

  void Init();
  void Clear();
  bool RegisterUITextRender(ITextRenderBaseMeta &meta);

  std::shared_ptr<ITextRenderBase> CreateTextRenderBaseByName(IResource *pSkinRes, const char *szName,
                                  IObject *pObject);

  std::shared_ptr<ITextRenderBase> CreateTextRender(IResource *pSkinRes, int nType, IObject *pObject);

  const char *GetTextRenderBaseName(int nType);

  void EnumTextRenderBaseName(pfnEnumTextRenderNameCallback callback,
                              long wParam, long lParam);

private:
  Application &m_app;
  std::vector<ITextRenderBaseMeta*> m_vecTextRenderCreator;
};
} // namespace ui