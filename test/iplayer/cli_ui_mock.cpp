#include "iplayer/cli_ui.h"

#include <assert.h>

#include "iplayer/log.h"
#include "iplayer/track_location.h"
#include "test/iplayer/test_ui_main.h"

namespace ip {

Cli::Cli(std::unique_ptr<IPlayerControl> player_ctl)
    : player_ctl_(std::move(player_ctl)) {}

Cli::~Cli() {}

void Cli::Run() {
  cli_future_ = std::async(std::launch::async, [&]() { UiThread(); });
}

void Cli::Exit() { cli_future_.get(); }

void Cli::UiThread() {
  test_ui_main(player_ctl_.get());  // test entry point
}

}  // namespace ip
