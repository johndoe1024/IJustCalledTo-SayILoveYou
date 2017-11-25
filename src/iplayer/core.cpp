#include "iplayer/core.h"

#include <memory>

#include "iplayer/log.h"
#include "iplayer/player.h"
#include "iplayer/player_control/player_control.h"
#include "iplayer/playlist.h"
#include "iplayer/track_location.h"
#include "iplayer/ui/cli_ui.h"

namespace ip {

void Core::Start() {
  auto player_control = std::make_unique<PlayerControl>();
  Cli cli(std::move(player_control));
  cli.Run();
}

}  // namespace ip
