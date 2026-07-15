#ifndef NET_BASE_TRANSPORT_INFO_H_
#define NET_BASE_TRANSPORT_INFO_H_

namespace net {
enum class TransportType {
  // The transport was established directly to a peer.
  kDirect,
  // The transport was established to a proxy of some kind.
  kProxied,
  // The transport was "established" to a cache entry.
  kCached,
  // Same as `kCached`, but the resource was initially loaded through a proxy.
  kCachedFromProxy,
};

struct TransportInfo {
  // The type of the transport.
  TransportType type = TransportType::kDirect;
};

}

#endif  // NET_BASE_TRANSPORT_INFO_H_
