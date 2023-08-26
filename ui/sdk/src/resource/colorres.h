#pragma once
#include "include/inc.h"
#include "include/util/color.h"
#include <vector>

namespace ui {
struct IColorResItem;
struct IColorRes;
class Resource;

//
//	color中的一项信息数据
//
class ColorResItem {
public:
  ColorResItem();
  ~ColorResItem();

  IColorResItem *GetIColorResItem();
  const char *GetColorString();
  bool IsMyColor(Color *p);

private:
  std::string m_strId;
  std::string m_strColor; // 颜色字符串
  ColorPtr m_pColor;

  COLORREF *m_pOriginColorValue; // 该COLOR未改变色调前的颜色值，用于还原

  // TODO:
  // 可以再进行扩展。例如文字的黑白颜色变换，是根据文字后面的背景色来决定的，而不是配死的
  bool m_bUseSkinHLS; // 该COLOR是否参与皮肤色调改变

public:
  void SetId(const char *ID);
  const char *GetId();
  const std::string &GetIdRef();
  void SetUseSkinHLS(bool b);
  bool GetUseSkinHLS();
  const std::string &GetColorStringRef();
  bool GetColor(ColorPtr *pColor, bool *bFirstTimeCreate = nullptr);
  void SetColor(const std::string &strColor);

  void SetAttribute(IMapAttribute *p);
  bool ModifyHLS(short h, short l, short s, int nFlag);

private:
  IColorResItem *m_pIColorResItem;
};

//
//	color列表
//
class ColorRes {
public:
  ColorRes(Resource *p);
  ~ColorRes();

  IColorRes &GetIColorRes();

  long GetColorCount();
  IColorResItem *GetColorResItem(long lIndex);
  void GetColor(const char *szColorId, Color **pp);
  const char *GetColorId(Color *p);

public:
  ColorResItem *GetColorItem(int nIndex);
  ColorResItem *GetColorItem(const std::string &strID);
  bool InsertColor(const std::string &strID, const char *szColor,
                   ColorResItem **pItem);
  bool ModifyColor(const std::string &strID, const std::string &strColor);
  bool RemoveColor(const std::string &strID);
  void Clear();

  bool LoadItem(IMapAttribute *pMapAttrib, const char *szData);
  bool ChangeSkinHLS(short h, short l, short s, int nFlag);

private:
  Resource *m_pSkinRes;
  IColorRes *m_pIColorRes;
  std::vector<ColorResItem *> m_vColors;
};

} // namespace ui