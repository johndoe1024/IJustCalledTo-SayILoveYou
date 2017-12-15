#pragma once

#include "iplayer/i_track_provider.h"

#include <mutex>

namespace ip {

class FsTrackProvider : public ITrackProvider {
 public:
  std::error_code List(const std::string& uri,
                       std::vector<TrackLocation>* locations) const override;
  TrackInfo GetTrackInfo(const TrackLocation& track) override;
  std::unique_ptr<ITrackIO> OpenTrack(const TrackLocation& track,
                                      std::error_code& ec) override;

 private:
  std::error_code ListDir(const std::string& dir,
                          std::vector<std::string>* files) const;

  mutable std::mutex mutex_;
};

}  // namespace ip
