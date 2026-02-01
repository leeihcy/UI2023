#ifndef _HTML_CSS_CSSVARIABLEDATA_H_
#define _HTML_CSS_CSSVARIABLEDATA_H_

#include "html/base/atomic_string.h"
#include "html/base/memory.h"
#include <string>

namespace html {
class CSSParserToken;

class CSSVariableData {
public:
 CSSVariableData()
      : length_(0),
        is_animation_tainted_(false),
        is_attr_tainted_(false),
        needs_variable_resolution_(false),
        is_8bit_(true),
        has_font_units_(false),
        has_root_font_units_(false),
        has_line_height_units_(false),
        has_dashed_functions_(false) {}

  CSSVariableData(const std::u16string&& text,
                  bool is_animation_tainted,
                  bool is_attr_tainted,
                  bool needs_variable_resolution,
                  bool has_font_units,
                  bool has_root_font_units,
                  bool has_line_height_units,
                  bool has_dashed_functions);

  static const int kMaxVariableBytes = 2097152;

  static void ExtractFeatures(const CSSParserToken &token, bool &has_font_units,
                              bool &has_root_font_units,
                              bool &has_line_height_units,
                              bool &has_dashed_functions);

  static A<CSSVariableData> Create(const std::u16string&& original_text,
                                 bool is_animation_tainted,
                                 bool is_attr_tainted,
                                 bool needs_variable_resolution,
                                 bool has_font_units,
                                 bool has_root_font_units,
                                 bool has_line_height_units,
                                 bool has_dashed_functions) {
    if (original_text.length() > kMaxVariableBytes) {
      // This should have been blocked off during variable substitution.
      abort();
    }

    return A<CSSVariableData>::make_new(std::move(original_text), is_animation_tainted, is_attr_tainted,
        needs_variable_resolution, has_font_units, has_root_font_units,
        has_line_height_units, has_dashed_functions);
  }

  AtomicString  GetOriginalText() { return m_original_text; }
  
private:
  const unsigned length_ : 22;
  const unsigned is_animation_tainted_ : 1;       // bool.
  const unsigned is_attr_tainted_ : 1;            // bool.
  const unsigned needs_variable_resolution_ : 1;  // bool.
  const unsigned is_8bit_ : 1;                    // bool.
  unsigned has_font_units_ : 1;                   // bool.
  unsigned has_root_font_units_ : 1;              // bool.
  unsigned has_line_height_units_ : 1;            // bool.
  unsigned has_dashed_functions_ : 1;             // bool.
  unsigned /* unused_ */ : 2;

  AtomicString m_original_text;
};
}

#endif