#pragma once
#include "imageres.h"
#if 0 // defined(OS_WIN)
#include "cursorres.h"
#include "gifres.h"
#endif
#include "src/util/DataStruct/list.h"

namespace ui {
struct UIElement;
struct IUIElement;
struct IImageManager;

class ImageTagElementInfo {
public:
  std::string strId;
  UIElement *pXmlElement; // xml结点
};

class ImageManager {
public:
  ImageManager(Resource *pSkinRes);
  ~ImageManager(void);

  IImageManager &GetIImageManager();

  ImageRes &GetImageRes();
#if 0 // defined(OS_WIN)
  CursorRes &GetCursorRes();
  GifRes &GetGifRes();
#endif
  UIElement *GetImageXmlElem(const char *szId);

  IImageResItem *InsertImageItem(IMAGE_ITEM_TYPE eType, const char *szID,
                                 const char *szPath);
  bool ModifyImageItem(const char *szID, const char *szPath);
  bool RemoveImageItem(const char *szID);

  bool ModifyImageItemInRunTime(const char *szID, const char *szPath);
  bool ModifyImageItemAlpha(const char *szID, byte nAlphaPercent);

  void Clear();
  bool ChangeSkinHLS(short h, short l, short s, int nFlag);

  static int UIParseImageTagCallback(IUIElement *, IResource *pSkinRes);

private:
  // ISkinTagParse
  int ParseNewElement(UIElement *pElem);
  void OnNewChild(UIElement *pElem);

private:
  // 数据持久层
  IImageManager *m_pIImageManager;

  typedef UIList<ImageTagElementInfo> _MyList;
  typedef UIListItem<ImageTagElementInfo> _MyListItem;
  _MyList m_listUIElement;

  // 对象属性
  ImageRes m_resImage; // 界面图片
#if 0 // defined(OS_WIN)
  CursorRes m_resCursor; // 鼠标样式图片
  GifRes m_resGif;       // 动画
#endif
  Resource *m_pSkinRes;
};

} // namespace ui