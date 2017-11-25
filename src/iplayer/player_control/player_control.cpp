#include "iplayer/player_control/player_control.h"

#include "iplayer/log.h"
#include "iplayer/track_location.h"

namespace ip {

PlayerControl::PlayerControl() {}

void PlayerControl::Shutdown() { player_.Shutdown(); }

void PlayerControl::Play() {
  std::lock_guard<std::mutex> lock(mutex_);
  LOG("[D] Play ! Play ! Play !\n");
}

void PlayerControl::AddTrack(TrackLocation track_location) {
  std::lock_guard<std::mutex> lock(mutex_);
  LOG("[D] new track added: '%s'\n", track_location.Uri().c_str());
}

}  // namespace ip
