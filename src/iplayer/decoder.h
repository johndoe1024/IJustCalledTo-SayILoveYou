#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <future>

#include "iplayer/i_track_provider.h"
#include "iplayer/track_location.h"

namespace ip {

class Decoder {
  enum class Status { kThreadRunning, kThreadPaused, kThreadExit };

 public:
  using CompletionCb = std::function<void(const std::error_code&)>;

  Decoder(std::unique_ptr<ITrackProvider> provider, const TrackLocation& track,
          CompletionCb completion_cb);
  virtual ~Decoder();

  void Play(const TrackLocation& track);
  void Pause();
  void Unpause();
  std::chrono::seconds GetPlayedTime() const;

 private:
  void DecoderThread(std::unique_ptr<ITrackProvider> provider,
                     TrackLocation track, CompletionCb completion_cb);

  std::mutex pause_mutex_;
  std::atomic<bool> exit_decoder_thread_;
  std::atomic<std::chrono::seconds> played_time_;
  Status status_;
  std::future<void> decoder_future_;
};

}  // namespace ip
