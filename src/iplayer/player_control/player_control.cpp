#include "iplayer/player_control/player_control.h"

#include <assert.h>
#include <iostream>

#include "iplayer/log.h"

namespace ip {

PlayerControl::PlayerControl(Core* core)
    : core_(core),
      status_(Status::kStop),
      repeat_track_(false) {}

void PlayerControl::Exit() {
  std::lock_guard<std::mutex> lock(mutex_);
  StopImpl();
  core_->Stop();
}

// TODO: move try/catch into caller
void PlayerControl::Play() {
  std::lock_guard<std::mutex> lock(mutex_);
  try {
    if (status_ == Status::kPause) {
      Unpause();
      return;
    }
    if (status_ == Status::kPlay) {
      return;
    }
    auto track = playlist_.SelectTrack(0);
    PlayTrack(track);
  } catch (const std::exception& e) {
    LOG("[E] %s", e.what());
  }
}

void PlayerControl::Pause() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (status_ != Status::kPlay) {
    return;
  }
  assert(decoder_);
  decoder_->Pause();
}

void PlayerControl::Stop() {
  std::lock_guard<std::mutex> lock(mutex_);
  StopImpl();
}

void PlayerControl::Next() {
  std::lock_guard<std::mutex> lock(mutex_);
  try {
    auto track = playlist_.SelectTrack(1);
    PlayTrack(track);
  } catch (const std::exception& e) {
    LOG("[E] %s", e.what());
    StopImpl();
  }
}

void PlayerControl::Previous() {
  std::lock_guard<std::mutex> lock(mutex_);
  try {
    auto track = playlist_.SelectTrack(-1);
    PlayTrack(track);
  } catch (const std::exception& e) {
    LOG("[D] %s", e.what());
  }
}

// TODO: current_track_ should not exists, it should use the playlist instead.
// TODO: when active track is removed videolan stops and play the one that took
// place
void PlayerControl::Replay() {
  std::lock_guard<std::mutex> lock(mutex_);
  PlayTrack(current_track_);
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

void PlayerControl::StopImpl() {
  if (decoder_) {
    decoder_->Exit();
  }
  playlist_.SetTrack(0);
  status_ = Status::kStop;
}

void PlayerControl::PlayTrack(const TrackLocation& track) {
  if (decoder_) {
    decoder_->Exit();
  }

  // this handler will be called from decoder's thread context just before
  // returning, it mustn't call directly PlayerControl methods that's why every
  // handlers will queue execution in Core's thread context
  auto on_completion = [this](const std::error_code& ec) {
    if (ec) {
      LOG("[D] completion callback error: %s (%d)", ec.message().c_str(),
          ec.value());
      return;
    }
    if (repeat_track_) {
      core_->QueueExecution(std::bind(&PlayerControl::Replay, this));
    } else {
      core_->QueueExecution(std::bind(&PlayerControl::Next, this));
    }
  };
  status_ = Status::kPlay;
  decoder_ = std::make_unique<Decoder>(track, std::move(on_completion));
  current_track_ = track;
  LOG("[D] Playing ! Playing ! Playing !");
}

void PlayerControl::AddTrack(const TrackLocation& track_location) {
  std::lock_guard<std::mutex> lock(mutex_);
  LOG("[D] new track added: '%s'", track_location.c_str());
  playlist_.AddTrack({track_location});
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
  std::vector<TrackLocation> tracks;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    tracks = playlist_.GetTracks();
  }
  for (const auto& track : tracks) {
    std::cout << track << std::endl;
  }
}

}  // namespace ip
