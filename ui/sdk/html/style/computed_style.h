#ifndef _HTML_STYLE_COMPUTED_STYLE_H_
#define _HTML_STYLE_COMPUTED_STYLE_H_

#include "html/css/style_color.h"
#include "html/style/data_ref.h"
#include "html/style/fill_layer.h"

namespace html {

class StyleBackgroundData {
public:
  StyleBackgroundData();
  StyleBackgroundData(const StyleBackgroundData &);

  static std::shared_ptr<StyleBackgroundData> Create() {
    return std::make_shared<StyleBackgroundData>();
  }
  // 用于实现 写时拷贝
  std::shared_ptr<StyleBackgroundData> Copy() const {
    return std::make_shared<StyleBackgroundData>(*this);
  }

  bool operator==(const StyleBackgroundData &other) const {
    return (background_ == other.background_ &&
            background_color_ == other.background_color_);
  }
  bool operator!=(const StyleBackgroundData &other) const {
    return !(*this == other);
  }

  FillLayer background_;
  StyleColor background_color_;

};

class ComputedStyle {
public:
  ComputedStyle();
  // void SetMarginLeft(const Length& v) { };

  void SetBackgroundColor(const StyleColor &v) {
    if (!(background_data_->background_color_ == v))
      background_data_.Access()->background_color_ = v;
  }

  DataRef<StyleBackgroundData> background_data_;
};

} // namespace html

#endif