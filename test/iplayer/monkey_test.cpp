#include "iplayer/core.h"
#include "iplayer/i_player_control.h"

#include <future>
#include <iostream>

#include "test_ui_main.h"

int main(int, char* []) {
  ip::Core core;
  core.Start();
  return 0;
}

namespace ip {

void test_ui_main(IPlayerControl* player) {
  try {
    {
      std::vector<std::future<void>> futures;
      for (size_t i = 0; i < 50; ++i) {
        std::string random_name("music" + std::to_string(i % 32));

        auto add_track = [&]() { player->AddTrack(random_name); };
        futures.push_back(std::async(std::launch::async, add_track));

        auto remove_duplicates = [&]() { player->RemoveDuplicateTrack(); };
        futures.push_back(std::async(std::launch::async, remove_duplicates));

        auto remove_track = [&]() { player->RemoveTrack("random_name"); };
        futures.push_back(std::async(std::launch::async, remove_track));

        auto play = [&]() { player->Play(); };
        futures.push_back(std::async(std::launch::async, play));

        auto pause = [&]() { player->Pause(); };
        futures.push_back(std::async(std::launch::async, pause));

        auto restart_current = [&]() { player->RestartCurrentTrack(); };
        futures.push_back(std::async(std::launch::async, restart_current));

        auto show_playlist = [&]() { player->ShowPlaylist(); };
        futures.push_back(std::async(std::launch::async, show_playlist));

        auto next = [&]() { player->Next(); };
        futures.push_back(std::async(std::launch::async, next));

        auto previous = [&]() { player->Previous(); };
        futures.push_back(std::async(std::launch::async, previous));

        auto set_random_track = [&]() { player->SetRandomTrackEnabled(true); };
        futures.push_back(std::async(std::launch::async, set_random_track));

        auto repeat_playlist = [&]() {
          player->SetRepeatPlaylistEnabled(true);
        };
        futures.push_back(std::async(std::launch::async, repeat_playlist));

        auto repeat_track = [&]() { player->SetRepeatTrackEnabled(true); };
        futures.push_back(std::async(std::launch::async, repeat_track));

        auto get_current_track = [&]() {
          player->GetCurrentTrackInfo(nullptr);
        };
        futures.push_back(std::async(std::launch::async, get_current_track));
      }
      for (auto& future : futures) {
        future.get();  // forward exceptions
      }
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  player->Exit();
}

}  // namespace ip
