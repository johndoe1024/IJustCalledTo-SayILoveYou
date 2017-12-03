#include "iplayer/player_control.h"

#include <assert.h>
#include <algorithm>

#include "iplayer/dummy_decoder.h"
#include "iplayer/fs_track_provider.h"
#include "iplayer/mad_decoder.h"
#include "iplayer/utils/log.h"

namespace ip {

PlayerControl::PlayerControl(Core* core)
    : core_(core), status_(Status::kStop) {}

void PlayerControl::Exit() {
  std::lock_guard<std::mutex> lock(mutex_);
  StopAndSeekBegin();
  core_->Stop();
}

void PlayerControl::Play() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (status_ == Status::kPause) {
    Unpause();
    return;
  }
  if (status_ == Status::kPlay) {
    return;
  }
  TrackInfo track;
  auto ec = playlist_.SeekTrack(0, Playlist::SeekWay::kCurrent, &track);
  if (ec) {
    StopAndSeekBegin();
    return;
  }
  PlayTrack(track.Location());
}

void PlayerControl::Pause() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (!decoder_) {
    return;
  }
  if (status_ == Status::kPlay) {
    decoder_->Pause();
    status_ = Status::kPause;
  } else if (status_ == Status::kPause) {
    decoder_->Unpause();
    status_ = Status::kPlay;
  }
}

void PlayerControl::Stop() {
  std::lock_guard<std::mutex> lock(mutex_);
  StopAndSeekBegin();
}

void PlayerControl::Next() {
  std::lock_guard<std::mutex> lock(mutex_);
  TrackInfo track;
  auto ec = playlist_.SeekTrack(1, Playlist::SeekWay::kCurrent, &track);
  if (ec) {
    StopAndSeekBegin();
    return;
  }
  PlayTrack(track.Location());
}

void PlayerControl::Previous() {
  std::lock_guard<std::mutex> lock(mutex_);
  TrackInfo track;
  auto ec = playlist_.SeekTrack(-1, Playlist::SeekWay::kCurrent, &track);
  if (ec && ec != std::errc::no_such_file_or_directory) {
    StopAndSeekBegin();
    return;
  }
  PlayTrack(track.Location());
}

void PlayerControl::RestartCurrentTrack() {
  std::lock_guard<std::mutex> lock(mutex_);
  TrackInfo track;
  auto ec = playlist_.SeekTrack(1, Playlist::SeekWay::kCurrent, &track);
  if (ec) {
    StopAndSeekBegin();
    return;
  }
  PlayTrack(track.Location());
}

void PlayerControl::SetRepeatPlaylistEnabled(bool value) {
  std::lock_guard<std::mutex> lock(mutex_);
  playlist_.SetRepeatPlaylistEnabled(value);
}

void PlayerControl::SetRepeatTrackEnabled(bool value) {
  std::lock_guard<std::mutex> lock(mutex_);
  playlist_.SetRepeatTrackEnabled(value);
}

void PlayerControl::SetRandomTrackEnabled(bool value) {
  std::lock_guard<std::mutex> lock(mutex_);
  playlist_.SetModeRandom(value);
}

void PlayerControl::Unpause() {
  // private method so no synchronization
  if (status_ != Status::kPause || !decoder_) {
    return;
  }
  decoder_->Unpause();
  status_ = Status::kPlay;
}

void PlayerControl::StopAndSeekBegin() {
  decoder_.reset();
  playlist_.SeekTrack(0, Playlist::SeekWay::kBegin, nullptr);
  status_ = Status::kStop;
}

void PlayerControl::PlayTrack(const TrackLocation& track) {
  // this handler will be called from decoder's thread context just before
  // returning, it mustn't call directly PlayerControl methods
  auto on_completion = [this](const std::error_code& ec) {
    if (ec) {
      LOG("[D] completion callback error: %s (%d)", ec.message().c_str(),
          ec.value());
      return;
    }
    core_->QueueExecution(std::bind(&PlayerControl::Next, this));
  };
  status_ = Status::kPlay;
  auto provider = std::make_unique<FsTrackProvider>();
  decoder_ = std::make_unique<Decoder>(std::move(provider), track,
                                       std::move(on_completion));
}

void PlayerControl::AddTrack(const std::vector<TrackLocation>& locations) {
  std::lock_guard<std::mutex> lock(mutex_);
  // add track without its metadata and get those async to keep responsive ui
  playlist_.AddTrack(locations);

  auto get_all_info = [this, locations]() {
    auto provider = std::make_unique<FsTrackProvider>();
    std::unordered_map<TrackLocation, TrackInfo> infos;
    for (const auto& location : locations) {
      auto track = provider->GetTrackInfo(location);
      infos.insert({location, track});
    }
    {
      std::lock_guard<std::mutex> lock(mutex_);
      playlist_.SetTrackInfo(std::move(infos));
    }
  };
  core_->QueueExecution(get_all_info);
}

TrackInfo PlayerControl::GetCurrentTrackInfo(
    std::chrono::seconds* elapsed) const {
  std::lock_guard<std::mutex> lock(mutex_);
  if (elapsed) {
    if (decoder_) {
      *elapsed = decoder_->GetPlayedTime();
    } else {
      *elapsed = std::chrono::seconds(0);
    }
  }
  return playlist_.CurrentTrack();
}

void PlayerControl::RemoveTrack(const TrackLocation& track_location) {
  std::lock_guard<std::mutex> lock(mutex_);
  playlist_.RemoveTrack({track_location});
}

void PlayerControl::RemoveDuplicateTrack() {
  std::lock_guard<std::mutex> lock(mutex_);
  playlist_.RemoveDuplicate();
}

std::vector<TrackInfo> PlayerControl::ShowPlaylist() const {
  decltype(playlist_.GetTracks()) playlist;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    playlist = playlist_.GetTracks();
  }
  std::vector<TrackInfo> tracks;
  std::transform(std::make_move_iterator(std::begin(playlist)),
                 std::make_move_iterator(std::end(playlist)),
                 std::back_inserter(tracks),
                 [](TrackInfo&& track) { return std::move(track); });
  return tracks;
}

}  // namespace ip
