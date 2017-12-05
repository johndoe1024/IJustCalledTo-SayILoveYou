#include "iplayer/track_provider_resolver.h"

#include <assert.h>
#include <algorithm>

#include "iplayer/dummy_track_provider.h"

namespace ip {

ITrackProviderPtr TrackProviderResolver::Get(const TrackLocation& track) const {
  // lookup for a scheme ending with '://'
  const auto scheme_pos = track.find("://");
  if (scheme_pos == std::string::npos) {
    // use dummy provider as default when no scheme is specified
    return std::make_unique<DummyTrackProvider>();
  }

  // look in registered factories
  std::lock_guard<std::mutex> lock(mutex_);
  auto scheme = track.substr(0, scheme_pos);
  auto factory_it = providers_.find(scheme);
  if (factory_it == std::cend(providers_)) {
    return nullptr;
  }
  return factory_it->second();
}

void TrackProviderResolver::Register(const std::string& scheme,
                                     ProviderFactory factory) {
  const auto separator = scheme.find("://");
  assert(separator != std::string::npos);

  std::lock_guard<std::mutex> lock(mutex_);
  providers_[scheme.substr(0, separator)] = factory;
}

}  // namespace ip
