#pragma once

#include <vector>

#include "iplayer/track_info.h"
#include "iplayer/track_location.h"

// Provide control over player like a remote would do. Thread-safe and non
// blocking interface to avoid any ui freeze

namespace ip {

class IPlayerControl {
 public:
  virtual ~IPlayerControl() {}
  virtual void Exit() = 0;

  virtual void Play() = 0;
  virtual void Pause() = 0;
  virtual void Stop() = 0;
  virtual void Previous() = 0;
  virtual void Next() = 0;
  virtual void SetRepeatTrackEnabled(bool enable) = 0;
  virtual void SetRepeatPlaylistEnabled(bool enable) = 0;
  virtual void SetRandomTrackEnabled(bool value) = 0;

  virtual void AddUri(const std::string& uri) = 0;
  virtual void AddTrack(const std::vector<TrackLocation>& track_location) = 0;
  virtual TrackInfo GetCurrentTrackInfo(
      std::chrono::seconds* elapsed) const = 0;
  virtual void RemoveTrack(const TrackLocation& track_location) = 0;
  virtual void RemoveDuplicateTrack() = 0;
  virtual std::vector<TrackInfo> ShowPlaylist(
      size_t* current_track_index) const = 0;
};

}  // namespace ip
