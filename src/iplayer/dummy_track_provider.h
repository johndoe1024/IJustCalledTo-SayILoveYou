#pragma once

#include "iplayer/i_track_provider.h"

namespace ip {

class DummyTrackProvider : public ITrackProvider {
 public:
  std::error_code List(const std::string& uri,
                       std::vector<TrackLocation>* locations) const override;
  TrackInfo GetTrackInfo(const TrackLocation& track) override;
  std::unique_ptr<ITrackIO> OpenTrack(const TrackLocation& track,
                                      std::error_code& ec) override;
};

}  // namespace ip
