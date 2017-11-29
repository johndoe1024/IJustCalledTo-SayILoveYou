#include "iplayer/core.h"

#include <memory>

#include "iplayer/decoder.h"
#include "iplayer/log.h"
#include "iplayer/player_control.h"
#include "iplayer/playlist.h"
#include "iplayer/track_location.h"
#include "iplayer/cli_ui.h"

namespace ip {

Core::Core() : exec_queue_(std::this_thread::get_id()) {}

void Core::Start() {
  auto player_control = std::make_unique<PlayerControl>(this);
  Cli cli(std::move(player_control));
  cli.Run();
  exec_queue_.Run();  // would be nice to have asio::io_service here
}

void Core::Stop() { exec_queue_.Exit(); }

void Core::QueueExecution(AsyncFunc func) { exec_queue_.Push(func); }

}  // namespace ip
