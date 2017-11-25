#include "iplayer/player_control/player_control.h"

#include <assert.h>

#include "iplayer/log.h"

namespace ip {

PlayerControl::PlayerControl() : status_(Status::kStop) {}

void PlayerControl::Exit() {
  std::lock_guard<std::mutex> lock(mutex_);
  StopImpl();
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
    AdvanceTrack(0);
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
    AdvanceTrack(1);
  } catch (const std::exception& e) {
    LOG("[E] %s", e.what());
    StopImpl();
  }
}

void PlayerControl::Previous() {
  std::lock_guard<std::mutex> lock(mutex_);
  try {
    AdvanceTrack(-1);
  } catch (const std::exception& e) {
    LOG("[D] %s", e.what());
  }
}

void PlayerControl::AdvanceTrack(int64_t pos) {
  auto track_location = playlist_.AdvanceTrack(pos);
  if (decoder_) {
    decoder_->Exit();
  }
  decoder_ = std::make_unique<Decoder>(track_location);
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

void PlayerControl::StopImpl() {
  if (decoder_) {
    decoder_->Exit();
  }
  playlist_.SetTrack(0);
  status_ = Status::kStop;
}

void PlayerControl::AddTrack(const TrackLocation& track_location) {
  std::lock_guard<std::mutex> lock(mutex_);
  LOG("[D] new track added: '%s'", track_location.c_str());
  playlist_.AddTrack({track_location});
}

}  // namespace ip
