#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace ip {

// This class is an execution queue that I would replace with asio's io_service
// for async processing.

class ExecQueue {
 public:
  using Func = std::function<void()>;

  ExecQueue(std::thread::id exec_thread_id);
  void Run();
  void Exit();

  void Push(Func func);

 private:
  std::mutex mutex_;
  std::condition_variable cv_;
  std::queue<Func> queue_;
  std::atomic<bool> exit_;
  std::thread::id exec_thread_id_;  // associated thread for execution
};

}  // namespace ip
