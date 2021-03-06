#include "iplayer/decoder_factory.h"

namespace ip {

IDecoderPtr DecoderFactory::Create(const std::string& codec,
                                   const TrackInfo& track,
                                   CompletionCb completion_cb) const {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = decoders_.find(codec);
  if (it == std::cend(decoders_)) {
    return nullptr;
  }
  return it->second(track, std::move(completion_cb));
}

void DecoderFactory::Register(const std::string& codec, Builder builder) {
  std::lock_guard<std::mutex> lock(mutex_);
  decoders_[codec] = builder;
}

}  // namespace ip
