#pragma once
#include "include/interface/imeta.h"
#include "include/interface/ipanel.h"

namespace ui {

struct PanelMeta : public MetaImpl<IPanel> {
  static PanelMeta& Get() {
    static PanelMeta s;
    return s;
  }

  Uuid UUID() override { return {0xCBA1B3CC, 0x3E6B, 0x11EE, {0xB0, 0x72, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }

  const char* Name() override { return "panel"; }

  MetaDetail Detail() override {
    MetaDetail param;
    param.major_type = OBJ_PANEL;
    param.minor_type = PANEL_SIMPLE;
    param.category = CATEGORY_CONTAINER;
    param.descrption = "容器基类，负责内部控制布局";
    param.default_size = {300, 300};
    return param;
  }
};

struct RoundPanelMeta : public MetaImpl<IRoundPanel> {
  static RoundPanelMeta& Get() {
    static RoundPanelMeta s;
    return s;
  }

  Uuid UUID() override { return {0x021AC934, 0x3E6C, 0x11EE, {0x82, 0xD1, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }

  const char* Name() override { return "round-panel"; }

  MetaDetail Detail() override {
    MetaDetail param;
    param.major_type = OBJ_PANEL;
    param.minor_type = PANEL_SIMPLE;
    param.category = CATEGORY_CONTAINER;
    param.descrption = "圆角容器";
    param.default_size = {300, 300};
    return param;
  }
};

} // namespace ui