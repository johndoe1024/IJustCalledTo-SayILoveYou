#pragma once

#include <chrono>
#include <memory>
#include <system_error>

#include "iplayer/i_track_io.h"
#include "iplayer/track_location.h"

namespace ip {

class TrackInfo {
 public:
  TrackInfo() : number_(0), duration_(0) {}

  TrackInfo(const TrackLocation& location)
      : location_(location), number_(0), duration_(0) {}

  TrackInfo(const TrackLocation& location, const std::string& title,
            uint32_t number, std::chrono::seconds duration,
            const std::string& codec)
      : location_(location),
        codec_(codec),
        title_(title),
        number_(number),
        duration_(duration) {}

  TrackInfo(TrackInfo&& o)
      : location_(std::move(o.location_)),
        codec_(std::move(o.codec_)),
        title_(std::move(o.title_)),
        number_(o.number_),
        duration_(o.duration_) {}

  TrackInfo(const TrackInfo& o)
      : location_(std::move(o.location_)),
        codec_(std::move(o.codec_)),
        title_(std::move(o.title_)),
        number_(o.number_),
        duration_(o.duration_) {}

  TrackInfo& operator=(TrackInfo&& o) {
    if (this == &o) {
      return *this;
    }
    location_ = std::move(o.location_);
    codec_ = std::move(o.codec_);
    title_ = std::move(o.title_);
    number_ = o.number_;
    duration_ = o.duration_;
    return *this;
  }

  TrackInfo& operator=(const TrackInfo& o) {
    if (this == &o) {
      return *this;
    }
    location_ = o.location_;
    codec_ = o.codec_;
    title_ = o.title_;
    number_ = o.number_;
    duration_ = o.duration_;
    return *this;
  }

  void SetTrackNumber(uint32_t number) { number_ = number; }
  void SetTitle(const std::string& title) { title_ = title; }
  void SetDuration(std::chrono::seconds duration) { duration_ = duration; }
  void SetCodec(const std::string& codec) { codec_ = codec; }

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

bool operator==(const TrackInfo& lhs, const TrackInfo& rhs);
bool operator!=(const TrackInfo& lhs, const TrackInfo& rhs);

}  // namespace ip
