#include "iplayer/player_control/player_control.h"

#include <assert.h>
#include <iostream>

#include "iplayer/log.h"

namespace ip {

PlayerControl::PlayerControl(Core* core)
    : core_(core), status_(Status::kStop) {}

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
  auto on_completion = [this](const std::error_code& ec) {
    if (ec) {
      LOG("[D] completion callback error: %s (%d)", ec.message().c_str(),
          ec.value());
      return;
    }
    core_->QueueExecution(std::bind(&PlayerControl::Next, this));
  };
  decoder_ =
      std::make_unique<Decoder>(track_location, std::move(on_completion));
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
