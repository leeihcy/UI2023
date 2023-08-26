#pragma once
#include "include/interface/imeta.h"

namespace ui {

// 默认meta，最终被实际meta覆盖。
struct ObjectMeta : public MetaImpl<IObject> {
  static ObjectMeta& Get() {
    static ObjectMeta s;
    return s;
  }

  Uuid UUID() override { return Uuid::Null(); }
  const char* Name() override { return "Object"; }

  MetaDetail Detail() override {
    MetaDetail param = {0};
    return param;
  }
};

} // namespace ui