#include "url/url_util.h"

#include "base/strings/string_util.h"
#include "url/url_constants.h"
#include "url/url_parse_internal.h"

#include <vector>

namespace url {

// A pair for representing a standard scheme name and the SchemeType for it.
struct SchemeWithType {
  std::string scheme;
  SchemeType type;
};

// A pair for representing a scheme and a custom protocol handler for it.
//
// This pair of strings must be normalized protocol handler parameters as
// described in the Custom Handler specification.
// https://html.spec.whatwg.org/multipage/system-state.html#normalize-protocol-handler-parameters
struct SchemeWithHandler {
  std::string scheme;
  std::string handler;
};


// List of currently registered schemes and associated properties.
struct SchemeRegistry {
  // Standard format schemes (see header for details).
  std::vector<SchemeWithType> standard_schemes = {
      {kHttpsScheme, SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION},
      {kHttpScheme, SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION},
      // Yes, file URLs can have a hostname, so file URLs should be handled as
      // "standard". File URLs never have a port as specified by the SchemeType
      // field.  Unlike other SCHEME_WITH_HOST schemes, the 'host' in a file
      // URL may be empty, a behavior which is special-cased during
      // canonicalization.
      {kFileScheme, SCHEME_WITH_HOST},
      {kFtpScheme, SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION},
      {kWssScheme,
       SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION},  // WebSocket secure.
      {kWsScheme, SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION},  // WebSocket.
      {kFileSystemScheme, SCHEME_WITHOUT_AUTHORITY},
  };

  // Schemes that are allowed for referrers.
  //
  // WARNING: Adding (1) a non-"standard" scheme or (2) a scheme whose URLs have
  // opaque origins could lead to surprising behavior in some of the referrer
  // generation logic. In order to avoid surprises, be sure to have adequate
  // test coverage in each of the multiple code locations that compute
  // referrers.
  std::vector<SchemeWithType> referrer_schemes = {
      {kHttpsScheme, SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION},
      {kHttpScheme, SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION},
  };

  // Schemes that do not trigger mixed content warning.
  std::vector<std::string> secure_schemes = {
      kHttpsScheme,
      kWssScheme,
      kDataScheme,
      kAboutScheme,
  };

  // Schemes that normal pages cannot link to or access (i.e., with the same
  // security rules as those applied to "file" URLs).
  std::vector<std::string> local_schemes = {
      kFileScheme,
  };

  // Schemes that cause pages loaded with them to not have access to pages
  // loaded with any other URL scheme.
  std::vector<std::string> no_access_schemes = {
      kAboutScheme,
      kJavaScriptScheme,
      kDataScheme,
  };

  // Schemes that can be sent CORS requests.
  std::vector<std::string> cors_enabled_schemes = {
      kHttpsScheme,
      kHttpScheme,
      kDataScheme,
  };

  // Schemes that can be used by web to store data (local storage, etc).
  std::vector<std::string> web_storage_schemes = {
      kHttpsScheme, kHttpScheme, kFileScheme, kFtpScheme, kWssScheme, kWsScheme,
  };

  // Schemes that can bypass the Content-Security-Policy (CSP) checks.
  std::vector<std::string> csp_bypassing_schemes = {};

  // Schemes that are strictly empty documents, allowing them to commit
  // synchronously.
  std::vector<std::string> empty_document_schemes = {
      kAboutScheme,
  };

  // Non-special schemes that should be treated as opaque path URLs for
  // compatibility reasons.
  std::vector<std::string> opaque_non_special_schemes = {
      // See https://crrev.com/c/5465607 for the reason.
      kAndroidScheme,
      // Temporarily opted-out. See https://crrev.com/c/5569365.
      kDrivefsScheme,
      // Temporarily opted-out. See https://crrev.com/c/5568919.
      kChromeosSteamScheme,
      kSteamScheme,
      // Temporarily opted-out. See https://crrev.com/c/5578066.
      kMaterializedViewScheme,
  };

  // Schemes with a predefined default custom handler.
  std::vector<SchemeWithHandler> predefined_handler_schemes;

  bool allow_non_standard_schemes = false;
};

const SchemeRegistry& GetSchemeRegistry() {
  static SchemeRegistry s;
  return s;
}

// Pass this enum through for methods which would like to know if whitespace
// removal is necessary.
enum WhitespaceRemovalPolicy {
  REMOVE_WHITESPACE,
  DO_NOT_REMOVE_WHITESPACE,
};

// Given a string and a range inside the string, compares it to the given
// lower-case |compare_to| buffer.
template <typename CHAR>
inline bool DoCompareSchemeComponent(std::basic_string_view<CHAR> spec,
                                     const Component& component,
                                     const char* compare_to) {
  if (component.is_empty())
    return compare_to[0] == 0;  // When component is empty, match empty scheme.
  
  return base::EqualsCaseInsensitiveASCII(component.AsViewOn(spec), compare_to);
}


// Returns true and sets |type| to the SchemeType of the given scheme
// identified by |scheme| within |spec| if in |schemes|.
template <typename CHAR>
bool DoIsInSchemes(std::optional<std::basic_string_view<CHAR>> input,
                   SchemeType* type,
                   const std::vector<SchemeWithType>& schemes) {
  if (!input.has_value() || input->empty()) {
    return false;  // Empty or invalid schemes are non-standard.
  }

  auto input_value = input.value();

  for (const SchemeWithType& scheme_with_type : schemes) {
    if (base::EqualsCaseInsensitiveASCII(input_value,
                                         scheme_with_type.scheme)) {
      *type = scheme_with_type.type;
      return true;
    }
  }
  return false;
}

template <typename CHAR>
bool DoIsStandard(std::optional<std::basic_string_view<CHAR>> input,
                  SchemeType* type) {
  return DoIsInSchemes(input, type, GetSchemeRegistry().standard_schemes);
}

template <typename CHAR>
bool DoIsOpaqueNonSpecial(std::basic_string_view<CHAR> spec,
                          const Component& scheme) {
  if (scheme.is_empty()) {
    return false;
  }
  auto scheme_view = scheme.AsViewOn(spec);
  for (const std::string& s : GetSchemeRegistry().opaque_non_special_schemes) {
    if (base::EqualsCaseInsensitiveASCII(scheme_view, s)) {
      return true;
    }
  }
  return false;
}


template <typename CHAR>
bool DoCanonicalize(std::basic_string_view<CHAR> spec,
                    bool trim_path_end,
                    WhitespaceRemovalPolicy whitespace_policy,
                    CharsetConverter* charset_converter,
                    CanonOutput* output,
                    Parsed* output_parsed) {
  Component scheme;
  if (!ExtractScheme(spec, &scheme)) {
    return false;
  }
  // This is the parsed version of the input URL, we have to canonicalize it
  // before storing it in our object.
  bool success;
  SchemeType scheme_type = SCHEME_WITH_HOST_PORT_AND_USER_INFORMATION;
  if (DoCompareSchemeComponent(spec, scheme, url::kFileScheme)) {
    // File URLs are special.
    success = CanonicalizeFileUrl(spec, ParseFileUrl(spec), charset_converter,
                                  output, output_parsed);
  } else if (DoCompareSchemeComponent(spec, scheme, url::kFileSystemScheme)) {
    // Filesystem URLs are special.
    success =
        CanonicalizeFileSystemUrl(spec, ParseFileSystemUrl(spec),
                                  charset_converter, output, output_parsed);

  } else if (DoIsStandard(std::optional(scheme.AsViewOn(spec)), &scheme_type)) {
    // All "normal" URLs.
    success = CanonicalizeStandardUrl(spec, ParseStandardUrl(spec), scheme_type,
                                      charset_converter, output, output_parsed);

  } else {
    // Non-special scheme URLs like data:, mailto: and javascript:.
    if (!DoIsOpaqueNonSpecial(spec, scheme)) {
      success = CanonicalizeNonSpecialUrl(
          spec, ParseNonSpecialUrlInternal(spec, trim_path_end),
          charset_converter, *output, *output_parsed);
    } else {
      success = CanonicalizePathUrl(spec, ParsePathUrl(spec, trim_path_end),
                                    output, output_parsed);
    }
  }
  return success;
}

bool Canonicalize(std::string_view spec,
                  bool trim_path_end,
                  CharsetConverter* charset_converter,
                  CanonOutput* output,
                  Parsed* output_parsed)
{
    return DoCanonicalize(spec, trim_path_end, REMOVE_WHITESPACE,
                        charset_converter, output, output_parsed);
}

bool Canonicalize(std::u16string_view spec,
                  bool trim_path_end,
                  CharsetConverter* charset_converter,
                  CanonOutput* output,
                  Parsed* output_parsed) {
  return DoCanonicalize(spec, trim_path_end, REMOVE_WHITESPACE,
                        charset_converter, output, output_parsed);
}


bool CompareSchemeComponent(std::string_view spec,
                            const Component& component,
                            const char* compare_to) {
  return DoCompareSchemeComponent(spec, component, compare_to);
}

bool CompareSchemeComponent(std::u16string_view spec,
                            const Component& component,
                            const char* compare_to) {
  return DoCompareSchemeComponent(spec, component, compare_to);
}


bool IsStandard(const char* spec, const Component& scheme) {
  SchemeType unused_scheme_type;
  // SAFETY: It's unsafe. Do not use this function.
  return DoIsStandard(scheme.maybe_as_string_view_on(spec),
                      &unused_scheme_type);
}

bool IsStandard(std::optional<std::string_view> scheme) {
  SchemeType unused_scheme_type;
  return DoIsStandard(scheme, &unused_scheme_type);
}

bool IsStandardScheme(std::string_view scheme) {
  return IsStandard(scheme);
}

bool IsStandard(std::optional<std::u16string_view> scheme) {
  SchemeType unused_scheme_type;
  return DoIsStandard(scheme, &unused_scheme_type);
}
}