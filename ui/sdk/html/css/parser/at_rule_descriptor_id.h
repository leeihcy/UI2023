#ifndef _HTML_CSS_PARSER_CSSATRULEDESCRIPTORS_H_
#define _HTML_CSS_PARSER_CSSATRULEDESCRIPTORS_H_

#include <string>

namespace html {

enum class AtRuleDescriptorID {
  Invalid = 0,
  Variable = 1,           // "variable"
  AdditiveSymbols = 2,    // "additive-symbols"
  AscentOverride = 3,     // "ascent-override"
  BasePalette = 4,        // "base-palette"
  BaseUrl = 5,            // "base-url"
  DescentOverride = 6,    // "descent-override"
  Fallback = 7,           // "fallback"
  FontDisplay = 8,        // "font-display"
  FontFamily = 9,         // "font-family"
  FontFeatureSettings = 10,  // "font-feature-settings"
  FontStretch = 11,       // "font-stretch"
  FontStyle = 12,         // "font-style"
  FontVariant = 13,       // "font-variant"
  FontVariationSettings = 14,  // "font-variation-settings"
  FontWeight = 15,        // "font-weight"
  Hash = 16,              // "hash"
  Hostname = 17,          // "hostname"
  Inherits = 18,          // "inherits"
  InitialValue = 19,      // "initial-value"
  LineGapOverride = 20,   // "line-gap-override"
  Navigation = 21,        // "navigation"
  Negative = 22,          // "negative"
  OverrideColors = 23,    // "override-colors"
  Pad = 24,               // "pad"
  Pathname = 25,          // "pathname"
  Pattern = 26,           // "pattern"
  Port = 27,              // "port"
  Prefix = 28,            // "prefix"
  Protocol = 29,          // "protocol"
  Range = 30,             // "range"
  Result = 31,            // "result"
  Search = 32,            // "search"
  SizeAdjust = 33,        // "size-adjust"
  SpeakAs = 34,           // "speak-as"
  Src = 35,               // "src"
  Suffix = 36,            // "suffix"
  Symbols = 37,           // "symbols"
  Syntax = 38,            // "syntax"
  System = 39,            // "system"
  Types = 40,             // "types"
  UnicodeRange = 41,      // "unicode-range"
};

const int numAtRuleDescriptors = 42;
const int MaxAtRuleDescriptorNameLength = 24;

AtRuleDescriptorID AtRuleDescriptorNameToId(const std::u16string& name);
AtRuleDescriptorID AtRuleDescriptorIDNameToIdByHash(const char* name, unsigned int len);

}

#endif