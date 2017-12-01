#include "iplayer/playlist.h"

#include <algorithm>

#include <assert.h>

namespace ip {

std::vector<TrackLocation> CreateTrackLocations(size_t count, size_t repeat) {
  std::vector<TrackLocation> locations;
  for (size_t r = 0; r < repeat; ++r) {
    for (size_t i = 0; i < count; ++i) {
      locations.push_back("foo_" + std::to_string(i));
    }
  }
  return locations;
}

bool CaseAddTrack() {
  Playlist playlist;
  auto locations = CreateTrackLocations(300000, 1);
  playlist.AddTrack(locations);
  playlist.AddTrack(locations);
  if (playlist.GetTracks().size() != locations.size() * 2) {
    return false;
  }
  return true;
}

bool CaseRemoveTrack() {
  Playlist playlist;
  auto locations = CreateTrackLocations(100000, 3);
  playlist.AddTrack(locations);

  std::unordered_set<TrackLocation> to_rm{"foo_0", "foo_1000"};
  playlist.RemoveTrack(to_rm);

  // try to find the removed tracks
  auto pred = [&](const TrackInfo& info) {
    return to_rm.find(info.Location()) != std::cend(to_rm);
  };
  auto content = playlist.GetTracks();
  auto found_it = std::find_if(std::cbegin(content), std::cend(content), pred);
  if (found_it != std::cend(content)) {
    return false;
  }
  return true;
}

bool CaseRemoveDuplicates() {
  Playlist playlist;
  auto locations = CreateTrackLocations(100000, 3);
  playlist.AddTrack(locations);

  playlist.RemoveDuplicate();

  // try to find the removed tracks
  auto content = playlist.GetTracks();
  auto pred = [&](const TrackInfo& info) { return info.Location() == "foo_0"; };
  auto count = std::count_if(std::cbegin(content), std::cend(content), pred);
  if (count > 1) {
    return false;
  }
  return true;
}

bool CaseRepeatTrack() {
  Playlist playlist;
  auto locations = CreateTrackLocations(100, 3);
  playlist.AddTrack(locations);

  TrackInfo first_track;
  auto ec = playlist.SeekTrack(1, Playlist::SeekWay::kCurrent, &first_track);
  if (ec) {
    return false;
  }

  TrackInfo track;
  ec = playlist.SeekTrack(1, Playlist::SeekWay::kCurrent, &track);
  if (ec) {
    return false;
  }

  if (first_track.Location() == track.Location()) {
    return false;
  }

  playlist.SetRepeatTrackEnabled(true);

  TrackInfo repeat_track;
  ec = playlist.SeekTrack(1, Playlist::SeekWay::kCurrent, &repeat_track);
  if (ec) {
    return false;
  }

  if (repeat_track.Location() != track.Location()) {
    return false;
  }
  return true;
}

bool CaseRepeatPlaylist() {
  std::error_code ec;
  Playlist playlist;
  auto locations = CreateTrackLocations(10, 1);
  playlist.AddTrack(locations);

  TrackInfo track;
  while (!ec) {
    ec = playlist.SeekTrack(1, Playlist::SeekWay::kCurrent, &track);
  }
  if (ec != std::errc::no_such_file_or_directory) {
    return false;
  }

  playlist.SetRepeatPlaylistEnabled(true);

  for (size_t i = 0; i < 5; ++i) {
    for (const auto& location : locations) {
      if (playlist.CurrentTrack().Location() != location) {
        return false;
      }
      ec = playlist.SeekTrack(1, Playlist::SeekWay::kCurrent, &track);
      if (ec) {
        return false;
      }
    }
  }
  return true;
}

bool CaseRandomPlay() {
  std::error_code ec;
  Playlist playlist;
  auto locations = CreateTrackLocations(10, 1);
  playlist.AddTrack(locations);

  playlist.SetModeRandom(true);

  std::vector<TrackLocation> play_order;
  TrackInfo track = playlist.CurrentTrack();
  for (size_t i = 0; i < locations.size() - 1; ++i) {
    play_order.push_back(track.Location());

    ec = playlist.SeekTrack(1, Playlist::SeekWay::kCurrent, &track);
    if (ec) {
      return false;
    }
  }

  auto playlist_order = playlist.GetTracks();
  auto pred = [](const TrackLocation& loc, const TrackInfo& track) {
    return loc == track.Location();
  };
  return std::equal(std::cbegin(play_order), std::cend(play_order),
                    std::cbegin(playlist_order), pred) == false;
}

}  // namespace ip

int main() {
  if (!ip::CaseAddTrack()) {
    return 1;
  }
  if (!ip::CaseRemoveTrack()) {
    return 1;
  }
  if (!ip::CaseRemoveDuplicates()) {
    return 1;
  }
  if (!ip::CaseRepeatTrack()) {
    return 1;
  }
  if (!ip::CaseRepeatPlaylist()) {
    return 1;
  }
  if (!ip::CaseRandomPlay()) {
    return 1;
  }
  return 0;
}
