#pragma once
#include "include/inc.h"
#include "include/util/color.h"
#include <vector>
#include <string>

namespace ui {
struct IColorResItem;
struct IColorRes;
class ResourceBundle;

//
//	color中的一项信息数据
//
class ColorResItem {
public:
  ColorResItem();
  ~ColorResItem();

  IColorResItem *GetIColorResItem();
  const char *GetColorString();
  // bool IsMyColor(Color *p);

private:
  std::string m_strId;
  std::string m_strColor; // 颜色字符串

  Color m_color;
  bool  m_valid = false;

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
  Color GetColor();
  void SetColor(const std::string &strColor);

  void SetAttribute(IAttributeMap *p);
  bool ModifyHLS(short h, short l, short s, int nFlag);

private:
  std::unique_ptr<IColorResItem> m_pIColorResItem;
};

//
//	color列表
//
class ColorRes {
public:
  ColorRes(ResourceBundle *p);
  ~ColorRes();

  IColorRes &GetIColorRes();

  long GetCount();

  Color GetColor(const char *id, bool* exist=nullptr);
  const char *GetColorId(Color *p);

public:
  ColorResItem *GetColorItem(int nIndex);
  ColorResItem *GetColorItem(const std::string &id);
  bool InsertColor(const std::string &id, const char *szColor,
                   ColorResItem **pItem);
  bool ModifyColor(const std::string &id, const std::string &strColor);
  bool RemoveColor(const std::string &id);
  void Clear();

  bool LoadItem(IAttributeMap *attribute_map, const char *szData);
  bool ChangeSkinHLS(short h, short l, short s, int nFlag);

private:
  ResourceBundle *m_resource_bundle;
  IColorRes *m_pIColorRes;
  std::vector<ColorResItem *> m_items;
};

} // namespace ui