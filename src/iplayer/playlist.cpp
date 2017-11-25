#include "iplayer/playlist.h"

namespace ip {

Playlist::Playlist() : current_track_(0) {}

void Playlist::AddTrack(const std::vector<TrackLocation>& tracks) {
  // TODO: optimize with rvalue references
  // std::move(std::begin(tracks), std::end(tracks),
  // std::back_inserter(playlist_));
  std::copy(std::cbegin(tracks), std::cend(tracks),
            std::back_inserter(playlist_));
}

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
