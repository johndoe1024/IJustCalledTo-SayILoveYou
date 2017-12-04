#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <future>

#include "iplayer/i_track_provider.h"
#include "iplayer/track_location.h"

// this decoder update played time on a dedicated thread

namespace ip {

class Decoder {
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
  std::condition_variable pause_cv_;
  std::atomic<bool> paused_;
  std::atomic<bool> exit_decoder_thread_;
  std::atomic<std::chrono::seconds> played_time_;
  std::future<void> decoder_future_;
};

}  // namespace ip
