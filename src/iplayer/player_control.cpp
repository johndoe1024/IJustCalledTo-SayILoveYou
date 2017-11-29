#include "iplayer/player_control.h"

#include <assert.h>
#include <iostream>

#include "iplayer/fs_track_provider.h"
#include "iplayer/log.h"

namespace ip {

PlayerControl::PlayerControl(Core* core)
    : core_(core),
      status_(Status::kStop),
      repeat_playlist_(false),
      repeat_track_(false) {}

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
  auto track = playlist_.SeekTrack(0, Playlist::SeekWay::kCurrent);
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
  if (playlist_.Remaining() == 0) {
    if (!repeat_playlist_) {
      StopAndSeekBegin();
      return;
    }
    // continue with first item in the playlist
    LOG("[D] repeat_playlist mode on, restarting playlist");
    auto info = playlist_.SeekTrack(0, Playlist::SeekWay::kBegin);
    PlayTrack(info.Location());
  } else {
    auto info = playlist_.SeekTrack(1, Playlist::SeekWay::kCurrent);
    PlayTrack(info.Location());
  }
}

void PlayerControl::Previous() {
  std::lock_guard<std::mutex> lock(mutex_);
  auto track = playlist_.SeekTrack(-1, Playlist::SeekWay::kCurrent);
  PlayTrack(track.Location());
}

void PlayerControl::RestartCurrentTrack() {
  std::lock_guard<std::mutex> lock(mutex_);
  try {
    auto track = playlist_.CurrentTrack();
    PlayTrack(track.Location());
  } catch (const std::exception& e) {
    LOG("[D] %s", e.what());
    // videolan plays the next one (which index became equal to current)
    StopAndSeekBegin();
  }
}

void PlayerControl::SetRepeatPlaylistEnabled(bool value) {
  std::lock_guard<std::mutex> lock(mutex_);
  repeat_playlist_ = value;
}

void PlayerControl::SetRepeatTrackEnabled(bool value) {
  std::lock_guard<std::mutex> lock(mutex_);
  repeat_track_ = value;
}

void PlayerControl::SetRandomTrackEnabled(bool value) {
  std::lock_guard<std::mutex> lock(mutex_);
  random_mode_ = value;
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
  playlist_.SeekTrack(0, Playlist::SeekWay::kBegin);
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
    if (repeat_track_) {
      core_->QueueExecution(
          std::bind(&PlayerControl::RestartCurrentTrack, this));
    } else {
      core_->QueueExecution(std::bind(&PlayerControl::Next, this));
    }
  };
  status_ = Status::kPlay;
  std::cout << "Playing " << track << std::endl;
  auto provider = std::make_unique<FsTrackProvider>();
  decoder_ = std::make_unique<Decoder>(std::move(provider), track,
                                       std::move(on_completion));
}

void PlayerControl::AddTrack(const TrackLocation& location) {
  std::lock_guard<std::mutex> lock(mutex_);
  std::cout << "Added " << location << std::endl;
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

void PlayerControl::ShowPlaylist() const {
  std::deque<TrackInfo> tracks;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    tracks = playlist_.GetTracks();
  }
  for (const auto& track : tracks) {
    std::cout << track.Location() << std::endl;
  }
}

}  // namespace ip
