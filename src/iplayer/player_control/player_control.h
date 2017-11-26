#pragma once

#include "iplayer/i_player_control.h"

#include <atomic>
#include <mutex>

#include "iplayer/core.h"
#include "iplayer/decoder.h"
#include "iplayer/playlist.h"

namespace ip {

class PlayerControl : public IPlayerControl {
  enum class Status { kStop, kPause, kPlay };

 public:
  PlayerControl(Core* core);
  void Exit() override;

  void Play() override;
  void Pause() override;
  void Stop() override;
  void Previous() override;
  void Next() override;

  //  void SetEnabledRandom(bool enable) override;
  //  void SetEnabledRepeat(bool enable) override;

  void AddTrack(const TrackLocation& track_location) override;
  void RemoveTrack(const TrackLocation& track_location) override;
  void RemoveDuplicateTrack() override;
  void ShowPlaylist() const override;

 private:
  void Unpause();
  void StopImpl();
  void AdvanceTrack(int64_t pos);

  mutable std::mutex mutex_;
  Core* core_;
  Status status_;
  bool shuffle_mode_;
  bool repeat_mode_;
  std::unique_ptr<Decoder> decoder_;
  Playlist playlist_;
};

}  // namespace ip
