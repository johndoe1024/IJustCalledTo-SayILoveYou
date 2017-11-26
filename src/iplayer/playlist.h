#pragma once

#include <vector>
#include <deque>

#include "iplayer/track_location.h"

namespace ip {

class Playlist {
 public:
  using TrackId = uint32_t;

  Playlist();

  void AddTrack(const std::vector<TrackLocation>& tracks);
  void RemoveTrack(const std::vector<TrackLocation>& tracks);
  void RemoveDuplicate();

  std::vector<TrackLocation> GetTracks() const;
  TrackLocation SelectTrack(int64_t relative_pos);
  TrackLocation SetTrack(TrackId id);
  TrackLocation CurrentTrack() const;

 private:
  void RemoveTrack(std::vector<TrackId> track_ids);

  std::deque<TrackLocation> playlist_;
  TrackId current_track_;
};

}  // namespace ip
