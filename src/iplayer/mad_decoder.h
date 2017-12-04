#pragma once

#include "iplayer/i_decoder.h"

#include <atomic>
#include <functional>
#include <future>
#include <mutex>

#include "iplayer/track_info.h"
#include "iplayer/track_location.h"

struct pa_simple;
struct mad_header;
struct mad_pcm;

namespace ip {

class MadDecoder : public IDecoder {
 public:
  using CompletionCb = std::function<void(const std::error_code&)>;

  MadDecoder(const TrackInfo& track, CompletionCb completion_cb);
  virtual ~MadDecoder();

  void Pause() override;
  void Unpause() override;
  std::chrono::seconds GetPlayedTime() const override;

 private:
  void DecoderThread(TrackInfo track_info, CompletionCb completion_cb);
  std::error_code Decode(const TrackInfo& track);

  int Output(struct mad_header const* header, struct mad_pcm* pcm);

  std::atomic<bool> paused_;
  std::mutex pause_mutex_;
  std::condition_variable pause_cv_;
  std::atomic<bool> exit_decoder_thread_;
  std::atomic<std::chrono::seconds> played_time_;
  std::future<void> decoder_future_;

  pa_simple* device_;
};

}  // namespace ip
