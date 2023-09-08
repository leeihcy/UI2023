#pragma once
#include "src/util/windows.h"

namespace ui {
class Application;
struct ITextRenderBase;

class TextRenderFactory {
public:
  TextRenderFactory(Application &app);
  ~TextRenderFactory();

  void Init();
  void Clear();

  bool RegisterUITextRenderBaseCreateData(const char *szName, int nType,
                                          pfnUICreateTextRenderBasePtr pfunc);

  bool CreateTextRenderBaseByName(IResource *pSkinRes, const char *szName,
                                  IObject *pObject, ITextRenderBase **ppOut);

  bool CreateTextRender(IResource *pSkinRes, int nType, IObject *pObject,
                        ITextRenderBase **ppOut);

  const char *GetTextRenderBaseName(int nType);

  void EnumTextRenderBaseName(pfnEnumTextRenderBaseNameCallback callback,
                              long wParam, long lParam);

private:
  Application &m_app;

  struct UITEXTRENDERBASE_CREATE_INFO {
    std::string m_strName;
    int m_nRenderType;
    pfnUICreateTextRenderBasePtr m_func;
  };

  typedef std::vector<UITEXTRENDERBASE_CREATE_INFO *>
      UITEXTRENDERBASE_CREATE_DATA;
  UITEXTRENDERBASE_CREATE_DATA
      m_vecUITextRenderBaseCreateData; // 用于从字符串创建TextRenderBase
};
} // namespace ui