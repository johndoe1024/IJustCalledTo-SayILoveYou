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
  auto locations = CreateTrackLocations(1000, 1);
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
  auto locations = CreateTrackLocations(10000, 3);
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
  return 0;
}
