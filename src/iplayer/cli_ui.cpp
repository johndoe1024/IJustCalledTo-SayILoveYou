#include "iplayer/cli_ui.h"

#include <assert.h>
#include <iostream>
#include "assert.h"

#include "iplayer/log.h"
#include "iplayer/track_location.h"

namespace ip {

// clang-format off
void PrintHelp() {
  std::cout << std::endl << "Welcome to Imaginary Player " IPLAYER_VERSION << std::endl << std::endl;
  std::cout << "Commands:" << std::endl
            << "---------" << std::endl
            << "play                        play playlist's current track" << std::endl
            << "prev                        previous track (unpause)" << std::endl
            << "next                        next track (unpause)" << std::endl
            << "stop                        stop and return at start of playlist" << std::endl
            << "repeat_track on/off         current track will repeat" << std::endl
            << "repeat_playlist on/off      playlist will restart to play"  << std::endl
            << "add_track [track_name]      add track (metadata is dynamically created)" << std::endl
            << "show_track                  display information about current track" << std::endl
            << "remove_track [track_name]   remove 'track_name" << std::endl
            << "remove_duplicates           remove duplicate track" << std::endl
            << "show_playlist               show playlist" << std::endl
            << std::endl
            << "Use 'help' to display this message. Use 'exit' or 'quit' for leaving" << std::endl
            << std::endl;
}
// clang-format on

Cli::Cli(std::unique_ptr<IPlayerControl> player_ctl)
    : player_ctl_(std::move(player_ctl)) {}

Cli::~Cli() { assert(!ui_thread_.joinable()); }

void Cli::Dispatch(const std::string& command, const std::string& parameters) {
  if (command.empty()) {
    return;
  }

  if (command == "play") {
    player_ctl_->Play();
  } else if (command == "next") {
    player_ctl_->Next();
  } else if (command == "prev") {
    player_ctl_->Previous();
  } else if (command == "stop") {
    player_ctl_->Stop();
  } else if (command == "repeat_track") {
    player_ctl_->SetRepeatTrackEnabled(parameters == "on");
  } else if (command == "repeat_playlist") {
    player_ctl_->SetRepeatPlaylistEnabled(parameters == "on");
  } else if (command == "add_track") {
    TrackLocation track = parameters;
    player_ctl_->AddTrack(std::move(parameters));
  } else if (command == "show_track") {
    player_ctl_->ShowTrack();
  } else if (command == "remove_track") {
    player_ctl_->RemoveTrack(std::move(parameters));
  } else if (command == "remove_duplicates") {
    player_ctl_->RemoveDuplicateTrack();
  } else if (command == "show_playlist") {
    player_ctl_->ShowPlaylist();
  } else if (command == "help") {
    PrintHelp();
  } else {
    std::cout << "Error: unknown command '" << command << "'" << std::endl;
  }
}

void Cli::Run() {
  ui_thread_ = std::thread([this]() { UiThread(); });
}

void Cli::Exit() {
  if (ui_thread_.joinable()) {
    ui_thread_.join();
  }
}

void Cli::UiThread() {
  PrintHelp();

  while (true) {
    std::string input;
    std::cout << ">>> ";
    std::getline(std::cin, input);

    const auto first_space_index = input.find(' ');
    const auto command = input.substr(0, first_space_index);
    const auto parameters = input.substr(first_space_index + 1, input.size());
    LOG("[D] processing command='%s' with parameters='%s'", command.c_str(),
        parameters.c_str());
    if (command == "exit" || command == "quit") {
      player_ctl_->Exit();
      return;
    }
    Dispatch(command, parameters);
  }
}

}  // namespace ip
