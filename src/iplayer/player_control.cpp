#include "iplayer/player_control.h"

#include <assert.h>

#include "iplayer/fs_track_provider.h"
#include "iplayer/log.h"

namespace ip {

PlayerControl::PlayerControl(Core* core)
    : core_(core), status_(Status::kStop) {}

void PlayerControl::Exit() {
  std::lock_guard<std::mutex> lock(mutex_);
  StopAndSeekBegin();
  core_->Stop();
}

void PlayerControl::Play() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (status_ == Status::kPause) {
    Unpause();
    return;
  }
  if (status_ == Status::kPlay) {
    return;
  }
  TrackInfo track;
  if (!playlist_.SeekTrack(0, Playlist::SeekWay::kCurrent, &track)) {
    StopAndSeekBegin();
    return;
  }
  PlayTrack(track.Location());
}

void PlayerControl::Pause() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (status_ != Status::kPlay) {
    return;
  }
  if (decoder_) {
    decoder_->Pause();
  }
}

void PlayerControl::Stop() {
  std::lock_guard<std::mutex> lock(mutex_);
  StopAndSeekBegin();
}

void PlayerControl::Next() {
  std::lock_guard<std::mutex> lock(mutex_);
  TrackInfo track;
  if(!playlist_.SeekTrack(1, Playlist::SeekWay::kCurrent, &track)) {
    StopAndSeekBegin();
    return;
  }
  PlayTrack(track.Location());
}

void PlayerControl::Previous() {
  std::lock_guard<std::mutex> lock(mutex_);
  TrackInfo track;
  if(!playlist_.SeekTrack(-1, Playlist::SeekWay::kCurrent, &track)) {
    StopAndSeekBegin();
    return;
  }
  PlayTrack(track.Location());
}

void PlayerControl::RestartCurrentTrack() {
  std::lock_guard<std::mutex> lock(mutex_);
  TrackInfo track;
  if (!playlist_.SeekTrack(1, Playlist::SeekWay::kCurrent, &track)) {
    StopAndSeekBegin();
    return;
  }
  PlayTrack(track.Location());
}

void PlayerControl::SetRepeatPlaylistEnabled(bool value) {
  std::lock_guard<std::mutex> lock(mutex_);
  playlist_.SetRepeatPlaylistEnabled(value);
}

void PlayerControl::SetRepeatTrackEnabled(bool value) {
  std::lock_guard<std::mutex> lock(mutex_);
  playlist_.SetRepeatTrackEnabled(value);
}

void PlayerControl::SetRandomTrackEnabled(bool value) {
  std::lock_guard<std::mutex> lock(mutex_);
  playlist_.SetModeRandom(value);
}

void PlayerControl::Unpause() {
  // private method so no synchronization
  if (status_ != Status::kPause) {
    return;
  }
  decoder_->Unpause();
  status_ = Status::kPlay;
}

void PlayerControl::StopAndSeekBegin() {
  decoder_.reset();
  playlist_.SeekTrack(0, Playlist::SeekWay::kBegin, nullptr);
  status_ = Status::kStop;
}

void PlayerControl::PlayTrack(const TrackLocation& track) {
  // this handler will be called from decoder's thread context just before
  // returning, it mustn't call directly PlayerControl methods
  auto on_completion = [this](const std::error_code& ec) {
    if (ec) {
      LOG("[D] completion callback error: %s (%d)", ec.message().c_str(),
          ec.value());
      return;
    }
    core_->QueueExecution(std::bind(&PlayerControl::Next, this));
  };
  status_ = Status::kPlay;
  auto provider = std::make_unique<FsTrackProvider>();
  decoder_ = std::make_unique<Decoder>(std::move(provider), track,
                                       std::move(on_completion));
}

void PlayerControl::AddTrack(const TrackLocation& location) {
  std::lock_guard<std::mutex> lock(mutex_);
  playlist_.AddTrack({location});

  // should be done async (with a lock)
  auto provider = std::make_unique<FsTrackProvider>();
  auto track_info = provider->GetTrackInfo(location);
  playlist_.SetTrackInfo({{location, std::move(track_info)}});
}

TrackInfo PlayerControl::GetCurrentTrackInfo(
    std::chrono::seconds* elapsed) const {
  std::lock_guard<std::mutex> lock(mutex_);
  if (elapsed && decoder_) {
    *elapsed = decoder_->GetPlayedTime();
  }
  return playlist_.CurrentTrack();
}

// TODO: Playlist::RemoveTrack could notify if current track has been deleted
// so PlayerControl::RemoveTrack could call StopImpl to mimic videolan
void PlayerControl::RemoveTrack(const TrackLocation& track_location) {
  std::lock_guard<std::mutex> lock(mutex_);
  playlist_.RemoveTrack({track_location});
}

void PlayerControl::RemoveDuplicateTrack() {
  std::lock_guard<std::mutex> lock(mutex_);
  playlist_.RemoveDuplicate();
}

std::deque<TrackInfo> PlayerControl::ShowPlaylist() const {
  std::deque<TrackInfo> tracks;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    tracks = playlist_.GetTracks();
  }
  return tracks;
}

}  // namespace ip
