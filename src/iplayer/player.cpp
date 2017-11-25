#include "iplayer/player.h"

#include "iplayer/log.h"

namespace ip {

Player::Player() : stop_(false), play_thread_([&]() { PlayThread(); }) {}

void Player::Shutdown() {
  stop_ = true;
  play_thread_.join();
}

void Player::Play(const TrackLocation& location) { TRACE(); }

void Player::Pause() { TRACE(); }

void Player::Unpause() { TRACE(); }

void Player::PlayThread() {
  TRACE();

  while (true) {
    if (stop_) {
      LOG("[D] Exiting\n");
      return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }
}

}  // namespace ip
