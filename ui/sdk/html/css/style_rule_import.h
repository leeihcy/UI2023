#ifndef _HTML_CSS_STYLERULE_IMPORT_H_
#define _HTML_CSS_STYLERULE_IMPORT_H_

#include "html/css/style_rule.h"
#include <assert.h>

namespace html {
class StyleSheetContents;

class StyleRuleImport final : public StyleRuleBase {
public:
  StyleRuleImport() : StyleRuleBase(StyleRuleBase::kImport) {}

  StyleSheetContents* ParentStyleSheet() const {
    return parent_style_sheet_;
  }
  void SetParentStyleSheet(StyleSheetContents* sheet) {
    assert(sheet);
    parent_style_sheet_ = sheet;
  }
  void ClearParentStyleSheet() { parent_style_sheet_ = nullptr; }
  void RequestStyleSheet() { 
    assert(false);
  }
private:
  StyleSheetContents* parent_style_sheet_;
};

template<>
struct DowncastTraits<StyleRuleImport> {
  static bool AllowFrom(const StyleRuleBase& from) {
    return from.GetType() == StyleRuleBase::kImport;
  }
};

}

#endif