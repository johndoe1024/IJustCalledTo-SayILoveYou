#pragma once

#include <vector>

#include "iplayer/track_location.h"

namespace ip {

class Playlist {
 public:
  using TrackId = uint32_t;

  Playlist();

  void AddTrack(const std::vector<TrackLocation>& tracks);
  TrackLocation SelectNextTrack();
  TrackLocation SelectPreviousTrack();
  TrackLocation CurrentTrack() const;

 private:
  std::vector<TrackLocation> playlist_;
  TrackId next_track_;
};

}  // namespace imp
