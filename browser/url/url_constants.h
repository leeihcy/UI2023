#ifndef URL_URL_CONSTANTS_H_
#define URL_URL_CONSTANTS_H_

#include <stddef.h>

namespace url {

inline constexpr char kHttpScheme[] = "http";
inline constexpr char16_t kHttpScheme16[] = u"http";
inline constexpr char kHttpsScheme[] = "https";
inline constexpr char16_t kHttpsScheme16[] = u"https";

inline constexpr char kFtpScheme[] = "ftp";
inline constexpr char16_t kFtpScheme16[] = u"ftp";

inline constexpr char kWsScheme[] = "ws";
inline constexpr char16_t kWsScheme16[] = u"ws";
inline constexpr char kWssScheme[] = "wss";
inline constexpr char16_t kWssScheme16[] = u"wss";

}

#endif