#include "iplayer/player_control/player_control.h"

#include <assert.h>

#include "iplayer/log.h"

namespace ip {

PlayerControl::PlayerControl() : status_(Status::kStop) {}

void PlayerControl::Exit() {
  if (decoder_) {
    decoder_->Exit();
  }
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
    PlayNextTrack();
  } catch (const std::exception& e) {
    LOG("[E] %s", e.what());
  }
}

void PlayerControl::Pause() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (status_ != Status::kPlay) {
    return;
  }
  decoder_->Pause();
}

void PlayerControl::Next() {
  std::lock_guard<std::mutex> lock(mutex_);
  try {
    PlayNextTrack();
  } catch (const std::exception& e) {
    LOG("[E] %s", e.what());
  }
}

void PlayerControl::PlayNextTrack() {
  auto track = playlist_.SelectNextTrack();
  if (decoder_) {
    decoder_->Exit();
  }
  decoder_ = std::make_unique<Decoder>(track);
  LOG("[D] Playing ! Playing ! Playing !");
}

void PlayerControl::Unpause() {
  // private method so no synchronization
  if (status_ != Status::kPause) {
    return;
  }
  decoder_->Unpause();
  status_ = Status::kPlay;
}

void PlayerControl::AddTrack(const TrackLocation& track_location) {
  std::lock_guard<std::mutex> lock(mutex_);
  LOG("[D] new track added: '%s'", track_location.c_str());
  playlist_.AddTrack({track_location});
}

}  // namespace ip
