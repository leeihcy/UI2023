#pragma once
#include "include/interface/iobjectdescription.h"
#include "include/interface/ipanel.h"

namespace ui {

class PanelDescription : public IObjectDescription {
public:
  static IObjectDescription *Get() {
    static PanelDescription s;
    return &s;
  }

  virtual void CreateInstance(ui::IResource *p, void **pp) override {
    *pp = IPanel::create(p);
  }

  virtual OBJ_TYPE GetMajorType() override { return OBJ_PANEL; }
  virtual long GetMinorType() override { return PANEL_SIMPLE; }
  virtual const wchar_t *GetCategory() override { return CATEGORY_CONTAINER; }
  virtual const wchar_t *GetTagName() override { return L"Panel"; }
  virtual Uuid GetUuid() override { return IPanel::UUID(); }

  virtual void GetDefaultSize(long &w, long &h) override {
    w = 100;
    h = 100;
  }
  virtual HBITMAP LoadIcon() override { return 0; }
};

class RoundPanelDescription : public PanelDescription {
public:
  static IObjectDescription *Get() {
    static RoundPanelDescription s;
    return &s;
  }

  virtual void CreateInstance(ui::IResource *p, void **pp) override {
    *pp = IRoundPanel::create(p);
  }
  virtual const wchar_t *GetTagName() override { return L"RoundPanel"; }
  virtual Uuid GetUuid() override { return IRoundPanel::UUID(); }
};

} // namespace ui