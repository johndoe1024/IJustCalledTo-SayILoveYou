#pragma once

#include "iplayer/utils/exec_queue.h"
#include "iplayer/track_provider_resolver.h"

namespace ip {

class Core {
 public:
  using AsyncFunc = ExecQueue::Func;

  Core();
  void Start();
  void Stop();

  void QueueExecution(AsyncFunc func);
  ITrackProviderPtr GetTrackProvider(const TrackLocation& location) const;

 private:
  void Run();

  ExecQueue exec_queue_;
  TrackProviderResolver provider_resolver_;
};

}  // namespace ip
