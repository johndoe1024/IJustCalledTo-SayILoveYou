#include "iplayer/playlist.h"

#include <algorithm>
#include <unordered_set>

namespace ip {

Playlist::Playlist() : current_track_(0) {}

void Playlist::AddTrack(const std::vector<TrackLocation>& tracks) {
  std::copy(std::begin(tracks), std::end(tracks),
            std::back_inserter(playlist_));
}

void Playlist::RemoveTrack(const std::vector<TrackLocation>& tracks) {
  for (const auto& track : tracks) {
    playlist_.erase(
        std::remove(std::begin(playlist_), std::end(playlist_), track),
        std::end(playlist_));
  }
}

void Playlist::RemoveDuplicate() {
  std::unordered_set<TrackLocation> known;
  auto is_known = [&](const TrackLocation& track) {
    if (known.find(track) == std::end(known)) {
      known.insert(track);
      return false;
    }
    return true;
  };
  playlist_.erase(
      std::remove_if(std::begin(playlist_), std::end(playlist_), is_known),
      std::end(playlist_));
}

std::vector<TrackLocation> Playlist::GetTracks() const {
  std::vector<TrackLocation> tracks;
  std::copy(std::cbegin(playlist_), std::cend(playlist_),
            std::back_inserter(tracks));
  return tracks;
}

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
