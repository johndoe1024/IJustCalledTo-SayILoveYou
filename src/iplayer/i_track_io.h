#pragma once

#include <cstdint>

#include "iplayer/track_location.h"

// Abstraction to read track from any source (currently unused)

namespace ip {

class ITrackIO {
 public:
  virtual ~ITrackIO() {}
  virtual void Open(const TrackLocation& track) = 0;
  virtual size_t Read(uint8_t* buffer, size_t len) = 0;
  virtual void Close() = 0;
};

}  // namespace ip
