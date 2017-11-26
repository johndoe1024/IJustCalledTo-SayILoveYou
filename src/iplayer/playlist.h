#pragma once

#include <vector>
#include <deque>

#include "iplayer/track_location.h"

namespace ip {

class Playlist {
 public:
  using TrackId = uint32_t;
  enum class SeekWay { kBegin = 0, kCurrent };

  Playlist();

  void AddTrack(const std::vector<TrackLocation>& tracks);
  void RemoveTrack(const std::vector<TrackLocation>& tracks);
  void RemoveDuplicate();

  std::vector<TrackLocation> GetTracks() const;
  TrackLocation SeekTrack(int64_t pos, SeekWay offset_type);
  TrackLocation CurrentTrack() const;
  size_t Remaining() const;

 private:
  void RemoveTrack(std::vector<TrackId> track_ids);

  std::deque<TrackLocation> playlist_;
  TrackId current_track_;
};

}  // namespace ip
