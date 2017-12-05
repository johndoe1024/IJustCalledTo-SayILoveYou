#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <system_error>

namespace ip {

class IDecoder {
 public:
  using CompletionCb = std::function<void(const std::error_code&)>;

  virtual ~IDecoder() {}

  virtual void Pause() = 0;
  virtual void Unpause() = 0;
  virtual std::chrono::seconds GetPlayedTime() const = 0;
};

using IDecoderPtr = std::unique_ptr<IDecoder>;

}  // namespace ip
