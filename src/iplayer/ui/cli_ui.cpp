#include "iplayer/ui/cli_ui.h"

#include <assert.h>
#include <iostream>

#include "iplayer/log.h"
#include "iplayer/track_location.h"

namespace ip {

Cli::Cli(std::unique_ptr<IPlayerControl> player_ctl)
    : player_ctl_(std::move(player_ctl)) {}

void Cli::Dispatch(const std::string& command, const std::string& parameters) {
  if (command == "play") {
    player_ctl_->Play();
  } else if (command == "add_track") {
    TrackLocation track = parameters;
    player_ctl_->AddTrack(std::move(parameters));
  } else if (command == "next") {
    player_ctl_->Next();
  } else {
    std::cout << "Error: unknown command '" << command << "'" << std::endl;
  }
}

void Cli::Run() {
  std::cout << "Imaginary player " IPLAYER_VERSION << std::endl;

  Dispatch("add_track", "hello.music");
  Dispatch("add_track", "world.music");

  while (true) {
    std::string input;
    std::cout << ">>> ";
    std::getline(std::cin, input);

    const auto first_space_index = input.find(' ');
    const auto command = input.substr(0, first_space_index);
    const auto parameters = input.substr(first_space_index + 1, input.size());
    LOG("[D] processing command='%s' with parameters='%s'", command.c_str(),
        parameters.c_str());
    if (command == "exit") {
      player_ctl_->Exit();
      return;
    }
    Dispatch(command, parameters);
  }
}

}  // namespace ip
