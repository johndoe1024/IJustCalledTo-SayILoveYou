#pragma once

#include "iplayer/track_location.h"

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
  //    virtual void SetEnabledRandom(bool enable) = 0;
  //    virtual void SetEnabledRepeat(bool enable) = 0;

  virtual void AddTrack(const TrackLocation& track_location) = 0;
  virtual void RemoveTrack(const TrackLocation& track_location) = 0;
  virtual void RemoveDuplicateTrack() = 0;
  virtual void ShowPlaylist() const = 0;
};

}  // namespace ip
