#pragma once

#include <vector>

#include "iplayer/track_location.h"

namespace ip {

class Playlist {
 public:
  using TrackId = uint32_t;

  Playlist();

  void AddTrack(const std::vector<TrackLocation>& tracks);
  TrackLocation AdvanceTrack(int64_t relative_pos);
  TrackLocation SetTrack(TrackId id);
  TrackLocation CurrentTrack() const;

 private:
  std::vector<TrackLocation> playlist_;
  TrackId current_track_;
};

}  // namespace imp
