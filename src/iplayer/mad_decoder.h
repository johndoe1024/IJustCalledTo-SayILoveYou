#pragma once

#include "iplayer/i_decoder.h"

#include <atomic>
#include <functional>
#include <future>
#include <mutex>

#include <pulse/simple.h>
#include <mad.h>

#include "iplayer/i_track_provider.h"
#include "iplayer/track_location.h"

namespace ip {

class MadDecoder : public IDecoder {
 public:
  using CompletionCb = std::function<void(const std::error_code&)>;

  MadDecoder(std::unique_ptr<ITrackProvider> provider,
             const TrackLocation& track, CompletionCb completion_cb);
  virtual ~MadDecoder();

  void Pause() override;
  void Unpause() override;
  std::chrono::seconds GetPlayedTime() const override;

 private:
  void DecoderThread(std::unique_ptr<ITrackProvider> provider,
                     TrackLocation track, CompletionCb completion_cb);
  int Output(struct mad_header const *header, struct mad_pcm *pcm);

  std::atomic<bool> paused_;
  std::mutex pause_mutex_;
  std::condition_variable pause_cv_;
  std::atomic<bool> exit_decoder_thread_;
  std::atomic<std::chrono::seconds> played_time_;
  std::future<void> decoder_future_;

  // mad
  pa_simple* device_;
  struct mad_stream mad_stream_;
  struct mad_frame mad_frame_;
  struct mad_synth mad_synth_;
};

}  // namespace ip
