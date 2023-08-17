#pragma once
#include "include/interface/isvg.h"
#include "sdk/include/interface/iobjectdescription.h"
#include "src/private_inc.h"

namespace ui {
namespace svg {

class SvgDescription : public ui::IObjectDescription {
public:
  static IObjectDescription *Get() {
    static SvgDescription s;
    return &s;
  }

  virtual void CreateInstance(ui::IResource *p, void **pp) override {
    *pp = ISvg::create(p);
  }

  virtual OBJ_TYPE GetMajorType() override { return OBJ_CONTROL; }
  virtual long GetMinorType() override { return CONTROL_SVG; }
  virtual const wchar_t *GetCategory() override { return CATEGORY_SVG; }
  virtual const wchar_t *GetTagName() override { return L"Svg"; }
  virtual Uuid GetUuid() override { return ISvg::UUID(); }

  virtual void GetDefaultSize(long &w, long &h) override {
    w = 200;
    h = 200;
  }
  virtual HBITMAP LoadIcon() override { return 0; }
};

} // namespace svg
} // namespace ui