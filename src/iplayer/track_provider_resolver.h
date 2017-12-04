#pragma once

#include <functional>
#include <map>
#include <mutex>

#include "iplayer/i_track_provider.h"
#include "iplayer/track_location.h"

namespace ip {

class TrackProviderResolver {
 public:
  using ProviderFactory = std::function<ITrackProviderPtr()>;
  ITrackProviderPtr Get(const TrackLocation& location) const;
  void Register(const std::string& scheme, ProviderFactory factory);

 private:
  mutable std::mutex mutex_;
  std::map<std::string, ProviderFactory> providers_;
};

}  // namespace ip
