#pragma once

#include <memory>
#include <system_error>
#include <vector>

#include "iplayer/i_track_io.h"
#include "iplayer/track_info.h"

// Abstraction for specific services (Deezer, Spotify, ...)

namespace ip {

class ITrackProvider {
 public:
  virtual ~ITrackProvider() {}
  virtual std::error_code List(const std::string& uri,
                               std::vector<TrackLocation>* locations) const = 0;
  virtual TrackInfo GetTrackInfo(const TrackLocation& track) = 0;
  virtual std::unique_ptr<ITrackIO> OpenTrack(const TrackLocation& track,
                                              std::error_code& ec) = 0;
};

using ITrackProviderPtr = std::unique_ptr<ITrackProvider>;

}  // namespace ip
