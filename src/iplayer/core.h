#pragma once

#include "iplayer/utils/exec_queue.h"

namespace ip {

class Core {
 public:
  using AsyncFunc = ExecQueue::Func;

  Core();
  void Start();  // instanciate everything and start execution queue
  void Stop();  // stop the exec queue

  void QueueExecution(AsyncFunc func);  // post 'func' to be executed later

 private:
  void Run();

  ExecQueue exec_queue_;
};

}  // namespace ip
