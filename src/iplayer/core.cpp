#include "iplayer/core.h"

#include <memory>

#include "iplayer/cli_ui.h"
#include "iplayer/dummy_decoder.h"
#include "iplayer/fs_track_provider.h"
#include "iplayer/mad_decoder.h"
#include "iplayer/player_control.h"
#include "iplayer/playlist.h"
#include "iplayer/track_location.h"
#include "iplayer/utils/log.h"

namespace ip {

Core::Core() : exec_queue_(std::this_thread::get_id()) {}

void Core::Start() {
  // this will allow to resolve which component should be used depending on uri,
  // this will fallback on DummyDecoder when codec is unknown
  provider_resolver_.Register(
      "file://", []() { return std::make_unique<FsTrackProvider>(); });

  // map decoder to codec name
  decoders_.Register("dummy", &DecoderBuilder<DummyDecoder>);
#ifdef IPLAYER_DECODER_MAD
  decoders_.Register("mp3", &DecoderBuilder<MadDecoder>);
#endif  // IPLAYER_DECODER_MAD

  auto player_control = std::make_unique<PlayerControl>(this);
  Cli cli(std::move(player_control));
  cli.Run();
  exec_queue_.Run();
}

void Core::Stop() { exec_queue_.Exit(); }

void Core::QueueExecution(AsyncFunc func) { exec_queue_.Push(func); }

ITrackProviderPtr Core::GetTrackProvider(const TrackLocation& location) const {
  return provider_resolver_.Get(location);
}

}  // namespace ip
