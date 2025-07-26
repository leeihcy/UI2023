#pragma once
#include <vector>
#include "include/uiapi.h"

namespace ui {
class Application;
struct IResource;
struct IObject;
struct IMeta;
struct ITextRenderBase;

class TextRenderFactory {
public:
  TextRenderFactory(Application &app);
  ~TextRenderFactory();

  void Init();
  void Clear();
  bool RegisterUITextRender(IMeta &meta);

  bool CreateTextRenderBaseByName(IResource *pSkinRes, const char *szName,
                                  IObject *pObject, ITextRenderBase **ppOut);

  bool CreateTextRender(IResource *pSkinRes, int nType, IObject *pObject,
                        ITextRenderBase **ppOut);

  const char *GetTextRenderBaseName(int nType);

  void EnumTextRenderBaseName(pfnEnumTextRenderNameCallback callback,
                              long wParam, long lParam);

private:
  Application &m_app;
  std::vector<IMeta *> m_vecTextRenderCreator;
};
} // namespace ui