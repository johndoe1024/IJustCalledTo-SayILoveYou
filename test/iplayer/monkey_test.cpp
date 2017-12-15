#include "iplayer/core.h"
#include "iplayer/i_player_control.h"

#include <future>
#include <iostream>
#include <thread>

#include "test_ui_main.h"

int main(int, char* []) {
  ip::Core core;
  core.Start();
  return 0;
}

namespace ip {

std::vector<TrackLocation> CreateTrackLocations(size_t count, size_t repeat) {
  std::vector<TrackLocation> locations;
  for (size_t r = 0; r < repeat; ++r) {
    for (size_t i = 0; i < count; ++i) {
      locations.push_back("foo_" + std::to_string(i));
    }
  }
  return locations;
}

void test_ui_main(IPlayerControl* player) {
  {
    player->AddTrack(CreateTrackLocations(500, 3));
    std::vector<std::thread> threads;
    try {
      for (size_t i = 0; i < 500; ++i) {
        std::this_thread::yield();
        std::string random_name("music" + std::to_string(i % 32));

        auto add_track = [&]() { player->AddTrack({random_name}); };
        threads.push_back(std::thread(std::move(add_track)));
        auto remove_duplicates = [&]() { player->RemoveDuplicateTrack(); };
        threads.push_back(std::thread(std::move(remove_duplicates)));
        auto remove_track = [&]() { player->RemoveTrack("random_name"); };
        threads.push_back(std::thread(std::move(remove_track)));
        auto play = [&]() { player->Play(); };
        threads.push_back(std::thread(std::move(play)));
        auto pause = [&]() { player->Pause(); };
        threads.push_back(std::thread(std::move(pause)));
        auto show_playlist = [&]() { player->ShowPlaylist(nullptr); };
        threads.push_back(std::thread(std::move(show_playlist)));
        auto next = [&]() { player->Next(); };
        threads.push_back(std::thread(std::move(next)));
        auto previous = [&]() { player->Previous(); };
        threads.push_back(std::thread(std::move(previous)));
        auto set_random_track = [&]() { player->SetRandomTrackEnabled(true); };
        threads.push_back(std::thread(std::move(set_random_track)));
        auto repeat_playlist = [&]() {
          player->SetRepeatPlaylistEnabled(true);
        };
        threads.push_back(std::thread(std::move(repeat_playlist)));
        auto repeat_track = [&]() { player->SetRepeatTrackEnabled(true); };
        threads.push_back(std::thread(std::move(repeat_track)));
        auto get_current_track = [&]() {
          player->GetCurrentTrackInfo(nullptr);
        };
        threads.push_back(std::thread(std::move(get_current_track)));
      }
    } catch (std::exception& e) {
      // 'Resource temporarily unavailable' with too many threads, std::async
      // as a very low number of available threads
      std::cerr << e.what() << std::endl;
    }
    for (auto& thread : threads) {
      thread.join();
    }
  }
  player->Exit();
}

}  // namespace ip
