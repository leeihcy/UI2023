#ifndef _HTML_PLATFOMR_GEOMETRY_LENGTH_H_
#define _HTML_PLATFOMR_GEOMETRY_LENGTH_H_

namespace html {

class Length {  
 public:
  enum class ValueRange { kAll, kNonNegative };

  // FIXME: This enum makes it hard to tell in general what values may be
  // appropriate for any given Length.
  enum Type : unsigned char {
    kAuto,
    kPercent,
    kFixed,
    kMinContent,
    kMaxContent,
    kMinIntrinsic,
    kFillAvailable,
    kFitContent,
    kCalculated,
    kExtendToZoom,
    kDeviceWidth,
    kDeviceHeight,
    kNone,    // only valid for max-width, max-height, or contain-intrinsic-size
    kContent  // only valid for flex-basis
  };

  Length() : int_value_(0), quirk_(false), type_(kAuto), is_float_(false) {}

  explicit Length(Length::Type t)
      : int_value_(0), quirk_(false), type_(t), is_float_(false) {
    // DCHECK_NE(t, kCalculated);
  }

  Length(int v, Length::Type t, bool q = false)
      : int_value_(v), quirk_(q), type_(t), is_float_(false) {
    // DCHECK_NE(t, kCalculated);
  }

private:
  union {
    int int_value_;
    float float_value_;
  };
  bool quirk_;
  unsigned char type_;
  bool is_float_;
};

}

#endif