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
  void RestartCurrentTrack() override;
  void SetRepeatTrackEnabled(bool enable) override;
  void SetRepeatPlaylistEnabled(bool enable) override;
  void SetRandomTrackEnabled(bool value) override;

  void AddTrack(const TrackLocation& track_location) override;
  void ShowTrack() const override;
  void RemoveTrack(const TrackLocation& track_location) override;
  void RemoveDuplicateTrack() override;
  void ShowPlaylist() const override;

 private:
  void Unpause();
  void StopAndSeekBegin();
  void SelectTrack(int64_t pos, TrackLocation* track_location);
  void PlayTrack(const TrackLocation& track_location);

  mutable std::mutex mutex_;
  Core* core_;
  Status status_;
  bool random_mode_;
  std::atomic<bool> repeat_playlist_;
  std::atomic<bool> repeat_track_;
  std::unique_ptr<Decoder> decoder_;
  Playlist playlist_;
};

}  // namespace ip
