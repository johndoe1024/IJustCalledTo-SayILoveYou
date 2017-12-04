#pragma once

#include "iplayer/decoder_factory.h"
#include "iplayer/track_provider_resolver.h"
#include "iplayer/utils/exec_queue.h"

namespace ip {

class Core {
 public:
  using AsyncFunc = ExecQueue::Func;

  Core();
  void Start();
  void Stop();

  void QueueExecution(AsyncFunc func);
  ITrackProviderPtr GetTrackProvider(const TrackLocation& location) const;

  template <typename... Args>
  IDecoderPtr CreateDecoder(Args&&... args) {
    return decoders_.Create(std::forward<Args>(args)...);
  }

 private:
  void Run();

  ExecQueue exec_queue_;
  TrackProviderResolver provider_resolver_;
  DecoderFactory decoders_;
};

}  // namespace ip
