#pragma once

#include "iplayer/track_location.h"

#include <atomic>
#include <thread>

namespace ip {

class Playlist;

class Player{

  public:
    Player();
    void Shutdown();

    void Play(const TrackLocation& location);
    void Pause();
    void Unpause();

  private:
    void PlayThread();

    Playlist* playlist_;
    std::atomic_bool stop_;
    std::thread play_thread_;
};

}  // namespace imp
