#include "html/style/fill_layer.h"

namespace html {


FillLayer::FillLayer(EFillLayerType type, bool use_initial_values)
    : next_(nullptr) {

    }

FillLayer::FillLayer(const FillLayer& o)
    : next_(o.next_ ? new FillLayer(*o.next_) : nullptr) {

    }

FillLayer::~FillLayer() {
  if (next_) {
    delete next_;
  }
}

bool FillLayer::operator==(const FillLayer& o) const {
  return LayerPropertiesEqual(o) &&
         ((next_ && o.next_) ? *next_ == *o.next_ : next_ == o.next_);
}

bool FillLayer::LayerPropertiesEqual(const FillLayer& o) const {
  return false; // base::ValuesEquivalent(image_, o.image_) &&
  //        position_x_ == o.position_x_ && position_y_ == o.position_y_ &&
  //        background_x_origin_ == o.background_x_origin_ &&
  //        background_y_origin_ == o.background_y_origin_ &&
  //        attachment_ == o.attachment_ && clip_ == o.clip_ &&
  //        composite_ == o.composite_ && blend_mode_ == o.blend_mode_ &&
  //        origin_ == o.origin_ && repeat_x_ == o.repeat_x_ &&
  //        repeat_y_ == o.repeat_y_ && size_type_ == o.size_type_ &&
  //        size_length_ == o.size_length_ && type_ == o.type_;

}


}