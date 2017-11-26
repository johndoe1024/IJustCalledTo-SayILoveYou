#include "iplayer/utils/exec_queue.h"

#include <assert.h>

namespace ip {

// enforce it's always the same thread which unqueue and execute callbacks
ExecQueue::ExecQueue(std::thread::id exec_thread_id)
    : exit_(false), exec_thread_id_(exec_thread_id) {}

void ExecQueue::Push(Func fn) {
  std::lock_guard<std::mutex> lock(mutex_);
  assert(exit_ == false);
  queue_.push(fn);
  cv_.notify_one();
}

void ExecQueue::Run() {
  assert(exec_thread_id_ == std::this_thread::get_id());
  std::queue<Func> queue;
  while (!exit_) {
    // wait for something to execute or until exit_ == true
    {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_.wait(lock, [this]() { return queue_.size() || exit_; });
      std::swap(queue, queue_);
    }

    // empty the queue
    while (!queue.empty()) {
      auto func = queue.front();
      func();
      queue.pop();
    }
  }
}

void ExecQueue::Exit() {
  exit_ = true;
  cv_.notify_one();
}

}  // namespace ip