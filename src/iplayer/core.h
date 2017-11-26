#pragma once

#include "iplayer/utils/exec_queue.h"

namespace ip {

class Core {
 public:
  using AsyncFunc = ExecQueue::Func;

  Core();
  void Start();
  void Stop();

  void QueueExecution(AsyncFunc func);

 private:
  void Run();

  ExecQueue exec_queue_;
};

}  // namespace ip
