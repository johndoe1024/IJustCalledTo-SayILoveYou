#include "iplayer/playlist.h"

#include <assert.h>
#include <algorithm>

namespace ip {

Playlist::Playlist() : current_track_(0) {}

void Playlist::AddTrack(const std::vector<TrackLocation>& tracks) {
  std::transform(std::cbegin(tracks), std::cend(tracks),
                 std::back_inserter(playlist_),
                 [](const TrackLocation& loc) { return TrackInfo{loc}; });
}

void Playlist::SetTrackInfo(
    const std::unordered_map<TrackLocation, TrackInfo>& tracks) {
  for (auto it = std::begin(playlist_); it != std::end(playlist_); ++it) {
    auto found_it = tracks.find(it->Location());
    if (found_it != std::cend(tracks)) {
      *it = found_it->second;
    }
  }
}

void Playlist::RemoveTrack(const std::unordered_set<TrackLocation>& tracks) {
  auto pred = [&](const TrackInfo& item) {
    return tracks.find(item.Location()) != std::cend(tracks);
  };
  playlist_.erase(
      std::remove_if(std::begin(playlist_), std::end(playlist_), pred),
      std::end(playlist_));
}

void Playlist::RemoveDuplicate() {
  std::unordered_set<TrackLocation> known;
  auto is_known = [&](const TrackInfo& item) {
    if (known.find(item.Location()) == std::end(known)) {
      known.insert(item.Location());
      return false;
    }
    return true;
  };
  playlist_.erase(
      std::remove_if(std::begin(playlist_), std::end(playlist_), is_known),
      std::end(playlist_));
}

std::deque<TrackInfo> Playlist::GetTracks() const { return playlist_; }

TrackInfo Playlist::SeekTrack(int64_t pos, SeekWay offset_type) {
  if (offset_type == SeekWay::kBegin) {
    assert(pos >= 0);
    current_track_ = static_cast<uint32_t>(pos);
    return playlist_.at(current_track_);
  } else {
    // TrackOffsetType::kCurrent
    int64_t new_pos = current_track_ + pos;
    if (new_pos < 0) {
      current_track_ = 0;
    } else {
      current_track_ = static_cast<uint32_t>(new_pos);
    }
  }
  return playlist_.at(current_track_);
}

TrackInfo Playlist::CurrentTrack() const {
  return playlist_.at(current_track_);
}

size_t Playlist::Remaining() const {
  assert(current_track_ < playlist_.size());
  return playlist_.size() - 1 - current_track_;
}

}  // namespace ip
