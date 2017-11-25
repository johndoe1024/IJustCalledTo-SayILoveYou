#include "iplayer/playlist.h"

namespace ip {

Playlist::Playlist() : next_track_(0) {}

void Playlist::AddTrack(const std::vector<TrackLocation>& tracks) {
  // TODO: optimize with rvalue references
  // std::move(std::begin(tracks), std::end(tracks),
  // std::back_inserter(playlist_));
  std::copy(std::cbegin(tracks), std::cend(tracks),
            std::back_inserter(playlist_));
}

TrackLocation Playlist::SelectNextTrack() {
  return playlist_.at(next_track_++);
}

TrackLocation Playlist::SelectPreviousTrack() {
  return playlist_.at(--next_track_ - 1);
}

TrackLocation Playlist::CurrentTrack() const {
  if (next_track_ == 0) {
    return {};
  }
  return playlist_.at(next_track_ - 1);
}

}  // namespace ip
