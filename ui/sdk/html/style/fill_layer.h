#ifndef _HTML_STYLE_FILL_LAYER_H_
#define _HTML_STYLE_FILL_LAYER_H_

#include "html/platform/geometry/length.h"
#include "html/platform/geometry/length_size.h"
#include "html/style/computed_style_constants.h"


namespace html {

class FillLayer {
public:
  FillLayer(EFillLayerType, bool use_initial_values = false);
  ~FillLayer();

  FillLayer(const FillLayer& o);

  bool operator==(const FillLayer&) const;
  bool operator!=(const FillLayer& o) const { return !(*this == o); }
  bool LayerPropertiesEqual(const FillLayer& o) const;

private:
  FillLayer* next_ = nullptr;
  
  // Persistent<StyleImage> image_;

  Length position_x_;
  Length position_y_;

  LengthSize size_length_;

  unsigned attachment_ : 2;           // EFillAttachment
  unsigned clip_ : 2;                 // EFillBox
  unsigned origin_ : 2;               // EFillBox
  unsigned repeat_x_ : 3;             // EFillRepeat
  unsigned repeat_y_ : 3;             // EFillRepeat
  unsigned composite_ : 4;            // CompositeOperator
  unsigned size_type_ : 2;            // EFillSizeType
  unsigned blend_mode_ : 5;           // BlendMode
  unsigned background_x_origin_ : 2;  // BackgroundEdgeOrigin
  unsigned background_y_origin_ : 2;  // BackgroundEdgeOrigin

  unsigned image_set_ : 1;
  unsigned attachment_set_ : 1;
  unsigned clip_set_ : 1;
  unsigned origin_set_ : 1;
  unsigned repeat_x_set_ : 1;
  unsigned repeat_y_set_ : 1;
  unsigned pos_x_set_ : 1;
  unsigned pos_y_set_ : 1;
  unsigned background_x_origin_set_ : 1;
  unsigned background_y_origin_set_ : 1;
  unsigned composite_set_ : 1;
  unsigned blend_mode_set_ : 1;
};

}

#endif