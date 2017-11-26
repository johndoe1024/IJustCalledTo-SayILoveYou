#pragma once

#include <chrono>
#include <memory>
#include <system_error>

#include "iplayer/i_track_io.h"
#include "iplayer/track_location.h"

namespace ip {

class TrackInfo {
 public:
  TrackInfo(const TrackLocation& location, const std::string& title,
            uint32_t number, std::chrono::seconds duration,
            const std::string& codec)
      : location_(location),
        codec_(codec),
        title_(title),
        number_(number),
        duration_(duration) {}

  TrackLocation Location() const { return location_; }
  uint32_t TrackNumber() const { return number_; }
  std::string Title() const { return title_; }
  std::chrono::seconds Duration() const { return duration_; }
  std::string Codec() const { return codec_; }

 private:
  TrackLocation location_;
  std::string codec_;
  std::string title_;
  uint32_t number_;
  std::chrono::seconds duration_;
};

}  // namespace ip
