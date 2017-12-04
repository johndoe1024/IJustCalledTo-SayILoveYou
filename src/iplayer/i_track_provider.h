#pragma once

#include <system_error>

#include "iplayer/i_track_io.h"
#include "iplayer/track_info.h"

// Abstraction for specific services (Deezer, Spotify, ...)

namespace ip {

class ITrackProvider {
 public:
  virtual ~ITrackProvider() {}
  virtual TrackInfo GetTrackInfo(const TrackLocation& track) = 0;
  virtual std::unique_ptr<ITrackIO> OpenTrack(const TrackLocation& track,
                                              std::error_code& ec) = 0;
};

}  // namespace ip
