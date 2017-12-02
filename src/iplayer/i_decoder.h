#pragma once

#include <chrono>
#include <system_error>

namespace ip {

class IDecoder {
 public:
  virtual ~IDecoder() {}

  virtual void Pause() = 0;
  virtual void Unpause() = 0;
  virtual std::chrono::seconds GetPlayedTime() const = 0;
};

}  // namespace ip
