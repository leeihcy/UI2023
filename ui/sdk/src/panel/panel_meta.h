#pragma once
#include "include/interface/imeta.h"
#include "include/interface/ipanel.h"

namespace ui {

class PanelMeta : public MetaImpl<IPanel> {
public:
  static PanelMeta& Get() {
    static PanelMeta s;
    return s;
  }

  Uuid UUID() override { return {0xCBA1B3CC, 0x3E6B, 0x11EE, {0xB0, 0x72, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }

  const char* Name() override { return "Panel"; }

  MetaDetail Detail() override {
    MetaDetail param;
    param.major_type = OBJ_PANEL;
    param.minor_type = PANEL_SIMPLE;
    param.category = CATEGORY_CONTAINER;
    param.descrption = u8"容器基类，负责内部控制布局";
    param.default_size = {300, 300};
    return param;
  }
};

class RoundPanelMeta : public MetaImpl<IRoundPanel> {
public:
  static IMeta *Get() {
    static RoundPanelMeta s;
    return &s;
  }

  Uuid UUID() override { return {0x021AC934, 0x3E6C, 0x11EE, {0x82, 0xD1, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }

  const char* Name() override { return "RoundPanel"; }

  MetaDetail Detail() override {
    MetaDetail param;
    param.major_type = OBJ_PANEL;
    param.minor_type = PANEL_SIMPLE;
    param.category = CATEGORY_CONTAINER;
    param.descrption = u8"容器基类，负责内部控制布局";
    param.default_size = {300, 300};
    return param;
  }
};

} // namespace ui