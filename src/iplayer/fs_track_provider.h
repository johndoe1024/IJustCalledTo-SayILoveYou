#pragma once

#include "iplayer/i_track_provider.h"

namespace ip {

class FsTrackProvider : public ITrackProvider {
 public:
  TrackInfo GetTrackInfo(const TrackLocation& track) override;
  std::unique_ptr<ITrackIO> OpenTrack(const TrackLocation& track,
                                      std::error_code& ec) override;
};

}  // namespace ip
