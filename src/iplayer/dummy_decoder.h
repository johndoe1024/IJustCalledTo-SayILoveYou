#pragma once

#include "iplayer/i_decoder.h"

#include <atomic>
#include <future>
#include <mutex>

#include "iplayer/track_info.h"

namespace ip {

class DummyDecoder : public IDecoder {
 public:
  DummyDecoder(const TrackInfo& track, IDecoder::CompletionCb completion_cb);
  virtual ~DummyDecoder();

  void Pause() override;
  void Unpause() override;
  std::chrono::seconds GetPlayedTime() const override;

 private:
  void DecoderThread(TrackInfo track, CompletionCb completion_cb);

  std::mutex pause_mutex_;
  std::condition_variable pause_cv_;
  std::atomic<bool> paused_;
  std::atomic<bool> exit_decoder_thread_;
  std::atomic<std::chrono::seconds> played_time_;
  std::future<void> decoder_future_;
};

}  // namespace ip
