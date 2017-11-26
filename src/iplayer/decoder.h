#pragma once

#include "iplayer/track_location.h"

#include <atomic>
#include <mutex>
#include <thread>

namespace ip {

class Decoder {
  enum class Status { kThreadRunning, kThreadPaused, kThreadExited };

 public:
  Decoder(const TrackLocation& track);
  virtual ~Decoder();
  void Exit();

  void Play(const TrackLocation& track);
  void Pause();
  void Unpause();

 private:
  void DecoderThread(TrackLocation track);

  std::mutex pause_mutex_;
  std::atomic<bool> exit_decoder_thread_;
  Status status_;
  std::thread decoder_thread_;
};

}  // namespace ip
