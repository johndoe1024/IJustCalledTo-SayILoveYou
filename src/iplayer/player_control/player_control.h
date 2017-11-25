#pragma once

#include "iplayer/i_player_control.h"

#include <atomic>
#include <mutex>

#include "iplayer/player.h"
#include "iplayer/playlist.h"

namespace ip {

class PlayerControl : public IPlayerControl {
  enum class Status { kStop, kPause, kPlay };

 public:
  PlayerControl();
  void Exit() override;

  void Play() override;
  void Pause() override;
  // void Previous() override;
  void Next() override;

  //  void Previous() override;
  //  void RemoveDuplicateTrack() override;
  //  void SetEnabledRandom(bool enable) override;
  //  void SetEnabledRepeat(bool enable) override;

  void AddTrack(const TrackLocation& track_location) override;

 private:
  void Unpause();
  void PlayNextTrack();

  std::mutex mutex_;
  Status status_;
  bool shuffle_mode_;
  bool repeat_mode_;
  std::unique_ptr<Decoder> decoder_;
  Playlist playlist_;
};

}  // namespace imp
