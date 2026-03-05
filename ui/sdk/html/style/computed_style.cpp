#include "html/style/computed_style.h"

namespace html {

StyleBackgroundData::StyleBackgroundData()
    : background_(FillLayer(EFillLayerType::kBackground, true)),
      background_color_(StyleColor(Color::kTransparent)) {}

StyleBackgroundData::StyleBackgroundData(const StyleBackgroundData &other)
    : background_(other.background_),
      background_color_(other.background_color_) {}
}