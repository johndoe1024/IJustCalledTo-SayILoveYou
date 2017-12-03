#pragma once

#include "iplayer/i_player_control.h"

#include <atomic>
#include <mutex>

#include "iplayer/core.h"
#include "iplayer/i_decoder.h"
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

  void AddUri(const std::string& uri) override;
  void AddTrack(const std::vector<TrackLocation>& track_location) override;
  TrackInfo GetCurrentTrackInfo(std::chrono::seconds* elapsed) const override;
  void RemoveTrack(const TrackLocation& track_location) override;
  void RemoveDuplicateTrack() override;
  std::vector<TrackInfo> ShowPlaylist() const override;

 private:
  void Unpause();
  void StopAndSeekBegin();
  void SelectTrack(int64_t pos, TrackLocation* track_location);
  void PlayTrack(const TrackLocation& track_location);

  mutable std::mutex mutex_;
  Core* core_;
  Status status_;
  std::unique_ptr<IDecoder> decoder_;
  Playlist playlist_;
};

}  // namespace ip
