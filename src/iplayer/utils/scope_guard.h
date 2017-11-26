#pragma once

#include <utility>

// from Boost.ScopeExit

namespace ip {

template <typename T>
class ScopeGuard {
 public:
  explicit ScopeGuard(T&& handler)
      : handler_(std::forward<T>(handler)), cancelled_(false) {}
  ~ScopeGuard() {
    if (cancelled_) {
      return;
    }
    try {
      handler_();
    } catch (...) {
    }
  }
  void Cancel() { cancelled_ = true; }

 private:
  T handler_;
  bool cancelled_;
};

template <typename T>
ScopeGuard<T> CreateScopeGuard(T&& handler) {
  return ScopeGuard<T>(std::forward<T>(handler));
}

}  // namespace ip
