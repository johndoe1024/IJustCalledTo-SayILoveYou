#pragma once

#include "iplayer/i_decoder.h"

#include <atomic>
#include <functional>
#include <future>
#include <mutex>

#include "iplayer/i_track_provider.h"
#include "iplayer/track_location.h"

namespace ip {

class DummyDecoder : public IDecoder {
 public:
  using CompletionCb = std::function<void(const std::error_code&)>;

  DummyDecoder(std::unique_ptr<ITrackProvider> provider,
               const TrackLocation& track, CompletionCb completion_cb);
  virtual ~DummyDecoder();

  void Pause() override;
  void Unpause() override;
  std::chrono::seconds GetPlayedTime() const override;

 private:
  void DecoderThread(std::unique_ptr<ITrackProvider> provider,
                     TrackLocation track, CompletionCb completion_cb);

  std::atomic<bool> paused_;
  std::mutex pause_mutex_;
  std::condition_variable pause_cv_;
  std::atomic<bool> exit_decoder_thread_;
  std::atomic<std::chrono::seconds> played_time_;
  std::future<void> decoder_future_;
};

}  // namespace ip
