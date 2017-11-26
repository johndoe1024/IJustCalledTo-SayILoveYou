#pragma once

#include <vector>
#include <unordered_map>

#include "iplayer/track_location.h"

namespace ip {

class Playlist {
 public:
  using TrackId = uint32_t;

  Playlist();

  void AddTrack(const std::vector<TrackLocation>& tracks);
  void RemoveTrack(const std::vector<TrackLocation>& tracks);

  std::vector<TrackLocation> GetTracks() const;
  TrackLocation AdvanceTrack(int64_t relative_pos);
  TrackLocation SetTrack(TrackId id);
  TrackLocation CurrentTrack() const;

 private:
  void RemoveTrack(std::vector<TrackId> track_ids);

  std::vector<TrackLocation> playlist_;
  std::unordered_multimap<TrackLocation, TrackId> location_to_id_;
  TrackId current_track_;
};

}  // namespace ip
