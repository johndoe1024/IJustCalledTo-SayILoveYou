#pragma once

#include "iplayer/i_player_control.h"

#include <mutex>
#include <atomic>

#include "iplayer/playlist.h"
#include "iplayer/player.h"

namespace ip {

class PlayerControl : public IPlayerControl {
 public:
  PlayerControl();

  void Play() override;
//  void Pause() override;
//  void Next() override;
//  void Previous() override;
//  void RemoveDuplicateTrack() override;
//  void SetEnabledRandom(bool enable) override;
//  void SetEnabledRepeat(bool enable) override;

  void AddTrack(TrackLocation track_location) override;
  void Shutdown() override;

  Playlist* playlist() const;

 private:
  std::mutex mutex_;
  bool shuffle_mode_;
  bool repeat_mode_;
  Player player_;
  Playlist playlist_;
};

}  // namespace imp
