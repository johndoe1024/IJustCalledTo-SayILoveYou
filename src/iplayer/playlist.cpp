#include "iplayer/playlist.h"

#include <algorithm>

namespace ip {

Playlist::Playlist() : current_track_(0) {}

void Playlist::AddTrack(const std::vector<TrackLocation>& tracks) {
  // TODO: optimize with rvalue references
  const auto playlist_size = playlist_.size();
  for (size_t i = 0; i < tracks.size(); ++i) {
    const auto track_id = i + playlist_size;
    playlist_.push_back(tracks[i]);
    location_to_id_.emplace(tracks[i], track_id);
  }
}

void Playlist::RemoveTrack(const std::vector<TrackLocation>& tracks) {
  // resolved all ids to be removed with the unordered_map
  std::vector<TrackId> remove_ids;
  for (const auto& track : tracks) {
    auto range = location_to_id_.equal_range(track);
    for (auto it = range.first; it != range.second; ++it) {
      remove_ids.push_back(it->second);
    }
    // remove from the map aswell
    location_to_id_.erase(range.first, range.second);
  }
  RemoveTrack(remove_ids);
}

// BEWARE: keep in mind that the location_to_id_ map must also be updated !
void Playlist::RemoveTrack(std::vector<TrackId> track_ids) {
  std::sort(std::begin(track_ids), std::end(track_ids));

  // rebuild a vector without removed ids
  std::vector<TrackLocation> new_playlist;
  auto last_end = std::begin(playlist_);

  for (auto it = std::begin(track_ids); it != std::end(track_ids); ++it) {
    auto end = std::begin(playlist_) + *it;
    std::move(last_end, end, std::back_inserter(new_playlist));
    last_end = std::next(end, 1);
  }
  std::move(last_end, std::end(playlist_), std::back_inserter(new_playlist));
  std::swap(playlist_, new_playlist);

  // adjust current track by finding how many removed items had a lesser index
  auto it = std::lower_bound(std::cbegin(track_ids), std::cend(track_ids),
                             current_track_, std::less_equal<>());
  auto offset = std::distance(std::cbegin(track_ids), it);
  if (offset < current_track_) {
    current_track_ -= offset;
  } else {
    current_track_ = 0;
  }
}

std::vector<TrackLocation> Playlist::GetTracks() const { return playlist_; }

TrackLocation Playlist::AdvanceTrack(int64_t relative_pos) {
  int64_t pos = current_track_ + relative_pos;
  if (pos < 0) {
    current_track_ = 0;
  } else {
    current_track_ = static_cast<uint32_t>(pos);
  }
  return playlist_.at(current_track_);
}

TrackLocation Playlist::SetTrack(TrackId track_id) {
  current_track_ = track_id;
  return playlist_.at(current_track_);
}

TrackLocation Playlist::CurrentTrack() const {
  return playlist_.at(current_track_);
}

}  // namespace ip
