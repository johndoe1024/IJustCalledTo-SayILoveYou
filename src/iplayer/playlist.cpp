#include "iplayer/playlist.h"

#include <assert.h>
#include <algorithm>
#include <functional>
#include <numeric>

// 'random' feature requirements:
// - when disabled original playlist must continue from current index
// - when enabled: all tracks get shuffled
// - when adding track: random list should keep its order (don't re-shuffle)
// - when removing track: random list should keep its order
// - when using 'previous': previous random track must be played
// - must be fast (access time and lookup for removal)
// - must not waste space (no deep-copy of playlist)
// - bonus: remove duplicates shouldn't remove current track

namespace ip {

Playlist::Playlist()
    : current_track_(0), random_mode_(false), prng_(dev_random_()) {}

void Playlist::AddTrack(const std::vector<TrackLocation>& tracks) {
  std::transform(std::cbegin(tracks), std::cend(tracks),
                 std::back_inserter(playlist_),
                 [](const TrackLocation& loc) { return TrackInfo{loc}; });
  if (!random_mode_) {
    return;
  }

  // update random map
  assert(playlist_.size() == real_to_random_.size());
  for (size_t i = 0; i < tracks.size(); ++i) {
    // append new indexes and randomize their positions
    real_to_random_.push_back(static_cast<TrackId>(playlist_.size() + i));
    std::uniform_int_distribution<TrackId> random_index_generator(
        0, static_cast<TrackId>(playlist_.size()));
    std::iter_swap(std::rbegin(real_to_random_),
                   std::begin(real_to_random_) + random_index_generator(prng_));
  }
}

void Playlist::RemoveIf(std::function<bool(const TrackInfo&)> pred) {
  playlist_.erase(
      std::remove_if(std::begin(playlist_), std::end(playlist_), pred),
      std::end(playlist_));
  if (current_track_ >= playlist_.size()) {
    current_track_ = 0;
  }
  // TODO: find a fast way to update maps and keep order while removing items
  //       so previous will go back to the same track
  if (random_mode_) {
    Shuffle();
  }
}

void Playlist::RemoveTrack(const std::unordered_set<TrackLocation>& tracks) {
  auto pred = [&](const TrackInfo& item) {
    return tracks.find(item.Location()) != std::cend(tracks);
  };
  RemoveIf(pred);
}

void Playlist::RemoveDuplicate() {
  std::unordered_set<TrackLocation> known;
  auto is_known = [&](const TrackInfo& item) {
    if (known.find(item.Location()) == std::end(known)) {
      known.insert(item.Location());
      return false;
    }
    return true;
  };
  RemoveIf(is_known);
}

std::deque<TrackInfo> Playlist::GetTracks() const { return playlist_; }

void Playlist::SetTrackInfo(
    const std::unordered_map<TrackLocation, TrackInfo>& tracks) {
  for (auto it = std::begin(playlist_); it != std::end(playlist_); ++it) {
    auto found_it = tracks.find(it->Location());
    if (found_it != std::cend(tracks)) {
      *it = found_it->second;
    }
  }
}

TrackInfo Playlist::SeekTrack(int64_t pos, SeekWay offset_type) {
  if (offset_type == SeekWay::kBegin) {
    assert(pos >= 0);
    current_track_ = static_cast<TrackId>(pos);
  }
  // TrackOffsetType::kCurrent
  int64_t new_pos = current_track_ + pos;
  if (new_pos < 0 || static_cast<size_t>(new_pos) >= playlist_.size()) {
    current_track_ = 0;
  } else {
    current_track_ = static_cast<TrackId>(new_pos);
  }
  if (random_mode_) {
    return playlist_.at(real_to_random_[current_track_]);
  } else {
    return playlist_.at(current_track_);
  }
}

TrackInfo Playlist::CurrentTrack() const {
  return playlist_.at(current_track_);
}

size_t Playlist::Remaining() const {
  assert(current_track_ == 0 ? true : current_track_ < playlist_.size());
  return playlist_.size() - 1 - current_track_;
}

void Playlist::Shuffle() {
  // fill randomized with a range from 0 to playlist_.size and shuffle
  std::vector<TrackId> randomized(playlist_.size());
  std::iota(std::begin(randomized), std::end(randomized), 0);
  std::shuffle(std::begin(randomized), std::end(randomized), prng_);

  // create the mapping between real track id to real id (index on playlist_)
  real_to_random_.clear();
  std::copy(std::cbegin(randomized), std::cend(randomized),
            std::back_inserter(real_to_random_));
}

void Playlist::SetModeRandom(bool value) {
  random_mode_ = value;
  if (random_mode_) {
    Shuffle();
  }
}

bool Playlist::IsModeRandom() const { return random_mode_; }

}  // namespace ip
