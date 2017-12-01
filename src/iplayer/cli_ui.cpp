#include "iplayer/cli_ui.h"

#include <assert.h>
#include <iostream>

#include "iplayer/log.h"
#include "iplayer/track_location.h"

namespace ip {

// clang-format off
void PrintHelp() {
  std::cout << std::endl << "Welcome to Imaginary Player " IPLAYER_VERSION << std::endl << std::endl;
  std::cout << "Commands:" << std::endl
            << "---------" << std::endl
            << "play / p                    play playlist's current track" << std::endl
            << "prev / b                    previous track (unpause)" << std::endl
            << "next / n                    next track (unpause)" << std::endl
            << "stop                        stop and return at start of playlist" << std::endl
            << "repeat_track on/off         current track will repeat" << std::endl
            << "repeat_playlist on/off      playlist will restart when finished"  << std::endl
            << "random_track on/off         play random tracks from playlist"  << std::endl
            << "add_track [track_name] / a  add track (metadata is dynamically created)" << std::endl
            << "show_track / s              display information about current track" << std::endl
            << "remove_track [track_name]   remove 'track_name" << std::endl
            << "remove_duplicates           remove duplicate track" << std::endl
            << "show_playlist / pl          show playlist" << std::endl
            << std::endl
            << "Use 'help' to display this message. Use 'exit' or 'quit' for leaving" << std::endl
            << std::endl;
}
// clang-format on

void PrintTrackInfo(const TrackInfo& track, std::chrono::seconds* elapsed) {
  std::string elapsed_str;
  if (!elapsed) {
    elapsed_str = "N/A";
  } else {
    elapsed_str = std::to_string(elapsed->count());
  }

  std::cout << "Track: " << track.TrackNumber() << std::endl
            << track.Location() << std::endl
            << "Duration: " << track.Duration().count() << "sec" << std::endl
            << "Elapsed: " << elapsed_str << "sec" << std::endl
            << "Title: " << track.Title() << std::endl
            << "Codec: " << track.Codec() << std::endl;
}

void PrintPlaylistInfo(const std::deque<TrackInfo>& playlist) {
  for (const auto& track : playlist) {
    std::cout << track.Location() << " - ["
              << std::to_string(track.TrackNumber()) << "] - "
              << std::to_string(track.Duration().count()) << "s - "
              << track.Title() << std::endl;
  }
}

Cli::Cli(std::unique_ptr<IPlayerControl> player_ctl)
    : player_ctl_(std::move(player_ctl)) {}

Cli::~Cli() {}

void Cli::Dispatch(const std::string& command, const std::string& parameters) {
  if (command.empty()) {
    return;
  }

  try {
    if (command == "play" || command == "p") {
      player_ctl_->Play();
      auto track = player_ctl_->GetCurrentTrackInfo(nullptr);
      std::cout << "Playing " << track.Title() << " (" << track.Location()
                << ")" << std::endl;
    } else if (command == "next" || command == "n") {
      player_ctl_->Next();
    } else if (command == "prev" || command == "b") {
      player_ctl_->Previous();
    } else if (command == "stop") {
      player_ctl_->Stop();
    } else if (command == "repeat_track") {
      player_ctl_->SetRepeatTrackEnabled(parameters == "on");
    } else if (command == "repeat_playlist") {
      player_ctl_->SetRepeatPlaylistEnabled(parameters == "on");
    } else if (command == "random_track") {
      player_ctl_->SetRandomTrackEnabled(parameters == "on");
    } else if (command == "add_track" || command == "a") {
      TrackLocation track = parameters;
      player_ctl_->AddTrack({parameters});
      std::cout << "Added " << parameters << std::endl;
    } else if (command == "show_track" || command == "s") {
      std::chrono::seconds elapsed;
      auto track = player_ctl_->GetCurrentTrackInfo(&elapsed);
      PrintTrackInfo(track, &elapsed);
    } else if (command == "remove_track") {
      player_ctl_->RemoveTrack(std::move(parameters));
    } else if (command == "remove_duplicates") {
      player_ctl_->RemoveDuplicateTrack();
    } else if (command == "show_playlist" || command == "pl") {
      auto playlist = player_ctl_->ShowPlaylist();
      PrintPlaylistInfo(playlist);
    } else if (command == "help" || command == "h") {
      PrintHelp();
    } else {
      std::cout << "Error: unknown command '" << command << "'" << std::endl;
    }
  } catch (const std::exception& e) {
    LOG("[E] %s", e.what());
    std::cerr << e.what();
  }
}


void Cli::Run() {
  cli_future_ = std::async(std::launch::async, [&]() { UiThread(); });
}

void Cli::Exit() { cli_future_.get(); }

void Cli::UiThread() {
  // for tests
  Dispatch("add_track", "hello.music");
  Dispatch("add_track", "world1.music");
  Dispatch("add_track", "world2.music");
  Dispatch("add_track", "world3.music");
  Dispatch("add_track", "world4.music");
  Dispatch("add_track", "world2.music");
  Dispatch("add_track", "world5.music");
  Dispatch("add_track", "world6.music");
  Dispatch("add_track", "world2.music");

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
