#pragma once

#include <functional>
#include <map>
#include <memory>
#include <mutex>

#include "iplayer/i_decoder.h"
#include "iplayer/track_info.h"

namespace ip {

template <typename T>
IDecoderPtr DecoderBuilder(const TrackInfo& track,
                           IDecoder::CompletionCb completion_cb) {
  return std::make_unique<T>(track, completion_cb);
}

class DecoderFactory {
 public:
  using CompletionCb = std::function<void(const std::error_code&)>;

  using Builder = std::function<IDecoderPtr(const TrackInfo&, CompletionCb)>;

  IDecoderPtr Create(const std::string& codec, const TrackInfo& track,
                     CompletionCb completion_cb) const;

  void Register(const std::string& codec, Builder builder);

 private:
  mutable std::mutex mutex_;
  std::map<std::string, Builder> decoders_;
};

}  // namespace ip
