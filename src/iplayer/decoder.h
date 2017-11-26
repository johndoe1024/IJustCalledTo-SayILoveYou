#pragma once

#include "iplayer/track_location.h"

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>

namespace ip {

class Decoder {
  enum class Status { kThreadRunning, kThreadPaused, kThreadExited };

 public:
  using CompletionCb = std::function<void(const std::error_code&)>;

  Decoder(const TrackLocation& track, CompletionCb completion_cb);
  virtual ~Decoder();
  void Exit();

  void Play(const TrackLocation& track);
  void Pause();
  void Unpause();

 private:
  void DecoderThread(TrackLocation track, CompletionCb completion_cb);

  std::mutex pause_mutex_;
  std::atomic<bool> exit_decoder_thread_;
  Status status_;
  std::thread decoder_thread_;
};

}  // namespace ip
