#pragma once

#include <deque>
#include <functional>
#include <random>
#include <system_error>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "iplayer/track_info.h"

namespace ip {

class Playlist {
 public:
  using TrackId = uint32_t;
  enum class SeekWay { kBegin = 0, kCurrent };

  Playlist();

  void AddTrack(const std::vector<TrackLocation>& tracks);
  void SetTrackInfo(const std::unordered_map<TrackLocation, TrackInfo>& tracks);
  void RemoveTrack(const std::unordered_set<TrackLocation>& tracks);
  void RemoveDuplicate();

  std::deque<TrackInfo> GetTracks() const;
  std::error_code SeekTrack(int64_t pos, SeekWay offset_type, TrackInfo* track);
  TrackInfo CurrentTrack() const;
  size_t Remaining() const;

  void SetRepeatPlaylistEnabled(bool value);
  void SetRepeatTrackEnabled(bool value);
  void SetModeRandom(bool value);
  bool IsModeRandom() const;

 private:
  void RemoveTrack(std::vector<TrackId> track_ids);
  void Shuffle();
  void RemoveIf(std::function<bool(const TrackInfo&)> predicate);

  std::deque<TrackInfo> playlist_;
  TrackId current_track_;
  bool repeat_playlist_;
  bool repeat_track_;
  bool random_mode_;
  std::vector<TrackId> real_to_random_;
  std::random_device dev_random_;
  std::mt19937 prng_;
};

}  // namespace ip
